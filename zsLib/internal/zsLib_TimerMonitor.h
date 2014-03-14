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

#ifndef ZSLIB_INTERNAL_SOCKETMONITOR_H_c01514fd3a9af7d11f32093baae8c546
#define ZSLIB_INTERNAL_SOCKETMONITOR_H_c01514fd3a9af7d11f32093baae8c546

#include <zsLib/types.h>
#include <zsLib/Log.h>
#include <zsLib/MessageQueueThread.h>

#include <boost/noncopyable.hpp>
#include <map>
#include <list>

#ifdef __QNX__
#include <pthread.h>
#endif //__QNX__


namespace zsLib
{
  class Timer;
  typedef boost::shared_ptr<Timer> TimerPtr;
  typedef boost::weak_ptr<Timer> TimerWeakPtr;

  namespace internal
  {
    class TimerMonitor;
    typedef boost::shared_ptr<TimerMonitor> TimerMonitorPtr;
    typedef boost::weak_ptr<TimerMonitor> TimerMonitorWeakPtr;

    class TimerMonitor : public boost::noncopyable
    {
    public:
      ZS_DECLARE_TYPEDEF_PTR(zsLib::XML::Element, Element)
      ZS_DECLARE_TYPEDEF_PTR(zsLib::XML::Text, Text)

    protected:
      TimerMonitor();

      void init();

    public:
      ~TimerMonitor();

      static TimerMonitorPtr singleton();
      static TimerMonitorPtr create();

      static void setPriority(ThreadPriorities priority);

      void monitorBegin(TimerPtr timer);
      void monitorEnd(zsLib::Timer &timer);

      void operator()();

    private:
      zsLib::Log::Params log(const char *message) const;
      static zsLib::Log::Params slog(const char *message);

      void cancel();

      Duration fireTimers();
      void wakeUp();

    private:
      AutoPUID mID;

      RecursiveLock mLock;
      Lock mFlagLock;
      boost::condition_variable mFlagNotify;

      TimerMonitorWeakPtr mThisWeak;
      TimerMonitorPtr mGracefulShutdownReference;

      ThreadPtr mThread;
      bool mShouldShutdown;

      typedef std::map<PUID, TimerWeakPtr> TimerMap;

      TimerMap mMonitoredTimers;

#ifdef __QNX__
      pthread_cond_t      mCondition;
      pthread_mutex_t     mMutex;
#endif //__QNX__
    };
  }
}

#endif //ZSLIB_INTERNAL_SOCKETMONITOR_H_c01514fd3a9af7d11f32093baae8c546
