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

#pragma once

#ifndef ZSLIB_INTERNAL_SOCKETMONITOR_H_c01514fd3a9af7d11f32093baae8c546
#define ZSLIB_INTERNAL_SOCKETMONITOR_H_c01514fd3a9af7d11f32093baae8c546

#include <condition_variable>

#include <zsLib/types.h>
#include <zsLib/Log.h>
#include <zsLib/MessageQueueThread.h>
#include <zsLib/Singleton.h>

#include <map>
#include <list>

#ifdef __QNX__
#include <pthread.h>
#endif //__QNX__


namespace zsLib
{
  ZS_DECLARE_CLASS_PTR(Timer)

  namespace internal
  {
    ZS_DECLARE_CLASS_PTR(TimerMonitor)

    class TimerMonitor : public noncopyable,
                         public ISingletonManagerDelegate
    {
    public:
      ZS_DECLARE_TYPEDEF_PTR(zsLib::XML::Element, Element)
      ZS_DECLARE_TYPEDEF_PTR(zsLib::XML::Text, Text)

    protected:
      TimerMonitor();

      void init();

    public:
      ~TimerMonitor();

      static TimerMonitorPtr singleton();
      static TimerMonitorPtr create();

      static void setPriority(ThreadPriorities priority);

      void monitorBegin(TimerPtr timer);
      void monitorEnd(zsLib::Timer &timer);

      void operator()();

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark TimerMonitor => ISingletonManagerDelegate
      #pragma mark

      virtual void notifySingletonCleanup();

    private:
      zsLib::Log::Params log(const char *message) const;
      static zsLib::Log::Params slog(const char *message);

      void cancel();

      Microseconds fireTimers();
      void wakeUp();

    private:
      AutoPUID mID;

      RecursiveLock mLock;
      Lock mFlagLock;
      std::condition_variable mFlagNotify;

      TimerMonitorWeakPtr mThisWeak;
      TimerMonitorPtr mGracefulShutdownReference;

      ThreadPtr mThread;
      bool mShouldShutdown;

      typedef std::map<PUID, TimerWeakPtr> TimerMap;

      TimerMap mMonitoredTimers;

#ifdef __QNX__
      pthread_cond_t      mCondition;
      pthread_mutex_t     mMutex;
#endif //__QNX__
    };
  }
}

#endif //ZSLIB_INTERNAL_SOCKETMONITOR_H_c01514fd3a9af7d11f32093baae8c546
