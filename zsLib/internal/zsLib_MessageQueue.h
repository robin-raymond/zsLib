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

#ifndef ZSLIB_INTERNAL_MESSAGEQUEUE_H_9169ff56fd856a4d060fe12816e81a5e
#define ZSLIB_INTERNAL_MESSAGEQUEUE_H_9169ff56fd856a4d060fe12816e81a5e

#include <zsLib/types.h>
#include <zsLib/IMessageQueue.h>

#include <queue>

namespace zsLib
{
  namespace internal
  {
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark 
    #pragma mark MessageQueue
    #pragma mark 

    class MessageQueue : public IMessageQueue
    {
    protected:
      struct make_private {};
      friend interaction IMessageQueue;

    public:
      MessageQueue(
                   const make_private &,
                   IMessageQueueNotifyPtr notify
                   );
      ~MessageQueue();

    public:
      //-----------------------------------------------------------------------
      #pragma mark 
      #pragma mark MessageQueue => IMessageQueue
      #pragma mark 

      static MessageQueuePtr create(IMessageQueueNotifyPtr notify);

      virtual void post(IMessageQueueMessageUniPtr message) override;

      virtual size_type getTotalUnprocessedMessages() const override;

    public:
      //-----------------------------------------------------------------------
      #pragma mark 
      #pragma mark MessageQueue => (friends)
      #pragma mark 

      void process();
      void processOnlyOneMessage();

    protected:
      //-----------------------------------------------------------------------
      #pragma mark 
      #pragma mark MessageQueue => (data)
      #pragma mark 

      mutable Lock mLock;
      std::queue<IMessageQueueMessageUniPtr> mMessages;
      IMessageQueueNotifyPtr mNotify;
    };
  }
}

#endif //ZSLIB_INTERNAL_MESSAGEQUEUE_H_9169ff56fd856a4d060fe12816e81a5e
