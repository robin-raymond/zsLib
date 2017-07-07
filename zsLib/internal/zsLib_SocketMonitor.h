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

#include <zsLib/types.h>
#include <zsLib/Socket.h>
#include <zsLib/IPAddress.h>
#include <zsLib/Event.h>
#include <zsLib/Log.h>
#include <zsLib/Singleton.h>

#include <unordered_map>
#include <map>
#include <set>

#ifndef _WIN32
#include <sys/poll.h>
#else
typedef size_t nfds_t;
#endif //ndef _WIN32

#define ZSLIB_SETTING_SOCKET_MONITOR_THREAD_PRIORITY "zsLib/socket-monitor/thread-priority"

namespace zsLib
{
  ZS_DECLARE_CLASS_PTR(Socket)

  namespace internal
  {
    ZS_DECLARE_CLASS_PTR(SocketMonitorLoadBalancer);
    ZS_DECLARE_CLASS_PTR(SocketMonitor);

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

#ifndef _WIN32
      struct tagWSAEVENT {};
      typedef tagWSAEVENT * WSAEVENT;
#endif //ndef _WIN32

      typedef WSAEVENT EventHandle;

      ZS_DECLARE_STRUCT_PTR(EventHandleHolder);
      struct EventHandleHolder {
        EventHandleHolder(EventHandle event);
        ~EventHandleHolder();

        EventHandle mEventHandle;
      };

      typedef ::pollfd poll_fd;
      typedef ::nfds_t poll_size;
      typedef decltype(poll_fd::events) event_type;

      typedef std::map<SOCKET, poll_size> SocketIndexMap;

      typedef std::pair<SocketPtr, event_type> FiredEventPair;

    public:
      SocketSet();
      ~SocketSet();

      poll_fd *preparePollingFDs(poll_size &outSize, EventHandle * &outEvents);

      void firedEvent(
                      SocketPtr socket,
                      event_type event
                      );

      FiredEventPair *getFiredEvents(poll_size &outSize);

      void delegateGone(SocketPtr socket);
      SocketPtr *getSocketsWithDelegateGone(poll_size &outSize);

      void clear();

      bool isDirty() const {return mDirty;}

#ifdef _WIN32
      void setWakeUpEvent(HANDLE handle);
#endif //_WIN32

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

#ifdef _WIN32
      static long toNetworkEvents(event_type events);
#endif //_WIN32

      zsLib::Log::Params log(const char *message) const;

    private:
      AutoPUID mID;
      poll_size mOfficialAllocationSize {0};
      poll_size mOfficialCount {0};
      poll_fd *mOfficialSet {NULL};
      EventHandle *mOfficialHandleSet {NULL};
      EventHandleHolderPtr *mOfficialHandleHolderSet {NULL};

      poll_size mPollingAllocationSize {0};
      poll_size mPollingCount {0};
      poll_fd *mPollingSet {NULL};
      EventHandle *mPollingHandleSet {NULL};
      EventHandleHolderPtr *mPollingHandleHolderSet {NULL};

      poll_size mPollingFiredEventCount {0};
      FiredEventPair *mPollingFiredEvents {NULL};

      poll_size mPollingSocketsWithDelegateGoneCount {0};
      SocketPtr *mPollingSocketsWithDelegateGone {NULL};

      SocketIndexMap mSocketIndexes;

      bool mDirty {true};
    };

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark SocketMonitor
    #pragma mark

    class SocketMonitor
    {
    public:
      ZS_DECLARE_TYPEDEF_PTR(zsLib::XML::Element, Element);
      ZS_DECLARE_TYPEDEF_PTR(zsLib::XML::Text, Text);

      friend class SocketMonitorLoadBalancer;

      typedef SocketSet::event_type event_type;
      typedef SocketSet::poll_size poll_size;
      typedef SocketSet::poll_fd poll_fd;
      typedef SocketSet::FiredEventPair FiredEventPair;
      typedef SocketSet::EventHandle EventHandle;

    protected:
      SocketMonitor();
      SocketMonitor(const SocketMonitor &) = delete;

      static SocketMonitorPtr create();

    public:
      ~SocketMonitor();
      static SocketMonitorPtr link();

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

    protected:
      void shutdown();
      PUID getID() const { return mID; }

    private:
      void cancel();

      void processWaiting();
      void wakeUp();
      void createWakeUpSocket();
      void cleanWakeUpSocket();

      zsLib::Log::Params log(const char *message) const;
      static zsLib::Log::Params slog(const char *message);

    private:
      AutoPUID mID;
      RecursiveLock mLock;
      SocketMonitorWeakPtr mThisWeak;
      SocketMonitorPtr mGracefulReference;

      ThreadPtr mThread;
      std::atomic<bool> mShouldShutdown {};
      typedef std::unordered_map<SOCKET, SocketWeakPtr> SocketMap;
      SocketMap mMonitoredSockets;

      typedef std::list<zsLib::EventPtr> EventList;
      EventList mWaitingForRebuildList;

#ifdef _WIN32
      HANDLE mWakeupEvent {};
#else
      IPAddress mWakeUpAddress;
      SocketPtr mWakeUpSocket;
#endif //_WIN32

      SocketSet mSocketSet;
    };
  }
}
