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

#ifdef __APPLE__

#include <zsLib/internal/zsLib_MessageQueueThreadUsingMainThreadMessageQueueForApple.h>
#include <zsLib/Log.h>
#include <zsLib/helpers.h>
#include <zsLib/Stringize.h>

#include <boost/ref.hpp>
#include <boost/thread.hpp>


namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
  namespace internal
  {
    // These are the CFRunLoopSourceRef callback functions.
    void RunLoopSourcePerformRoutine (void *info);
    void MoreMessagesLoopSourcePerformRoutine (void *info);

    //-------------------------------------------------------------------------
    MessageQueueThreadUsingMainThreadMessageQueueForApplePtr MessageQueueThreadUsingMainThreadMessageQueueForApple::singleton()
    {
      static SingletonLazySharedPtr<MessageQueueThreadUsingMainThreadMessageQueueForApple> singleton(MessageQueueThreadUsingMainThreadMessageQueueForApple::create());
      MessageQueueThreadUsingMainThreadMessageQueueForApplePtr result = singleton.singleton();
      if (!result) {
        ZS_LOG_WARNING(Detail, slog("singleton gone"))
      }
      return result;
    }

    //-------------------------------------------------------------------------
    MessageQueueThreadUsingMainThreadMessageQueueForApplePtr MessageQueueThreadUsingMainThreadMessageQueueForApple::create()
    {
      MessageQueueThreadUsingMainThreadMessageQueueForApplePtr thread(new MessageQueueThreadUsingMainThreadMessageQueueForApple);
      thread->mQueue = zsLib::MessageQueue::create(thread);
      thread->init();
      return thread;
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingMainThreadMessageQueueForApple::init()
    {
      //CFRunLoopSourceContext context = {0, this, NULL, NULL, NULL, NULL, NULL, &RunLoopSourceScheduleRoutine,RunLoopSourceCancelRoutine, RunLoopSourcePerformRoutine};
      mRunLoop = CFRunLoopGetMain();

      CFRunLoopSourceContext context = {0, this, NULL, NULL, NULL, NULL, NULL, NULL,NULL, &RunLoopSourcePerformRoutine};
      mProcessMessageLoopSource = CFRunLoopSourceCreate(NULL, 1, &context);
      if ( (NULL != mProcessMessageLoopSource) && CFRunLoopSourceIsValid(mProcessMessageLoopSource))
      {
        CFRunLoopAddSource(mRunLoop, mProcessMessageLoopSource, kCFRunLoopDefaultMode);

        if (!CFRunLoopContainsSource(mRunLoop, mProcessMessageLoopSource, kCFRunLoopDefaultMode))
        {
          ZS_THROW_CUSTOM(Exceptions::MessageQueueSourceNotAdded, "run loop input source is not added.")
        }
      }
      else
      {
        ZS_THROW_CUSTOM(Exceptions::MessageQueueSourceNotValid, "run loop input source is not valid.")
      }

      CFRunLoopSourceContext contextMoreMessages = {0, this, NULL, NULL, NULL, NULL, NULL, NULL,NULL, &MoreMessagesLoopSourcePerformRoutine};
      mMoreMessagesLoopSource = CFRunLoopSourceCreate(NULL, 1, &contextMoreMessages);
      if ( (NULL != mMoreMessagesLoopSource)  && CFRunLoopSourceIsValid(mMoreMessagesLoopSource))
      {
        CFRunLoopAddSource(mRunLoop, mMoreMessagesLoopSource, kCFRunLoopDefaultMode);

        if (!CFRunLoopContainsSource(mRunLoop, mMoreMessagesLoopSource, kCFRunLoopDefaultMode))
        {
          ZS_THROW_CUSTOM(Exceptions::MessageQueueSourceNotAdded, "run loop input source is not added.")
        }
      }
      else
      {
        ZS_THROW_CUSTOM(Exceptions::MessageQueueSourceNotValid, "run loop input source is not valid.")
      }

    }

    //-------------------------------------------------------------------------
    MessageQueueThreadUsingMainThreadMessageQueueForApple::MessageQueueThreadUsingMainThreadMessageQueueForApple() :
      mIsShutdown(false),
      mProcessMessageLoopSource(NULL),
      mMoreMessagesLoopSource(NULL)
    {
    }

    //-------------------------------------------------------------------------
    MessageQueueThreadUsingMainThreadMessageQueueForApple::~MessageQueueThreadUsingMainThreadMessageQueueForApple()
    {
      waitForShutdown();

      if (NULL != mProcessMessageLoopSource)
      {
        CFRunLoopSourceInvalidate(mProcessMessageLoopSource);
      }

      if (NULL != mMoreMessagesLoopSource)
      {
        CFRunLoopSourceInvalidate(mMoreMessagesLoopSource);
      }
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingMainThreadMessageQueueForApple::process()
    {
      //process only one message in que
      mQueue->processOnlyOneMessage();

      //if there are more messages to process signal it
      if (this->getTotalUnprocessedMessages() > 0)
      {
        if ( (NULL == mMoreMessagesLoopSource) || !CFRunLoopSourceIsValid(mMoreMessagesLoopSource))
        {
          ZS_THROW_CUSTOM(Exceptions::MessageQueueSourceNotValid, "run loop input source is not valid.")
        }
        else
        {
          CFRunLoopSourceSignal(mMoreMessagesLoopSource);
          CFRunLoopWakeUp(mRunLoop);
        }
      }

    }

    //-------------------------------------------------------------------------
    zsLib::Log::Params MessageQueueThreadUsingMainThreadMessageQueueForApple::slog(const char *message)
    {
      return zsLib::Log::Params(message, "MessageQueueThreadUsingMainThreadMessageQueueForApple");
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingMainThreadMessageQueueForApple::post(IMessageQueueMessagePtr message)
    {
      if (mIsShutdown) {
        ZS_THROW_CUSTOM(Exceptions::MessageQueueAlreadyDeleted, "message posted to message queue after message queue was deleted.")
      }
      mQueue->post(message);
    }

    //-------------------------------------------------------------------------
    IMessageQueue::size_type MessageQueueThreadUsingMainThreadMessageQueueForApple::getTotalUnprocessedMessages() const
    {
      AutoLock lock(mLock);
      return mQueue->getTotalUnprocessedMessages();
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingMainThreadMessageQueueForApple::notifyMessagePosted()
    {
      AutoLock lock(mLock);
      if ( (NULL == mProcessMessageLoopSource) || !CFRunLoopSourceIsValid(mProcessMessageLoopSource))
      {
        ZS_THROW_CUSTOM(Exceptions::MessageQueueSourceNotValid, "run loop input source is not valid.")
      }
      CFRunLoopSourceSignal(mProcessMessageLoopSource);
      CFRunLoopWakeUp(mRunLoop);
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingMainThreadMessageQueueForApple::waitForShutdown()
    {
      AutoLock lock(mLock);
      mQueue.reset();

      if (NULL != mProcessMessageLoopSource)
      {
        CFRunLoopSourceInvalidate(mProcessMessageLoopSource);
        mProcessMessageLoopSource = NULL;

      }

      if (NULL != mMoreMessagesLoopSource)
      {
        CFRunLoopSourceInvalidate(mMoreMessagesLoopSource);
        mMoreMessagesLoopSource = NULL;
      }

      mIsShutdown = true;
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingMainThreadMessageQueueForApple::setThreadPriority(ThreadPriorities threadPriority)
    {
      // no-op
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingMainThreadMessageQueueForApple::processMessagesFromThread()
    {
      mQueue->process();
    }

    //-------------------------------------------------------------------------
    void RunLoopSourcePerformRoutine (void *info)
    {
      MessageQueueThreadUsingMainThreadMessageQueueForApple* obj = (MessageQueueThreadUsingMainThreadMessageQueueForApple*)info;
      obj->process();
    }

    //-------------------------------------------------------------------------
    void MoreMessagesLoopSourcePerformRoutine (void *info)
    {
      MessageQueueThreadUsingMainThreadMessageQueueForApple* obj = (MessageQueueThreadUsingMainThreadMessageQueueForApple*)info;
      obj->notifyMessagePosted();
    }
  }
}

#endif //__APPLE__

