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

#include <zsLib/Exception.h>
#include <zsLib/IMessageQueue.h>

namespace zsLib
{
  class ProxyBase
  {
  public:
    struct Exceptions
    {
      ZS_DECLARE_CUSTOM_EXCEPTION(DelegateGone)
      ZS_DECLARE_CUSTOM_EXCEPTION(MissingDelegateMessageQueue)
    };
  };

  template <typename XINTERFACE>
  class Proxy : public ProxyBase
  {
  public:
    struct Exceptions
    {
      ZS_DECLARE_CUSTOM_EXCEPTION_ALT_BASE(DelegateGone, ProxyBase::Exceptions::DelegateGone)
    };

    ZS_DECLARE_TYPEDEF_PTR(XINTERFACE, Delegate)

  public:
    //------------------------------------------------------------------------
    // PURPOSE: Create a proxy for a delegate interaction by assuming the
    //          object that implemented the interaction derived from
    //          zsLib::MessageQueueAssociator.
    // EXAMPLE:
    //
    // class MyClassThatReceivesEvents : public IDelegate,
    //                                   public MessageQueueAssociator {
    //
    //     ...
    //     void receiveEvents() {
    //       mObject->subscribeEvents(mWeakThis.lock());
    //     }
    //     ...
    //
    //   protected:
    //     MyClassThatReceivesEventsWeakPtr mWeakThis;
    //     ObjectPtr mObject;
    // };
    //
    // void Object::subscribeEvents(IDelegatePtr delegate) {
    //   IDelegatePtr delegateProxy = zsLib::Proxy<IDelegate>::create(delegate);
    //
    static DelegatePtr create(DelegatePtr delegate, bool throwDelegateGone = false, int line = __LINE__, const char *fileName = __FILE__)                                 {return delegate->proxy_implementation_for_this_interface_is_not_defined();}

    //------------------------------------------------------------------------
    // PURPOSE: Create a proxy for a delegate interaction. The object is first
    //          checked if it derived from the zsLib::MessageQueueAssociator
    //          and the associated message queue is used and the method will
    //          used the passed in IMessageQueuePtr as a fallback.
    // EXAMPLE:
    //
    // void Object::subscribeEvents(IMessageQueuePtr queue, IDelegatePtr delegate) {
    //   IDelegatePtr delegateProxy = zsLib::Proxy<IDelegate>::create(queue, delegate);
    //
    static DelegatePtr create(IMessageQueuePtr queue, DelegatePtr delegate, bool throwDelegateGone = false, int line = __LINE__, const char *fileName = __FILE__)         {return delegate->proxy_implementation_for_this_interface_is_not_defined();}

    //------------------------------------------------------------------------
    // PURPOSE: Create a proxy for a delegate interaction. The object's
    //          message queue from zsLib::MessageQueueAssociator is only used
    //          as a fallback if the queue passsed in is null.
    static DelegatePtr createUsingQueue(IMessageQueuePtr queue, DelegatePtr delegate, bool throwDelegateGone = false, int line = __LINE__, const char *fileName = __FILE__) { return delegate->proxy_implementation_for_this_interface_is_not_defined(); }

    //------------------------------------------------------------------------
    // PURPOSE: Create a proxy for a delegate interaction but use a weak
    //          reference to the original delegate (thus the delegate might
    //          disappear at any time). Assumes the objec that implemented the
    //          delegate derived from zsLib::MessageQueueAssociator.
    // EXAMPLE:
    //
    // void Object::subscribeEvents(IDelegatePtr delegate) {
    //   IDelegatePtr delegateProxy = zsLib::Proxy<IDelegate>::createWeak(delegate);
    //
    //   try {
    //     delegate->onEvent();
    //   } catch(zsLib::Proxy<IDelegate>::Exceptions::DelegateGone) {
    //     // NOTE: delegate is already destroyed - can clean up this delegate proxy now
    //   }
    //
    static DelegatePtr createWeak(DelegatePtr delegate, bool throwDelegateGone = false, int line = __LINE__, const char *fileName = __FILE__)                             {return delegate->proxy_implementation_for_this_interface_is_not_defined();}

