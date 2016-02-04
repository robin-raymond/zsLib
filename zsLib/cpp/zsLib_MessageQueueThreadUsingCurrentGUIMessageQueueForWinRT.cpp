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

#include <zsLib/types.h>

#ifdef WINRT

#include <zsLib/internal/zsLib_MessageQueueThreadUsingCurrentGUIMessageQueueForWinRT.h>
#include <zsLib/internal/zsLib_MessageQueueThreadBasic.h>
#include <zsLib/Log.h>
#include <zsLib/helpers.h>
#include <zsLib/Stringize.h>
#include <zsLib/Singleton.h>

#include <Windows.h>
#include <tchar.h>

using namespace Windows::UI::Core;

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
  namespace internal
  {

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------


    //-------------------------------------------------------------------------
    MessageQueueThreadPtr MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::singleton()
    {
      CoreDispatcher ^dispatcher = nullptr;
      if (nullptr != Windows::ApplicationModel::Core::CoreApplication::MainView) {
        dispatcher = Windows::ApplicationModel::Core::CoreApplication::MainView->Dispatcher;
      }
      if (!dispatcher) {
        static SingletonLazySharedPtr<MessageQueueThreadBasic> singleton(MessageQueueThreadBasic::create("zsLib.winrt.backgroundDispatcher"));
        return singleton.singleton();
      }

      static SingletonLazySharedPtr<MessageQueueThreadUsingCurrentGUIMessageQueueForWindows> singleton(MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::create(dispatcher));
      return singleton.singleton();
    }

    //-------------------------------------------------------------------------
    MessageQueueThreadUsingCurrentGUIMessageQueueForWindowsPtr MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::create(CoreDispatcher ^dispatcher)
    {
      MessageQueueThreadUsingCurrentGUIMessageQueueForWindowsPtr thread(new MessageQueueThreadUsingCurrentGUIMessageQueueForWindows);
      thread->mQueue = zsLib::MessageQueue::create(thread);
      thread->mDispatcher = dispatcher;
      assert(thread->mDispatcher);
      return thread;
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::dispatch(MessageQueueThreadUsingCurrentGUIMessageQueueForWindowsPtr queue)
    {
      queue->process();
    }

    //-------------------------------------------------------------------------
    MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::MessageQueueThreadUsingCurrentGUIMessageQueueForWindows()
    {
    }

    //-------------------------------------------------------------------------
    MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::~MessageQueueThreadUsingCurrentGUIMessageQueueForWindows()
    {
      mThisWeak.reset();
      mDispatcher = nullptr;
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::process()
    {
      mQueue->processOnlyOneMessage(); // process only one message at a time since this must be syncrhonized through the GUI message queue
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::processMessagesFromThread()
    {
      mQueue->process();
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::post(IMessageQueueMessageUniPtr message)
    {
      if (mIsShutdown) {
        ZS_THROW_CUSTOM(Exceptions::MessageQueueAlreadyDeleted, "message posted to message queue after message queue was deleted.")
      }
      mQueue->post(std::move(message));
    }

    //-------------------------------------------------------------------------
    IMessageQueue::size_type MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::getTotalUnprocessedMessages() const
    {
      return mQueue->getTotalUnprocessedMessages();
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::notifyMessagePosted()
    {
      CoreDispatcher ^dispatcher;
      MessageQueueThreadUsingCurrentGUIMessageQueueForWindowsPtr queue;

      {
        AutoLock lock(mLock);

        if (nullptr == mDispatcher) {
          ZS_THROW_CUSTOM(Exceptions::MessageQueueAlreadyDeleted, "message posted to message queue after message queue was deleted.")
        }
        dispatcher = mDispatcher;
        queue = mThisWeak.lock();
      }

      auto callback = [queue] () {dispatch(queue);};

      dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler(callback)
      );
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::waitForShutdown()
    {
      mIsShutdown = true;
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::setThreadPriority(ThreadPriorities threadPriority)
    {
      // no-op
    }
  }
}

#endif //WINRT
