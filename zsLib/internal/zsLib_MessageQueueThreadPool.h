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

#ifndef ZSLIB_INTERNAL_MESSAGEQUEUETHREADPOOL_H_e140ea49aaff413fed9a0487ce58baa64fba5a51
#define ZSLIB_INTERNAL_MESSAGEQUEUETHREADPOOL_H_e140ea49aaff413fed9a0487ce58baa64fba5a51

#include <zsLib/MessageQueueThread.h>
#include <zsLib/MessageQueue.h>

namespace zsLib
{
  namespace internal
  {
    ZS_DECLARE_CLASS_PTR(MessageQueueThreadPoolDispatcherThread)
    ZS_DECLARE_CLASS_PTR(MessageQueueThreadPoolQueueNotifier)

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark MessageQueueThreadPool
    #pragma mark

    class MessageQueueThreadPool
    {
    public:
      friend MessageQueueThreadPoolQueueNotifier;
      friend MessageQueueThreadPoolDispatcherThread;

      typedef std::list<MessageQueueThreadPoolDispatcherThreadPtr> DispatcherThreadList;
      typedef std::queue<MessageQueueThreadPoolDispatcherThreadPtr> DispatcherThreadQueue;
      typedef std::queue<MessageQueueThreadPoolQueueNotifierPtr> MessageNotifierQueue;

    protected:
      void init();

      void notifyPosted(MessageQueueThreadPoolQueueNotifierPtr queue);
      void notifyIdle(MessageQueueThreadPoolDispatcherThreadPtr dispatcher);
      void processOneQueue();

    protected:
      MessageQueueThreadPoolWeakPtr mThisWeak;
      mutable Lock mLock;

      DispatcherThreadList mThreads;
      DispatcherThreadQueue mIdleThreads;
      MessageNotifierQueue mPendingQueues;

      size_t mMissingIdle {0};
    };
    
  }
}

#endif //ZSLIB_INTERNAL_MESSAGEQUEUETHREADPOOL_H_e140ea49aaff413fed9a0487ce58baa64fba5a51