    //------------------------------------------------------------------------
    // PURPOSE: Create a proxy for a delegate interaction but use a weak
    //          reference to the original delegate (thus the delegate might
    //          disappear at any time). Checks if the delegate was derived
    //          from zsLib::MessageQueueAssociator and if not then uses the
    //          IMessageQueue passed in instead.
    // EXAMPLE:
    //
    // void Object::subscribeEvents(IMessageQueuePtr queue, IDelegatePtr delegate) {
    //   IDelegatePtr delegateProxy = zsLib::Proxy<IDelegate>::createWeak(queue, delegate);
    //
    //   try {
    //     delegate->onEvent();
    //   } catch(zsLib::Proxy<IDelegate>::Exceptions::DelegateGone) {
    //     // NOTE: delegate is already destroyed - can clean up this delegate proxy now
    //   }
    //
    static DelegatePtr createWeak(IMessageQueuePtr queue, DelegatePtr delegate, bool throwDelegateGone = false, int line = __LINE__, const char *fileName = __FILE__)     {return delegate->proxy_implementation_for_this_interface_is_not_defined();}

    //------------------------------------------------------------------------
    // PURPOSE: Create a proxy for a delegate interaction but use a weak
    ///         reference to the original delegate. The object's message queue
    //          from zsLib::MessageQueueAssociator is only used as a fallback
    //          if the queue passsed in is null.
    static DelegatePtr createWeakUsingQueue(IMessageQueuePtr queue, DelegatePtr delegate, bool throwDelegateGone = false, int line = __LINE__, const char *fileName = __FILE__) { return delegate->proxy_implementation_for_this_interface_is_not_defined(); }

    //------------------------------------------------------------------------
    // PURPOSE: Create a no operation proxy for a delegate interaction but
    //          does not reference any original delegate.
    // EXAMPLE:
    //
    // void Object::ignoreEvents(IMessageQueuePtr queue) {
    //   IDelegatePtr delegateProxy = zsLib::Proxy<IDelegate>::createNoop(queue);
    //
    //   try {
    //     delegate->onEvent();
    //   } catch(zsLib::Proxy<IDelegate>::Exceptions::DelegateGone) {
    //     // NOTE: delegate is already destroyed - can clean up this delegate proxy now
    //   }
    //
    static DelegatePtr createNoop(IMessageQueuePtr queue, bool throwsDelegateGone = false, int line = __LINE__, const char *fileName = __FILE__)                                   {return DelegatePtr()->proxy_implementation_for_this_interface_is_not_defined();}

    //------------------------------------------------------------------------
    // PURPOSE: Returns true if the delegate passed in is a proxy.
    // EXAMPLE:
    //
    // void Object::subscribeEvents(IDelegatePtr delegate) {
    //   bool isProxy = zsLib::Proxy<IDelegate>::isProxy(delegate);
    //
    static bool isProxy(DelegatePtr delegate)                                                                                                                             {return delegate->proxy_implementation_for_this_interface_is_not_defined();}

    //------------------------------------------------------------------------
    // PURPOSE: Returns the original delegate from a proxy (or if the object
    //          is not a proxy just returns the object passed in since it is
    //          the delegate).
    // EXAMPLE:
    //
    // void Object::subscribeEvents(IDelegatePtr delegate) {
    //   IDelegatePtr originalDelegatePtr = zsLib::Proxy<IDelegate>::original(delegate);
    //
    static DelegatePtr original(DelegatePtr delegate, bool throwDelegateGone = false)                                                                                     {return delegate->proxy_implementation_for_this_interface_is_not_defined();}

    //------------------------------------------------------------------------
    // PURPOSE: Returns the associated message queue from a proxy (or NULL if
    //          no message queue is associated).
    // EXAMPLE:
    //
    // void Object::subscribeEvents(IDelegatePtr delegate) {
    //   IMessageQueuePtr queue = zsLib::Proxy<IDelegate>::getAssociatedMessageQueue(delegate);
    //
    static IMessageQueuePtr getAssociatedMessageQueue(DelegatePtr delegate)                                                                                               {return delegate->proxy_implementation_for_this_interface_is_not_defined();}
  };
}

#include <zsLib/internal/zsLib_Proxy.h>

namespace zsLib
{
  // get the total number of proxies that are currently constructed and not destroyed
  ULONG proxyGetTotalConstructed();
  void proxyDump();
}


#define ZS_DECLARE_INTERACTION_PROXY(xInteractionName)                                                                                                        ZS_INTERNAL_DECLARE_INTERACTION_PROXY(xInteractionName)
#define ZS_DECLARE_TYPEDEF_PROXY(xOriginalType, xNewTypeName)                                                                                                 ZS_INTERNAL_DECLARE_TYPEDEF_PROXY(xOriginalType, xNewTypeName)
#define ZS_DECLARE_USING_PROXY(xNamespace, xExistingType)                                                                                                     ZS_INTERNAL_DECLARE_USING_PROXY(xNamespace, xExistingType)

