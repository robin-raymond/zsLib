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

#ifndef ZSLIB_INTERNAL_PROXY_SUBSCRIPTIONS_H_cf7229753b441247ccece9f3b92317ed96045660
#define ZSLIB_INTERNAL_PROXY_SUBSCRIPTIONS_H_cf7229753b441247ccece9f3b92317ed96045660

#include <zsLib/types.h>
#include <zsLib/helpers.h>

#include <map>

namespace zsLib
{
  ZS_DECLARE_FORWARD_SUBSYSTEM(zsLib)

  namespace internal
  {
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark ProxySubscriptions
    #pragma mark

    template <typename XINTERFACE, typename SUBSCRIPTIONBASECLASS>
    class ProxySubscriptions
    {
    public:
      class Subscription;
      class DelegateImpl;

      typedef SUBSCRIPTIONBASECLASS SubscriptionBaseClass;
      typedef boost::shared_ptr<SubscriptionBaseClass> SubscriptionBaseClassPtr;
      typedef boost::weak_ptr<SubscriptionBaseClass>   SubscriptionBaseClassWeakPtr;

      typedef boost::shared_ptr<Subscription> SubscriptionPtr;
      typedef boost::weak_ptr<Subscription>   SubscriptionWeakPtr;

      typedef boost::shared_ptr<XINTERFACE> DelegatePtr;
      typedef boost::weak_ptr<XINTERFACE>   DelegateWeakPtr;
      typedef zsLib::Proxy<XINTERFACE>      DelegateProxy;

      typedef std::pair<SubscriptionWeakPtr, DelegatePtr> SubscriptionDelegatePair;

      typedef std::map<Subscription *, SubscriptionDelegatePair> SubscriptionDelegateMap;
      typedef boost::shared_ptr<SubscriptionDelegateMap> SubscriptionDelegateMapPtr;
      typedef boost::weak_ptr<SubscriptionDelegateMap> SubscriptionDelegateMapWeakPtr;
      typedef typename SubscriptionDelegateMap::size_type size_type;

      typedef bool Bogus;
      typedef std::map<SubscriptionPtr, Bogus> SubscriptionBackgroundMap;

      typedef SUBSCRIPTIONBASECLASS BaseSubscription;
      typedef boost::shared_ptr<SUBSCRIPTIONBASECLASS> BaseSubscriptionPtr;
      typedef boost::weak_ptr<SUBSCRIPTIONBASECLASS> BaseSubscriptionWeakPtr;

      typedef boost::shared_ptr<DelegateImpl> DelegateImplPtr;
      typedef boost::weak_ptr<DelegateImpl>   DelegateImplWeakPtr;

    public:
      ProxySubscriptions() {}
      ~ProxySubscriptions() {}

      SubscriptionPtr subscribe(
                                DelegatePtr originalDelegate,
                                IMessageQueuePtr queue = IMessageQueuePtr(),
                                bool strongReferenceToDelgate = false
                                )
      {
        SubscriptionPtr subscription(new Subscription);
        subscription->mThisWeak = subscription;
        subscription->mDelegateImpl = mDelegateImpl;

        if (originalDelegate) {
          DelegatePtr delegate;
          if (strongReferenceToDelgate) {
            delegate = DelegateProxy::create(queue, originalDelegate);
          } else {
            delegate = DelegateProxy::createWeak(queue, originalDelegate);
          }
          mDelegateImpl->subscribe(subscription, delegate);
        }

        return subscription;
      }

      DelegatePtr delegate() const
      {
        return mDelegateImpl;
      }

      DelegatePtr delegate(
                           BaseSubscriptionPtr subscription,
                           bool strongReferenceToDelgate = false
                           ) const
      {
        if (!subscription) return DelegatePtr();

        DelegatePtr result = mDelegateImpl->find((Subscription *)(subscription.get()));
        if (!result) return result;

        if (strongReferenceToDelgate) {
          return DelegateProxy::create(result); //  create a strong reference
        }
        return DelegateProxy::createWeak(result); //  create a strong reference
      }

      void clear()
      {
        return mDelegateImpl->clear((Subscription *)NULL);
      }

