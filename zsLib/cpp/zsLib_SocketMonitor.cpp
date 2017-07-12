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
#include <zsLib/internal/zsLib_MessageQueueThread.h>

#include <zsLib/ISettings.h>
#include <zsLib/Stringize.h>
#include <zsLib/helpers.h>
#include <zsLib/XML.h>

#include <zsLib/SafeInt.h>

#include <stdlib.h>

#include <unordered_map>

#define ZSLIB_SOCKET_MONITOR_TIMEOUT_IN_MILLISECONDS (10*(1000))

namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib_socket)}

namespace zsLib
{
  namespace internal
  {
    ZS_DECLARE_CLASS_PTR(SocketMonitorSettingsDefaults);

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

    //-----------------------------------------------------------------------
#ifdef _WIN32
    static zsLib::Log::Params slog(const char *message, const char *object)
    {
      return zsLib::Log::Params(message, object);
    }
#endif //_WIN32


    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark SocketMonitorSettingsDefaults
    #pragma mark

    class SocketMonitorSettingsDefaults : public ISettingsApplyDefaultsDelegate
    {
    public:
      //-----------------------------------------------------------------------
      ~SocketMonitorSettingsDefaults()
      {
        ISettings::removeDefaults(*this);
      }

      //-----------------------------------------------------------------------
      static SocketMonitorSettingsDefaultsPtr singleton()
      {
        static SingletonLazySharedPtr<SocketMonitorSettingsDefaults> singleton(create());
        return singleton.singleton();
      }

      //-----------------------------------------------------------------------
      static SocketMonitorSettingsDefaultsPtr create()
      {
        auto pThis(make_shared<SocketMonitorSettingsDefaults>());
        ISettings::installDefaults(pThis);
        return pThis;
      }

      //-----------------------------------------------------------------------
      virtual void notifySettingsApplyDefaults() override
      {
        ISettings::setString(ZSLIB_SETTING_SOCKET_MONITOR_THREAD_PRIORITY, "normal");
      }
    };

