/*

 Copyright (c) 2015, Robin Raymond
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

#include <zsLib/Promise.h>

#include <map>

namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib)}


namespace zsLib
{
  namespace internal
  {
    ZS_DECLARE_CLASS_PTR(PromiseMultiDelegate)
    ZS_DECLARE_CLASS_PTR(PromiseBroadcastDelegate)

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark Promise
    #pragma mark

    const char *Promise::toString(PromiseStates state)
    {
      switch (state) {
        case PromiseState_Pending:  return "pending";
        case PromiseState_Resolved: return "resolve";
        case PromiseState_Rejected: return "rejected";
      }
      return "unknown";
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark PromiseMultiDelegate
    #pragma mark

    class PromiseMultiDelegate : public zsLib::Promise
    {
    private:
      //-----------------------------------------------------------------------
      PromiseMultiDelegate(
                           const std::list<PromisePtr> &promises,
                           IMessageQueuePtr queue
                           ) :
        Promise(promises, queue)
      {}

    public:
      //-----------------------------------------------------------------------
      static PromiseMultiDelegatePtr create(
                                            IMessageQueuePtr queue,
                                            const std::list<PromisePtr> &promises,
                                            bool allMode,
                                            bool ignoreRejections
                                            )
      {
        PromiseMultiDelegatePtr pThis(new PromiseMultiDelegate(promises, queue));
        pThis->mAllMode = allMode;
        pThis->mIgnoredRejections = ignoreRejections;
        pThis->mThisWeak = pThis;

        // capture all promise information
        {
          for (auto iter = promises.begin(); iter != promises.end(); ++iter)
          {
            auto promise = (*iter);
            pThis->mPendingPromises[promise->getID()] = promise;
          }
        }

        // link to this
        {
          for (auto iter = promises.begin(); iter != promises.end(); ++iter)
          {
            auto promise = (*iter);
            promise->thenWeak(pThis);
          }
        }

        if (promises.size() < 1) {
          pThis->resolve();
        }

        return pThis;
      }

    protected:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark PromiseMultiDelegate => IPromiseDelegate
      #pragma mark

      //-----------------------------------------------------------------------
      virtual void onPromiseSettled(PromisePtr) {}

      //-----------------------------------------------------------------------
      virtual void onPromiseResolved(PromisePtr promise)
      {
        PromisePtr pThis;

        {
          AutoRecursiveLock lock(mLock);

          pThis = mThisWeak.lock();
          if (!pThis) return;

          if (mFired) return;

          if (mAllMode) {
            auto found = mPendingPromises.find(promise->getID());
            if (found == mPendingPromises.end()) return;

            mPendingPromises.erase(found);
            if (mPendingPromises.size() > 0) return;
          } else {
            mPendingPromises.clear();
          }

          mFired = true;
        }

        pThis->resolve();
      }

      //-----------------------------------------------------------------------
      virtual void onPromiseRejected(PromisePtr promise)
      {
        PromisePtr pThis;

        {
          AutoRecursiveLock lock(mLock);

          pThis = mThisWeak.lock();
          if (!pThis) return;

          if (mFired) return;

          if (mAllMode) {
            if (mIgnoredRejections) {
              auto found = mPendingPromises.find(promise->getID());
              if (found == mPendingPromises.end()) return;

              mPendingPromises.erase(found);
              if (mPendingPromises.size() > 0) return;
            }
          }

          mFired = true;
          mPendingPromises.clear();
        }

        pThis->reject(promise->reason<Any>());
      }

    private:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark PromiseMultiDelegate => (internal)
      #pragma mark

      typedef PUID PromiseID;
      typedef std::map<PromiseID, PromisePtr> PromiseMap;

      bool mAllMode {false};
      bool mIgnoredRejections {false};

      PromiseMap mPendingPromises;
    };


    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark PromiseBroadcastDelegate
    #pragma mark

    class PromiseBroadcastDelegate : public zsLib::Promise
    {
    private:
      //-----------------------------------------------------------------------
      PromiseBroadcastDelegate(
                              const std::list<PromisePtr> &promises,
                              IMessageQueuePtr queue
                              ) :
        Promise(promises, queue)
      {}

    public:
      //-----------------------------------------------------------------------
      static PromiseBroadcastDelegatePtr create(
                                               IMessageQueuePtr queue,
                                               const std::list<PromisePtr> &promises
                                               )
      {
        PromiseBroadcastDelegatePtr pThis(new PromiseBroadcastDelegate(promises, queue));
        pThis->mThisWeak = pThis;
        pThis->thenWeak(pThis);
        return pThis;
      }

    protected:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark PromiseAllDelegate => IPromiseDelegate
      #pragma mark

      //-----------------------------------------------------------------------
      virtual void onPromiseSettled(PromisePtr) {}

      //-----------------------------------------------------------------------
      virtual void onPromiseResolved(PromisePtr promise)
      {
        {
          AutoRecursiveLock lock(mLock);

          if (mFired) return;
          mFired = true;
        }

        for (auto iter = mPromises.begin(); iter != mPromises.end(); ++iter)
        {
          auto promise = (*iter);
          promise->resolve(promise->value<Any>());
        }
      }

      //-----------------------------------------------------------------------
      virtual void onPromiseRejected(PromisePtr promise)
      {
        {
          AutoRecursiveLock lock(mLock);

          if (mFired) return;
          mFired = true;
        }

        for (auto iter = mPromises.begin(); iter != mPromises.end(); ++iter)
        {
          auto promise = (*iter);
          promise->reject(promise->reason<Any>());
        }
      }

    private:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark PromiseBroacastDelegate => (internal)
      #pragma mark

    };
  }

  //-------------------------------------------------------------------------
  //-------------------------------------------------------------------------
  //-------------------------------------------------------------------------
  //-------------------------------------------------------------------------
  #pragma mark
  #pragma mark Promise
  #pragma mark

  //---------------------------------------------------------------------------
  Promise::Promise(IMessageQueuePtr queue) :
    internal::Promise(queue)
  {
  }

  //---------------------------------------------------------------------------
  Promise::Promise(
                   const std::list<PromisePtr> &promises,
                   IMessageQueuePtr queue
                   ) :
    internal::Promise(promises, queue)
  {
  }

  //---------------------------------------------------------------------------
  Promise::~Promise()
  {
    mThisWeak.reset();
  }

  //---------------------------------------------------------------------------
  PromisePtr Promise::create(IMessageQueuePtr queue)
  {
    PromisePtr pThis(new Promise(queue));
    pThis->mThisWeak = pThis;
    return pThis;
  }

  //---------------------------------------------------------------------------
  PromisePtr Promise::all(
                          const PromiseList &promises,
                          IMessageQueuePtr queue
                          )
  {
    return internal::PromiseMultiDelegate::create(queue, promises, true, false);
  }

  //---------------------------------------------------------------------------
  PromisePtr Promise::allSettled(
                                 const PromiseList &promises,
                                 IMessageQueuePtr queue
                                 )
  {
    return internal::PromiseMultiDelegate::create(queue, promises, true, true);
  }

  //---------------------------------------------------------------------------
  PromisePtr Promise::race(
                           const PromiseList &promises,
                           IMessageQueuePtr queue
                           )
  {
    return internal::PromiseMultiDelegate::create(queue, promises, false, false);
  }

  //---------------------------------------------------------------------------
  PromisePtr Promise::broadcast(
                                const PromiseList &promises,
                                IMessageQueuePtr queue
                                )
  {
    return internal::PromiseBroadcastDelegate::create(queue, promises);
  }

  //---------------------------------------------------------------------------
  PromisePtr Promise::broadcast(
                                const PromiseWeakList &promises,
                                IMessageQueuePtr queue
                                )
  {
    PromiseList tempPromises;
    for (auto iter = promises.begin(); iter != promises.end(); ++iter) {
      auto promise = (*iter).lock();
      tempPromises.push_back(promise);
    }
    return internal::PromiseBroadcastDelegate::create(queue, tempPromises);
  }

  //---------------------------------------------------------------------------
  PromisePtr Promise::createResolved(
                                     AnyPtr value,
                                     IMessageQueuePtr queue
                                     )
  {
    PromisePtr promise = Promise::create(queue);
    promise->resolve(value);
    return promise;
  }

  //---------------------------------------------------------------------------
  PromisePtr Promise::createRejected(
                                     AnyPtr reason,
                                     IMessageQueuePtr queue
                                     )
  {
    PromisePtr promise = Promise::create(queue);
    promise->reject(reason);
    return promise;
  }

  //---------------------------------------------------------------------------
  void Promise::resolve(AnyPtr value)
  {
    IPromiseDelegatePtr delegate;

    {
      AutoRecursiveLock lock(mLock);
      ZS_THROW_INVALID_USAGE_IF(isSettled())

      mState = PromiseState_Resolved;
      mValue = value;
      if (mThen) {
        delegate = IPromiseDelegateProxy::create(getAssociatedMessageQueue(), mThen);
      } else {
        IPromiseDelegatePtr then = mThenWeak.lock();
        if (then) {
          delegate = IPromiseDelegateProxy::create(getAssociatedMessageQueue(), then);
        }
      }

      mThisBackground.reset();
      mThen.reset();
      mThenWeak.reset();
    }

    if (delegate) {
      auto pThis = mThisWeak.lock();
      try {
        delegate->onPromiseResolved(pThis);
        delegate->onPromiseSettled(pThis);
      } catch(IPromiseDelegateProxy::Exceptions::DelegateGone &) {
      }
    }
  }

  //---------------------------------------------------------------------------
  void Promise::reject(AnyPtr reason)
  {
    IPromiseDelegatePtr delegate;

    {
      AutoRecursiveLock lock(mLock);
      ZS_THROW_INVALID_USAGE_IF(isSettled())

      mState = PromiseState_Rejected;
      mReason = reason;

      if (mThen) {
        delegate = IPromiseDelegateProxy::create(getAssociatedMessageQueue(), mThen);
      } else {
        IPromiseDelegatePtr then = mThenWeak.lock();
        if (then) {
          delegate = IPromiseDelegateProxy::create(getAssociatedMessageQueue(), then);
        }
      }

      mThisBackground.reset();
      mThen.reset();
      mThenWeak.reset();
    }

    if (delegate) {
      auto pThis = mThisWeak.lock();
      try {
        delegate->onPromiseRejected(pThis);
        delegate->onPromiseSettled(pThis);
      } catch(IPromiseDelegateProxy::Exceptions::DelegateGone &) {
      }
    }
  }

  //---------------------------------------------------------------------------
  void Promise::resolveAll(
                           const PromiseList &promises,
                           AnyPtr value
                           )
  {
    for (auto iter = promises.begin(); iter != promises.end(); ++iter) {
      auto promise = (*iter);
      if (!promise) continue;
      promise->resolve(value);
    }
  }

  //---------------------------------------------------------------------------
  void Promise::resolveAll(
                           const PromiseWeakList &promises,
                           AnyPtr value
                           )
  {
    for (auto iter = promises.begin(); iter != promises.end(); ++iter) {
      auto promise = (*iter).lock();
      if (!promise) continue;
      promise->resolve(value);
    }
  }

  //---------------------------------------------------------------------------
  void Promise::rejectAll(
                          const PromiseList &promises,
                          AnyPtr reason
                          )
  {
    for (auto iter = promises.begin(); iter != promises.end(); ++iter) {
      auto promise = (*iter);
      if (!promise) continue;
      promise->reject(reason);
    }
  }

  //---------------------------------------------------------------------------
  void Promise::rejectAll(
                          const PromiseWeakList &promises,
                          AnyPtr reason
                          )
  {
    for (auto iter = promises.begin(); iter != promises.end(); ++iter) {
      auto promise = (*iter).lock();
      if (!promise) continue;
      promise->reject(reason);
    }
  }
  
  //---------------------------------------------------------------------------
  void Promise::then(IPromiseDelegatePtr inDelegate)
  {
    IPromiseDelegatePtr delegate;

    {
      AutoRecursiveLock lock(mLock);

      if (!isSettled()) {
        mThen = inDelegate;
        mThenWeak.reset();
        return;
      }

      delegate = IPromiseDelegateProxy::create(getAssociatedMessageQueue(), inDelegate);
      if (!delegate) return;

      if (isResolved()) goto resolved;
      goto rejected;
    }

  resolved:
    {
      auto pThis = mThisWeak.lock();
      try {
        delegate->onPromiseResolved(pThis);
        delegate->onPromiseSettled(pThis);
      } catch(IPromiseDelegateProxy::Exceptions::DelegateGone &) {
      }
      return;
    }

  rejected:
    {
      auto pThis = mThisWeak.lock();
      try {
        delegate->onPromiseRejected(pThis);
        delegate->onPromiseSettled(pThis);
      } catch(IPromiseDelegateProxy::Exceptions::DelegateGone &) {
      }
      return;
    }
  }

  //---------------------------------------------------------------------------
  void Promise::thenWeak(IPromiseDelegatePtr inDelegate)
  {
    {
      AutoRecursiveLock lock(mLock);
      if (isSettled()) {
        then(inDelegate);
        return;
      }

      mThen.reset();
      mThenWeak = inDelegate;
    }
  }

  //---------------------------------------------------------------------------
  bool Promise::isSettled() const
  {
    AutoRecursiveLock lock(mLock);
    return PromiseState_Pending != mState;
  }

  //---------------------------------------------------------------------------
  bool Promise::isResolved() const
  {
    AutoRecursiveLock lock(mLock);
    return PromiseState_Resolved == mState;
  }

  //---------------------------------------------------------------------------
  bool Promise::isRejected() const
  {
    AutoRecursiveLock lock(mLock);
    return PromiseState_Rejected == mState;
  }

  //---------------------------------------------------------------------------
  void Promise::background()
  {
    AutoRecursiveLock lock(mLock);

    if (isSettled()) return;

    ZS_THROW_INVALID_USAGE_IF(!mThen)

    mThisBackground = mThisWeak.lock();
  }

  //---------------------------------------------------------------------------
  void Promise::onPromiseResolved(PromisePtr promise)
  {
    {
      AutoRecursiveLock lock(mLock);

      if (isSettled()) return;
      if (mFired) return;

      mFired = true;
    }

    resolve(promise->value<Any>());
  }

  //---------------------------------------------------------------------------
  void Promise::onPromiseRejected(PromisePtr promise)
  {
    {
      AutoRecursiveLock lock(mLock);

      if (isSettled()) return;
      if (mFired) return;

      mFired = true;
    }

    reject(promise->reason<Any>());
  }
}