      size_type size() const
      {
        return mDelegateImpl->size((Subscription *)NULL);
      }

    public:

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark ProxySubscriptions::Subscription
      #pragma mark

      class Subscription : public SUBSCRIPTIONBASECLASS
      {
      public:
        Subscription() :
          mID(zsLib::createPUID())
        {
        }

        ~Subscription()
        {
          mThisWeak.reset();
          cancel();
        }

        virtual PUID getID() const {return mID;}

        virtual void cancel()
        {
          mDelegateImpl->cancel(this);
        }

        virtual void background()
        {
          mDelegateImpl->background(mThisWeak.lock());
        }

      public:
        PUID mID;
        SubscriptionWeakPtr mThisWeak;
        DelegateImplPtr mDelegateImpl;
      };
      
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark ProxySubscriptions::DelegateImpl
      #pragma mark

      class DelegateImpl : public XINTERFACE
      {
      public:
        typedef ProxySubscriptions::SubscriptionDelegateMap SubscriptionDelegateMap;

        DelegateImpl() : mSubscriptions(new SubscriptionDelegateMap) {}
        ~DelegateImpl() {}

        template<typename PARAM>
        static void fillWithSubscription(PARAM &, SubscriptionWeakPtr &, SubscriptionPtr &, bool &filled)
        {
        }

        template<typename PARAM>
        static void fillWithSubscription(SubscriptionBaseClassPtr &result, SubscriptionWeakPtr &source, SubscriptionPtr &output, bool &filled)
        {
          output = source.lock();
          result = output;
          filled = true;
        }

        void subscribe(SubscriptionPtr &subscription, DelegatePtr &delegate)
        {
          AutoRecursiveLock lock(mLock);
          SubscriptionDelegateMapPtr temp(new SubscriptionDelegateMap(*mSubscriptions));
          (*temp)[subscription.get()] = SubscriptionDelegatePair(subscription, delegate);
          mSubscriptions = temp;
        }

        void cancel(Subscription *gone)
        {
          AutoRecursiveLock lock(mLock);
          SubscriptionDelegateMapPtr temp(new SubscriptionDelegateMap(*mSubscriptions));
          typename SubscriptionDelegateMap::iterator found = temp->find(gone);
          if (found == temp->end()) return;
          temp->erase(found);
          mSubscriptions = temp;
        }

        void background(SubscriptionPtr subscription)
        {
          AutoRecursiveLock lock(mLock);
          mBackgroundSubscriptions[subscription] = true;
        }

        void clear(Subscription *ignore)
        {
          AutoRecursiveLock lock(mLock);
          SubscriptionDelegateMapPtr temp(new SubscriptionDelegateMap);
          mSubscriptions = temp;
          mBackgroundSubscriptions.clear();
        }

        size_type size(Subscription *ignore)
        {
          AutoRecursiveLock lock(mLock);
          return mSubscriptions->size();
        }

        DelegatePtr find(Subscription *subscription)
        {
          AutoRecursiveLock lock(mLock);
          typename SubscriptionDelegateMap::iterator found = mSubscriptions->find(subscription);
          if (found == mSubscriptions->end()) return DelegatePtr();

          return (*found).second.second;
        }

      public:
        zsLib::RecursiveLock mLock;
        SubscriptionDelegateMapPtr mSubscriptions;

        SubscriptionBackgroundMap mBackgroundSubscriptions;
      };

    public:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark ProxySubscriptions => (data)
      #pragma mark

      DelegateImplPtr mDelegateImpl;
    };
  }
}

#define ZS_INTERNAL_DECLARE_INTERACTION_PROXY_SUBSCRIPTION(xInteractionName, xDelegateName)                                       \
  interaction xInteractionName;                                                                                                   \
  typedef boost::shared_ptr<xInteractionName> xInteractionName##Ptr;                                                              \
  typedef boost::weak_ptr<xInteractionName> xInteractionName##WeakPtr;                                                            \
  typedef zsLib::ProxySubscriptions<xDelegateName, xInteractionName> xDelegateName##Subscriptions;


