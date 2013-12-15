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

#ifndef ZSLIB_PROXY_SUBSCRIPTIONS_H_6f362898f150c24cdb3616387e1e6a72add08b85
#define ZSLIB_PROXY_SUBSCRIPTIONS_H_6f362898f150c24cdb3616387e1e6a72add08b85

#include <zsLib/Exception.h>
#include <zsLib/MessageQueue.h>

#include <zsLib/Proxy.h>

namespace zsLib
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
    typedef size_t size_type;
    typedef boost::shared_ptr<XINTERFACE> DelegatePtr;
    typedef boost::weak_ptr<XINTERFACE>   DelegateWeakPtr;

    typedef SUBSCRIPTIONBASECLASS BaseSubscription;
    typedef boost::shared_ptr<BaseSubscription> BaseSubscriptionPtr;
    typedef boost::weak_ptr<BaseSubscription>   BaseSubscriptionWeakPtr;

    class Subscription;
    typedef boost::shared_ptr<Subscription> SubscriptionPtr;
    typedef boost::weak_ptr<Subscription>   SubscriptionWeakPtr;

  public:
    //-------------------------------------------------------------------------
    // PURPOSE: subscribe to all events fired on the "delegate()" object
    // NOTE:    will auto-create a proxy and return weak (or strong)
    //          reference to the original proxy.
    SubscriptionPtr subscribe(
                              DelegatePtr delgate,
                              IMessageQueuePtr queue = IMessageQueuePtr(),
                              bool strongReferenceToDelgate = false
                              )                           {return DelegatePtr()->proxy_implementation_for_this_interface_is_not_defined();}

    //-------------------------------------------------------------------------
    // PURPOSE: obtain the delegate where events fired on the delegate
    //          cause all the attached subscriptions to fire as well.
    DelegatePtr delegate() const                          {return DelegatePtr()->proxy_implementation_for_this_interface_is_not_defined();}

    //-------------------------------------------------------------------------
    // PURPOSE: obtain a strong (or weak) proxy reference to the original
    //          delegate given a subscription.
    DelegatePtr delegate(
                         BaseSubscriptionPtr subscription,
                         bool strongReferenceToDelgate = true
                         ) const                          {return DelegatePtr()->proxy_implementation_for_this_interface_is_not_defined();}

    //-------------------------------------------------------------------------
    // PURPOSE: clear out any subscriptions and prevent any delegates from
    //          receiving events.
    void clear()                                          {return DelegatePtr()->proxy_implementation_for_this_interface_is_not_defined();}

    //-------------------------------------------------------------------------
    // PURPOSE: obtain the total number of subscribers to the delegate being
    //          fired.
    size_t size() const                                   {return DelegatePtr()->proxy_implementation_for_this_interface_is_not_defined();}

  public:
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark Subscription
    #pragma mark

    class Subscription : public BaseSubscription
    {
      //-----------------------------------------------------------------------
      // PURPOSE: cancel the active subscription to delegate events
      virtual PUID getID() const = 0;

      //-----------------------------------------------------------------------
      // PURPOSE: cancel the active subscription to delegate events
      virtual void cancel() = 0;

      //-----------------------------------------------------------------------
      // PURPOSE: cause events to continue to fire for the subscribed delegate
      //          even if a reference to the subscription object is not
      //          mantained.
      virtual void background() = 0;
    };
  };
}

#include <zsLib/internal/zsLib_ProxySubscriptions.h>

#define ZS_DECLARE_INTERACTION_PROXY_SUBSCRIPTION(xInteractionName, xDelegateName)                                                                            ZS_INTERNAL_DECLARE_INTERACTION_PROXY_SUBSCRIPTION(xInteractionName, xDelegateName)

#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_BEGIN(xInterface, xSubscriptionClass)                                                                                  ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_BEGIN(xInterface, xSubscriptionClass)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_END()                                                                                                                  ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_END()

#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_TYPEDEF(xOriginalType, xTypeAlias)                                                                                     ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_TYPEDEF(xOriginalType, xTypeAlias)

#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_TYPES_AND_VALUES(xSubscriptionsMapTypename, xSubscriptionsMapVariable, xSubscriptionsMapKeyTypename, xDelegatPtrTypename, xDelegateProxyTypename)    ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_TYPES_AND_VALUES(xSubscriptionsMapTypename, xSubscriptionsMapVariable, xSubscriptionsMapKeyTypename, xDelegatPtrTypename, xDelegateProxyTypename)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_ERASE_KEY(xSubscriptionsMapKeyValue)                                                                                                                 ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_ERASE_KEY(xSubscriptionsMapKeyValue)

#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_0(xMethod)                                                                                                      ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_0(xMethod)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_1(xMethod,t1)                                                                                                   ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_1(xMethod,t1)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_2(xMethod,t1,t2)                                                                                                ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_2(xMethod,t1,t2)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_3(xMethod,t1,t2,t3)                                                                                             ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_3(xMethod,t1,t2,t3)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_4(xMethod,t1,t2,t3,t4)                                                                                          ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_4(xMethod,t1,t2,t3,t4)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_5(xMethod,t1,t2,t3,t4,t5)                                                                                       ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_5(xMethod,t1,t2,t3,t4,t5)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_6(xMethod,t1,t2,t3,t4,t5,t6)                                                                                    ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_6(xMethod,t1,t2,t3,t4,t5,t6)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_7(xMethod,t1,t2,t3,t4,t5,t6,t7)                                                                                 ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_7(xMethod,t1,t2,t3,t4,t5,t6,t7)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_8(xMethod,t1,t2,t3,t4,t5,t6,t7,t8)                                                                              ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_8(xMethod,t1,t2,t3,t4,t5,t6,t7,t8)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_9(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)                                                                           ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_9(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_10(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)                                                                      ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_10(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_11(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)                                                                  ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_11(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_12(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)                                                              ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_12(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_13(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)                                                          ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_13(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_14(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)                                                      ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_14(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_15(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)                                                  ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_15(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_16(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)                                              ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_16(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_17(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)                                          ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_17(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_18(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)                                      ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_18(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_19(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)                                  ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_19(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_20(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)                              ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_20(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_21(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)                          ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_21(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_22(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)                      ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_22(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_23(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)                  ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_23(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_24(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)              ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_24(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)
#define ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_25(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)          ZS_INTERNAL_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_25(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)

#endif //ZSLIB_PROXY_SUBSCRIPTIONS_H_6f362898f150c24cdb3616387e1e6a72add08b85
