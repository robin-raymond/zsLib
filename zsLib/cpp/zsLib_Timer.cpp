/*

 Copyright (c) 2014, Robin Raymond
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 The views and conclusions contained in the software and documentation are those
 of the authors and should not be interpreted as representing official policies,
 either expressed or implied, of the FreeBSD Project.
 
 */

#include <zsLib/internal/zsLib_Timer.h>

#include <zsLib/Exception.h>
#include <zsLib/internal/zsLib_TimerMonitor.h>
#include <zsLib/helpers.h>

#ifndef ZSLIB_EVENTING_NOOP
#include <zsLib/internal/zsLib.events.h>
#else
#include <zsLib/eventing/noop.h>
#endif //ndef ZSLIB_EVENTING_NOOP

namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib)}

namespace zsLib
{
  ZS_EVENTING_TASK(Timer);
  ZS_EVENTING_TASK_OPCODE(Timer, Event);

  namespace internal
  {
    //-------------------------------------------------------------------------
    Timer::Timer(
                 const make_private &,
                 ITimerDelegatePtr delegate,
                 Microseconds timeout,
                 bool repeat,
                 size_t maxFiringsAtOnce
                 )
    {
      mDelegate = ITimerDelegateProxy::createWeak(delegate);
      ZS_THROW_INVALID_USAGE_IF(!mDelegate);
      ZS_THROW_INVALID_USAGE_IF(!ITimerDelegateProxy::isProxy(mDelegate)); // NOTE: the delegate passed in is not associated with a message queue

      mOnceOnly = !repeat;
      mMaxFiringsAtOnce = maxFiringsAtOnce;
      mTimeout = timeout;
      mID = createPUID();
      mMonitored = false;
      mFireNextAt = (std::chrono::system_clock::now() + timeout);

      ZS_EVENTING_3(x, i, Trace, TimerCreate, zs, Timer, Start, puid, id, mID, bool, repeat, repeat, duration, timeoutInMicroseconds, timeout.count());
    }

    //---------------------------------------------------------------------------
    Timer::~Timer()
    {
      mThisWeak.reset();
      cancel();
      ZS_EVENTING_1(x, i, Trace, TimerDestroy, zs, Timer, Stop, puid, id, mID);
    }

    //---------------------------------------------------------------------------
    TimerPtr Timer::create(
                           ITimerDelegatePtr delegate,
                           Microseconds timeout,
                           bool repeat,
                           size_t maxFiringTimerAtOnce
                           )
    {
      TimerPtr timer(make_shared<Timer>(make_private{}, delegate, timeout, repeat, maxFiringTimerAtOnce));
      timer->mThisWeak = timer;

      internal::TimerMonitorPtr singleton = internal::TimerMonitor::singleton();
      if (singleton) {
        singleton->monitorBegin(timer);
      }
      timer->mMonitored = true;
      return timer;
    }

    //---------------------------------------------------------------------------
    TimerPtr Timer::create(
                           ITimerDelegatePtr delegate,
                           Time timeout
                           )
    {
      Time now = zsLib::now();
      if (now > timeout) {
        return create(delegate, Microseconds(0), false, 1);
      }
      Microseconds waitTime = std::chrono::duration_cast<Microseconds>(timeout - now);
      return create(delegate, waitTime, false, 1);
    }

    //---------------------------------------------------------------------------
    void Timer::cancel()
    {
      {
        AutoRecursiveLock lock(mLock);
        mThisBackground.reset();
        if (!mMonitored)
          return;
      }

      internal::TimerMonitorPtr singleton = internal::TimerMonitor::singleton();
      if (singleton) {
        singleton->monitorEnd(*this);
      }

      {
        AutoRecursiveLock lock(mLock);
        mThisBackground.reset();
        mMonitored = false;
      }
    }

    //---------------------------------------------------------------------------
    void Timer::background(bool background)
    {
      AutoRecursiveLock lock(mLock);

      if (!background)
        mThisBackground.reset();
      else
        mThisBackground = mThisWeak.lock();
    }

    //-------------------------------------------------------------------------
    bool Timer::tick(const Time &time, Microseconds &sleepTime)
    {
      AutoRecursiveLock lock(mLock);
      bool fired = false;
      UINT totalFires = 0;

      while (mFireNextAt < time)
      {
        fired = true;
        try {
          ZS_EVENTING_1(x, i, Insane, TimerEvent, zs, Timer, Event, puid, id, mID);

          mDelegate->onTimer(mThisWeak.lock());
        } catch (ITimerDelegateProxy::Exceptions::DelegateGone &) {
          mOnceOnly = true;   // this has to stop firing now that the proxy to the delegate points to something that is now gone
          break;
        }

        mFireNextAt += mTimeout;
        ++totalFires;

        if (mOnceOnly)  // do not allow the timer to fire more than once
          break;

        if (totalFires >= mMaxFiringsAtOnce) {
          // Do not want the timer to wake up from a sleep only to discover
          // that it has missed hundreds or thousands of events and flood
          // the delegate with missed timers. Thus, if the total number of
          // triggered timers fired at once is greater than a reasonable
          // maximum then the timer firings must be stopped to prevent a timer
          // event flood.
          mFireNextAt = time + mTimeout;  // the next timeout resets to the clock plus the timeout
          break;
        }
      }

      if (!mOnceOnly) {
		  Microseconds diff = std::chrono::duration_cast<Microseconds>(mFireNextAt - time);
        if (diff < sleepTime)
          sleepTime = diff;
      }

      if (!fired)
        return false;

      if (mOnceOnly) {
        mDelegate.reset();
        mThisBackground.reset();
      }

      return mOnceOnly;
    }
  } // namespace internal

  //---------------------------------------------------------------------------
  ITimerPtr ITimer::create(
                            ITimerDelegatePtr delegate,
                            Microseconds timeout,
                            bool repeat,
                            size_t maxFiringTimerAtOnce
                            )
  {
    return internal::Timer::create(delegate, timeout, repeat, maxFiringTimerAtOnce);
  }

  //---------------------------------------------------------------------------
  ITimerPtr ITimer::create(
                           ITimerDelegatePtr delegate,
                           Time timeout
                           )
  {
    return internal::Timer::create(delegate, timeout);
  }

} // namespace zsLib
