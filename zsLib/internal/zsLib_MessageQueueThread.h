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
