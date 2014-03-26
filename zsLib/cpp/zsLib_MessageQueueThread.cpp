/*
 *  Created by Robin Raymond.
 *  Copyright 2009-2013. Robin Raymond. All rights reserved.
 *
 * This file is part of zsLib.
 *
 * zsLib is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License (LGPL) as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * zsLib is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with zsLib; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */

#include <zsLib/MessageQueueThread.h>
#include <zsLib/internal/zsLib_MessageQueueThreadBasic.h>
#include <zsLib/internal/zsLib_MessageQueueThreadUsingCurrentGUIMessageQueueForWindows.h>
#include <zsLib/internal/zsLib_MessageQueueThreadUsingMainThreadMessageQueueForApple.h>
#include <zsLib/internal/zsLib_MessageQueueThreadUsingBlackberryChannels.h>
#include <zsLib/Log.h>

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
  namespace internal
  {
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
#error MUST IMPLEMENT THIS
#endif //_WIN32
    }
  }

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

  namespace internal
  {
    struct StrToPriority
    {
      const char *mStr;
      ThreadPriorities mPriority;
    };

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
  }

  using internal::StrToPriority;

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

  void setThreadPriority(
                         Thread &thread,
                         ThreadPriorities threadPriority
                         )
  {
    internal::setThreadPriority(thread.native_handle(), threadPriority);
  }

  MessageQueueThreadPtr MessageQueueThread::createBasic(const char *threadName, ThreadPriorities threadPriority)
  {
    return internal::MessageQueueThreadBasic::create(threadName);
  }

  MessageQueueThreadPtr MessageQueueThread::singletonUsingCurrentGUIThreadsMessageQueue()
  {
#ifdef _WIN32
    return internal::MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::singleton();
#elif defined(__APPLE__)
      return internal::MessageQueueThreadUsingMainThreadMessageQueueForApple::singleton();
#elif defined(__QNX__)
      return internal::MessageQueueThreadUsingBlackberryChannels::singleton();
#else
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : warning: "
#pragma message(__LOC__"Need to implement this method on current target platform...")

    return internal::MessageQueueThreadBasic::create();
#endif //_WIN32
  }
}
