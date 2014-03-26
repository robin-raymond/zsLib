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