#define ZS_DECLARE_PROXY_BEGIN(xInterface)                                                                                                                    ZS_INTERNAL_DECLARE_PROXY_BEGIN(xInterface, true)
#define ZS_DECLARE_PROXY_WITH_DELEGATE_MESSAGE_QUEUE_OPTIONAL_BEGIN(xInterface)                                                                               ZS_INTERNAL_DECLARE_PROXY_BEGIN(xInterface, false)
#define ZS_DECLARE_PROXY_END()                                                                                                                                ZS_INTERNAL_DECLARE_PROXY_END()

#define ZS_DECLARE_PROXY_TYPEDEF(xOriginalType, xTypeAlias)                                                                                                   ZS_INTERNAL_DECLARE_PROXY_TYPEDEF(xOriginalType, xTypeAlias)

#define ZS_DECLARE_PROXY_METHOD_SYNC_0(xMethod)                                                                                                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_0(ZS_LIB_NO_CONST, xMethod)
#define ZS_DECLARE_PROXY_METHOD_SYNC_1(xMethod,t1)                                                                                                            ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_1(ZS_LIB_NO_CONST, xMethod,t1)
#define ZS_DECLARE_PROXY_METHOD_SYNC_2(xMethod,t1,t2)                                                                                                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_2(ZS_LIB_NO_CONST, xMethod,t1,t2)
#define ZS_DECLARE_PROXY_METHOD_SYNC_3(xMethod,t1,t2,t3)                                                                                                      ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_3(ZS_LIB_NO_CONST, xMethod,t1,t2,t3)
#define ZS_DECLARE_PROXY_METHOD_SYNC_4(xMethod,t1,t2,t3,t4)                                                                                                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_4(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4)
#define ZS_DECLARE_PROXY_METHOD_SYNC_5(xMethod,t1,t2,t3,t4,t5)                                                                                                ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_5(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5)
#define ZS_DECLARE_PROXY_METHOD_SYNC_6(xMethod,t1,t2,t3,t4,t5,t6)                                                                                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_6(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6)
#define ZS_DECLARE_PROXY_METHOD_SYNC_7(xMethod,t1,t2,t3,t4,t5,t6,t7)                                                                                          ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_7(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7)
#define ZS_DECLARE_PROXY_METHOD_SYNC_8(xMethod,t1,t2,t3,t4,t5,t6,t7,t8)                                                                                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_8(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8)
#define ZS_DECLARE_PROXY_METHOD_SYNC_9(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)                                                                                    ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_9(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)
#define ZS_DECLARE_PROXY_METHOD_SYNC_10(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)                                                                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_10(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)
#define ZS_DECLARE_PROXY_METHOD_SYNC_11(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)                                                                           ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_11(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)
#define ZS_DECLARE_PROXY_METHOD_SYNC_12(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)                                                                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_12(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)
#define ZS_DECLARE_PROXY_METHOD_SYNC_13(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)                                                                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_13(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)
#define ZS_DECLARE_PROXY_METHOD_SYNC_14(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)                                                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_14(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)
#define ZS_DECLARE_PROXY_METHOD_SYNC_15(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)                                                           ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_15(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)
#define ZS_DECLARE_PROXY_METHOD_SYNC_16(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)                                                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_16(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)
#define ZS_DECLARE_PROXY_METHOD_SYNC_17(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)                                                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_17(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)
#define ZS_DECLARE_PROXY_METHOD_SYNC_18(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)                                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_18(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)
#define ZS_DECLARE_PROXY_METHOD_SYNC_19(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)                                           ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_19(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)
#define ZS_DECLARE_PROXY_METHOD_SYNC_20(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)                                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_20(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)
#define ZS_DECLARE_PROXY_METHOD_SYNC_21(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)                                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_21(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)
#define ZS_DECLARE_PROXY_METHOD_SYNC_22(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_22(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)
#define ZS_DECLARE_PROXY_METHOD_SYNC_23(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)                           ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_23(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)
#define ZS_DECLARE_PROXY_METHOD_SYNC_24(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_24(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)
#define ZS_DECLARE_PROXY_METHOD_SYNC_25(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_25(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)

