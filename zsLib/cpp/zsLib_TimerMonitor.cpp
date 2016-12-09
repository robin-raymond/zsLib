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

#include <zsLib/internal/zsLib_TimerMonitor.h>
#include <zsLib/internal/zsLib_Timer.h>
#include <zsLib/internal/zsLib_MessageQueueThread.h>

#include <zsLib/ISettings.h>
#include <zsLib/XML.h>
#include <zsLib/helpers.h>

#ifdef __QNX__
#include <sys/time.h>
#endif //__QNX__

namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib)}

namespace zsLib
{
  namespace internal
  {
    ZS_DECLARE_CLASS_PTR(TimerMonitorSettingsDefaults);

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark TimerMonitorSettingsDefaults
    #pragma mark

    class TimerMonitorSettingsDefaults : public ISettingsApplyDefaultsDelegate
    {
    public:
      //-----------------------------------------------------------------------
      ~TimerMonitorSettingsDefaults()
      {
        ISettings::removeDefaults(*this);
      }

      //-----------------------------------------------------------------------
      static TimerMonitorSettingsDefaultsPtr singleton()
      {
        static SingletonLazySharedPtr<TimerMonitorSettingsDefaults> singleton(create());
        return singleton.singleton();
      }

      //-----------------------------------------------------------------------
      static TimerMonitorSettingsDefaultsPtr create()
      {
        auto pThis(make_shared<TimerMonitorSettingsDefaults>());
        ISettings::installDefaults(pThis);
        return pThis;
      }

      //-----------------------------------------------------------------------
      virtual void notifySettingsApplyDefaults() override
      {
        ISettings::setString(ZSLIB_SETTING_TIMER_MONITOR_THREAD_PRIORITY, "normal");
      }
    };

    //-------------------------------------------------------------------------
    void installTimerMonitorSettingsDefaults()
    {
      TimerMonitorSettingsDefaults::singleton();
    }

    //-------------------------------------------------------------------------
    TimerMonitor::TimerMonitor() :
      mShouldShutdown(false)
    {
#ifdef __QNX__
      static pthread_cond_t defaultCondition = PTHREAD_COND_INITIALIZER;
      static pthread_mutex_t defaultMutex = PTHREAD_MUTEX_INITIALIZER;

      memcpy(&mCondition, &defaultCondition, sizeof(mCondition));
      memcpy(&mMutex, &defaultMutex, sizeof(mMutex));
#endif //__QNX__
      ZS_LOG_DETAIL(log("created"))
    }

    //-------------------------------------------------------------------------
    void TimerMonitor::init()
    {
    }

    //-------------------------------------------------------------------------
    TimerMonitor::~TimerMonitor()
    {
      mThisWeak.reset();
      ZS_LOG_DETAIL(log("destroyed"))
      cancel();

#ifdef __QNX__
      pthread_cond_destroy(&mCondition);
      pthread_mutex_destroy(&mMutex);
#endif //__QNX__
    }

    //-------------------------------------------------------------------------
    TimerMonitorPtr TimerMonitor::singleton()
    {
      static SingletonLazySharedPtr<TimerMonitor> singleton(TimerMonitor::create());
      TimerMonitorPtr result = singleton.singleton();
      if (!result) {
        ZS_LOG_WARNING(Detail, slog("singleton gone"))
      }

      static zsLib::SingletonManager::Register registerSingleton("org.zsLib.TimerMonitor", result);
      return result;
    }

    //-------------------------------------------------------------------------
    TimerMonitorPtr TimerMonitor::create()
    {
      TimerMonitorPtr pThis(new TimerMonitor);
      pThis->mThisWeak = pThis;
      pThis->init();
      return pThis;
    }

    //-------------------------------------------------------------------------
    void TimerMonitor::monitorBegin(TimerPtr timer)
    {
      AutoRecursiveLock lock(mLock);

      if (!mThread) {
        mThread = ThreadPtr(new std::thread(std::ref(*this)));
        setThreadPriority(mThread->native_handle(), zsLib::threadPriorityFromString(ISettings::getString(ZSLIB_SETTING_TIMER_MONITOR_THREAD_PRIORITY)));
      }

      PUID timerID = timer->getID();
      mMonitoredTimers[timerID] = TimerWeakPtr(timer);
      wakeUp();
    }

    //-------------------------------------------------------------------------
    void TimerMonitor::monitorEnd(Timer &timer)
    {
      AutoRecursiveLock lock(mLock);

      PUID timerID = timer.getID();

      TimerMap::iterator result = mMonitoredTimers.find(timerID);   // if the timer was scheduled to be monitored, then need to remove it from being monitored
      if (mMonitoredTimers.end() == result)
        return;

      mMonitoredTimers.erase(result);
      wakeUp();
    }

