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
#include <zsLib/Log.h>
#include <zsLib/helpers.h>

#include <boost/ref.hpp>
#include <boost/thread.hpp>

#include <pthread.h>

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
  namespace internal
  {
    //-------------------------------------------------------------------------
    MessageQueueThreadBasicPtr MessageQueueThreadBasic::create(const char *threadName, ThreadPriorities threadPriority)
    {
      MessageQueueThreadBasicPtr thread(new MessageQueueThreadBasic(threadName));
      thread->mQueue = zsLib::MessageQueue::create(thread);
      thread->mThread = ThreadPtr(new boost::thread(boost::ref(*thread.get())));
      thread->mThreadPriority = threadPriority;

      zsLib::setThreadPriority(*(thread->mThread), threadPriority);
      return thread;
    }

    //-------------------------------------------------------------------------
    MessageQueueThreadBasic::MessageQueueThreadBasic(const char *threadName) :
      mThreadName(threadName),
      mMustShutdown(0),
      mIsShutdown(false)
    {
    }

    void MessageQueueThreadBasic::operator()()
    {
      bool shouldShutdown = false;

      MessageQueuePtr queue = mQueue;

#ifdef __QNX__
      if (!mThreadName.isEmpty()) {
        pthread_setname_np(pthread_self(), mThreadName);
      }
#else
#ifndef _LINUX
#ifndef _ANDROID
      if (!mThreadName.isEmpty()) {
        pthread_setname_np(mThreadName);
      }
#endif // _ANDROID
#endif // _LINUX
#endif // __QNX__

      do
      {
        queue->process(); // process all pending data now
        mEvent.reset();   // should be safe to reset the notification now that we are done processing

        queue->process(); // small window between the process and the reset where more events could have arrived so process those now
        shouldShutdown = (0 != zsLib::atomicGetValue32(mMustShutdown));

        if (!shouldShutdown) {
          mEvent.wait();    // wait for the next event to arrive
          queue->process(); // process data in case shutdown gets activated
        }

        shouldShutdown = (0 != zsLib::atomicGetValue32(mMustShutdown));
      } while(!shouldShutdown);

      mIsShutdown = true;
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadBasic::post(IMessageQueueMessagePtr message)
    {
      if (mIsShutdown) {
        ZS_THROW_CUSTOM(IMessageQueue::Exceptions::MessageQueueGone, "message posted to message queue after message queue was deleted.")
      }
      mQueue->post(message);
    }

    //-------------------------------------------------------------------------
    IMessageQueue::size_type MessageQueueThreadBasic::getTotalUnprocessedMessages() const
    {
      AutoLock lock(mLock);
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

        zsLib::atomicSetValue32(mMustShutdown, 1);
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
      mQueue->process();
    }
  }
}
