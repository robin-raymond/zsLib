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

#include <zsLib/MessageQueue.h>
#include <zsLib/Log.h>

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
  //---------------------------------------------------------------------------
  MessageQueue::MessageQueue(
                             const make_private &,
                             IMessageQueueNotifyPtr notify
                             ) : internal::MessageQueue(notify)
  {
  }

  //---------------------------------------------------------------------------
  MessageQueuePtr MessageQueue::create(IMessageQueueNotifyPtr notify)
  {
    return make_shared<MessageQueue>(make_private{}, notify);
  }

  //---------------------------------------------------------------------------
  void MessageQueue::post(IMessageQueueMessageUniPtr message)
  {
    {
      AutoLock lock(mLock);
      mMessages.push(std::move(message));
    }
    mNotify->notifyMessagePosted();
  }

  //---------------------------------------------------------------------------
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

      // process the next message
      message->processMessage();
    } while (true);
  }

  //---------------------------------------------------------------------------
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

    // process the next message
    message->processMessage();
  }

  //---------------------------------------------------------------------------
  IMessageQueue::size_type MessageQueue::getTotalUnprocessedMessages() const
  {
    AutoLock lock(mLock);
    return static_cast<size_type>(mMessages.size());
  }

}
