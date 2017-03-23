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

#pragma once

#include <zsLib/Proxy.h>

namespace zsLib
{
  ZS_DECLARE_INTERACTION_PROXY(IPromiseDelegate)

  interaction IPromiseDelegate
  {
    virtual void onPromiseSettled(PromisePtr promise) = 0;
    virtual void onPromiseResolved(PromisePtr promise) = 0;
    virtual void onPromiseRejected(PromisePtr promise) = 0;
  };
}

#include <zsLib/internal/zsLib_Promise.h>

namespace zsLib
{
  class Promise : public internal::Promise
  {
  public:
    typedef std::list<PromisePtr> PromiseList;
    typedef std::list<PromiseWeakPtr> PromiseWeakList;

  public:
    Promise(
            const make_private &,
            IMessageQueuePtr queue
            );
    Promise(
            const make_private &,
            const std::list<PromisePtr> &promises,
            IMessageQueuePtr queue
            );

  public:
    ~Promise();

    static PromisePtr create(IMessageQueuePtr queue = IMessageQueuePtr());
    static PromisePtr all(
                          const PromiseList &promises,
                          IMessageQueuePtr queue = IMessageQueuePtr()
                          );
    static PromisePtr allSettled(
                                 const PromiseList &promises,
                                 IMessageQueuePtr queue = IMessageQueuePtr()
                                 );
    static PromisePtr race(
                           const PromiseList &promises,
                           IMessageQueuePtr queue = IMessageQueuePtr()
                           );
    static PromisePtr broadcast(
                                const PromiseList &promises,
                                IMessageQueuePtr queue = IMessageQueuePtr()
                                );
    static PromisePtr broadcast(
                                const PromiseWeakList &promises,
                                IMessageQueuePtr queue = IMessageQueuePtr()
                                );

    static PromisePtr createResolved(IMessageQueuePtr queue) {return Promise::createResolved(AnyPtr(), queue);}
    static PromisePtr createResolved(
                                     AnyPtr value = AnyPtr(),
                                     IMessageQueuePtr queue = IMessageQueuePtr()
                                     );
    static PromisePtr createRejected(IMessageQueuePtr queue) {return Promise::createRejected(AnyPtr(), queue);}
    static PromisePtr createRejected(
                                     AnyPtr reason = AnyPtr(),
                                     IMessageQueuePtr queue = IMessageQueuePtr()
                                     );

    PUID getID() const {return mID;}

    void resolve(AnyPtr value = AnyPtr());
    void reject(AnyPtr reason = AnyPtr());

    static void resolveAll(
                           const PromiseList &promises,
                           AnyPtr value = AnyPtr()
                           );
    static void resolveAll(
                           const PromiseWeakList &promises,
                           AnyPtr value = AnyPtr()
                           );
    static void rejectAll(
                          const PromiseList &promises,
                          AnyPtr reason = AnyPtr()
                          );
    static void rejectAll(
                          const PromiseWeakList &promises,
                          AnyPtr reason = AnyPtr()
                          );

    void then(IPromiseDelegatePtr delegate);
    void thenWeak(IPromiseDelegatePtr delegate);

    bool isSettled() const;
    bool isResolved() const;
    bool isRejected() const;

    void background();

    const std::list<PromisePtr> &promises() const {return mPromises;}

    template <typename data_type>
    std::shared_ptr<data_type> value() const {return ZS_DYNAMIC_PTR_CAST(data_type, mValue);}

    template <typename data_type>
    std::shared_ptr<data_type> reason() const {return ZS_DYNAMIC_PTR_CAST(data_type, mReason);}

    template <typename data_type>
    std::shared_ptr<data_type> userData() const {return ZS_DYNAMIC_PTR_CAST(data_type, mUserData);}

    void setUserData(AnyPtr userData) {mUserData = userData;}

    template <typename data_type>
    std::shared_ptr<data_type> referenceHolder() const {return ZS_DYNAMIC_PTR_CAST(data_type, mReferenceHolder);}

    void setReferenceHolder(AnyPtr referenceHolder) {mReferenceHolder = referenceHolder;}

  protected:
    virtual void onPromiseSettled(PromisePtr promise) {}
    virtual void onPromiseResolved(PromisePtr promise);
    virtual void onPromiseRejected(PromisePtr promise);
  };

  template <typename DataType, typename ReasonType = zsLib::Any, typename UserType = zsLib::Any>
  class PromiseWith : public Promise
  {
  public:
    typedef DataType UseDataType;
    typedef std::shared_ptr<UseDataType> UseDataTypePtr;
    typedef std::weak_ptr<UseDataType> UseDataTypeWeakPtr;

