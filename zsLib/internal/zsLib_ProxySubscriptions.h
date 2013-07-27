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

      typedef boost::shared_ptr<XINTERFACE> DelegatePtr;
      typedef boost::weak_ptr<XINTERFACE>   DelegateWeakPtr;
      typedef zsLib::Proxy<XINTERFACE>      DelegateProxy;

      typedef std::map<Subscription *, DelegatePtr> SubscriptionDelegateMap;
      typedef typename SubscriptionDelegateMap::size_type size_type;

      typedef boost::shared_ptr<Subscription> SubscriptionPtr;
      typedef boost::weak_ptr<Subscription>   SubscriptionWeakPtr;

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
          mDelegateImpl->subscribe(subscription.get(), delegate);
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

        ~DelegateImpl() {}

        void subscribe(Subscription *subscription, DelegatePtr &delegate)
        {
          AutoRecursiveLock lock(mLock);
          mSubscriptions[subscription] = delegate;
        }

        void cancel(Subscription *gone)
        {
          AutoRecursiveLock lock(mLock);
          typename SubscriptionDelegateMap::iterator found = mSubscriptions.find(gone);
          if (found == mSubscriptions.end()) return;

          mSubscriptions.erase(found);
        }

        void background(SubscriptionPtr subscription)
        {
          AutoRecursiveLock lock(mLock);
          mBackgroundSubscriptions[subscription] = true;
        }

        void clear(Subscription *ignore)
        {
          AutoRecursiveLock lock(mLock);
          mSubscriptions.clear();
          mBackgroundSubscriptions.clear();
        }

        size_type size(Subscription *ignore)
        {
          AutoRecursiveLock lock(mLock);
          return mSubscriptions.size();
        }

        DelegatePtr find(Subscription *subscription)
        {
          AutoRecursiveLock lock(mLock);
          typename SubscriptionDelegateMap::iterator found = mSubscriptions.find(subscription);
          if (found == mSubscriptions.end()) return DelegatePtr();

          return (*found).second;
        }

      public:
        zsLib::RecursiveLock mLock;
        SubscriptionDelegateMap mSubscriptions;

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

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_BEGIN(xInterface, xSubscriptionClass)                                             \
namespace zsLib                                                                                                                   \
{                                                                                                                                 \
  template<>                                                                                                                      \
  class ProxySubscriptions<xInterface, xSubscriptionClass> : public internal::ProxySubscriptions<xInterface, xSubscriptionClass>  \
  {                                                                                                                               \
  public:                                                                                                                         \
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


#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_0(xMethod)                                                                                   \
    virtual void xMethod() {                                                                                                                        \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod();                                                                                                             \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_1(xMethod,t1)                                                                                \
    virtual void xMethod(t1 v1) {                                                                                                                   \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1);                                                                                                           \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_2(xMethod,t1,t2)                                                                             \
    virtual void xMethod(t1 v1, t2 v2) {                                                                                                            \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1,v2);                                                                                                        \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_3(xMethod,t1,t2,t3)                                                                          \
    virtual void xMethod(t1 v1, t2 v2, t3 v3) {                                                                                                     \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1,v2,v3);                                                                                                     \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_4(xMethod,t1,t2,t3,t4)                                                                       \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4) {                                                                                              \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1,v2,v3,v4);                                                                                                  \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_5(xMethod,t1,t2,t3,t4,t5)                                                                    \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5) {                                                                                       \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1,v2,v3,v4,v5);                                                                                               \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_6(xMethod,t1,t2,t3,t4,t5,t6)                                                                 \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6) {                                                                                \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6);                                                                                            \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_7(xMethod,t1,t2,t3,t4,t5,t6,t7)                                                              \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7) {                                                                         \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7);                                                                                         \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_8(xMethod,t1,t2,t3,t4,t5,t6,t7,t8)                                                           \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8) {                                                                  \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8);                                                                                      \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_9(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)                                                        \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9) {                                                           \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9);                                                                                   \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_10(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)                                                   \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10) {                                                  \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10);                                                                               \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_11(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)                                               \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11) {                                         \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11);                                                                           \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_12(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)                                           \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12) {                                \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12);                                                                       \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_13(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)                                       \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13) {                       \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13);                                                                   \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_14(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)                                   \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14) {              \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14);                                                               \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_15(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)                               \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15) {     \
      AutoRecursiveLock lock(mLock);                                                                                                                \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                       \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                   \
        try {                                                                                                                                       \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15);                                                           \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                        \
          mSubscriptions.erase(current);                                                                                                            \
        }                                                                                                                                           \
      }                                                                                                                                             \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_16(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)                                                                                                                   \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16) {                                                                                    \
      AutoRecursiveLock lock(mLock);                                                                                                                                                                                                        \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                                                                                                               \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16);                                                                                                                                               \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          mSubscriptions.erase(current);                                                                                                                                                                                                    \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_17(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)                                                                                                               \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17) {                                                                           \
      AutoRecursiveLock lock(mLock);                                                                                                                                                                                                        \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                                                                                                               \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17);                                                                                                                                           \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          mSubscriptions.erase(current);                                                                                                                                                                                                    \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_18(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)                                                                                                           \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18) {                                                                  \
      AutoRecursiveLock lock(mLock);                                                                                                                                                                                                        \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                                                                                                               \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18);                                                                                                                                       \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          mSubscriptions.erase(current);                                                                                                                                                                                                    \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_19(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)                                                                                                       \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18, t19 v19) {                                                         \
      AutoRecursiveLock lock(mLock);                                                                                                                                                                                                        \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                                                                                                               \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19);                                                                                                                                   \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          mSubscriptions.erase(current);                                                                                                                                                                                                    \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_20(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)                                                                                                   \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18, t19 v19, t20 v20) {                                                \
      AutoRecursiveLock lock(mLock);                                                                                                                                                                                                        \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                                                                                                               \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20);                                                                                                                               \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          mSubscriptions.erase(current);                                                                                                                                                                                                    \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_21(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)                                                                                               \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18, t19 v19, t20 v20, t21 v21) {                                       \
      AutoRecursiveLock lock(mLock);                                                                                                                                                                                                        \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                                                                                                               \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20,v21);                                                                                                                           \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          mSubscriptions.erase(current);                                                                                                                                                                                                    \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_22(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)                                                                                           \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18, t19 v19, t20 v20, t21 v21, t22 v22) {                              \
      AutoRecursiveLock lock(mLock);                                                                                                                                                                                                        \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                                                                                                               \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20,v21,v22);                                                                                                                       \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          mSubscriptions.erase(current);                                                                                                                                                                                                    \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_23(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)                                                                                       \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18, t19 v19, t20 v20, t21 v21, t22 v22, t23 v23) {                     \
      AutoRecursiveLock lock(mLock);                                                                                                                                                                                                        \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                                                                                                               \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20,v21,v22,v23);                                                                                                                   \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          mSubscriptions.erase(current);                                                                                                                                                                                                    \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_24(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)                                                                                   \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18, t19 v19, t20 v20, t21 v21, t22 v22, t23 v23, t24 v24) {            \
      AutoRecursiveLock lock(mLock);                                                                                                                                                                                                        \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                                                                                                               \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20,v21,v22,v23,v24);                                                                                                               \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          mSubscriptions.erase(current);                                                                                                                                                                                                    \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#define ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_25(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)                                                                               \
    virtual void xMethod(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9, t10 v10, t11 v11, t12 v12, t13 v13, t14 v14, t15 v15, t16 v16, t17 v17, t18 v18, t19 v19, t20 v20, t21 v21, t22 v22, t23 v23, t24 v24, t25 v25) {   \
      AutoRecursiveLock lock(mLock);                                                                                                                                                                                                        \
      for (SubscriptionDelegateMap::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ) {                                                                                                                               \
        SubscriptionDelegateMap::iterator current = iter; ++iter;                                                                                                                                                                           \
        try {                                                                                                                                                                                                                               \
          (*current).second->xMethod(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20,v21,v22,v23,v24,v25);                                                                                                           \
        } catch(DelegateProxy::Exceptions::DelegateGone &) {                                                                                                                                                                                \
          mSubscriptions.erase(current);                                                                                                                                                                                                    \
        }                                                                                                                                                                                                                                   \
      }                                                                                                                                                                                                                                     \
    }

#endif //ZSLIB_INTERNAL_PROXY_SUBSCRIPTIONS_H_cf7229753b441247ccece9f3b92317ed96045660
