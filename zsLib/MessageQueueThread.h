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

#ifndef ZSLIB_MESSAGEQUEUETHREAD_H_573a8d930ea451693ccd53632e481cb0
#define ZSLIB_MESSAGEQUEUETHREAD_H_573a8d930ea451693ccd53632e481cb0

#include <zsLib/types.h>

namespace zsLib {

  enum ThreadPriorities
  {
    ThreadPriority_LowPriority,
    ThreadPriority_NormalPriority,
    ThreadPriority_HighPriority,
    ThreadPriority_HighestPriority,
    ThreadPriority_RealtimePriority
  };

  const char *toString(ThreadPriorities priority);
  ThreadPriorities threadPriorityFromString(const char *str);

  void setThreadPriority(
                         Thread &thread,
                         ThreadPriorities threadPriority
                         );
} // namespace zsLib

#include <zsLib/internal/zsLib_MessageQueueThread.h>

namespace zsLib
{
  interaction IMessageQueueThread : public IMessageQueue
  {
    virtual void waitForShutdown() = 0;

    virtual void setThreadPriority(ThreadPriorities priority) = 0;

    virtual void processMessagesFromThread() = 0;
  };

  class MessageQueueThread : public IMessageQueueThread
  {
  public:
    static MessageQueueThreadPtr createBasic(const char *threadName = NULL, ThreadPriorities threadPriority = ThreadPriority_NormalPriority);
    static MessageQueueThreadPtr singletonUsingCurrentGUIThreadsMessageQueue();
  };

#ifdef __QNX__

  ZS_DECLARE_INTERACTION_PTR(IQtCrossThreadNotifierDelegate)

  interaction IQtCrossThreadNotifierDelegate
  {
    virtual void processMessageFromThread() = 0;
  };

  ZS_DECLARE_INTERACTION_PTR(IQtCrossThreadNotifier)

  interaction IQtCrossThreadNotifier
  {
    static IQtCrossThreadNotifierPtr createNotifier();

    virtual void setDelegate(IQtCrossThreadNotifierDelegatePtr delegate) = 0;
    virtual void notifyMessagePosted() = 0;
  };

#endif // __QNX__
}

#endif //ZSLIB_MESSAGEQUEUETHREAD_H_573a8d930ea451693ccd53632e481cb0