#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_0(xMethod)                                                                                                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_0(ZS_LIB_CONST, xMethod)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_1(xMethod,t1)                                                                                                      ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_1(ZS_LIB_CONST, xMethod,t1)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_2(xMethod,t1,t2)                                                                                                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_2(ZS_LIB_CONST, xMethod,t1,t2)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_3(xMethod,t1,t2,t3)                                                                                                ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_3(ZS_LIB_CONST, xMethod,t1,t2,t3)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_4(xMethod,t1,t2,t3,t4)                                                                                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_4(ZS_LIB_CONST, xMethod,t1,t2,t3,t4)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_5(xMethod,t1,t2,t3,t4,t5)                                                                                          ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_5(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_6(xMethod,t1,t2,t3,t4,t5,t6)                                                                                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_6(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_7(xMethod,t1,t2,t3,t4,t5,t6,t7)                                                                                    ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_7(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_8(xMethod,t1,t2,t3,t4,t5,t6,t7,t8)                                                                                 ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_8(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_9(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)                                                                              ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_9(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_10(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)                                                                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_10(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_11(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)                                                                     ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_11(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_12(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)                                                                 ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_12(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_13(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)                                                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_13(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_14(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)                                                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_14(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_15(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)                                                     ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_15(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_16(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)                                                 ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_16(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_17(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)                                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_17(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_18(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)                                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_18(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_19(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)                                     ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_19(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_20(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)                                 ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_20(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_21(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_21(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_22(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_22(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_23(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)                     ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_23(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_24(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)                 ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_24(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_25(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_25(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)

#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_0(xMethod,r1)                                                                                                     ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_0(ZS_LIB_NO_CONST, xMethod,r1)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_1(xMethod,r1,t1)                                                                                                  ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_1(ZS_LIB_NO_CONST, xMethod,r1,t1)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_2(xMethod,r1,t1,t2)                                                                                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_2(ZS_LIB_NO_CONST, xMethod,r1,t1,t2)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_3(xMethod,r1,t1,t2,t3)                                                                                            ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_3(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_4(xMethod,r1,t1,t2,t3,t4)                                                                                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_4(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_5(xMethod,r1,t1,t2,t3,t4,t5)                                                                                      ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_5(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_6(xMethod,r1,t1,t2,t3,t4,t5,t6)                                                                                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_6(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_7(xMethod,r1,t1,t2,t3,t4,t5,t6,t7)                                                                                ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_7(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_8(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8)                                                                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_8(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_9(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9)                                                                          ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_9(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_10(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)                                                                     ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_10(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_11(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)                                                                 ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_11(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_12(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)                                                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_12(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_13(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)                                                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_13(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_14(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)                                                     ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_14(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_15(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)                                                 ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_15(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_16(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)                                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_16(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_17(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)                                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_17(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_18(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)                                     ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_18(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_19(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)                                 ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_19(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_20(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_20(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_21(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_21(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_22(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)                     ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_22(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_23(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)                 ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_23(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_24(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_24(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)
#define ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_25(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_25(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)

#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_0(xMethod,r1)                                                                                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_0(ZS_LIB_CONST, xMethod,r1)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_1(xMethod,r1,t1)                                                                                            ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_1(ZS_LIB_CONST, xMethod,r1,t1)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_2(xMethod,r1,t1,t2)                                                                                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_2(ZS_LIB_CONST, xMethod,r1,t1,t2)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_3(xMethod,r1,t1,t2,t3)                                                                                      ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_3(ZS_LIB_CONST, xMethod,r1,t1,t2,t3)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_4(xMethod,r1,t1,t2,t3,t4)                                                                                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_4(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_5(xMethod,r1,t1,t2,t3,t4,t5)                                                                                ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_5(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_6(xMethod,r1,t1,t2,t3,t4,t5,t6)                                                                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_6(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_7(xMethod,r1,t1,t2,t3,t4,t5,t6,t7)                                                                          ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_7(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_8(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8)                                                                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_8(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_9(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9)                                                                    ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_9(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_10(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)                                                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_10(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_11(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)                                                           ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_11(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_12(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)                                                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_12(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_13(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)                                                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_13(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_14(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)                                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_14(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_15(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)                                           ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_15(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_16(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)                                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_16(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_17(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)                                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_17(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_18(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_18(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_19(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)                           ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_19(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_20(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_20(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_21(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_21(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_22(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_22(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_23(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)           ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_23(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_24(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_24(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)
#define ZS_DECLARE_PROXY_METHOD_SYNC_CONST_RETURN_25(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_25(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)

