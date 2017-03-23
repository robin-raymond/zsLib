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

#include <zsLib/internal/zsLib_MessageQueue.h>

#include <zsLib/Log.h>

#ifndef ZSLIB_EVENTING_NOOP
#include <zsLib/internal/zsLib.events.h>

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }
#else
#include <zsLib/eventing/noop.h>
#endif //ndef ZSLIB_EVENTING_NOOP

namespace zsLib
{
  ZS_EVENTING_TASK(MessageQueue);

  namespace internal
  {
    //-------------------------------------------------------------------------
    MessageQueue::MessageQueue(
                               const make_private &,
                               IMessageQueueNotifyPtr notify
                               ) :
      mNotify(notify)
    {
      ZS_EVENTING_1(x, i, Trace, MessageQueueCreate, zs, MessageQueue, Start, this, this, this);
    }

    //-------------------------------------------------------------------------
    MessageQueue::~MessageQueue()
    {
      ZS_EVENTING_1(x, i, Trace, MessageQueueDestroy, zs, MessageQueue, Stop, this, this, this);
    }

    //-------------------------------------------------------------------------
    MessageQueuePtr MessageQueue::create(IMessageQueueNotifyPtr notify)
    {
      return make_shared<MessageQueue>(make_private{}, notify);
    }

    //-------------------------------------------------------------------------
    void MessageQueue::post(IMessageQueueMessageUniPtr message)
    {
      ZS_EVENTING_1(x, i, Insane, MessageQueuePost, zs, MessageQueue, Send, this, this, this);

      {
        AutoLock lock(mLock);
        mMessages.push(std::move(message));
      }
      mNotify->notifyMessagePosted();
    }

    //-------------------------------------------------------------------------
    void MessageQueue::process()
    {
      do
      {
        IMessageQueueMessagePtr message;

        {
          AutoLock lock(mLock);
          if (0 == mMessages.size())
            return;
          message = std::move(mMessages.front());
          mMessages.pop();
        }

        ZS_EVENTING_1(x, i, Insane, MessageQueueProcess, zs, MessageQueue, Receive, this, this, this);

        // process the next message
        message->processMessage();
      } while (true);
    }

    //-------------------------------------------------------------------------
    void MessageQueue::processOnlyOneMessage()
    {
      IMessageQueueMessagePtr message;

      {
        AutoLock lock(mLock);
        if (0 == mMessages.size())
          return;
        message = std::move(mMessages.front());
        mMessages.pop();
      }

      ZS_EVENTING_1(x, i, Insane, MessageQueueProcess, zs, MessageQueue, Receive, this, this, this);

      // process the next message
      message->processMessage();
    }

    //-------------------------------------------------------------------------
    IMessageQueue::size_type MessageQueue::getTotalUnprocessedMessages() const
    {
      AutoLock lock(mLock);
      ZS_EVENTING_2(x, i, Insane, MessageQueueTotalUnprocessedMessages, zs, MessageQueue, Info, this, this, this, size_t, messages, mMessages.size());
      return static_cast<size_type>(mMessages.size());
    }
  } // namespace internal

  //---------------------------------------------------------------------------
  IMessageQueuePtr IMessageQueue::create(IMessageQueueNotifyPtr notify)
  {
    return internal::MessageQueue::create(notify);
  }

} // namespace zsLib

