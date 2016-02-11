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

#ifndef ZSLIB_MESSAGEQUEUE_H_3b3e04ed0435a1db72915c7519694f0f
#define ZSLIB_MESSAGEQUEUE_H_3b3e04ed0435a1db72915c7519694f0f

#include <zsLib/types.h>
#include <zsLib/Exception.h>

namespace zsLib
{
  interaction IMessageQueueMessage
  {
    virtual const char *getDelegateName() const = 0;
    virtual const char *getMethodName() const = 0;

    virtual void processMessage() = 0;

    virtual ~IMessageQueueMessage() {}
  };

  interaction IMessageQueueNotify
  {
    virtual void notifyMessagePosted() = 0;
  };

  interaction IMessageQueue
  {
    struct Exceptions
    {
      ZS_DECLARE_CUSTOM_EXCEPTION(MessageQueueGone)
    };

    typedef size_t size_type;

    virtual void post(IMessageQueueMessageUniPtr message) = 0;

    virtual size_type getTotalUnprocessedMessages() const = 0;
  };
}

#include <zsLib/internal/zsLib_MessageQueue.h>

namespace zsLib
{
  class MessageQueue : public internal::MessageQueue
  {
  public:
    MessageQueue(
                 const make_private &,
                 IMessageQueueNotifyPtr notify
                 );
    ~MessageQueue();

  public:
    static MessageQueuePtr create(IMessageQueueNotifyPtr notify);

    virtual void post(IMessageQueueMessageUniPtr message);

    virtual size_type getTotalUnprocessedMessages() const;

    void process();
    void processOnlyOneMessage();
  };
}

#endif //ZSLIB_MESSAGEQUEUE_H_3b3e04ed0435a1db72915c7519694f0f