#define ZS_DECLARE_PROXY_METHOD_0(xMethod)                                                                                                                    ZS_INTERNAL_DECLARE_PROXY_METHOD_0(xMethod)
#define ZS_DECLARE_PROXY_METHOD_1(xMethod,t1)                                                                                                                 ZS_INTERNAL_DECLARE_PROXY_METHOD_1(xMethod,t1)
#define ZS_DECLARE_PROXY_METHOD_2(xMethod,t1,t2)                                                                                                              ZS_INTERNAL_DECLARE_PROXY_METHOD_2(xMethod,t1,t2)
#define ZS_DECLARE_PROXY_METHOD_3(xMethod,t1,t2,t3)                                                                                                           ZS_INTERNAL_DECLARE_PROXY_METHOD_3(xMethod,t1,t2,t3)
#define ZS_DECLARE_PROXY_METHOD_4(xMethod,t1,t2,t3,t4)                                                                                                        ZS_INTERNAL_DECLARE_PROXY_METHOD_4(xMethod,t1,t2,t3,t4)
#define ZS_DECLARE_PROXY_METHOD_5(xMethod,t1,t2,t3,t4,t5)                                                                                                     ZS_INTERNAL_DECLARE_PROXY_METHOD_5(xMethod,t1,t2,t3,t4,t5)
#define ZS_DECLARE_PROXY_METHOD_6(xMethod,t1,t2,t3,t4,t5,t6)                                                                                                  ZS_INTERNAL_DECLARE_PROXY_METHOD_6(xMethod,t1,t2,t3,t4,t5,t6)
#define ZS_DECLARE_PROXY_METHOD_7(xMethod,t1,t2,t3,t4,t5,t6,t7)                                                                                               ZS_INTERNAL_DECLARE_PROXY_METHOD_7(xMethod,t1,t2,t3,t4,t5,t6,t7)
#define ZS_DECLARE_PROXY_METHOD_8(xMethod,t1,t2,t3,t4,t5,t6,t7,t8)                                                                                            ZS_INTERNAL_DECLARE_PROXY_METHOD_8(xMethod,t1,t2,t3,t4,t5,t6,t7,t8)
#define ZS_DECLARE_PROXY_METHOD_9(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)                                                                                         ZS_INTERNAL_DECLARE_PROXY_METHOD_9(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)
#define ZS_DECLARE_PROXY_METHOD_10(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)                                                                                    ZS_INTERNAL_DECLARE_PROXY_METHOD_10(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)
#define ZS_DECLARE_PROXY_METHOD_11(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)                                                                                ZS_INTERNAL_DECLARE_PROXY_METHOD_11(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)
#define ZS_DECLARE_PROXY_METHOD_12(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)                                                                            ZS_INTERNAL_DECLARE_PROXY_METHOD_12(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)
#define ZS_DECLARE_PROXY_METHOD_13(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)                                                                        ZS_INTERNAL_DECLARE_PROXY_METHOD_13(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)
#define ZS_DECLARE_PROXY_METHOD_14(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)                                                                    ZS_INTERNAL_DECLARE_PROXY_METHOD_14(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)
#define ZS_DECLARE_PROXY_METHOD_15(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)                                                                ZS_INTERNAL_DECLARE_PROXY_METHOD_15(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)
#define ZS_DECLARE_PROXY_METHOD_16(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)                                                            ZS_INTERNAL_DECLARE_PROXY_METHOD_16(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)
#define ZS_DECLARE_PROXY_METHOD_17(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)                                                        ZS_INTERNAL_DECLARE_PROXY_METHOD_17(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)
#define ZS_DECLARE_PROXY_METHOD_18(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)                                                    ZS_INTERNAL_DECLARE_PROXY_METHOD_18(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)
#define ZS_DECLARE_PROXY_METHOD_19(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)                                                ZS_INTERNAL_DECLARE_PROXY_METHOD_19(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)
#define ZS_DECLARE_PROXY_METHOD_20(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)                                            ZS_INTERNAL_DECLARE_PROXY_METHOD_20(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)
#define ZS_DECLARE_PROXY_METHOD_21(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)                                        ZS_INTERNAL_DECLARE_PROXY_METHOD_21(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)
#define ZS_DECLARE_PROXY_METHOD_22(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)                                    ZS_INTERNAL_DECLARE_PROXY_METHOD_22(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)
#define ZS_DECLARE_PROXY_METHOD_23(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)                                ZS_INTERNAL_DECLARE_PROXY_METHOD_23(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)
#define ZS_DECLARE_PROXY_METHOD_24(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)                            ZS_INTERNAL_DECLARE_PROXY_METHOD_24(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)
#define ZS_DECLARE_PROXY_METHOD_25(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)                        ZS_INTERNAL_DECLARE_PROXY_METHOD_25(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)
