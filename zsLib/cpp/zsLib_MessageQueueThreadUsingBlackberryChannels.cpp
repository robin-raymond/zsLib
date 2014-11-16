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

#ifdef __QNX__

#include <zsLib/internal/zsLib_MessageQueueThreadUsingBlackberryChannels.h>
#include <zsLib/Log.h>
#include <zsLib/helpers.h>
#include <zsLib/Stringize.h>

#include <bps/bps.h>

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
  namespace internal
  {
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    #pragma mark
    #pragma mark MessageQueueThreadUsingBlackberryChannelsWrapper
    #pragma mark

    ZS_DECLARE_CLASS_PTR(MessageQueueThreadUsingBlackberryChannelsWrapper)

    class MessageQueueThreadUsingBlackberryChannelsWrapper
    {
    public:
      MessageQueueThreadUsingBlackberryChannelsWrapper() : mThreadQueue(MessageQueueThreadUsingBlackberryChannels::create())
      {
        mThreadQueue->setup();
      }
      ~MessageQueueThreadUsingBlackberryChannelsWrapper()
      {
        mThreadQueue->waitForShutdown();

        // See note above regarding bps_initialize.
        bps_shutdown();
      }

    public:
      MessageQueueThreadUsingBlackberryChannelsPtr mThreadQueue;
    };

    static pthread_once_t messageQueueKeyOnce = PTHREAD_ONCE_INIT;
    static pthread_key_t messageQueueKey;

    //-----------------------------------------------------------------------

    void messageQueueKeyDestructor(void* value) {
      MessageQueueThreadUsingBlackberryChannelsWrapperPtr *ptr = (MessageQueueThreadUsingBlackberryChannelsWrapperPtr *) value;
      (*ptr).reset();
      delete ptr;
      pthread_setspecific(messageQueueKey, NULL);
    }

    //-----------------------------------------------------------------------

    void makeMessageQueueKeyOnce() {
      pthread_key_create(&messageQueueKey, messageQueueKeyDestructor);
    }

    //-----------------------------------------------------------------------
    static MessageQueueThreadUsingBlackberryChannelsPtr getThreadMessageQueue()
    {
      pthread_once(&messageQueueKeyOnce, makeMessageQueueKeyOnce);

      if (!pthread_getspecific(messageQueueKey)) {
        MessageQueueThreadUsingBlackberryChannelsWrapperPtr * ptr = new MessageQueueThreadUsingBlackberryChannelsWrapperPtr(new MessageQueueThreadUsingBlackberryChannelsWrapper());
        pthread_setspecific(messageQueueKey, (void*) ptr);
      }

      MessageQueueThreadUsingBlackberryChannelsWrapperPtr *returnPtr = (MessageQueueThreadUsingBlackberryChannelsWrapperPtr *) pthread_getspecific(messageQueueKey);
      return (*returnPtr).get()->mThreadQueue;
    }

    //-----------------------------------------------------------------------
    MessageQueueThreadUsingBlackberryChannelsPtr MessageQueueThreadUsingBlackberryChannels::singleton()
    {
      return getThreadMessageQueue();
    }

    //-----------------------------------------------------------------------
    MessageQueueThreadUsingBlackberryChannelsPtr MessageQueueThreadUsingBlackberryChannels::create()
    {
      MessageQueueThreadUsingBlackberryChannelsPtr thread(new MessageQueueThreadUsingBlackberryChannels);
      thread->mQueue = zsLib::MessageQueue::create(thread);
      return thread;
    }

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    #pragma mark
    #pragma mark zsLib::internal::MessageQueueThreadUsingBlackberryChannels
    #pragma mark

    //-----------------------------------------------------------------------
    MessageQueueThreadUsingBlackberryChannels::MessageQueueThreadUsingBlackberryChannels()
    {
    }

    //-----------------------------------------------------------------------
    MessageQueueThreadUsingBlackberryChannels::~MessageQueueThreadUsingBlackberryChannels()
    {
    }

    //-----------------------------------------------------------------------
    void MessageQueueThreadUsingBlackberryChannels::setup()
    {
      // NOTE!!! Must be called for the main thread.
      mCrossThreadNotifier = IQtCrossThreadNotifier::createNotifier();
    }

    //-----------------------------------------------------------------------
    void MessageQueueThreadUsingBlackberryChannels::process()
    {
      MessageQueuePtr queue;

      {
        AutoLock lock(mLock);
        queue = mQueue;
        if (!mQueue)
          return;
      }

      queue->processOnlyOneMessage(); // process only one message at a time since this must be synchronized through the GUI message queue
    }

    //-----------------------------------------------------------------------
    void MessageQueueThreadUsingBlackberryChannels::post(IMessageQueueMessagePtr message)
    {
      MessageQueuePtr queue;
      {
        AutoLock lock(mLock);
        queue = mQueue;
        if (!queue) {
          ZS_THROW_CUSTOM(Exceptions::MessageQueueAlreadyDeleted, "message posted to message queue after message queue was deleted.")
        }
      }
      queue->post(message);
    }

    //-----------------------------------------------------------------------
    IMessageQueue::size_type MessageQueueThreadUsingBlackberryChannels::getTotalUnprocessedMessages() const
    {
      AutoLock lock(mLock);
      if (!mQueue)
        return 0;

      return mQueue->getTotalUnprocessedMessages();
    }

    //-----------------------------------------------------------------------
    void MessageQueueThreadUsingBlackberryChannels::notifyMessagePosted()
    {
      {
        AutoLock lock(mLock);
        mCrossThreadNotifier->setDelegate(singleton());
      }
      mCrossThreadNotifier->notifyMessagePosted();
    }

    //-----------------------------------------------------------------------
    void MessageQueueThreadUsingBlackberryChannels::waitForShutdown()
    {
      AutoLock lock(mLock);
      mQueue.reset();

      if (mCrossThreadNotifier)
      {
        mCrossThreadNotifier.reset();
      }
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingBlackberryChannels::setThreadPriority(ThreadPriorities threadPriority)
    {
      // no-op
    }

    //-----------------------------------------------------------------------
    void MessageQueueThreadUsingBlackberryChannels::processMessageFromThread()
    {
      MessageQueueThreadUsingBlackberryChannelsPtr queue(getThreadMessageQueue());
      queue->process();
    }
  }
}

#endif // __QNX__
