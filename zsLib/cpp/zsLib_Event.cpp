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

#include <zsLib/Event.h>
#include <zsLib/helpers.h>
#include <zsLib/Exception.h>

//namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib)}

namespace zsLib
{
  namespace internal
  {
    //-------------------------------------------------------------------------
    Event::Event(bool manualReset) 
#ifndef _WIN32
    : mManualReset(manualReset)
#endif //ndef 
    {
#ifdef _WIN32
      mEvent = CreateEventEx(NULL, NULL, manualReset ? CREATE_EVENT_MANUAL_RESET : 0, EVENT_ALL_ACCESS);
#endif //_WIN32
    }

    //-------------------------------------------------------------------------
    Event::~Event()
    {
#ifdef _WIN32
      if (NULL != mEvent) {
        ::CloseHandle(mEvent);
        mEvent = NULL;
      }
#endif //_WIN32
    }

  }

  //---------------------------------------------------------------------------
  EventPtr Event::create(bool manualReset) {
    return make_shared<Event>(manualReset);
  }

  //---------------------------------------------------------------------------
  void Event::reset()
  {
#ifdef _WIN32
    if (NULL == mEvent) return;
    ::ResetEvent(mEvent);
#else
    mNotified = false;
#endif //_WIN32
  }

  //---------------------------------------------------------------------------
  void Event::wait()
  {
#ifdef _WIN32
    if (NULL == mEvent) return;
    ::WaitForSingleObjectEx(mEvent, INFINITE, FALSE);
#else
    std::unique_lock<std::mutex> lock(mMutex);

    if (mManualReset) {
      bool notified = mNotified;
      if (notified) return;
    }

    mCondition.wait(lock);
#endif //WIN32
  }

  //---------------------------------------------------------------------------
  void Event::notify()
  {
#ifdef _WIN32
    if (NULL == mEvent) return;
    ::SetEvent(mEvent);
#else
    std::lock_guard<std::mutex> lock(mMutex);
    mNotified = true;
    if (mManualReset) {
      mCondition.notify_all();
    } else {
      mCondition.notify_one();
    }
#endif //_WIN32
  }

}
