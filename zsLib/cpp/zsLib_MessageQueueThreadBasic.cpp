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

#include <zsLib/internal/zsLib_MessageQueueThreadBasic.h>
#include <zsLib/internal/zsLib_MessageQueue.h>

#include <zsLib/Log.h>
#include <zsLib/helpers.h>

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
  namespace internal
  {
    //-------------------------------------------------------------------------
    MessageQueueThreadBasicPtr MessageQueueThreadBasic::create(const char *threadName, ThreadPriorities threadPriority)
    {
      MessageQueueThreadBasicPtr thread(new MessageQueueThreadBasic(threadName));
      thread->mQueue = MessageQueue::create(thread);
      thread->mThreadPriority = threadPriority;
      thread->mThread = ThreadPtr(new std::thread(std::ref(*thread.get())));

      zsLib::setThreadPriority(*(thread->mThread), threadPriority);
      return thread;
    }

    //-------------------------------------------------------------------------
    MessageQueueThreadBasic::MessageQueueThreadBasic(const char *threadName) :
      mThreadName(threadName)
    {
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadBasic::operator()()
    {
      bool shouldShutdown = false;

      MessageQueuePtr queue = mQueue;

      debugSetCurrentThreadName(mThreadName);

      do
      {
        queue->process(); // process all pending data now
        shouldShutdown = mMustShutdown;

        if (!shouldShutdown) {
          mEvent.wait();    // wait for the next event to arrive
          queue->process(); // process data in case shutdown gets activated
        }

        shouldShutdown = mMustShutdown;
      } while(!shouldShutdown);

      mIsShutdown = true;

      {
        AutoLock lock(mLock);
        mQueue.reset();
      }
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadBasic::post(IMessageQueueMessageUniPtr message)
    {
      if (mIsShutdown) {
        ZS_THROW_CUSTOM(IMessageQueue::Exceptions::MessageQueueGone, "message posted to message queue after message queue was deleted.")
      }
      mQueue->post(std::move(message));
    }

    //-------------------------------------------------------------------------
    IMessageQueue::size_type MessageQueueThreadBasic::getTotalUnprocessedMessages() const
    {
      AutoLock lock(mLock);
      if (!mQueue) return 0;
      return mQueue->getTotalUnprocessedMessages();
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadBasic::notifyMessagePosted()
    {
      mEvent.notify();
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadBasic::waitForShutdown()
    {
      ThreadPtr thread;
      {
        AutoLock lock(mLock);
        thread = mThread;

        mMustShutdown = true;
        mEvent.notify();
      }

      if (!thread)
        return;

      thread->join();

      {
        AutoLock lock(mLock);
        mThread.reset();
      }
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadBasic::setThreadPriority(ThreadPriorities threadPriority)
    {
      AutoLock lock(mLock);
      if (!mThread) return;

      if (threadPriority == mThreadPriority) return;

      mThreadPriority = threadPriority;

      zsLib::setThreadPriority(*mThread, threadPriority);
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadBasic::processMessagesFromThread()
    {
      if (mIsShutdown) return;
      mQueue->process();
    }
  }
}