#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_BEGIN(xInterface, xSubscriptionClass)                                             \
namespace zsLib                                                                                                                   \
{                                                                                                                                 \
  template<>                                                                                                                      \
  class ProxySubscriptions<xInterface, xSubscriptionClass> : public internal::ProxySubscriptions<xInterface, xSubscriptionClass>  \
  {                                                                                                                               \
  public:                                                                                                                         \
    typedef ProxySubscriptions<xInterface, xSubscriptionClass> ProxySubscriptionsType;                                            \
    class DerivedDelegateImpl;                                                                                                    \
                                                                                                                                  \
    ProxySubscriptions()                                                                                                          \
    {                                                                                                                             \
      mDelegateImpl = DelegateImplPtr(new DerivedDelegateImpl);                                                                   \
    }                                                                                                                             \
                                                                                                                                  \
    class DerivedDelegateImpl : public DelegateImpl                                                                               \
    {                                                                                                                             \
    public:                                                                                                                       \


#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_END()                                                                             \
    };                                                                                                                            \
  };                                                                                                                              \
}

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_TYPEDEF(xOriginalType, xTypeAlias)                                                \
    typedef xOriginalType xTypeAlias;


#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_TYPES_AND_VALUES(xSubscriptionsMapTypename, xSubscriptionsMapVariable, xSubscriptionsMapKeyTypename, xDelegatPtrTypename, xDelegateProxyTypename)  \
  typedef ProxySubscriptionsType::SubscriptionDelegateMap xSubscriptionsMapTypename;                                                                                                                      \
  typedef ProxySubscriptionsType::SubscriptionDelegateMap::key_type xSubscriptionsMapKeyTypename;                                                                                                         \
  typedef ProxySubscriptionsType::SubscriptionDelegateMap::value_type xDelegatPtrTypename;                                                                                                                \
  typedef ProxySubscriptionsType::DelegateProxy xDelegateProxyTypename;                                                                                                                                   \
  SubscriptionDelegateMapPtr _temp_subscriptions;                                                                                                                                                         \
  {                                                                                                                                                                                                       \
    AutoRecursiveLock lock(mLock);                                                                                                                                                                        \
    _temp_subscriptions = mSubscriptions;                                                                                                                                                                 \
  }                                                                                                                                                                                                       \
  xSubscriptionsMapTypename &xSubscriptionsMapVariable = (*_temp_subscriptions);

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_ITERATOR_VALUES(xIterator, xKeyValueName, xSusbcriptionWeakPtrValueName, xDelegatePtrValueName)  \
  const ProxySubscriptionsType::SubscriptionDelegateMap::key_type &xKeyValueName = (*xIterator).first; (void)xKeyValueName;                             \
  ProxySubscriptionsType::SubscriptionWeakPtr &xSusbcriptionWeakPtrValueName = (*xIterator).second.first; (void)xSusbcriptionWeakPtrValueName;          \
  ProxySubscriptionsType::DelegatePtr &xDelegatePtrValueName = (*xIterator).second.second; (void)xDelegatePtrValueName;

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_ERASE_KEY(xSubscriptionsMapKeyValue)                                                             \
  {DelegateImpl::cancel(xSubscriptionsMapKeyValue);}


