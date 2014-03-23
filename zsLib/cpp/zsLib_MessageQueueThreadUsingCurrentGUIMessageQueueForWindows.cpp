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

#ifdef _WIN32

#include <zsLib/internal/MessageQueueThreadUsingCurrentGUIMessageQueueForWindows.h>
#include <zsLib/Log.h>
#include <zsLib/helpers.h>
#include <zsLib/Stringize.h>

#include <boost/ref.hpp>
#include <boost/thread.hpp>
#include <boost/thread.hpp>

#include <tchar.h>

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
      thread->mQueue = zsLib::MessageQueue::create(thread);
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
      mHWND(NULL),
      mIsShutdown(false)
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
      mQueue->processOnlyOneMessage(); // process only one messsage at a time since this must be syncrhonized through the GUI message queue
    }

    //-------------------------------------------------------------------------
    void MessageQueueThreadUsingCurrentGUIMessageQueueForWindows::post(IMessageQueueMessagePtr message)
    {
      if (mIsShutdown) {
        ZS_THROW_CUSTOM(Exceptions::MessageQueueAlreadyDeleted, "message posted to message queue after message queue was deleted.")
      }
      mQueue->post(message);
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

#endif //_WIN32
