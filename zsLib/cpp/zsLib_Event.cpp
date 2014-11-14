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

namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib)}

namespace zsLib
{
  namespace internal
  {
    Event::Event() :
      mNotified(0)
    {
#ifdef __QNX__
      static pthread_cond_t gConditionInit = PTHREAD_COND_INITIALIZER;
      static pthread_mutex_t gMutexInit = PTHREAD_MUTEX_INITIALIZER;

      mCondition = gConditionInit;
      mMutex = gMutexInit;
#endif //__QNX__
    }

    Event::~Event()
    {
#ifdef __QNX__
      pthread_cond_destroy(&mCondition);
      pthread_mutex_destroy(&mMutex);
#endif //__QNX__
    }

  }

  EventPtr Event::create() {
    return EventPtr(new Event);
  }

  void Event::reset()
  {
    zsLib::atomicSetValue32(mNotified, 0);
  }

  void Event::wait()
  {
    DWORD notified = zsLib::atomicGetValue32(mNotified);
    if (0 != notified) {
      return;
    }

#ifdef __QNX__
    int result = pthread_mutex_lock(&mMutex);
    ZS_THROW_BAD_STATE_IF(0 != result)

    notified = zsLib::atomicGetValue32(mNotified);
    if (0 != notified) {
      result = pthread_mutex_unlock(&mMutex);
      ZS_THROW_BAD_STATE_IF(0 != result)
      return;
    }

    result = pthread_cond_wait(&mCondition, &mMutex);
    ZS_THROW_BAD_STATE_IF(0 != result)

    result = pthread_mutex_unlock(&mMutex);
    ZS_THROW_BAD_STATE_IF(0 != result)
#else
    boost::unique_lock<boost::mutex> lock(mMutex);
    notified = zsLib::atomicGetValue32(mNotified);
    if (0 != notified) return;
    mCondition.wait(lock);
#endif //__QNX__
  }

  void Event::notify()
  {
#ifdef __QNX__
    int result = pthread_mutex_lock(&mMutex);
    ZS_THROW_BAD_STATE_IF(0 != result)

    result = pthread_cond_signal(&mCondition);
    ZS_THROW_BAD_STATE_IF(0 != result)

    zsLib::atomicSetValue32(mNotified, 1);

    result = pthread_mutex_unlock(&mMutex);
    ZS_THROW_BAD_STATE_IF(0 != result)
#else
    boost::lock_guard<boost::mutex> lock(mMutex);
    zsLib::atomicSetValue32(mNotified, 1);
    mCondition.notify_one();
#endif //__QNX__
  }

}
