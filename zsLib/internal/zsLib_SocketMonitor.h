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
#include <zsLib/Socket.h>
#include <zsLib/IPAddress.h>
#include <zsLib/Event.h>
#include <zsLib/Log.h>

#include <boost/noncopyable.hpp>
#include <map>
#include <set>

#ifndef _WIN32
#include <sys/poll.h>
#endif //ndef _WIN32

namespace zsLib
{
  ZS_DECLARE_CLASS_PTR(Socket)

  namespace internal
  {
    ZS_DECLARE_CLASS_PTR(SocketMonitor)

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark SocketSet
    #pragma mark

    class SocketSet
    {
    public:
      ZS_DECLARE_TYPEDEF_PTR(zsLib::XML::Element, Element)
      ZS_DECLARE_TYPEDEF_PTR(zsLib::XML::Text, Text)

      typedef ::pollfd poll_fd;
      typedef ::nfds_t poll_size;
      typedef decltype(poll_fd::events) event_type;

      typedef std::map<SOCKET, poll_size> SocketIndexMap;

      typedef std::pair<SocketPtr, event_type> FiredEventPair;

    public:
      SocketSet();
      ~SocketSet();

      poll_fd *preparePollingFDs(poll_size &outSize);

      void firedEvent(
                      SocketPtr socket,
                      event_type event
                      );

      FiredEventPair *getFiredEvents(poll_size &outSize);

      void delegateGone(SocketPtr socket);
      SocketPtr *getSocketsWithDelegateGone(poll_size &outSize);

      void clear();

      bool isDirty() const {return mDirty;}

      void reset(SOCKET socket);
      void reset(
                 SOCKET socket,
                 event_type events
                 );

      void addEvents(
                     SOCKET socket,
                     event_type events
                     );
      void removeEvents(
                        SOCKET socket,
                        event_type events
                        );

    protected:
      void minOfficialAllocation(poll_size minSize);
      void minPollingAllocation(poll_size minSize);

      void append(
                  SOCKET socket,
                  event_type events
                  );

      zsLib::Log::Params log(const char *message) const;

    private:
      AutoPUID mID;
      poll_size mOfficialAllocationSize;
      poll_size mOfficialCount;
      poll_fd *mOfficialSet;

      poll_size mPollingAllocationSize;
      poll_size mPollingCount;
      poll_fd *mPollingSet;

      poll_size mPollingFiredEventCount;
      FiredEventPair *mPollingFiredEvents;

      poll_size mPollingSocketsWithDelegateGoneCount;
      SocketPtr *mPollingSocketsWithDelegateGone;

      SocketIndexMap mSocketIndexes;

      bool mDirty;
    };

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark SocketMonitor
    #pragma mark

    class SocketMonitor : public boost::noncopyable
    {
    public:
      ZS_DECLARE_TYPEDEF_PTR(zsLib::XML::Element, Element)
      ZS_DECLARE_TYPEDEF_PTR(zsLib::XML::Text, Text)

      typedef SocketSet::event_type event_type;
      typedef SocketSet::poll_size poll_size;
      typedef SocketSet::poll_fd poll_fd;
      typedef SocketSet::FiredEventPair FiredEventPair;

    protected:
      SocketMonitor();

      static SocketMonitorPtr create();

    public:
      ~SocketMonitor();
      static SocketMonitorPtr singleton();

      void monitorBegin(
                        SocketPtr socket,
                        bool monitorRead,
                        bool monitorWrite,
                        bool monitorException
                        );
      void monitorEnd(zsLib::Socket &socket);

      void monitorRead(const zsLib::Socket &socket);
      void monitorWrite(const zsLib::Socket &socket);
      void monitorException(const zsLib::Socket &socket);

      void operator()();

    private:
      void cancel();

      void processWaiting();
      void wakeUp();
      void createWakeUpSocket();

      zsLib::Log::Params log(const char *message) const;
      static zsLib::Log::Params slog(const char *message);

    private:
      AutoPUID mID;
      RecursiveLock mLock;
      SocketMonitorWeakPtr mThisWeak;
      SocketMonitorPtr mGracefulReference;

      ThreadPtr mThread;
      bool mShouldShutdown;
      typedef std::map<SOCKET, SocketWeakPtr> SocketMap;
      SocketMap mMonitoredSockets;

      typedef std::list<zsLib::EventPtr> EventList;
      EventList mWaitingForRebuildList;

      IPAddress mWakeUpAddress;
      SocketPtr mWakeUpSocket;

      SocketSet mSocketSet;
    };
  }
}

#endif //ZSLIB_INTERNAL_SOCKETMONITOR_H_c01514fd3a9af7d11f32093baae8c546
