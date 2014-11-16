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

#include <zsLib/internal/zsLib_SocketMonitor.h>
#include <zsLib/Stringize.h>
#include <zsLib/helpers.h>
#include <zsLib/XML.h>

#include <stdlib.h>

#define ZSLIB_SOCKET_MONITOR_TIMEOUT_IN_MILLISECONDS (10*(1000))

namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib_socket)}

namespace zsLib
{
  namespace internal
  {
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark (helpers)
    #pragma mark

    static String friendly(SocketSet::event_type events)
    {
      char positions[7];

      memset(&(positions[0]), '-', sizeof(positions));
      positions[6] = '\0';

      String result;

      if ((events & POLLRDNORM) != 0) {
        positions[0] = 'R';
      }
      if ((events & POLLWRNORM) != 0) {
        positions[1] = 'W';
      }
      if ((events & POLLERR) != 0) {
        positions[2] = 'E';
      }
      if ((events & POLLHUP) != 0) {
        positions[3] = 'H';
      }
      if ((events & POLLNVAL) != 0) {
        positions[4] = 'I';
      }

      if ((events & (~(POLLRDNORM | POLLWRNORM | POLLHUP | POLLNVAL))) != 0) {
        positions[5] = 'O';
      }

      return String((CSTR)(&(positions[0])));
    }

