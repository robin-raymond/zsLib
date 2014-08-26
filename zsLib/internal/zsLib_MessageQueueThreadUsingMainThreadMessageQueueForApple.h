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

#pragma once

#ifdef __APPLE__

#ifndef ZSLIB_INTERNAL_MESSAGEQUEUETHREADUSINGMAINTHREADMESSAGEQUEUEFORAPPLE_H_e059928c0dab4631bdaeab09d5b25847
#define ZSLIB_INTERNAL_MESSAGEQUEUETHREADUSINGMAINTHREADMESSAGEQUEUEFORAPPLE_H_e059928c0dab4631bdaeab09d5b25847

#include <zsLib/MessageQueue.h>
#include <zsLib/MessageQueueThread.h>
#include <zsLib/Exception.h>

#include <CoreFoundation/CoreFoundation.h>

namespace zsLib
{
  namespace internal
  {
    ZS_DECLARE_CLASS_PTR(MessageQueueThreadUsingMainThreadMessageQueueForApple)

    class MessageQueueThreadUsingMainThreadMessageQueueForApple : public MessageQueueThread, public IMessageQueueNotify
    {
    public:
      struct Exceptions
      {
        ZS_DECLARE_CUSTOM_EXCEPTION(MessageQueueAlreadyDeleted);
        ZS_DECLARE_CUSTOM_EXCEPTION(MessageQueueSourceNotAdded);
        ZS_DECLARE_CUSTOM_EXCEPTION(MessageQueueSourceNotValid);
        //ZS_DECLARE_CUSTOM_EXCEPTION(MessageQueueSourceAlreadyDeleted);
      };

    protected:
      MessageQueueThreadUsingMainThreadMessageQueueForApple();
      static MessageQueueThreadUsingMainThreadMessageQueueForApplePtr create();

      void init();

    public:
      ~MessageQueueThreadUsingMainThreadMessageQueueForApple();

      static MessageQueueThreadUsingMainThreadMessageQueueForApplePtr singleton();

      // IMessageQueue
      virtual void post(IMessageQueueMessagePtr message);

      virtual size_type getTotalUnprocessedMessages() const;

      // IMessageQueueNotify
      virtual void notifyMessagePosted();

      // IMessageQueueThread
      virtual void waitForShutdown();

      virtual void setThreadPriority(ThreadPriorities threadPriority);

      virtual void processMessagesFromThread();

    public:
      virtual void process();

    protected:
      static zsLib::Log::Params slog(const char *message);

    protected:
      mutable Lock mLock;

      MessageQueuePtr mQueue;

      CFRunLoopRef mRunLoop;
      CFRunLoopSourceRef mProcessMessageLoopSource;
      CFRunLoopSourceRef mMoreMessagesLoopSource;

      volatile bool mIsShutdown;
    };
  }
}

#endif //ZSLIB_INTERNAL_MESSAGEQUEUETHREADUSINGMAINTHREADMESSAGEQUEUEFORAPPLE_H_e059928c0dab4631bdaeab09d5b25847

#endif //__APPLE__
