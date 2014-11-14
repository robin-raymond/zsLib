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

#ifndef ZSLIB_INTERNAL_MESSAGEQUEUETHREAD_H_5d1955ad9e4c1689e30f9affd5ea319e
#define ZSLIB_INTERNAL_MESSAGEQUEUETHREAD_H_5d1955ad9e4c1689e30f9affd5ea319e

#include <zsLib/MessageQueue.h>
#include <boost/thread.hpp>

namespace zsLib
{
  namespace internal
  {
    void setThreadPriority(
                           Thread::native_handle_type handle,
                           ThreadPriorities threadPriority
                           );

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark MonitorPriorityHelper
    #pragma mark

    class MonitorPriorityHelper
    {
    public:
      //-----------------------------------------------------------------------
      MonitorPriorityHelper() :
        mPriority(ThreadPriority_NormalPriority)
      {
      }

      //-----------------------------------------------------------------------
      ThreadPriorities getPriority() const
      {
        AutoRecursiveLock lock(mLock);
        return mPriority;
      }

      //-----------------------------------------------------------------------
      bool setPriority(ThreadPriorities priority)
      {
        AutoRecursiveLock lock(mLock);
        if (priority == mPriority) return false;

        mPriority = priority;
        return true;
      }

      //-----------------------------------------------------------------------
      bool wasNotified() const
      {
        AutoRecursiveLock lock(mLock);
        return mNotified;
      }

      //-----------------------------------------------------------------------
      void notify()
      {
        AutoRecursiveLock lock(mLock);
        mNotified = true;
      }

    private:

      mutable RecursiveLock mLock;
      ThreadPriorities mPriority;
      bool mNotified {};
    };
    
  }
}

#endif //ZSLIB_INTERNAL_MESSAGEQUEUETHREAD_H_5d1955ad9e4c1689e30f9affd5ea319e