    //-------------------------------------------------------------------------
    static MonitorPriorityHelper &getSocketMonitorPrioritySingleton()
    {
      static Singleton<MonitorPriorityHelper, false> singleton;
      return singleton.singleton();
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark SocketSet
    #pragma mark

    //-------------------------------------------------------------------------
    SocketSet::SocketSet() :
      mOfficialAllocationSize(0),
      mOfficialCount(0),
      mOfficialSet(NULL),
      mPollingAllocationSize(0),
      mPollingCount(0),
      mPollingFiredEventCount(0),
      mPollingSet(NULL),
      mPollingFiredEvents(NULL),
      mDirty(true),
      mPollingSocketsWithDelegateGoneCount(0),
      mPollingSocketsWithDelegateGone(NULL)
    {
      ZS_LOG_BASIC(log("created"))
    }

    //-------------------------------------------------------------------------
    SocketSet::~SocketSet()
    {
      ZS_LOG_BASIC(log("destroyed"))

      delete [] mOfficialSet;
      delete [] mPollingSet;
      delete [] mPollingFiredEvents;
      delete [] mPollingSocketsWithDelegateGone;

      mOfficialAllocationSize = 0;
      mOfficialCount = 0;

      mPollingAllocationSize = 0;
      mPollingCount = 0;

      mPollingFiredEventCount = 0;

      mPollingSocketsWithDelegateGoneCount = 0;
    }

    //-------------------------------------------------------------------------
    SocketSet::poll_fd *SocketSet::preparePollingFDs(poll_size &outSize)
    {
      outSize = mOfficialCount;

      // scope: clean out previous sockets
      {
        for (poll_size index = 0; index < mPollingFiredEventCount; ++index) {
          mPollingFiredEvents[index].first.reset();
        }
        mPollingFiredEventCount = 0;
      }

      // scope: clean out sockets that had delegates gone
      {
        for (poll_size index = 0; index < mPollingSocketsWithDelegateGoneCount; ++index) {
          mPollingSocketsWithDelegateGone[index].reset();
        }
        mPollingSocketsWithDelegateGoneCount = 0;
      }

      if (!mDirty) {
        for (poll_size index = 0; index < mPollingCount; ++index) {
          mPollingSet[index].revents = 0; // reset events
        }
        return mPollingSet;
      }

      minPollingAllocation(mOfficialCount > 0 ? mOfficialCount : 1);

      mPollingCount = mOfficialCount;

      if (mOfficialCount > 0) {
        memcpy(mPollingSet, mOfficialSet, sizeof(poll_fd) * mOfficialCount);
      }

      mDirty = false;

      ZS_LOG_INSANE(log("preparing polling") + ZS_PARAM("count", mOfficialCount) + ZS_PARAM("allocated", mOfficialAllocationSize))

      return mPollingSet;
    }

    //-------------------------------------------------------------------------
    void SocketSet::firedEvent(
                               SocketPtr socket,
                               event_type event
                               )
    {
      ZS_THROW_BAD_STATE_IF(mPollingFiredEventCount >= mPollingCount) // can never grow larger than the polling count

      mPollingFiredEvents[mPollingFiredEventCount].first = socket;
      mPollingFiredEvents[mPollingFiredEventCount].second = event;
      ++mPollingFiredEventCount;

      ZS_LOG_INSANE(log("fire event") + ZS_PARAM("handle", socket->getSocket()) + ZS_PARAM("event", friendly(event)) + ZS_PARAM("fired count", mPollingFiredEventCount))
    }

    //-------------------------------------------------------------------------
    SocketSet::FiredEventPair *SocketSet::getFiredEvents(poll_size &outSize)
    {
      outSize = mPollingFiredEventCount;
      ZS_LOG_INSANE(log("get fired events") + ZS_PARAM("fired count", mPollingFiredEventCount))
      return mPollingFiredEvents;
    }

    //-------------------------------------------------------------------------
    void SocketSet::delegateGone(SocketPtr socket)
    {
      ZS_THROW_BAD_STATE_IF(mPollingSocketsWithDelegateGoneCount >= mPollingCount) // can never grow larger than the polling count

      mPollingSocketsWithDelegateGone[mPollingSocketsWithDelegateGoneCount] = socket;
      ++mPollingSocketsWithDelegateGoneCount;

      ZS_LOG_TRACE(log("delegate gone") + ZS_PARAM("handle", socket->getSocket()) + ZS_PARAM("gone count", mPollingSocketsWithDelegateGoneCount))
    }

    //-------------------------------------------------------------------------
    SocketPtr *SocketSet::getSocketsWithDelegateGone(poll_size &outSize)
    {
      outSize = mPollingSocketsWithDelegateGoneCount;
      ZS_LOG_INSANE(log("get delegates gone") + ZS_PARAM("gone count", mPollingSocketsWithDelegateGoneCount))
      return mPollingSocketsWithDelegateGone;
    }
    
    //-------------------------------------------------------------------------
    void SocketSet::clear()
    {
      ZS_LOG_DETAIL(log("clear"))

      for (poll_size index = 0; index < mPollingFiredEventCount; ++index) {
        mPollingFiredEvents[index].first.reset();
      }
      mPollingFiredEventCount = 0;

      for (poll_size index = 0; index < mPollingSocketsWithDelegateGoneCount; ++index) {
        mPollingSocketsWithDelegateGone[index].reset();
      }
      mPollingSocketsWithDelegateGoneCount = 0;

      mOfficialCount = 0;
      mSocketIndexes.clear();
    }

    //-------------------------------------------------------------------------
    void SocketSet::reset(SOCKET socket)
    {
      ZS_LOG_INSANE(log("reset") + ZS_PARAM("handle", socket))

      SocketIndexMap::iterator found = mSocketIndexes.find(socket);
      if (found == mSocketIndexes.end()) {
        return;  // socket is not found
      }

      mDirty = true;

      poll_size index = (*found).second;

      ZS_LOG_TRACE(log("reset found and will remove") + ZS_PARAM("handle", socket) + ZS_PARAM("index", index))

      if (index + 1 != mOfficialCount) {

        // 0 1 2 3 4 5  [6]

        // prune index 0
        // [0] [1] [6-0-1 = 5] [YES]

        // prune index 5
        // WOULD NEVER ENTER 5+1 == 6

        // prune index 4
        // [4] [5] [6-4-1 = 1] [YES]

        // prune index 3
        // [3] [4] [6-3-1 = 2] [YES]

        // prune the location
        memmove(&(mOfficialSet[index]), &(mOfficialSet[index+1]), sizeof(poll_fd) * (mOfficialCount - index - 1));
      }

      mSocketIndexes.erase(found);

      for (SocketIndexMap::iterator iter = mSocketIndexes.begin(); iter != mSocketIndexes.end(); ++iter)
      {
        poll_size &usingIndex = (*iter).second;
        if (usingIndex >= index) {
          // Shift every index position above the deleted index by one. This is
          // slow for socket removals but socket removals should be the
          // exception and not the normal use case.
          --usingIndex;
        }
      }

      --mOfficialCount;
    }

    //-------------------------------------------------------------------------
    void SocketSet::reset(
                          SOCKET socket,
                          event_type events
                          )
    {
      if (0 == events) {  // no events?
        reset(socket);
        return;
      }

      SocketIndexMap::iterator found = mSocketIndexes.find(socket);
      if (found == mSocketIndexes.end()) {
        ZS_LOG_TRACE(log("reset did not find existing thus appending") + ZS_PARAM("handle", socket) + ZS_PARAM("events", friendly(events)))
        append(socket, events);
        return;
      }

      poll_size index = (*found).second;

      mDirty = mDirty || (mOfficialSet[index].events != events);

      ZS_LOG_INSANE(log("reset found existing thus updating") + ZS_PARAM("handle", socket) + ZS_PARAM("index", index) + ZS_PARAM("events", friendly(events)) + ZS_PARAM("dirty", mDirty))

      mOfficialSet[index].events = events;
    }

    //-------------------------------------------------------------------------
    void SocketSet::addEvents(
                              SOCKET socket,
                              event_type events
                              )
    {
      if (0 == events) return;  // noop

      SocketIndexMap::iterator found = mSocketIndexes.find(socket);
      if (found == mSocketIndexes.end()) {
        ZS_LOG_TRACE(log("add events did not find existing thus appending") + ZS_PARAM("handle", socket) + ZS_PARAM("events", friendly(events)))
        append(socket, events);
        return;
      }

      poll_size index = (*found).second;
      event_type temp = mOfficialSet[index].events;
      mOfficialSet[index].events = mOfficialSet[index].events | events;

      mDirty = mDirty || (temp != mOfficialSet[index].events);

      ZS_LOG_INSANE(log("add events found existing thus updating") + ZS_PARAM("handle", socket) + ZS_PARAM("index", index) + ZS_PARAM("events", friendly(events)) + ZS_PARAM("now", friendly(mOfficialSet[index].events)) + ZS_PARAM("previous", friendly(temp)) + ZS_PARAM("dirty", mDirty))
    }

    //-------------------------------------------------------------------------
    void SocketSet::removeEvents(
                                 SOCKET socket,
                                 event_type events
                                 )
    {
      SocketIndexMap::iterator found = mSocketIndexes.find(socket);
      if (found == mSocketIndexes.end()) {
        ZS_LOG_WARNING(Insane, log("remove events did not find existing") + ZS_PARAM("handle", socket) + ZS_PARAM("events", friendly(events)))
        return;  // nothing to strip
      }

      poll_size index = (*found).second;
      event_type temp = mOfficialSet[index].events;
      mOfficialSet[index].events = mOfficialSet[index].events & (~events);

      mDirty = mDirty || (temp != mOfficialSet[index].events);

      if (0 == mOfficialSet[index].events) {
        ZS_LOG_TRACE(log("remove events no longer needs to monitor as no events are being monitored") + ZS_PARAM("handle", socket) + ZS_PARAM("index", index) + ZS_PARAM("events", friendly(events)) + ZS_PARAM("now", friendly(mOfficialSet[index].events)) + ZS_PARAM("previous", friendly(temp)) + ZS_PARAM("dirty", mDirty))
        reset(socket);
      } else {
        ZS_LOG_INSANE(log("remove events found existing thus updating") + ZS_PARAM("handle", socket) + ZS_PARAM("index", index) + ZS_PARAM("events", friendly(events)) + ZS_PARAM("now", friendly(mOfficialSet[index].events)) + ZS_PARAM("previous", friendly(temp)) + ZS_PARAM("dirty", mDirty))
      }
    }

    //-------------------------------------------------------------------------
    void SocketSet::minOfficialAllocation(poll_size minSize)
    {
      if (mOfficialAllocationSize < minSize) {
        ZS_LOG_TRACE(log("offical allocation resizing") + ZS_PARAM("min", minSize) + ZS_PARAM("allocated", mOfficialAllocationSize))

        poll_fd *set = new poll_fd[minSize];

        memset(set, 0, sizeof(poll_fd) * minSize);

        if (0 != mOfficialAllocationSize) {
          memcpy(set, mOfficialSet, sizeof(poll_fd) * mOfficialAllocationSize);
        }

        delete [] mOfficialSet;

        mOfficialSet = set;
        mOfficialAllocationSize = minSize;
      }
    }

    //-------------------------------------------------------------------------
    void SocketSet::minPollingAllocation(poll_size minSize)
    {
      if (mPollingAllocationSize < minSize) {
        ZS_LOG_TRACE(log("polling allocation resizing") + ZS_PARAM("min", minSize) + ZS_PARAM("allocated", mPollingAllocationSize))

        poll_fd *set = new poll_fd[minSize];
        FiredEventPair *firedEvents = new FiredEventPair[minSize];
        SocketPtr *gone = new SocketPtr[minSize];

        memset(set, 0, sizeof(poll_fd) * minSize);

        if (0 != mPollingAllocationSize) {
          memcpy(set, mPollingSet, sizeof(poll_fd) * mPollingAllocationSize);
        }

        delete [] mPollingSet;
        delete [] mPollingFiredEvents;
        delete [] mPollingSocketsWithDelegateGone;

        mPollingFiredEventCount = 0;

        mPollingSet = set;
        mPollingFiredEvents = firedEvents;
        mPollingSocketsWithDelegateGone = gone;
        mPollingAllocationSize = minSize;
      }
    }

    //-------------------------------------------------------------------------
    void SocketSet::append(
                           SOCKET socket,
                           event_type events
                           )
    {
      ZS_LOG_TRACE(log("new socket being monitored") + ZS_PARAM("handle", socket) + ZS_PARAM("events", friendly(events)))

      mDirty = true;

      minOfficialAllocation(mOfficialCount+1);

      mOfficialSet[mOfficialCount].fd = socket;
      mOfficialSet[mOfficialCount].events = events;
      mOfficialSet[mOfficialCount].revents = 0;

      mSocketIndexes[socket] = mOfficialCount;
      ++mOfficialCount;
    }

    //-----------------------------------------------------------------------
    zsLib::Log::Params SocketSet::log(const char *message) const
    {
      ElementPtr objectEl = Element::create("SocketSet");

      ElementPtr element = Element::create("id");

      TextPtr tmpTxt = Text::create();
      tmpTxt->setValueAndJSONEncode(string(mID));
      element->adoptAsFirstChild(tmpTxt);

      objectEl->adoptAsLastChild(element);

      return zsLib::Log::Params(message, objectEl);
    }
    
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark SocketMonitor
    #pragma mark

    //-------------------------------------------------------------------------
    SocketMonitor::SocketMonitor() :
      mShouldShutdown(false)
    {
    }

    //-------------------------------------------------------------------------
    SocketMonitor::~SocketMonitor()
    {
      mThisWeak.reset();
      cancel();
    }

    //-------------------------------------------------------------------------
    SocketMonitorPtr SocketMonitor::create()
    {
      SocketMonitorPtr pThis = SocketMonitorPtr(new SocketMonitor);
      pThis->mThisWeak = pThis;
      return pThis;
    }

    //-------------------------------------------------------------------------
    SocketMonitorPtr SocketMonitor::singleton()
    {
      static SingletonLazySharedPtr<SocketMonitor> singleton(SocketMonitor::create());

      class Once {
      public: Once() {getSocketMonitorPrioritySingleton().notify();}
      };
      static Once once;

      SocketMonitorPtr result = singleton.singleton();
      if (!result) {
        ZS_LOG_WARNING(Detail, slog("singleton gone"))
      }
      return result;
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::setPriority(ThreadPriorities priority)
    {
      MonitorPriorityHelper &prioritySingleton = getSocketMonitorPrioritySingleton();

      bool changed = prioritySingleton.setPriority(priority);
      if (!changed) return;

      if (!prioritySingleton.wasNotified()) return;

      SocketMonitorPtr singleton = SocketMonitor::singleton();
      if (!singleton) return;

      AutoRecursiveLock lock(singleton->mLock);
      if (!singleton->mThread) return;

      setThreadPriority(*singleton->mThread, priority);
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::monitorBegin(
                                     SocketPtr socket,
                                     bool monitorRead,
                                     bool monitorWrite,
                                     bool monitorException
                                     )
    {
      EventPtr event;
      {
        AutoRecursiveLock lock(mLock);

        if (!mThread) {
          mThread = ThreadPtr(new std::thread(std::ref(*(singleton().get()))));
          setThreadPriority(mThread->native_handle(), getSocketMonitorPrioritySingleton().getPriority());
        }

        SOCKET socketHandle = socket->getSocket();
        if (INVALID_SOCKET == socketHandle)                                             // nothing to monitor
          return;

        mMonitoredSockets[socketHandle] = socket;                                       // remember the socket is monitored

        event_type events = 0;
        if (monitorRead) events = events | POLLRDNORM;
        if (monitorWrite) events = events | POLLWRNORM;
        if (monitorException) events = events | POLLERR | POLLHUP | POLLNVAL;

        ZS_LOG_INSANE(log("monitor begin") + ZS_PARAM("handle", socketHandle) + ZS_PARAM("events", friendly(events)))

        mSocketSet.reset(socketHandle, events);

        event = zsLib::Event::create();
        mWaitingForRebuildList.push_back(event);                                        // socket handles cane be reused so we must ensure that the socket handles are rebuilt before returning

        wakeUp();
      }
      if (event)
        event->wait();
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::monitorEnd(zsLib::Socket &socket)
    {
      EventPtr event;
      {
        AutoRecursiveLock lock(mLock);

        SOCKET socketHandle = socket.getSocket();
        if (INVALID_SOCKET == socketHandle)                                             // nothing to monitor
          return;

        SocketMap::iterator found = mMonitoredSockets.find(socketHandle);
        if (found == mMonitoredSockets.end()) {
          ZS_LOG_WARNING(Trace, log("monitor end but socket was not being monitored") + ZS_PARAM("handle", socketHandle))
          return;                                                                       // socket was not being monitored
        }

        ZS_LOG_TRACE(log("monitor end") + ZS_PARAM("handle", socketHandle))

        mSocketSet.reset(socketHandle);

        mMonitoredSockets.erase(found);                                                 // clear out the socket since it is no longer being monitored

        event = zsLib::Event::create();
        mWaitingForRebuildList.push_back(event);                                        // socket handles cane be reused so we must ensure that the socket handles are rebuilt before returning

        wakeUp();
      }
      if (event)
        event->wait();
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::monitorRead(const zsLib::Socket &socket)
    {
      AutoRecursiveLock lock(mLock);

      SOCKET socketHandle = socket.getSocket();
      if (INVALID_SOCKET == socketHandle)                                             // nothing to monitor
        return;

      if (mMonitoredSockets.end() == mMonitoredSockets.find(socketHandle)) {
        // if the socket is not being monitored, then do nothing
        ZS_LOG_INSANE(log("monitor read but socket is not monitored") + ZS_PARAM("handle", socketHandle))
        return;
      }

      ZS_LOG_INSANE(log("monitor read") + ZS_PARAM("handle", socketHandle))

      mSocketSet.addEvents(socketHandle, POLLRDNORM);

      if (mSocketSet.isDirty()) {
        ZS_LOG_INSANE(log("monitor read waking up socket") + ZS_PARAM("handle", socketHandle))
        wakeUp();
      }
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::monitorWrite(const zsLib::Socket &socket)
    {
      AutoRecursiveLock lock(mLock);

      SOCKET socketHandle = socket.getSocket();
      if (INVALID_SOCKET == socketHandle)                                             // nothing to monitor
        return;

      if (mMonitoredSockets.end() == mMonitoredSockets.find(socketHandle)) {
        // if the socket is not being monitored, then do nothing
        ZS_LOG_INSANE(log("monitor write but socket is not monitored") + ZS_PARAM("handle", socketHandle))
        return;
      }

      ZS_LOG_INSANE(log("monitor write") + ZS_PARAM("handle", socketHandle))

      mSocketSet.addEvents(socketHandle, POLLWRNORM);

      if (mSocketSet.isDirty()) {
        ZS_LOG_INSANE(log("monitor write waking up socket") + ZS_PARAM("handle", socketHandle))
        wakeUp();
      }
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::monitorException(const zsLib::Socket &socket)
    {
      AutoRecursiveLock lock(mLock);

      SOCKET socketHandle = socket.getSocket();
      if (INVALID_SOCKET == socketHandle)                                             // nothing to monitor
        return;

      if (mMonitoredSockets.end() == mMonitoredSockets.find(socketHandle)) {
        // if the socket is not being monitored, then do nothing
        ZS_LOG_INSANE(log("monitor exception but socket is not monitored") + ZS_PARAM("handle", socketHandle))
        return;
      }

      ZS_LOG_INSANE(log("monitor exception") + ZS_PARAM("handle", socketHandle))

      mSocketSet.addEvents(socketHandle, POLLERR | POLLHUP | POLLNVAL);

      if (mSocketSet.isDirty()) {
        ZS_LOG_INSANE(log("monitor exception waking up socket") + ZS_PARAM("handle", socketHandle))
        wakeUp();
      }
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::operator()()
    {
      debugSetCurrentThreadName("com.zslib.socketMonitor");

      srand(static_cast<unsigned int>(time(NULL)));

      bool shouldShutdown = false;

      ZS_LOG_DETAIL(log("socket monitor thread started"))

      createWakeUpSocket();

      do
      {
        poll_fd *pollFDs = NULL;
        poll_size size = 0;

        {
          AutoRecursiveLock lock(mLock);
          processWaiting();

          pollFDs = mSocketSet.preparePollingFDs(size);

          ZS_LOG_INSANE(log("prepared FDs") + ZS_PARAM("total", size))
        }

        int result =

#ifdef _WIN32
        WSAPoll(pollFDs, size, ZSLIB_SOCKET_MONITOR_TIMEOUT_IN_MILLISECONDS);
#else
        poll(pollFDs, size, ZSLIB_SOCKET_MONITOR_TIMEOUT_IN_MILLISECONDS);
#endif //_WIN32

        ZS_LOG_INSANE(log("poll completed") + ZS_PARAM("result", result))

        bool redoWakeupSocket = false;

        // select completed, do notifications from select
        {
          AutoRecursiveLock lock(mLock);
          shouldShutdown = mShouldShutdown;

          if (result <= 0) goto completed;

          // figure out which sockets need updating
          for (poll_size index = 0; (result > 0) && (index < size); ++index)
          {
            poll_fd &record = pollFDs[index];

            if (0 == record.revents) continue;

            --result; // stop once we have found every triggered socket

            ZS_LOG_INSANE(log("socket event found") + ZS_PARAM("handle", record.fd) + ZS_PARAM("events", friendly(record.revents)))

            if (record.fd == mWakeUpSocket->getSocket()) {
              // related to wakeup socket
              if ((record.revents & POLLRDNORM) != 0) {
                ZS_LOG_INSANE(log("read wake-up socket"))

                bool wouldBlock = false;
                static DWORD gBogus = 0;
                static BYTE *bogus = (BYTE *)&gBogus;
                int noThrowError = 0;
                mWakeUpSocket->receive(bogus, sizeof(gBogus), &wouldBlock, 0, &noThrowError);
                if (0 != noThrowError) {
                  ZS_LOG_WARNING(Insane, log("wake up socket had a failure") + ZS_PARAM("error", noThrowError))
                  redoWakeupSocket = true;
                }
              }

              if ((record.revents & (POLLERR | POLLHUP | POLLNVAL)) != 0) {
                ZS_LOG_WARNING(Insane, log("wake up socket notified of a failure") + ZS_PARAM("revents", record.revents))
                redoWakeupSocket = true;
              }
              continue;
            }

            SocketMap::iterator found = mMonitoredSockets.find(record.fd);
            if (found == mMonitoredSockets.end()) {
              // socket was not found in map thus remove it
              ZS_LOG_WARNING(Insane, log("event fired but socket is not being monitor thus removing") + ZS_PARAM("handle", record.fd) + ZS_PARAM("events", friendly(record.revents)))
              mSocketSet.reset(record.fd);
              continue;
            }

            SocketPtr socket = (*found).second.lock();
            if (!socket) {
              // socket object is now gone and should no longer be monitored
              ZS_LOG_WARNING(Insane, log("event fired but socket is now gone thus removing") + ZS_PARAM("handle", record.fd) + ZS_PARAM("events", friendly(record.revents)))

              mSocketSet.reset(record.fd);
              mMonitoredSockets.erase(found);
              continue;
            }

            ZS_LOG_INSANE(log("event fired") + ZS_PARAM("handle", record.fd) + ZS_PARAM("events", friendly(record.revents)))

            mSocketSet.firedEvent(socket, record.revents);

            // check to see if should no longer be monitored

            if ((record.revents & POLLRDNORM) != 0) {
              mSocketSet.removeEvents(socket->getSocket(), POLLRDNORM);
            }
            if ((record.revents & POLLWRNORM) != 0) {
              mSocketSet.removeEvents(socket->getSocket(), POLLWRNORM);
            }
            if ((record.revents & (POLLERR | POLLHUP | POLLNVAL)) != 0) {
              ZS_LOG_WARNING(Insane, log("socket has exception thus removing monitor") + ZS_PARAM("handle", record.fd) + ZS_PARAM("events", friendly(record.revents)))

              // socket had a problem, do not monitor
              mSocketSet.reset(record.fd);
              mMonitoredSockets.erase(found);
              continue;
            }
          }
        }

      completed:

        // scope: fire notifications outside of the lock
        {
          poll_size totalFired = 0;
          FiredEventPair *fired = mSocketSet.getFiredEvents(totalFired);
          for (poll_size index = 0; index < totalFired; ++index)
          {
            FiredEventPair &record = fired[index];

            try {
              if ((record.second & POLLRDNORM) != 0) {
                record.first->notifyReadReady();
              }
              if ((record.second & POLLWRNORM) != 0) {
                record.first->notifyWriteReady();
              }
              if ((record.second & (POLLERR | POLLHUP | POLLNVAL)) != 0) {
                record.first->notifyException();
              }
            } catch (ISocketDelegateProxy::Exceptions::DelegateGone &) {
              // notified to a dead socket
              ZS_LOG_WARNING(Trace, log("delegate gone") + ZS_PARAM("handle", record.first->getSocket()) + ZS_PARAM("events", friendly(record.second)))
              mSocketSet.delegateGone(record.first);
            } catch (IMessageQueue::Exceptions::MessageQueueGone &) {
              ZS_LOG_FATAL(Basic, log("message queue gone"))
              shouldShutdown = true;
            }
          }
        }

        // scope: clean out sockets with delegates gone
        {
          AutoRecursiveLock lock(mLock);

          poll_size totalGone = 0;
          SocketPtr *gone = mSocketSet.getSocketsWithDelegateGone(totalGone);
          for (poll_size index = 0; index < totalGone; ++index)
          {
            SocketPtr &socket = gone[index];

            ZS_LOG_WARNING(Trace, log("removing socket because delegate is gone") + ZS_PARAM("handle", socket->getSocket()))

            mSocketSet.reset(socket->getSocket());

            SocketMap::iterator found = mMonitoredSockets.find(socket->getSocket());
            if (found != mMonitoredSockets.end()) {
              mMonitoredSockets.erase(found);
            }

            socket.reset();
          }
        }

        if (redoWakeupSocket) {
          // WARNING: DO NOT CALL FROM WITHIN A LOCK
          ZS_LOG_TRACE(log("redoing wake-up socket"))
          createWakeUpSocket();
        }

      } while (!shouldShutdown);

      ZS_LOG_DETAIL(log("socket thread is shutting down"))

      SocketMonitorPtr gracefulReference;

      {
        AutoRecursiveLock lock(mLock);

        // transfer the reference to the thread
        gracefulReference = mGracefulReference;
        mGracefulReference.reset();

        processWaiting();
        mMonitoredSockets.clear();
        mWaitingForRebuildList.clear();
        mSocketSet.clear();
        mWakeUpSocket.reset();
      }
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::cancel()
    {
      ZS_LOG_DETAIL(log("cancel called"))

      ThreadPtr thread;
      {
        AutoRecursiveLock lock(mLock);
        mGracefulReference = mThisWeak.lock();
        thread = mThread;

        mShouldShutdown = true;
        wakeUp();
      }

      if (!thread)
        return;

      thread->join();

      {
        AutoRecursiveLock lock(mLock);
        mThread.reset();
      }
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::processWaiting()
    {
      for (EventList::iterator iter = mWaitingForRebuildList.begin(); iter != mWaitingForRebuildList.end(); ++iter)
      {
        (*iter)->notify();
        ZS_LOG_DETAIL(log("notified"))
      }
      mWaitingForRebuildList.clear();
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::wakeUp()
    {
      int errorCode = 0;

      {
        AutoRecursiveLock lock(mLock);

        if (!mWakeUpSocket)               // is the wakeup socket created?
          return;

        if (!mWakeUpSocket->isValid())
        {
          ZS_LOG_ERROR(Basic, "Could not wake up socket monitor as wakeup socket was closed. This will cause a delay in the socket monitor response time.")
          return;
        }

        static DWORD gBogus = 0;
        static BYTE *bogus = (BYTE *)&gBogus;

        bool wouldBlock = false;
        mWakeUpSocket->send(bogus, sizeof(gBogus), &wouldBlock, 0, &errorCode);       // send a bogus packet to its own port to wake it up
      }

      if (0 != errorCode) {
        ZS_LOG_ERROR(Basic, log("Could not wake up socket monitor. This will cause a delay in the socket monitor response time") + ZS_PARAM("error", errorCode))
      }
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::createWakeUpSocket()
    {
      // WARNING: NEVER CALL THIS FROM WITHIN A LOCK

      // ignore SIGPIPE
      int tries = 0;
      bool useIPv6 = true;
      while (true)
      {
        // bind only on the loopback address
        bool error = false;
        try {
          SocketPtr wakeupSocket;

          // scope: create a wakeup socket
          {
            if (useIPv6) {
              wakeupSocket = zsLib::Socket::createUDP(zsLib::Socket::Create::IPv6);
            } else {
              wakeupSocket = zsLib::Socket::createUDP(zsLib::Socket::Create::IPv4);
            }
            wakeupSocket->unlinkSocketMonitor();  // do not hold a reference to ourself
          }

          // scope: create a binding address
          {
            AutoRecursiveLock lock(mLock);

            if (mWakeUpSocket) {
              mSocketSet.reset(mWakeUpSocket->getSocket());
            }

            mWakeUpSocket = wakeupSocket;

            if (useIPv6) {
              mWakeUpAddress = IPAddress::loopbackV6();
            } else {
              mWakeUpAddress = IPAddress::loopbackV4();
            }

            if (((tries > 5) && (tries < 10)) ||
                (tries > 15)) {
              mWakeUpAddress.setPort(5000+(rand()%(65525-5000)));
            } else {
              mWakeUpAddress.setPort(0);
            }

            mWakeUpSocket->setOptionFlag(zsLib::Socket::SetOptionFlag::NonBlocking, true);
            mWakeUpSocket->bind(mWakeUpAddress);
            mWakeUpAddress = mWakeUpSocket->getLocalAddress();
            mWakeUpSocket->connect(mWakeUpAddress);

            mSocketSet.reset(mWakeUpSocket->getSocket(), POLLRDNORM | POLLERR | POLLHUP | POLLNVAL);
          }
        } catch (zsLib::Socket::Exceptions::Unspecified &) {
          error = true;
        }

        if (!error)
        {
          break;
        }

        ZS_LOG_WARNING(Detail, log("unable to create wake-up socket"))

        std::this_thread::yield();       // do not hammer CPU

        if (tries > 10)
          useIPv6 = (tries%2 == 0);   // after 10 tries, start trying to bind using IPv4

        ZS_THROW_BAD_STATE_MSG_IF(tries > 500, "Unable to allocate any loopback ports for a wake-up socket")
      }
    }

    //-----------------------------------------------------------------------
    zsLib::Log::Params SocketMonitor::log(const char *message) const
    {
      ElementPtr objectEl = Element::create("SocketMonitor");

      ElementPtr element = Element::create("id");

      TextPtr tmpTxt = Text::create();
      tmpTxt->setValueAndJSONEncode(string(mID));
      element->adoptAsFirstChild(tmpTxt);

      objectEl->adoptAsLastChild(element);

      return zsLib::Log::Params(message, objectEl);
    }

    //-----------------------------------------------------------------------
    zsLib::Log::Params SocketMonitor::slog(const char *message)
    {
      return zsLib::Log::Params(message, "SocketMonitor");
    }

  }
}
