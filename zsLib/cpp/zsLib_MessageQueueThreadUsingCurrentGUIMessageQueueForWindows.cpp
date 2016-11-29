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

#if defined(_WIN32) && !defined(WINRT)

#include <zsLib/internal/zsLib_MessageQueueThreadUsingCurrentGUIMessageQueueForWindows.h>
#include <zsLib/internal/zsLib_MessageQueue.h>
#include <zsLib/Log.h>
#include <zsLib/helpers.h>
#include <zsLib/Stringize.h>
#include <zsLib/Singleton.h>

#include <Windows.h>
#include <tchar.h>

#pragma warning(disable: 4297)

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
  namespace internal
  {

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------

    static LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    class MessageQueueThreadUsingCurrentGUIMessageQueueForWindowsGlobal
    {
    public:
      MessageQueueThreadUsingCurrentGUIMessageQueueForWindowsGlobal() :
        mRegisteredWindowClass(0),
        mHiddenWindowClassName(_T("zsLibHiddenWindowe059928c0dab4631bdaeab09d5b25847")),
        mCustomMessageName(_T("zsLibCustomMessage3bbf9fd89d067b42860cc9074d64539f")),
        mModule(::GetModuleHandle(NULL))
      {
        WNDCLASS wndClass;
        memset(&wndClass, 0, sizeof(wndClass));

        wndClass.style = CS_HREDRAW | CS_VREDRAW;
        wndClass.lpfnWndProc = windowProc;
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hInstance = mModule;
        wndClass.hIcon = NULL;
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = mHiddenWindowClassName;

        mRegisteredWindowClass = ::RegisterClass(&wndClass);
        ZS_THROW_BAD_STATE_MSG_IF(0 == mRegisteredWindowClass, "RegisterClass failed with error: " + zsLib::string(::GetLastError()))

        mCustomMessage = ::RegisterWindowMessage(mCustomMessageName);
        ZS_THROW_BAD_STATE_IF(0 == mCustomMessage)
      }

      ~MessageQueueThreadUsingCurrentGUIMessageQueueForWindowsGlobal()
      {
        if (0 != mRegisteredWindowClass)
        {
          ZS_THROW_BAD_STATE_IF(0 == ::UnregisterClass(mHiddenWindowClassName, mModule))
          mRegisteredWindowClass = 0;
        }
      }

    public:
      HMODULE mModule;
      ATOM mRegisteredWindowClass;
      const TCHAR *mHiddenWindowClassName;
      const TCHAR *mCustomMessageName;
      UINT mCustomMessage;
    };

    //-------------------------------------------------------------------------
    static MessageQueueThreadUsingCurrentGUIMessageQueueForWindowsGlobal &getGlobal()
    {
      static MessageQueueThreadUsingCurrentGUIMessageQueueForWindowsGlobal global;
      return global;
    }

    //-------------------------------------------------------------------------
    MessageQueueThreadUsingCurrentGUIMessageQueueForWindowsPtr MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::singleton()
    {
      static SingletonLazySharedPtr<MessageQueueThreadUsingCurrentGUIMessageQueueForWindows> singleton(MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::create());
      return singleton.singleton();
    }

    //-------------------------------------------------------------------------
    MessageQueueThreadUsingCurrentGUIMessageQueueForWindowsPtr MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::create()
    {
      MessageQueueThreadUsingCurrentGUIMessageQueueForWindowsPtr thread(new MessageQueueThreadUsingCurrentGUIMessageQueueForWindows);
      thread->mQueue = MessageQueue::create(thread);
      thread->setup();
      return thread;
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::setup()
    {
      // make sure the window message queue was created by peeking a message
      MSG msg;
      memset(&msg, 0, sizeof(msg));
      ::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

      mHWND = ::CreateWindow(
        getGlobal().mHiddenWindowClassName,
        _T("zsLibHiddenWindow9127b0cb49457fdb969054c57cff6ed5efe771c0"),
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        getGlobal().mModule,
        NULL
      );
      ZS_THROW_BAD_STATE_IF(NULL == mHWND)
    }

    //-------------------------------------------------------------------------
    MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::MessageQueueThreadUsingCurrentGUIMessageQueueForWindows() :
      mHWND(NULL)
    {
    }

    //-------------------------------------------------------------------------
    MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::~MessageQueueThreadUsingCurrentGUIMessageQueueForWindows()
    {
      if (NULL != mHWND)
      {
        ZS_THROW_BAD_STATE_IF(0 == ::DestroyWindow(mHWND))
        mHWND = NULL;
      }
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
      AutoLock lock(mLock);
      if (NULL == mHWND) {
        ZS_THROW_CUSTOM(Exceptions::MessageQueueAlreadyDeleted, "message posted to message queue after message queue was deleted.")
      }

      ZS_THROW_BAD_STATE_IF(0 == ::PostMessage(mHWND, getGlobal().mCustomMessage, (WPARAM)0, (LPARAM)0))
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::waitForShutdown()
    {
      AutoLock lock(mLock);

      if (NULL != mHWND)
      {
        ZS_THROW_BAD_STATE_IF(0 == ::DestroyWindow(mHWND))
        mHWND = NULL;
      }

      mIsShutdown = true;
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::setThreadPriority(ThreadPriorities threadPriority)
    {
      // no-op
    }

    //-------------------------------------------------------------------------
    static LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
      if (getGlobal().mCustomMessage != uMsg)
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);

      MessageQueueThreadUsingCurrentGUIMessageQueueForWindowsPtr singleton = MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::singleton();
      singleton->process();
      return (LRESULT)0;
    }
  }
}

#endif //defined(_WIN32) && !defined(WINRT)
