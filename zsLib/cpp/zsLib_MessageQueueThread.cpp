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

#include <zsLib/internal/zsLib_MessageQueueThread.h>
#include <zsLib/internal/zsLib_MessageQueueThreadBasic.h>
#include <zsLib/internal/zsLib_MessageQueueThreadUsingCurrentGUIMessageQueueForWinRT.h>
#include <zsLib/internal/zsLib_MessageQueueThreadUsingCurrentGUIMessageQueueForWindows.h>
#include <zsLib/internal/zsLib_MessageQueueThreadUsingMainThreadMessageQueueForApple.h>
#include <zsLib/internal/zsLib_MessageQueueThreadUsingBlackberryChannels.h>
#include <zsLib/Log.h>

//namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
  namespace internal
  {
    //-------------------------------------------------------------------------
    void setThreadPriority(
                           Thread::native_handle_type handle,
                           ThreadPriorities threadPriority
                           )
    {
#ifndef _WIN32
      const int policy = SCHED_RR;
      const int minPrio = sched_get_priority_min(policy);
      const int maxPrio = sched_get_priority_max(policy);
      sched_param param;
      switch (threadPriority)
      {
        case ThreadPriority_LowPriority:
          param.sched_priority = minPrio + 1;
          break;
        case ThreadPriority_NormalPriority:
          param.sched_priority = (minPrio + maxPrio) / 2;
          break;
        case ThreadPriority_HighPriority:
          param.sched_priority = maxPrio - 3;
          break;
        case ThreadPriority_HighestPriority:
          param.sched_priority = maxPrio - 2;
          break;
        case ThreadPriority_RealtimePriority:
          param.sched_priority = maxPrio - 1;
          break;
      }
      pthread_setschedparam(handle, policy, &param);
#else
		  int priority = THREAD_PRIORITY_NORMAL;
		  switch (threadPriority) {
        case ThreadPriority_LowPriority:      priority = THREAD_PRIORITY_LOWEST; break;
        case ThreadPriority_NormalPriority:   priority = THREAD_PRIORITY_NORMAL; break;
        case ThreadPriority_HighPriority:     priority = THREAD_PRIORITY_ABOVE_NORMAL; break;
        case ThreadPriority_HighestPriority:  priority = THREAD_PRIORITY_HIGHEST; break;
        case ThreadPriority_RealtimePriority: priority = THREAD_PRIORITY_TIME_CRITICAL; break;
      }
#ifndef WINRT
		  auto result = SetThreadPriority(handle, priority);
      assert(0 != result);
#endif //ndef WINRT

#endif //_WIN32
    }

    //-------------------------------------------------------------------------
    struct StrToPriority
    {
      const char *mStr;
      ThreadPriorities mPriority;
    };

    //-------------------------------------------------------------------------
    static StrToPriority *getPriorities()
    {
      static StrToPriority gPriorities[] = {
        {
          "low",
          ThreadPriority_LowPriority
        },
        {
          "normal",
          ThreadPriority_NormalPriority
        },
        {
          "high",
          ThreadPriority_HighPriority
        },
        {
          "highest",
          ThreadPriority_HighestPriority
        },
        {
          "real-time",
          ThreadPriority_RealtimePriority
        },
        {
          "real time",
          ThreadPriority_RealtimePriority
        },
        {
          "realtime",
          ThreadPriority_RealtimePriority
        },
        {
          NULL,
          ThreadPriority_NormalPriority
        }
      };
      return gPriorities;
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark MessageQueueThread
    #pragma mark

    //-------------------------------------------------------------------------
    MessageQueueThreadPtr MessageQueueThread::createBasic(const char *threadName, ThreadPriorities threadPriority)
    {
      return internal::MessageQueueThreadBasic::create(threadName);
    }

    //-------------------------------------------------------------------------
    MessageQueueThreadPtr MessageQueueThread::singletonUsingCurrentGUIThreadsMessageQueue()
    {
#ifdef _WIN32
      return internal::MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::singleton();
#elif defined(__APPLE__)
      return internal::MessageQueueThreadUsingMainThreadMessageQueueForApple::singleton();
#elif defined(__QNX__)
      return internal::MessageQueueThreadUsingBlackberryChannels::singleton();
#elif defined(_ANDROID)
      return internal::MessageQueueThreadBasic::create("build");
#else
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "(" __STR1__(__LINE__) ") : warning: "
#pragma message(__LOC__"Need to implement this method on current target platform...")

      return internal::MessageQueueThreadBasic::create();
#endif //_WIN32
    }


  } // namespace internal

  //---------------------------------------------------------------------------
  const char *toString(ThreadPriorities priority)
  {
    switch (priority) {
    case ThreadPriority_LowPriority:      return "Low";
    case ThreadPriority_NormalPriority:   return "Normal";
    case ThreadPriority_HighPriority:     return "High";
    case ThreadPriority_HighestPriority:  return "Highest";
    case ThreadPriority_RealtimePriority: return "Real-time";
    }
    return "UNDEFINED";
  }

  using internal::StrToPriority;

  //---------------------------------------------------------------------------
  ThreadPriorities threadPriorityFromString(const char *str)
  {
    if (!str) return ThreadPriority_NormalPriority;

    String compareTo(str);
    compareTo.trim();

    StrToPriority *priorities = internal::getPriorities();

    for (size_t index = 0; NULL != priorities[index].mStr; ++index)
    {
      String compareStr(priorities[index].mStr);
      if (0 == String(priorities[index].mStr).compareNoCase(compareTo)) {
        return priorities[index].mPriority;
      }
    }

    return ThreadPriority_NormalPriority;
  }

  //---------------------------------------------------------------------------
  void setThreadPriority(
                         Thread &thread,
                         ThreadPriorities threadPriority
                         )
  {
    internal::setThreadPriority(thread.native_handle(), threadPriority);
  }

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark MessageQueueThread
  #pragma mark

  //---------------------------------------------------------------------------
  IMessageQueueThreadPtr IMessageQueueThread::createBasic(
                                                          const char *threadName,
                                                          ThreadPriorities threadPriority
                                                          )
  {
    return internal::MessageQueueThread::createBasic(threadName, threadPriority);
  }

  //---------------------------------------------------------------------------
  IMessageQueueThreadPtr IMessageQueueThread::singletonUsingCurrentGUIThreadsMessageQueue()
  {
    return internal::MessageQueueThread::singletonUsingCurrentGUIThreadsMessageQueue();
  }

}