    typedef ReasonType UseReasonType;
    typedef std::shared_ptr<UseReasonType> UseReasonTypePtr;
    typedef std::weak_ptr<UseReasonType> UseReasonTypeWeakPtr;

    typedef UserType UseUserType;
    typedef std::shared_ptr<UseUserType> UseUserTypePtr;
    typedef std::weak_ptr<UseUserType> UseUserTypeWeakPtr;

    typedef PromiseWith<DataType, ReasonType, UserType> PromiseWithType;
    typedef std::shared_ptr<PromiseWithType> PromiseWithTypePtr;
    typedef std::weak_ptr<PromiseWithType> PromiseWithTypeWeakPtr;

  public:
    PromiseWith(
                const make_private &,
                IMessageQueuePtr queue = IMessageQueuePtr()
                ) : Promise(make_private {}, queue) {}

  public:
    static PromiseWithTypePtr create(IMessageQueuePtr queue = IMessageQueuePtr()) {
      PromiseWithTypePtr pThis(make_shared<PromiseWith>(make_private{}, queue));
      pThis->mThisWeak = pThis;
      return pThis;
    }

    static PromiseWithTypePtr createFrom(PromisePtr genericPromise) {
      if (!genericPromise) return PromiseWithTypePtr();

      PromiseList promises;
      promises.push_back(genericPromise);
      IMessageQueuePtr queue = genericPromise->getAssociatedMessageQueue();
      PromiseWithTypePtr pThis(make_shared<PromiseWithType>(make_private{}, promises, queue));
      pThis->mThisWeak = pThis;
      genericPromise->thenWeak(pThis);
      return pThis;
    }

    static PromiseWithTypePtr createResolved(IMessageQueuePtr queue) {return PromiseWithType::createResolved(UseDataTypePtr(), queue);}
    static PromiseWithTypePtr createResolved(
                                             UseDataTypePtr value = UseDataTypePtr(),
                                             IMessageQueuePtr queue = IMessageQueuePtr()
                                             ) {
      PromiseWithTypePtr pThis(make_shared<PromiseWith>(make_private{}, queue));
      pThis->mThisWeak = pThis;
      pThis->resolve(value);
      return pThis;
    }
    static PromiseWithTypePtr createRejected(IMessageQueuePtr queue) {return PromiseWithType::createRejected(UseReasonTypePtr(), queue);}
    static PromiseWithTypePtr createRejected(
                                     UseReasonTypePtr reason = UseReasonTypePtr(),
                                     IMessageQueuePtr queue = IMessageQueuePtr()
                                     ) {
      PromiseWithTypePtr pThis(make_shared<PromiseWith>(make_private{}, queue));
      pThis->mThisWeak = pThis;
      pThis->reject(reason);
      return pThis;
    }

    UseDataTypePtr value() const {return Promise::value<DataType>();}
    UseReasonTypePtr reason() const {return Promise::reason<UseReasonType>();}
    UseUserTypePtr userData() const {return Promise::userData<UseUserType>();}
  };

  interaction IPromiseSettledDelegate : public IPromiseDelegate
  {
    virtual void onPromiseSettled(PromisePtr promise) = 0;
    virtual void onPromiseResolved(PromisePtr promise) {} // filtered
    virtual void onPromiseRejected(PromisePtr promise) {} // filtered
  };

  interaction IPromiseResolutionDelegate : public IPromiseDelegate
  {
    virtual void onPromiseSettled(PromisePtr promise) {}  // filtered
    virtual void onPromiseResolved(PromisePtr promise) = 0;
    virtual void onPromiseRejected(PromisePtr promise) = 0;
  };

  interaction IPromiseCatchDelegate : public IPromiseDelegate
  {
    virtual void onPromiseSettled(PromisePtr promise) {}  // filtered
    virtual void onPromiseResolved(PromisePtr promise) {} // filtered
    virtual void onPromiseRejected(PromisePtr promise) = 0;
  };
}

ZS_DECLARE_PROXY_WITH_DELEGATE_MESSAGE_QUEUE_OPTIONAL_BEGIN(zsLib::IPromiseDelegate)
ZS_DECLARE_PROXY_METHOD_1(onPromiseSettled, zsLib::PromisePtr)
ZS_DECLARE_PROXY_METHOD_1(onPromiseResolved, zsLib::PromisePtr)
ZS_DECLARE_PROXY_METHOD_1(onPromiseRejected, zsLib::PromisePtr)
ZS_DECLARE_PROXY_END()
