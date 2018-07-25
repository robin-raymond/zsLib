/*

 Copyright (c) 2016, Robin Raymond
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

#include <zsLib/internal/zsLib_MessageQueueManager.h>
#include <zsLib/IMessageQueueThreadPool.h>
#include <zsLib/IHelper.h>
#include <zsLib/ISettings.h>

#include <zsLib/Log.h>
#include <zsLib/XML.h>

#define ZSLIB_MESSAGE_QUEUE_MANAGER_RESERVED_GUI_THREAD_NAME "c745461ccd5bfd8427beeda5f952dc68fb09668a_zsLib.guiThread"

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
  namespace internal
  {
    ZS_DECLARE_USING_PTR(zsLib::XML, Element);
    ZS_DECLARE_USING_PTR(zsLib, IMessageQueueThread);

    ZS_DECLARE_CLASS_PTR(MessageQueueManagerSettingsDefaults);

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark (helpers)
    #pragma mark

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark SocketMonitorSettingsDefaults
    #pragma mark

    class MessageQueueManagerSettingsDefaults : public ISettingsApplyDefaultsDelegate
    {
    public:
      //-----------------------------------------------------------------------
      ~MessageQueueManagerSettingsDefaults()
      {
        ISettings::removeDefaults(*this);
      }

      //-----------------------------------------------------------------------
      static MessageQueueManagerSettingsDefaultsPtr singleton()
      {
        static SingletonLazySharedPtr<MessageQueueManagerSettingsDefaults> singleton(create());
        return singleton.singleton();
      }

      //-----------------------------------------------------------------------
      static MessageQueueManagerSettingsDefaultsPtr create()
      {
        auto pThis(make_shared<MessageQueueManagerSettingsDefaults>());
        ISettings::installDefaults(pThis);
        return pThis;
      }

      //-----------------------------------------------------------------------
      virtual void notifySettingsApplyDefaults() override
      {
        ISettings::setBool(ZSLIB_SETTING_MESSAGE_QUEUE_MANAGER_PROCESS_APPLICATION_MESSAGE_QUEUE_ON_QUIT, false);
      }
    };

    //-------------------------------------------------------------------------
    void installMessageQueueManagerSettingsDefaults()
    {
      MessageQueueManagerSettingsDefaults::singleton();
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark MessageQueueManager
    #pragma mark

    //-------------------------------------------------------------------------
    MessageQueueManager::MessageQueueManager(const make_private &) :
      mPending(0),
      mProcessApplicationQueueOnShutdown(ISettings::getBool(ZSLIB_SETTING_MESSAGE_QUEUE_MANAGER_PROCESS_APPLICATION_MESSAGE_QUEUE_ON_QUIT))
    {
      ZS_LOG_BASIC(log("created"))
    }

    //-------------------------------------------------------------------------
    void MessageQueueManager::init()
    {
      // AutoRecursiveLock lock(mLock);
    }

    //-------------------------------------------------------------------------
    MessageQueueManagerPtr MessageQueueManager::create()
    {
      MessageQueueManagerPtr pThis(make_shared<MessageQueueManager>(make_private{}));
      pThis->mThisWeak = pThis;
      pThis->init();
      return pThis;
    }

    //-------------------------------------------------------------------------
    MessageQueueManagerPtr MessageQueueManager::singleton()
    {
      AutoRecursiveLock lock(*IHelper::getGlobalLock());
      static SingletonLazySharedPtr<MessageQueueManager> singleton(create());
      MessageQueueManagerPtr result = singleton.singleton();

      static zsLib::SingletonManager::Register registerSingleton("org.zsLib.MessageQueueManager", result);

      if (!result) {
        ZS_LOG_WARNING(Detail, slog("singleton gone"))
      }

      return result;
    }

    //-------------------------------------------------------------------------
    MessageQueueManager::~MessageQueueManager()
    {
      ZS_LOG_BASIC(log("destroyed"))
      mThisWeak.reset();
      cancel();
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark MessageQueueManager => IMessageQueueManager
    #pragma mark

    //-------------------------------------------------------------------------
    IMessageQueuePtr MessageQueueManager::getMessageQueueForGUIThread()
    {
      return getMessageQueue(ZSLIB_MESSAGE_QUEUE_MANAGER_RESERVED_GUI_THREAD_NAME);
    }

    //-------------------------------------------------------------------------
    IMessageQueuePtr MessageQueueManager::getMessageQueue(const char *assignedQueueName)
    {
      String name(assignedQueueName);

      AutoRecursiveLock lock(mLock);

      // scope: check thread queues
      {
        MessageQueueMap::iterator found = mQueues.find(name);
        if (found != mQueues.end()) {
          ZS_LOG_TRACE(log("re-using existing message queue with name") + ZS_PARAM("name", name))
          return (*found).second;
        }
      }

      IMessageQueuePtr queue;

      // creating new thread
      if (ZSLIB_MESSAGE_QUEUE_MANAGER_RESERVED_GUI_THREAD_NAME == name) {
        ZS_LOG_TRACE(log("creating GUI thread"))
        queue = IMessageQueueThread::singletonUsingCurrentGUIThreadsMessageQueue();
      } else {

        ThreadPriorities priority = zsLib::ThreadPriority_NormalPriority;

        ThreadPriorityMap::const_iterator foundPriority = mThreadPriorities.find(name);
        if (foundPriority != mThreadPriorities.end()) {
          priority = (*foundPriority).second;
        }

        ZS_LOG_TRACE(log("creating thread queue") + ZS_PARAM("name", name) + ZS_PARAM("priority", zsLib::toString(priority)))

        queue = IMessageQueueThread::createBasic(name, priority);
      }

      mQueues[name] = queue;
      return queue;
    }

    //-------------------------------------------------------------------------
    IMessageQueuePtr MessageQueueManager::getThreadPoolQueue(
                                                             const char *assignedThreadPoolQueueName,
                                                             const char *registeredQueueName,
                                                             size_t minThreadsRequired
                                                             )
    {
      String poolName(assignedThreadPoolQueueName);
      String name(registeredQueueName);

      AutoRecursiveLock lock(mLock);

      // scope: check registered queues
      if (name.hasData()) {
        auto found = mRegisteredPoolQueues.find(String(poolName + ":" + name));
        if (found != mRegisteredPoolQueues.end()) {
          ZS_LOG_TRACE(log("re-using existing pool message queue with name") + ZS_PARAM("name", poolName + ":" + name))
          return (*found).second;
        }
      }

      ThreadPriorities priority = zsLib::ThreadPriority_NormalPriority;

      ThreadPriorityMap::const_iterator foundPriority = mThreadPriorities.find(poolName);
      if (foundPriority != mThreadPriorities.end()) {
        priority = (*foundPriority).second;
      }

      IMessageQueueThreadPoolPtr pool;
      size_t totalThreadsCreated = 0;

      {
        auto found = mPools.find(poolName);
        if (found != mPools.end()) {
          pool = (*found).second.first;
          totalThreadsCreated = (*found).second.second;
        }
      }

      if (!pool) {
        ZS_LOG_TRACE(log("creating thread pool") + ZS_PARAM("name", poolName))
        pool = IMessageQueueThreadPool::create();
      }

      while (totalThreadsCreated < minThreadsRequired) {
        ++totalThreadsCreated;
        ZS_LOG_TRACE(log("creating pool thread") + ZS_PARAM("poolName", poolName + "." + string(totalThreadsCreated)) + ZS_PARAM("priority", zsLib::toString(priority)))
        pool->createThread((poolName + "." + string(totalThreadsCreated)).c_str(), priority);
      }

      mPools[poolName] = MessageQueueThreadPoolPair(pool, totalThreadsCreated);

      IMessageQueuePtr queue = pool->createQueue();

      if (name.hasData()) {
        ZS_LOG_TRACE(log("registering queue with name") + ZS_PARAM("name", poolName + ":" + name))
        mRegisteredPoolQueues[String(poolName + ":" + name)] = queue;
      }

      return queue;
    }

    //-------------------------------------------------------------------------
    void MessageQueueManager::registerMessageQueueThreadPriority(
                                                                  const char *assignedQueueName,
                                                                  ThreadPriorities priority
                                                                  )
    {
      ZS_DECLARE_TYPEDEF_PTR(zsLib::IMessageQueueThread, IMessageQueueThread);

      AutoRecursiveLock lock(mLock);

      String name(assignedQueueName);
      mThreadPriorities[name] = priority;

      bool inUse = false;

      // scope: fix existing queue thread priorities
      {
        auto found = mQueues.find(name);
        if (found != mQueues.end()) {
          ZS_LOG_DEBUG(log("updating message queue thread") + ZS_PARAM("name", name) + ZS_PARAM("priority", zsLib::toString(priority)));

          IMessageQueuePtr queue = (*found).second;

          IMessageQueueThreadPtr thread = ZS_DYNAMIC_PTR_CAST(IMessageQueueThread, queue);
          if (thread) {
            thread->setThreadPriority(priority);
            inUse = true;
          } else {
            ZS_LOG_WARNING(Detail, log("found thread was not recognized as a message queue thread") + ZS_PARAM("name", name));
          }
        }
      }

      // scope: fix existing pool queue thrad priorities
      {
        auto found = mPools.find(name);
        if (found != mPools.end()) {
          ZS_LOG_DEBUG(log("updating message queue thread pool") + ZS_PARAM("name", name) + ZS_PARAM("priority", zsLib::toString(priority)));

          IMessageQueueThreadPoolPtr pool = (*found).second.first;

          pool->setThreadPriority(priority);
          inUse = true;
        }
      }

      if (!inUse) {
        ZS_LOG_DEBUG(log("message queue specified is not in use at yet") + ZS_PARAM("name", name) + ZS_PARAM("priority", zsLib::toString(priority)));
      }
    }

    //-------------------------------------------------------------------------
    IMessageQueueManager::MessageQueueMapPtr MessageQueueManager::getRegisteredQueues()
    {
      AutoRecursiveLock lock(mLock);

      MessageQueueMapPtr result(make_shared<MessageQueueMap>(mQueues));
      for (auto iter = mRegisteredPoolQueues.begin(); iter != mRegisteredPoolQueues.end(); ++iter) {
        auto name = (*iter).first;
        auto queue = (*iter).second;
        (*result)[name] = queue;
      }
      return result;
    }

    //-------------------------------------------------------------------------
    size_t MessageQueueManager::getTotalUnprocessedMessages() const
    {
      AutoRecursiveLock lock(mLock);

      size_t result = 0;

      for (MessageQueueMap::const_iterator iter = mQueues.begin(); iter != mQueues.end(); ++iter) {
        const IMessageQueuePtr &queue = (*iter).second;
        result += queue->getTotalUnprocessedMessages();
      }

      for (auto iter = mPools.begin(); iter != mPools.end(); ++iter) {
        auto pool = (*iter).second.first;
        result += (pool->hasPendingMessages() ? 1 : 0);
      }

      return result;
    }

    //-------------------------------------------------------------------------
    void MessageQueueManager::shutdownAllQueues()
    {
      ZS_LOG_DETAIL(log("shutdown all queues called"))

      AutoRecursiveLock lock(mLock);
      mGracefulShutdownReference = mThisWeak.lock();

      onWake();
    }

    //-------------------------------------------------------------------------
    void MessageQueueManager::blockUntilDone()
    {
      bool processApplicationQueueOnShutdown = false;

      MessageQueueMap queues;
      MessageQueuePoolMap pools;

      {
        AutoRecursiveLock lock(mLock);
        processApplicationQueueOnShutdown = mProcessApplicationQueueOnShutdown;
        queues = mQueues;
        pools = mPools;
      }

      size_t totalRemaining = 0;

      do
      {
        totalRemaining = 0;

        for (MessageQueueMap::iterator iter = queues.begin(); iter != queues.end(); ++iter)
        {
          const MessageQueueName &name = (*iter).first;
          IMessageQueuePtr queue = (*iter).second;

          if (name == ZSLIB_MESSAGE_QUEUE_MANAGER_RESERVED_GUI_THREAD_NAME) {
            if (!processApplicationQueueOnShutdown)
              continue;

            IMessageQueueThreadPtr thread = ZS_DYNAMIC_PTR_CAST(IMessageQueueThread, queue);

            if (thread) {
              thread->processMessagesFromThread();
            }
          }

          totalRemaining += queue->getTotalUnprocessedMessages();
        }

        for (auto iter = pools.begin(); iter != pools.end(); ++iter) {
          auto pool = (*iter).second.first;
          if (pool->hasPendingMessages()) ++totalRemaining;
        }

        if (totalRemaining < 1) {
          break;
        }

        std::this_thread::yield();
      } while (totalRemaining > 0);
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark MessageQueueManager => IWakeDelegate
    #pragma mark

    //-------------------------------------------------------------------------
    void MessageQueueManager::onWake()
    {
      AutoRecursiveLock lock(mLock);

      if (!mGracefulShutdownReference) return;

      if (0 != mPending) {
        --mPending;
        if (0 != mPending) return;
      }

      if ((mQueues.size() < 1) &&
          (mPools.size() < 1)) {
        mGracefulShutdownReference.reset();
        return;
      }

      for (MessageQueueMap::iterator iter = mQueues.begin(); iter != mQueues.end(); ++iter)
      {
        IMessageQueuePtr queue = (*iter).second;

        size_t remaining = queue->getTotalUnprocessedMessages();
        if (0 == remaining) continue;

        ++mPending;

        IWakeDelegateProxy::create(queue, mThisWeak.lock())->onWake();
        std::this_thread::yield();
      }

      for (auto iter = mPools.begin(); iter != mPools.end(); ++iter) {
        auto pool = (*iter).second.first;

        bool hasPending = pool->hasPendingMessages();
        if (!hasPending) continue;

        ++mPending;

        IMessageQueuePtr tempQueue = pool->createQueue();
        IWakeDelegateProxy::create(tempQueue, mThisWeak.lock())->onWake();
        std::this_thread::yield();
      }

      if (0 != mPending) {
        mFinalCheck = false;
        return;
      }

      if (0 == mPending) {
        if (!mFinalCheck) {
          mFinalCheck = true;

          if (mQueues.size() > 0) {
            // perform one-time double check to truly make sure all queues are empty
            for (auto iter = mQueues.begin(); iter != mQueues.end(); ++iter) {
              const MessageQueueName &name = (*iter).first;
              IMessageQueuePtr queue = (*iter).second;

              if (name == ZSLIB_MESSAGE_QUEUE_MANAGER_RESERVED_GUI_THREAD_NAME)
                continue;

              ++mPending;
              IWakeDelegateProxy::create(queue, mThisWeak.lock())->onWake();
              std::this_thread::yield();
              return;
            }
          }

          if (mPools.size() > 0) {
            auto pool = (*(mPools.begin())).second.first;

            ++mPending;

            IMessageQueuePtr tempQueue = pool->createQueue();
            IWakeDelegateProxy::create(tempQueue, mThisWeak.lock())->onWake();
            std::this_thread::yield();
            return;
          }
        }
      }

      // all queue are empty
      mFinalCheckComplete = true;
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark MessageQueueManager => IMessageQueueManagerForBackgrounding
    #pragma mark

    //-------------------------------------------------------------------------
    void MessageQueueManager::notifySingletonCleanup()
    {
      shutdownAllQueues();
      blockUntilDone();
      while (true) {
        if (mFinalCheckComplete) break;
        std::this_thread::yield();
      }
      cancel();
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark MessageQueueManager => (internal)
    #pragma mark

    //-------------------------------------------------------------------------
    MessageQueueManager::Params MessageQueueManager::log(const char *message) const
    {
      ElementPtr objectEl = Element::create("MessageQueueManager");
      IHelper::debugAppend(objectEl, "id", mID);
      return Params(message, objectEl);
    }

    //-------------------------------------------------------------------------
    MessageQueueManager::Params MessageQueueManager::slog(const char *message)
    {
      return Params(message, "MessageQueueManager");
    }

    //-------------------------------------------------------------------------
    MessageQueueManager::Params MessageQueueManager::debug(const char *message) const
    {
      return Params(message, toDebug());
    }

    //-------------------------------------------------------------------------
    ElementPtr MessageQueueManager::toDebug() const
    {
      AutoRecursiveLock lock(mLock);
      ElementPtr resultEl = Element::create("MessageQueueManager");

      IHelper::debugAppend(resultEl, "id", mID);
      IHelper::debugAppend(resultEl, "graceful reference", (bool)mGracefulShutdownReference);
      IHelper::debugAppend(resultEl, "final check", mFinalCheck);

      IHelper::debugAppend(resultEl, "total queues", mQueues.size());
      IHelper::debugAppend(resultEl, "total priorities", mThreadPriorities.size());

      IHelper::debugAppend(resultEl, "pools", mPools.size());
      IHelper::debugAppend(resultEl, "registered pool queues", mRegisteredPoolQueues.size());

      IHelper::debugAppend(resultEl, "process application queue on shutdown", mProcessApplicationQueueOnShutdown);

      return resultEl;
    }

    //-------------------------------------------------------------------------
    void MessageQueueManager::cancel()
    {
      ZS_LOG_DEBUG(log("cancel called"))

      while (true)
      {
        MessageQueueMapPtr queues = getRegisteredQueues();

        for (MessageQueueMap::iterator iter = queues->begin(); iter != queues->end(); ++iter)
        {
          MessageQueueName name = (*iter).first;
          IMessageQueuePtr queue = (*iter).second;

          size_t totalMessagesLeft = queue->getTotalUnprocessedMessages();
          if (totalMessagesLeft > 0) {
            ZS_LOG_WARNING(Basic, log("unprocessed messages are still in the queue - did you check getTotalUnprocessedMessages() to make sure all queues are empty before quiting?"))
          }

          IMessageQueueThreadPtr threadQueue = ZS_DYNAMIC_PTR_CAST(IMessageQueueThread, queue);

          if (threadQueue) {
            threadQueue->waitForShutdown();
          }

          // scope: remove the queue from the list of managed queues
          {
            AutoRecursiveLock lock(mLock);

            MessageQueueMap::iterator found = mQueues.find(name);
            if (found == mQueues.end()) {
              ZS_LOG_WARNING(Detail, log("message queue was not found in managed list of queues") + ZS_PARAM("name", name))
            }
            mQueues.erase(found);
          }
        }

        MessageQueuePoolMap pools;

        {
          AutoRecursiveLock lock(mLock);
          pools = mPools;

          mPools.clear();
        }

        for (auto iter_doNotUse = pools.begin(); iter_doNotUse != pools.end(); ) {
          auto current = iter_doNotUse;
          ++iter_doNotUse;

          auto pool = (*current).second.first;

          pool->waitForShutdown();

          pools.erase(current);
        }

        if ((queues->size() < 1) &&
            (pools.size() < 1)) {
          ZS_LOG_DEBUG(log("all queues / pools are now gone"))
          break;
        }
      }

      mGracefulShutdownReference.reset();
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------

  } // namespace internal

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark IMessageQueueManager
  #pragma mark

  //---------------------------------------------------------------------------
  IMessageQueuePtr IMessageQueueManager::getMessageQueueForGUIThread()
  {
    internal::MessageQueueManagerPtr singleton = internal::MessageQueueManager::singleton();
    if (!singleton) return IMessageQueuePtr();
    return singleton->getMessageQueueForGUIThread();
  }

  //---------------------------------------------------------------------------
  IMessageQueuePtr IMessageQueueManager::getMessageQueue(const char *assignedQueueName)
  {
    internal::MessageQueueManagerPtr singleton = internal::MessageQueueManager::singleton();
    if (!singleton) return IMessageQueuePtr();
    return singleton->getMessageQueue(assignedQueueName);
  }

  //---------------------------------------------------------------------------
  IMessageQueuePtr IMessageQueueManager::getThreadPoolQueue(
                                                            const char *assignedThreadPoolQueueName,
                                                            const char *registeredQueueName,
                                                            size_t minThreadsRequired
                                                            )
  {
    internal::MessageQueueManagerPtr singleton = internal::MessageQueueManager::singleton();
    if (!singleton) return IMessageQueuePtr();
    return singleton->getThreadPoolQueue(assignedThreadPoolQueueName, registeredQueueName, minThreadsRequired);
  }

  //---------------------------------------------------------------------------
  void IMessageQueueManager::registerMessageQueueThreadPriority(
                                                                const char *assignedQueueName,
                                                                ThreadPriorities priority
                                                                )
  {
    internal::MessageQueueManagerPtr singleton = internal::MessageQueueManager::singleton();
    if (!singleton) return;
    singleton->registerMessageQueueThreadPriority(assignedQueueName, priority);
  }

  //---------------------------------------------------------------------------
  IMessageQueueManager::MessageQueueMapPtr IMessageQueueManager::getRegisteredQueues()
  {
    internal::MessageQueueManagerPtr singleton = internal::MessageQueueManager::singleton();
    if (!singleton) return make_shared<IMessageQueueManager::MessageQueueMap>();
    return singleton->getRegisteredQueues();
  }

  //---------------------------------------------------------------------------
  size_t IMessageQueueManager::getTotalUnprocessedMessages()
  {
    internal::MessageQueueManagerPtr singleton = internal::MessageQueueManager::singleton();
    if (!singleton) return 0;
    return singleton->getTotalUnprocessedMessages();
  }

  //---------------------------------------------------------------------------
  void IMessageQueueManager::shutdownAllQueues()
  {
    internal::MessageQueueManagerPtr singleton = internal::MessageQueueManager::singleton();
    if (!singleton) return;
    singleton->shutdownAllQueues();
  }

  //-------------------------------------------------------------------------
  void IMessageQueueManager::blockUntilDone()
  {
    internal::MessageQueueManagerPtr singleton = internal::MessageQueueManager::singleton();
    if (!singleton) return;
    singleton->blockUntilDone();
  }


} // namespace zsLib