#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_0(xMethod)                                                                                   \
    virtual void xMethod() {                                                                                                                        \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second.second->xMethod();                                                                                                      \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_1(xMethod,t1)                                                                                \
    virtual void xMethod(t1 v1) {                                                                                                                   \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1);                                                                                                    \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_2(xMethod,t1,t2)                                                                             \
    virtual void xMethod(t1 v1, t2 v2) {                                                                                                            \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                    \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1,v2);                                                                                                 \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_3(xMethod,t1,t2,t3)                                                                          \
    virtual void xMethod(t1 v1, t2 v2, t3 v3) {                                                                                                     \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                    \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1,v2,v3);                                                                                              \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_4(xMethod,t1,t2,t3,t4)                                                                       \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4) {                                                                                              \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                    \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1,v2,v3,v4);                                                                                           \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_5(xMethod,t1,t2,t3,t4,t5)                                                                    \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5) {                                                                                       \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                    \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5);                                                                                        \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_6(xMethod,t1,t2,t3,t4,t5,t6)                                                                 \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6) {                                                                                \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                    \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6);                                                                                     \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_7(xMethod,t1,t2,t3,t4,t5,t6,t7)                                                              \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7) {                                                                         \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                    \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7);                                                                                  \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_8(xMethod,t1,t2,t3,t4,t5,t6,t7,t8)                                                           \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8) {                                                                  \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                    \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8);                                                                               \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_9(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)                                                        \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9) {                                                           \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                    \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9);                                                                            \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_10(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)                                                   \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10) {                                                  \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                  \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10);                                                                        \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_11(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)                                               \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11) {                                         \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                  \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11);                                                                    \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_12(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)                                           \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12) {                                \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t12>(v12, (*current).second.first, result, filled);                                                                  \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12);                                                                \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_13(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)                                       \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13) {                       \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t12>(v12, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t13>(v13, (*current).second.first, result, filled);                                                                  \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13);                                                            \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_14(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)                                   \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14) {              \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t12>(v12, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t13>(v13, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t14>(v14, (*current).second.first, result, filled);                                                                  \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14);                                                        \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_15(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)                               \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15) {     \
      SubscriptionDelegateMapPtr subscription;                                                                                                      \
      {                                                                                                                                             \
        AutoRecursiveLock lock(mLock);                                                                                                              \
        subscription = mSubscriptions;                                                                                                              \
      }                                                                                                                                             \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                         \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          bool filled = false;                                                                                                                      \
          SubscriptionPtr result;                                                                                                                   \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                    \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t12>(v12, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t13>(v13, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t14>(v14, (*current).second.first, result, filled);                                                                  \
          fillWithSubscription<t15>(v15, (*current).second.first, result, filled);                                                                  \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                          \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15);                                                    \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          cancel((*current).first);                                                                                                                 \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_16(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)                                                                                                                   \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16) {                                                                                    \
      SubscriptionDelegateMapPtr subscription;                                                                                                                                                                                              \
      {                                                                                                                                                                                                                                     \
        AutoRecursiveLock lock(mLock);                                                                                                                                                                                                      \
        subscription = mSubscriptions;                                                                                                                                                                                                      \
      }                                                                                                                                                                                                                                     \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                                                                                                                 \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          bool filled = false;                                                                                                                                                                                                              \
          SubscriptionPtr result;                                                                                                                                                                                                           \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t12>(v12, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t13>(v13, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t14>(v14, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t15>(v15, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t16>(v16, (*current).second.first, result, filled);                                                                                                                                                          \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                                                                                                                  \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16);                                                                                                                                        \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          cancel((*current).first);                                                                                                                                                                                                         \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_17(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)                                                                                                               \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17) {                                                                           \
      SubscriptionDelegateMapPtr subscription;                                                                                                                                                                                              \
      {                                                                                                                                                                                                                                     \
        AutoRecursiveLock lock(mLock);                                                                                                                                                                                                      \
        subscription = mSubscriptions;                                                                                                                                                                                                      \
      }                                                                                                                                                                                                                                     \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                                                                                                                 \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          bool filled = false;                                                                                                                                                                                                              \
          SubscriptionPtr result;                                                                                                                                                                                                           \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t12>(v12, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t13>(v13, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t14>(v14, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t15>(v15, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t16>(v16, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t17>(v17, (*current).second.first, result, filled);                                                                                                                                                          \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                                                                                                                  \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17);                                                                                                                                    \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          cancel((*current).first);                                                                                                                                                                                                         \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_18(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)                                                                                                           \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18) {                                                                  \
      SubscriptionDelegateMapPtr subscription;                                                                                                                                                                                              \
      {                                                                                                                                                                                                                                     \
        AutoRecursiveLock lock(mLock);                                                                                                                                                                                                      \
        subscription = mSubscriptions;                                                                                                                                                                                                      \
      }                                                                                                                                                                                                                                     \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                                                                                                                 \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          bool filled = false;                                                                                                                                                                                                              \
          SubscriptionPtr result;                                                                                                                                                                                                           \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t12>(v12, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t13>(v13, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t14>(v14, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t15>(v15, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t16>(v16, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t17>(v17, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t18>(v18, (*current).second.first, result, filled);                                                                                                                                                          \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                                                                                                                  \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18);                                                                                                                                \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          cancel((*current).first);                                                                                                                                                                                                         \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_19(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)                                                                                                       \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18, t19 v19) {                                                         \
      SubscriptionDelegateMapPtr subscription;                                                                                                                                                                                              \
      {                                                                                                                                                                                                                                     \
        AutoRecursiveLock lock(mLock);                                                                                                                                                                                                      \
        subscription = mSubscriptions;                                                                                                                                                                                                      \
      }                                                                                                                                                                                                                                     \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                                                                                                                 \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          bool filled = false;                                                                                                                                                                                                              \
          SubscriptionPtr result;                                                                                                                                                                                                           \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t12>(v12, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t13>(v13, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t14>(v14, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t15>(v15, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t16>(v16, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t17>(v17, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t18>(v18, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t19>(v19, (*current).second.first, result, filled);                                                                                                                                                          \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                                                                                                                  \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19);                                                                                                                            \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          cancel((*current).first);                                                                                                                                                                                                         \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_20(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)                                                                                                   \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18, t19 v19, t20 v20) {                                                \
      SubscriptionDelegateMapPtr subscription;                                                                                                                                                                                              \
      {                                                                                                                                                                                                                                     \
        AutoRecursiveLock lock(mLock);                                                                                                                                                                                                      \
        subscription = mSubscriptions;                                                                                                                                                                                                      \
      }                                                                                                                                                                                                                                     \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                                                                                                                 \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          bool filled = false;                                                                                                                                                                                                              \
          SubscriptionPtr result;                                                                                                                                                                                                           \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t12>(v12, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t13>(v13, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t14>(v14, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t15>(v15, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t16>(v16, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t17>(v17, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t18>(v18, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t19>(v19, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t20>(v20, (*current).second.first, result, filled);                                                                                                                                                          \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                                                                                                                  \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20);                                                                                                                        \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          cancel((*current).first);                                                                                                                                                                                                         \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_21(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)                                                                                               \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18, t19 v19, t20 v20, t21 v21) {                                       \
      SubscriptionDelegateMapPtr subscription;                                                                                                                                                                                              \
      {                                                                                                                                                                                                                                     \
        AutoRecursiveLock lock(mLock);                                                                                                                                                                                                      \
        subscription = mSubscriptions;                                                                                                                                                                                                      \
      }                                                                                                                                                                                                                                     \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                                                                                                                 \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          bool filled = false;                                                                                                                                                                                                              \
          SubscriptionPtr result;                                                                                                                                                                                                           \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t12>(v12, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t13>(v13, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t14>(v14, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t15>(v15, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t16>(v16, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t17>(v17, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t18>(v18, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t19>(v19, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t20>(v20, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t21>(v21, (*current).second.first, result, filled);                                                                                                                                                          \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                                                                                                                  \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20,v21);                                                                                                                    \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          cancel((*current).first);                                                                                                                                                                                                         \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_22(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)                                                                                           \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18, t19 v19, t20 v20, t21 v21, t22 v22) {                              \
      SubscriptionDelegateMapPtr subscription;                                                                                                                                                                                              \
      {                                                                                                                                                                                                                                     \
        AutoRecursiveLock lock(mLock);                                                                                                                                                                                                      \
        subscription = mSubscriptions;                                                                                                                                                                                                      \
      }                                                                                                                                                                                                                                     \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                                                                                                                 \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          bool filled = false;                                                                                                                                                                                                              \
          SubscriptionPtr result;                                                                                                                                                                                                           \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t12>(v12, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t13>(v13, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t14>(v14, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t15>(v15, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t16>(v16, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t17>(v17, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t18>(v18, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t19>(v19, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t20>(v20, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t21>(v21, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t22>(v22, (*current).second.first, result, filled);                                                                                                                                                          \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                                                                                                                  \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20,v21,v22);                                                                                                                \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          cancel((*current).first);                                                                                                                                                                                                         \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_23(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)                                                                                       \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18, t19 v19, t20 v20, t21 v21, t22 v22, t23 v23) {                     \
      SubscriptionDelegateMapPtr subscription;                                                                                                                                                                                              \
      {                                                                                                                                                                                                                                     \
        AutoRecursiveLock lock(mLock);                                                                                                                                                                                                      \
        subscription = mSubscriptions;                                                                                                                                                                                                      \
      }                                                                                                                                                                                                                                     \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                                                                                                                 \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          bool filled = false;                                                                                                                                                                                                              \
          SubscriptionPtr result;                                                                                                                                                                                                           \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t12>(v12, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t13>(v13, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t14>(v14, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t15>(v15, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t16>(v16, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t17>(v17, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t18>(v18, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t19>(v19, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t20>(v20, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t21>(v21, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t22>(v22, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t23>(v23, (*current).second.first, result, filled);                                                                                                                                                          \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                                                                                                                  \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20,v21,v22,v23);                                                                                                            \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          cancel((*current).first);                                                                                                                                                                                                         \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_24(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)                                                                                   \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18, t19 v19, t20 v20, t21 v21, t22 v22, t23 v23, t24 v24) {            \
      SubscriptionDelegateMapPtr subscription;                                                                                                                                                                                              \
      {                                                                                                                                                                                                                                     \
        AutoRecursiveLock lock(mLock);                                                                                                                                                                                                      \
        subscription = mSubscriptions;                                                                                                                                                                                                      \
      }                                                                                                                                                                                                                                     \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                                                                                                                 \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          bool filled = false;                                                                                                                                                                                                              \
          SubscriptionPtr result;                                                                                                                                                                                                           \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t12>(v12, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t13>(v13, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t14>(v14, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t15>(v15, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t16>(v16, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t17>(v17, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t18>(v18, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t19>(v19, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t20>(v20, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t21>(v21, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t22>(v22, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t23>(v23, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t24>(v24, (*current).second.first, result, filled);                                                                                                                                                          \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                                                                                                                  \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20,v21,v22,v23,v24);                                                                                                        \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          cancel((*current).first);                                                                                                                                                                                                         \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_25(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)                                                                               \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18, t19 v19, t20 v20, t21 v21, t22 v22, t23 v23, t24 v24, t25 v25) {   \
      SubscriptionDelegateMapPtr subscription;                                                                                                                                                                                              \
      {                                                                                                                                                                                                                                     \
        AutoRecursiveLock lock(mLock);                                                                                                                                                                                                      \
        subscription = mSubscriptions;                                                                                                                                                                                                      \
      }                                                                                                                                                                                                                                     \
      for (SubscriptionDelegateMap::iterator iter = subscription->begin(); iter != subscription->end(); ) {                                                                                                                                 \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          bool filled = false;                                                                                                                                                                                                              \
          SubscriptionPtr result;                                                                                                                                                                                                           \
          fillWithSubscription<t1>(v1, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t2>(v2, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t3>(v3, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t4>(v4, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t5>(v5, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t6>(v6, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t7>(v7, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t8>(v8, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t9>(v9, (*current).second.first, result, filled);                                                                                                                                                            \
          fillWithSubscription<t10>(v10, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t11>(v11, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t12>(v12, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t13>(v13, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t14>(v14, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t15>(v15, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t16>(v16, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t17>(v17, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t18>(v18, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t19>(v19, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t20>(v20, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t21>(v21, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t22>(v22, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t23>(v23, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t24>(v24, (*current).second.first, result, filled);                                                                                                                                                          \
          fillWithSubscription<t25>(v25, (*current).second.first, result, filled);                                                                                                                                                          \
          if ((filled) && (!result)) {cancel((*current).first); continue;}                                                                                                                                                                  \
          (*current).second.second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20,v21,v22,v23,v24,v25);                                                                                                    \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          cancel((*current).first);                                                                                                                                                                                                         \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#endif //ZSLIB_INTERNAL_PROXY_SUBSCRIPTIONS_H_cf7229753b441247ccece9f3b92317ed96045660