    //-------------------------------------------------------------------------
    void TimerMonitor::operator()()
    {
      bool shouldShutdown = false;

      debugSetCurrentThreadName("org.zsLib.timer");

      do
      {
        Microseconds duration {};
        // wait completed, do notifications from select
        {
          AutoRecursiveLock lock(mLock);
          shouldShutdown = mShouldShutdown;

          duration = fireTimers();
        }

#ifdef __QNX__
        struct timeval tp;
        struct timespec ts;
        memset(&tp, 0, sizeof(tp));
        memset(&ts, 0, sizeof(ts));

        int rc =  gettimeofday(&tp, NULL);
        ZS_THROW_BAD_STATE_IF(0 != rc)

        // Convert from timeval to timespec
        ts.tv_sec  = tp.tv_sec;
        ts.tv_nsec = tp.tv_usec * 1000;

        // add the time to expire from now
        ts.tv_sec += duration.seconds();
        ts.tv_nsec += ((duration - Seconds(duration.total_seconds()))).total_nanoseconds();

        // this could have caused tv_nsec to wrapped above second mark since it started in absolute time since epoch
        if (ts.tv_nsec >= (Seconds(1).total_nanoseconds())) {
          Duration wrapSeconds = Seconds(ts.tv_nsec / (Seconds(1).total_nanoseconds()));
          ts.tv_sec += wrapSeconds.total_seconds();
          ts.tv_nsec -= wrapSeconds.total_nanoseconds();
        }

        rc = pthread_mutex_lock(&mMutex);
        ZS_THROW_BAD_STATE_IF(0 != rc)

        rc = pthread_cond_timedwait(&mCondition, &mMutex, &ts);
        ZS_THROW_BAD_STATE_IF((0 != rc) && (ETIMEDOUT != rc))

        rc = pthread_mutex_unlock(&mMutex);
        ZS_THROW_BAD_STATE_IF(0 != rc)
#else
        std::unique_lock<std::mutex> flagLock(mFlagLock);
        mFlagNotify.wait_for(flagLock, duration);
#endif //__QNX__

        // notify all those timers needing to be notified
      } while (!shouldShutdown);

      TimerMonitorPtr gracefulReference;

      {
        AutoRecursiveLock lock(mLock);

        // transfer graceful shutdown reference to thread
        gracefulReference = mGracefulShutdownReference;
        mGracefulShutdownReference.reset();

        // go through timers and cancel them completely
        for (TimerMap::iterator monIter = mMonitoredTimers.begin(); monIter != mMonitoredTimers.end(); ) {
          TimerMap::iterator current = monIter;
          ++monIter;

          TimerPtr timer = current->second.lock();
          if (timer)
            timer->background(false);
        }
        mMonitoredTimers.clear();
      }
    }

    //-----------------------------------------------------------------------
    void TimerMonitor::notifySingletonCleanup()
    {
      cancel();
    }

    //-----------------------------------------------------------------------
    zsLib::Log::Params TimerMonitor::log(const char *message) const
    {
      ElementPtr objectEl = Element::create("TimerMonitor");

      ElementPtr element = Element::create("id");

      TextPtr tmpTxt = Text::create();
      tmpTxt->setValueAndJSONEncode(string(mID));
      element->adoptAsFirstChild(tmpTxt);

      objectEl->adoptAsLastChild(element);

      return zsLib::Log::Params(message, objectEl);
    }

    //-----------------------------------------------------------------------
    zsLib::Log::Params TimerMonitor::slog(const char *message)
    {
      return zsLib::Log::Params(message, "TimerMonitor");
    }

    //-------------------------------------------------------------------------
    void TimerMonitor::cancel()
    {
      ThreadPtr thread;
      {
        AutoRecursiveLock lock(mLock);
        mGracefulShutdownReference = mThisWeak.lock();
        thread = mThread;
        mThread.reset();

        mShouldShutdown = true;
        wakeUp();
      }

      if (!thread)
        return;

      if (thread->joinable()) {
        thread->join();
      }
    }

    //-------------------------------------------------------------------------
    Microseconds TimerMonitor::fireTimers()
    {
      AutoRecursiveLock lock(mLock);

      Time time = std::chrono::system_clock::now();

      Microseconds duration = Seconds(1);

      for (TimerMap::iterator monIter = mMonitoredTimers.begin(); monIter != mMonitoredTimers.end(); )
      {
        TimerMap::iterator current = monIter;
        ++monIter;

        TimerPtr timer = (current->second).lock();
        bool done = true;

        if (timer)
          done = timer->tick(time, duration);

        if (done)
          mMonitoredTimers.erase(current);
      }
      return duration;
    }

    //-------------------------------------------------------------------------
    void TimerMonitor::wakeUp()
    {
#ifdef __QNX__
      int rc = pthread_mutex_lock(&mMutex);
      ZS_THROW_BAD_STATE_IF(0 != rc)

      rc = pthread_cond_signal(&mCondition);
      ZS_THROW_BAD_STATE_IF(0 != rc)

      rc = pthread_mutex_unlock(&mMutex);
      ZS_THROW_BAD_STATE_IF(0 != rc)

#else
      mFlagNotify.notify_one();
#endif //__QNX__
    }
  }
}