    //-------------------------------------------------------------------------
    void installSocketMonitorSettingsDefaults()
    {
      SocketMonitorSettingsDefaults::singleton();
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark SocketMonitorLoadBalancer
    #pragma mark
    
    class SocketMonitorLoadBalancer : public ISingletonManagerDelegate
    {
    protected:
      struct make_private {};

    public:
      struct SocketMonitorInfo
      {
        size_t totalMonitored_ {};
        SocketMonitorPtr monitor_;
      };

      typedef std::unordered_map<PUID, SocketMonitorInfo> SocketMonitorMap;

    protected:

      //-----------------------------------------------------------------------
      static SocketMonitorLoadBalancerPtr create()
      {
        auto pThis(make_shared<SocketMonitorLoadBalancer>(make_private{}));
        pThis->thisWeak_ = pThis;
        pThis->init();
        return pThis;
      }

      //-----------------------------------------------------------------------
      static SocketMonitorLoadBalancerPtr singleton()
      {
        static SingletonLazySharedPtr<SocketMonitorLoadBalancer> singleton(create());

        SocketMonitorLoadBalancerPtr result = singleton.singleton();
        if (!result) {
          ZS_LOG_WARNING(Detail, slog("singleton gone"))
        }

        static zsLib::SingletonManager::Register registerSingleton("org.zsLib.SocketMonitorLoadBalancer", result);
        return result;
      }

      //-----------------------------------------------------------------------
      void init()
      {
      }

    public:
      //-----------------------------------------------------------------------
      SocketMonitorLoadBalancer(const make_private &) :
#ifdef _WIN32
        maxSocketsPerMonitor_(WSA_MAXIMUM_WAIT_EVENTS)
#else
        maxSocketsPerMonitor_(FD_SETSIZE)
#endif //_WIN32
      {
        --maxSocketsPerMonitor_; // wake up socket/event
      }

      //-----------------------------------------------------------------------
      ~SocketMonitorLoadBalancer()
      {
        thisWeak_.reset();
        cancel();
      }

      //-----------------------------------------------------------------------
      static SocketMonitorPtr link()
      {
        auto pThis = singleton();
        if (!pThis) return SocketMonitorPtr();

        return pThis->internalLink();
      }

      //-----------------------------------------------------------------------
      static void unlink(PUID id)
      {
        auto pThis = singleton();
        if (!pThis) return;

        pThis->internalUnlink(id);
      }

    protected:

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark SocketMonitorLoadBalancer => ISingletonManagerDelegate
      #pragma mark

      //-----------------------------------------------------------------------
      virtual void notifySingletonCleanup() override
      {
        cancel();
      }

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark SocketMonitorLoadBalancer => (internal)
      #pragma mark

      //-----------------------------------------------------------------------
      void cancel()
      {
        SocketMonitorMap monitors;

        {
          AutoRecursiveLock lock(lock_);

          if (shutdown_) return;
          shutdown_ = true;

          monitors = socketMonitors_;
          socketMonitors_.clear();
        }

        for (auto iter = monitors.begin(); iter != monitors.end(); ++iter) {
          auto &info = (*iter).second;

          info.monitor_->shutdown();
        }
      }

      //-----------------------------------------------------------------------
      SocketMonitorPtr internalLink()
      {
        AutoRecursiveLock lock(lock_);

        SocketMonitorInfo *foundInfo {};

        for (auto iter = socketMonitors_.begin(); iter != socketMonitors_.end(); ++iter) {
          auto &info = (*iter).second;

          if (!foundInfo) {
            foundInfo = &info;
            continue;
          }

          if (info.totalMonitored_ >= foundInfo->totalMonitored_) continue;
          foundInfo = &info;
        }

        if (foundInfo) {
          if (foundInfo->totalMonitored_ >= maxSocketsPerMonitor_) {
            foundInfo = NULL;
          }
        }

        if (foundInfo) {
          ++(foundInfo->totalMonitored_);
          return foundInfo->monitor_;
        }

        SocketMonitorInfo info;
        info.monitor_ = SocketMonitor::create();
        info.totalMonitored_ = 1;

        socketMonitors_[info.monitor_->getID()] = info;

        return info.monitor_;
      }

      //-----------------------------------------------------------------------
      void internalUnlink(PUID id)
      {
        SocketMonitorPtr shutdownMonitor;

        {
          AutoRecursiveLock lock(lock_);

          auto found = socketMonitors_.find(id);
          if (found == socketMonitors_.end()) return;

          auto &info = (*found).second;

          if (info.totalMonitored_ > 0) {
            --(info.totalMonitored_);
          }

          if (0 == info.totalMonitored_) {
            shutdownMonitor = info.monitor_;
            socketMonitors_.erase(found);
          }
        }

        if (!shutdownMonitor) return;

        shutdownMonitor->shutdown();
      }

      //-----------------------------------------------------------------------
      static zsLib::Log::Params slog(const char *message)
      {
        return zsLib::Log::Params(message, "SocketMonitorLoadBalancer");
      }

    protected:
      SocketMonitorLoadBalancerWeakPtr thisWeak_;

      RecursiveLock lock_;

      bool shutdown_ {false};

      size_t maxSocketsPerMonitor_ {};
      SocketMonitorMap socketMonitors_;
    };

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark SocketSet
    #pragma mark

    //-------------------------------------------------------------------------
    SocketSet::SocketSet()
    {
      ZS_LOG_BASIC(log("created"))
    }

    //-------------------------------------------------------------------------
    SocketSet::~SocketSet()
    {
      ZS_LOG_BASIC(log("destroyed"))

      delete [] mOfficialSet;
#ifdef _WIN32
      delete [] mOfficialHandleSet;
      delete [] mOfficialHandleHolderSet;
#endif //_WIN32

      delete [] mPollingSet;
#ifdef _WIN32
      delete [] mPollingHandleSet;
      delete [] mPollingHandleHolderSet;
#endif //_WIN32
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
    SocketSet::poll_fd *SocketSet::preparePollingFDs(
                                                     poll_size &outSize,
                                                     EventHandle * &outEvents
                                                     )
    {
      outSize = mOfficialCount;
      outEvents = NULL;

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
        outEvents = mPollingHandleSet;
        return mPollingSet;
      }

      minPollingAllocation(mOfficialCount > 0 ? mOfficialCount : 1);

      mPollingCount = mOfficialCount;

      if (mOfficialCount > 0) {
        memcpy(mPollingSet, mOfficialSet, sizeof(poll_fd) * mOfficialCount);
#ifdef _WIN32
        memcpy(mPollingHandleSet, mOfficialHandleSet, sizeof(EventHandle) * mOfficialCount);
        for (decltype(mPollingCount) loop = 0; loop < mPollingCount; ++loop) {
          mPollingHandleHolderSet[loop] = mOfficialHandleHolderSet[loop];
        }
        for (decltype(mPollingCount) loop = mPollingCount; loop < mPollingAllocationSize; ++loop) {
          mPollingHandleHolderSet[loop] = EventHandleHolderPtr();
        }
#endif //_WIN32
      }

      mDirty = false;

      ZS_LOG_INSANE(log("preparing polling") + ZS_PARAM("count", mOfficialCount) + ZS_PARAM("allocated", mOfficialAllocationSize))

      outEvents = mPollingHandleSet;
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

#ifdef _WIN32
    //-------------------------------------------------------------------------
    void SocketSet::setWakeUpEvent(HANDLE eventHandle)
    {
      ZS_LOG_TRACE(log("wakeup event created") + ZS_PARAM("event", (PTRNUMBER)eventHandle))

      mDirty = true;

      minOfficialAllocation(mOfficialCount + 1);

      mOfficialSet[mOfficialCount].fd = INVALID_SOCKET;
      mOfficialSet[mOfficialCount].events = POLLRDNORM | POLLERR | POLLHUP | POLLNVAL;
      mOfficialSet[mOfficialCount].revents = 0;

      mOfficialHandleSet[mOfficialCount] = eventHandle;
      mOfficialHandleHolderSet[mOfficialCount] = make_shared<EventHandleHolder>((HANDLE)NULL);  // do not release event handle as part of set

      ++mOfficialCount;
    }
#endif //_WIN32

    //-------------------------------------------------------------------------
    void SocketSet::reset(SOCKET socket)
    {
      ZS_LOG_INSANE(log("reset") + ZS_PARAM("handle", socket))

      SocketIndexMap::iterator found = mSocketIndexes.find(socket);
      if (found == mSocketIndexes.end()) {
        return;  // socket is not found
      }

      mDirty = true;

      decltype(mOfficialCount) index = (*found).second;

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
        decltype(index) total = mOfficialCount - index - 1;
        memmove(&(mOfficialSet[index]), &(mOfficialSet[index+1]), sizeof(poll_fd) * total);
#ifdef _WIN32
        memmove(&(mOfficialHandleSet[index]), &(mOfficialHandleSet[index+1]), sizeof(EventHandle) * total);
        for (decltype(index) moveIndex = index, count = 0; count < total; ++count, ++moveIndex) {
          mOfficialHandleHolderSet[moveIndex] = mOfficialHandleHolderSet[moveIndex+1];
        }
#endif //_WIN32
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
#ifdef _WIN32
      auto selectResult = WSAEventSelect(socket, mOfficialHandleSet[index], toNetworkEvents(mOfficialSet[index].events));
      assert(0 == selectResult);
#endif //_WIN32

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

#ifdef _WIN32
      auto selectResult = WSAEventSelect(socket, mOfficialHandleSet[index], toNetworkEvents(mOfficialSet[index].events));
      assert(0 == selectResult);
#endif //_WIN32
      }
    }

    //-------------------------------------------------------------------------
    void SocketSet::minOfficialAllocation(poll_size minSize)
    {
      if (mOfficialAllocationSize < minSize) {
        ZS_LOG_TRACE(log("offical allocation resizing") + ZS_PARAM("min", minSize) + ZS_PARAM("allocated", mOfficialAllocationSize))

        poll_fd *set = new poll_fd[minSize] {};
#ifdef _WIN32
        EventHandle *setHandle = new EventHandle[minSize] {};
        EventHandleHolderPtr *setHandleHolder = new EventHandleHolderPtr[minSize] {};
#endif //_WIN32

        if (0 != mOfficialAllocationSize) {
          memcpy(set, mOfficialSet, sizeof(poll_fd) * mOfficialAllocationSize);
#ifdef _WIN32
          memcpy(setHandle, mOfficialHandleSet, sizeof(EventHandle) * mOfficialAllocationSize);
          for (decltype(mOfficialAllocationSize) loop = 0; loop < mOfficialAllocationSize; ++loop) {
            setHandleHolder[loop] = mOfficialHandleHolderSet[loop]; 
          }
#endif //_WIN32
        }

        delete [] mOfficialSet;
#ifdef _WIN32
        delete [] mOfficialHandleSet;
        delete [] mOfficialHandleHolderSet;
#endif //_WIN32

        mOfficialSet = set;
#ifdef _WIN32
        mOfficialHandleSet = setHandle;
        mOfficialHandleHolderSet = setHandleHolder;
#endif //_WIN32
        mOfficialAllocationSize = minSize;
      }
    }

    //-------------------------------------------------------------------------
    void SocketSet::minPollingAllocation(poll_size minSize)
    {
      if (mPollingAllocationSize < minSize) {
        ZS_LOG_TRACE(log("polling allocation resizing") + ZS_PARAM("min", minSize) + ZS_PARAM("allocated", mPollingAllocationSize))

        poll_fd *set = new poll_fd[minSize] {};
#ifdef _WIN32
        EventHandle *setHandle = new EventHandle[minSize] {};
        EventHandleHolderPtr *setHandleHolder = new EventHandleHolderPtr[minSize] {};
#endif //_WIN32
        FiredEventPair *firedEvents = new FiredEventPair[minSize] {};
        SocketPtr *gone = new SocketPtr[minSize] {};

        if (0 != mPollingAllocationSize) {
          memcpy(set, mPollingSet, sizeof(poll_fd) * mPollingAllocationSize);
#ifdef _WIN32
          memcpy(setHandle, mPollingHandleSet, sizeof(EventHandle) * mPollingAllocationSize);
          for (decltype(mPollingAllocationSize) loop = 0; loop < mPollingAllocationSize; ++loop) {
            setHandleHolder[loop] = mPollingHandleHolderSet[loop];
          }
#endif //_WIN32
        }

        delete [] mPollingSet;
#ifdef _WIN32
        delete [] mPollingHandleSet;
        delete [] mPollingHandleHolderSet;
#endif //_WIN32
        delete [] mPollingFiredEvents;
        delete [] mPollingSocketsWithDelegateGone;

        mPollingFiredEventCount = 0;

        mPollingSet = set;
#ifdef _WIN32
        mPollingHandleSet = setHandle;
        mPollingHandleHolderSet = setHandleHolder;
#endif //_WIN32
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
#ifdef _WIN32
      auto eventHandle = WSACreateEvent();
      ZS_LOG_TRACE(log("created event handle") + ZS_PARAM("event", (PTRNUMBER)eventHandle))
      if (WSA_INVALID_EVENT == eventHandle) {
        eventHandle = NULL;
        ZS_LOG_WARNING(Detail, log("create event handle failed") + ZS_PARAM("error", WSAGetLastError()))
      } else {
        auto selectResult = WSAEventSelect(socket, eventHandle, toNetworkEvents(events));
        assert(0 == selectResult);
      }
      mOfficialHandleSet[mOfficialCount] = eventHandle;
      mOfficialHandleHolderSet[mOfficialCount] = make_shared<EventHandleHolder>(eventHandle);
#endif //_WIN32

      mSocketIndexes[socket] = mOfficialCount;
      ++mOfficialCount;
    }

#ifdef _WIN32
    //-----------------------------------------------------------------------
    long SocketSet::toNetworkEvents(event_type events)
    {
      long result {};
      if (0 != (POLLRDNORM & events)) {
        result |= (FD_READ | FD_ACCEPT);
      }
      if (0 != (POLLWRNORM & events)) {
        result |= (FD_WRITE | FD_CONNECT);
      }
      if (0 != ((POLLERR | POLLHUP | POLLNVAL) & events)) {
        result |= (FD_CLOSE);
      }
      return result;
    }
#endif //_WIN32

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
    #pragma mark SocketSet::EventHandleHolder
    #pragma mark

    //-------------------------------------------------------------------------
    SocketSet::EventHandleHolder::EventHandleHolder(EventHandle handle) :
      mEventHandle(handle)
    {
#ifdef _WIN32
      ZS_LOG_TRACE(slog("holding event handle", "SocketSet::EventHandleHolder") + ZS_PARAM("event", (PTRNUMBER)mEventHandle))
#endif //_WIN32
    }

    //-------------------------------------------------------------------------
    SocketSet::EventHandleHolder::~EventHandleHolder()
    {
#ifdef _WIN32
      ZS_LOG_TRACE(slog("closing event handle", "SocketSet::EventHandleHolder") + ZS_PARAM("event", (PTRNUMBER)mEventHandle))
      if (NULL != mEventHandle) {
        WSACloseEvent(mEventHandle);
        mEventHandle = NULL;
      }
#endif //_WIN32
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark SocketMonitor
    #pragma mark

    //-------------------------------------------------------------------------
    SocketMonitor::SocketMonitor()
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
    SocketMonitorPtr SocketMonitor::link()
    {
      return SocketMonitorLoadBalancer::link();
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
          auto pThis = mThisWeak.lock();
          mThread = ThreadPtr(new std::thread(std::ref(*(pThis.get()))));
          setThreadPriority(mThread->native_handle(), zsLib::threadPriorityFromString(ISettings::getString(ZSLIB_SETTING_SOCKET_MONITOR_THREAD_PRIORITY)));
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

      SocketMonitorLoadBalancer::unlink(mID);
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
      debugSetCurrentThreadName("org.zsLib.socketMonitor");

      srand(static_cast<unsigned int>(time(NULL)));

      ZS_LOG_DETAIL(log("socket monitor thread started"))

      createWakeUpSocket();

      do
      {
        EventHandle *pollEvents = NULL;
        poll_fd *pollFDs = NULL;
        poll_size size = 0;

        {
          AutoRecursiveLock lock(mLock);
          processWaiting();

          pollFDs = mSocketSet.preparePollingFDs(size, pollEvents);

          ZS_LOG_INSANE(log("prepared FDs") + ZS_PARAM("total", size))
        }

        int lastError {};

        auto result =

#ifdef _WIN32
        WSAWaitForMultipleEvents(SafeInt<DWORD>(size), pollEvents, FALSE, ZSLIB_SOCKET_MONITOR_TIMEOUT_IN_MILLISECONDS, FALSE);
#else
        poll(pollFDs, size, ZSLIB_SOCKET_MONITOR_TIMEOUT_IN_MILLISECONDS);
        if (-1 == result) {
          lastError = errno;
        }
#endif //_WIN32

        ZS_LOG_INSANE(log("poll completed") + ZS_PARAM("result", result) + ZS_PARAM("error", lastError));

        bool redoWakeupSocket = false;

        // select completed, do notifications from select
        {
          AutoRecursiveLock lock(mLock);

#ifndef _WIN32
          if (result <= 0) goto completed;

          for (poll_size index = 0; (result > 0) && (index < size); ++index)
          {
#else
          if (WSA_WAIT_TIMEOUT == result) goto completed;
          if (WSA_WAIT_FAILED == result) {
            lastError = WSAGetLastError();
            ZS_LOG_INSANE(log("poll completed") + ZS_PARAM("result", result) + ZS_PARAM("error", lastError));
            goto completed;
          }

          decltype(result) index = result - WSA_WAIT_EVENT_0;
          decltype(result) nextIndex = index;

          while (true)
          {
            index = nextIndex;
            if (nextIndex >= size) goto completed;
            ++nextIndex;

            result = WSAWaitForMultipleEvents(SafeInt<DWORD>(size - index), &(pollEvents[index]), FALSE, 0, FALSE);
            if (WSA_WAIT_TIMEOUT == result) goto completed;
            if ((WSA_WAIT_EVENT_0 == result) &&
                (0 == index)) continue; // special event based wake-up event

            index = (result - WSA_WAIT_EVENT_0) + index;
            if (index >= nextIndex) nextIndex = index+1;


#endif //_WIN32

          // figure out which sockets need updating
            poll_fd &record = pollFDs[index];

#ifdef _WIN32
            WSANETWORKEVENTS networkEvents {};
            auto enumResult = WSAEnumNetworkEvents(record.fd, pollEvents[index], &networkEvents);
            if (0 != enumResult) {
              ZS_LOG_WARNING(Detail, log("failed to enum network events") + ZS_PARAM("error", WSAGetLastError()))
              goto completed;
            }

            record.revents = 0;

            if (0 != (networkEvents.lNetworkEvents & FD_CONNECT)) {
              if (NOERROR != networkEvents.iErrorCode[FD_CONNECT_BIT]) {
                record.revents |= POLLERR;
              } else {
                record.revents |= POLLWRNORM;
              }
            }
            if (0 != (networkEvents.lNetworkEvents & FD_CLOSE)) {
              if (NOERROR != networkEvents.iErrorCode[FD_CLOSE_BIT]) {
                record.revents |= POLLERR;
              } else {
                record.revents |= POLLHUP;
              }
            }
            if (0 != (networkEvents.lNetworkEvents & FD_READ)) {
              record.revents |= POLLRDNORM;
            }
            if (0 != (networkEvents.lNetworkEvents & FD_WRITE)) {
              record.revents |= POLLWRNORM;
            }
            if (0 != (networkEvents.lNetworkEvents & FD_ACCEPT)) {
              record.revents |= POLLRDNORM;
            }
            if (0 != (networkEvents.lNetworkEvents & FD_ROUTING_INTERFACE_CHANGE)) {
              if (NOERROR != networkEvents.iErrorCode[FD_ROUTING_INTERFACE_CHANGE_BIT]) {
                record.revents |= POLLERR;
              }
            }
#endif //_WIN32

            if (0 == record.revents) {
              ZS_LOG_INSANE(log("no event found set"))
              continue;
            }

#ifndef _WIN32
            --result; // stop once we have found every triggered socket
#endif //ndef _WIN32

            ZS_LOG_INSANE(log("socket event found") + ZS_PARAM("handle", record.fd) + ZS_PARAM("events", friendly(record.revents)))

#ifndef _WIN32
            if (record.fd == mWakeUpSocket->getSocket()) {
              // related to wakeup socket
              if ((record.revents & POLLRDNORM) != 0) {
                ZS_LOG_INSANE(log("read wake-up socket"))

                bool wouldBlock = false;
                static char gBogus[65536] {};
                static BYTE *bogus = (BYTE *)&gBogus;
                int noThrowError = 0;
                auto totalRead = mWakeUpSocket->receive(bogus, sizeof(gBogus), &wouldBlock, 0, &noThrowError);
                if (0 != noThrowError) {
                  ZS_LOG_WARNING(Insane, log("wake up socket had a failure") + ZS_PARAM("error", noThrowError))
                  redoWakeupSocket = true;
                }
                if (wouldBlock) {
                  ZS_LOG_WARNING(Insane, log("wake up socket would block") + ZS_PARAM("total read", totalRead))
                }
              }

              if ((record.revents & (POLLERR | POLLHUP | POLLNVAL)) != 0) {
                ZS_LOG_WARNING(Insane, log("wake up socket notified of a failure") + ZS_PARAM("revents", record.revents))
                redoWakeupSocket = true;
              }
              continue;
            }
#endif //_WIN32

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
              mShouldShutdown = true;
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

#ifndef _WIN32
        if (redoWakeupSocket) {
          // WARNING: DO NOT CALL FROM WITHIN A LOCK
          ZS_LOG_TRACE(log("redoing wake-up socket"))
          createWakeUpSocket();
        }
#endif //ndef _WIN32

      } while (!mShouldShutdown);

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
        cleanWakeUpSocket();
      }
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::shutdown()
    {
      ZS_LOG_DETAIL(log("shutdown called"));
      cancel();
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::cancel()
    {
      ZS_LOG_DETAIL(log("cancel called"));

      ThreadPtr thread;
      SocketMonitorPtr gracefulReference;

      {
        AutoRecursiveLock lock(mLock);
        gracefulReference = mGracefulReference = mThisWeak.lock();
        thread = mThread;
        mThread.reset();

        mShouldShutdown = true;
        wakeUp();
      }

      if (!thread)
        return;

      if (thread->joinable()) {
        thread->join();
      }
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::processWaiting()
    {
      if (mWaitingForRebuildList.size() < 1) return;
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
#ifdef _WIN32
      if (NULL == mWakeupEvent) return;
      ::SetEvent(mWakeupEvent);
#else
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
#endif //_WIN32
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::createWakeUpSocket()
    {
#ifdef _WIN32
      mWakeupEvent = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);

      if (NULL != mWakeupEvent) {
        mSocketSet.setWakeUpEvent(mWakeupEvent);
      }
#else // _WIN32
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
#endif // _WIN32
    }

    //-------------------------------------------------------------------------
    void SocketMonitor::cleanWakeUpSocket()
    {
#ifdef _WIN32
      if (NULL != mWakeupEvent) {
        ::CloseHandle(mWakeupEvent);
        mWakeupEvent = NULL;
      }
#else // _WIN32
      mWakeUpSocket.reset();
#endif // _WIN32
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
