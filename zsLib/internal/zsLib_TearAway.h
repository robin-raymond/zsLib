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

#ifndef ZSLIB_INTERNAL_TEAR_AWAY_H_b93ee5a7a6bee3a329b1e47d719ceccd154e44ec
#define ZSLIB_INTERNAL_TEAR_AWAY_H_b93ee5a7a6bee3a329b1e47d719ceccd154e44ec

#include <zsLib/internal/zsLib_Proxy.h>

namespace zsLib
{
  template <typename XINTERFACE, typename XDATATYPE>
  class TearAway;

  namespace internal
  {
    template <typename XINTERFACE, typename XDATATYPE>
    class TearAway : public XINTERFACE
    {
    public:
      ZS_DECLARE_TYPEDEF_PTR(XINTERFACE, TearAwayInterface)
      ZS_DECLARE_TYPEDEF_PTR(XDATATYPE, TearAwayData)

    public:
      TearAway(
               TearAwayInterfacePtr original,
               TearAwayDataPtr data = TearAwayDataPtr()
               ) :
        mOriginal(original),
        mData(data)
      {}

      ~TearAway() {}

      TearAwayInterfacePtr getDelegate() const {return mOriginal;}

      bool ignoreMethodCall() const {return false;}

      TearAwayDataPtr getData() {return mData;}

    protected:
      TearAwayInterfacePtr mOriginal;
      TearAwayDataPtr mData;
    };
  }
}

#define ZS_INTERNAL_DECLARE_INTERACTION_TEAR_AWAY(xInteractionName, xDataType)                      \
  interaction xInteractionName;                                                                               \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::shared_ptr<xInteractionName> xInteractionName##Ptr;            \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::weak_ptr<xInteractionName> xInteractionName##WeakPtr;          \
  typedef zsLib::TearAway<xInteractionName, xDataType> xInteractionName##TearAway;

#define ZS_INTERNAL_DECLARE_TYPEDEF_TEAR_AWAY(xOriginalType, xNewTypeName, xDataType)                         \
  typedef xOriginalType xNewTypeName;                                                                         \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::shared_ptr<xNewTypeName> xNewTypeName##Ptr;                    \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::weak_ptr<xNewTypeName> xNewTypeName##WeakPtr;                  \
  typedef zsLib::TearAway<xNewTypeName, xDataType> xNewTypeName##TearAway;

#define ZS_INTERNAL_DECLARE_USING_TEAR_AWAY(xNamespace, xExistingType)                                        \
  using xNamespace::xExistingType;                                                                            \
  using xNamespace::xExistingType##Ptr;                                                                       \
  using xNamespace::xExistingType##WeakPtr;                                                                   \
  using xNamespace::xExistingType##TearAway;

#define ZS_INTERNAL_DECLARE_TEAR_AWAY_BEGIN(xInterface, xDataType)                                            \
namespace zsLib                                                                                               \
{                                                                                                             \
  template<>                                                                                                  \
  class TearAway<xInterface, xDataType> : public internal::TearAway<xInterface, xDataType>                    \
  {                                                                                                           \
  public:                                                                                                     \
    ZS_DECLARE_TYPEDEF_PTR(xInterface, TearAwayInterface)                                                     \
    typedef TearAway<xInterface, xDataType> TearAwayType;                                                     \
    ZS_DECLARE_PTR(TearAwayType)                                                                              \
    ZS_DECLARE_TYPEDEF_PTR(xDataType, TearAwayData)                                                           \
                                                                                                              \
  private:                                                                                                    \
    TearAway(TearAwayInterfacePtr original, TearAwayDataPtr data = TearAwayDataPtr()) : internal::TearAway<xInterface, xDataType>(original, data) {} \
                                                                                                              \
  public:                                                                                                     \
    static TearAwayInterfacePtr create(                                                                       \
                                      TearAwayInterfacePtr original,                                          \
                                      TearAwayDataPtr data = TearAwayDataPtr()                                \
                                      )                                                                       \
    {                                                                                                         \
      if (!original)                                                                                          \
        return original;                                                                                      \
                                                                                                              \
      return TearAwayTypePtr(new TearAwayType(original, data));                                               \
    }                                                                                                         \
                                                                                                              \
    static bool isTearAway(TearAwayInterfacePtr tearAway)                                                     \
    {                                                                                                         \
      if (!tearAway)                                                                                          \
        return false;                                                                                         \
                                                                                                              \
      TearAwayType *wrapper = dynamic_cast<TearAwayType *>(tearAway.get());                                   \
      return (wrapper ? true : false);                                                                        \
    }                                                                                                         \
                                                                                                              \
    static TearAwayInterfacePtr original(TearAwayInterfacePtr tearAway)                                       \
    {                                                                                                         \
      if (!tearAway)                                                                                          \
        return tearAway;                                                                                      \
                                                                                                              \
      TearAwayType *wrapper = dynamic_cast<TearAwayType *>(tearAway.get());                                   \
      if (wrapper) {                                                                                          \
        return wrapper->getDelegate();                                                                        \
      }                                                                                                       \
      return tearAway;                                                                                        \
    }                                                                                                         \
                                                                                                              \
    static TearAwayDataPtr data(TearAwayInterfacePtr tearAway)                                                \
    {                                                                                                         \
      if (!tearAway)                                                                                          \
        return TearAwayDataPtr();                                                                             \
                                                                                                              \
      TearAwayType *wrapper = dynamic_cast<TearAwayType *>(tearAway.get());                                   \
      if (wrapper) {                                                                                          \
        return wrapper->getData();                                                                            \
      }                                                                                                       \
      return TearAwayDataPtr();                                                                               \
    }

#define ZS_INTERNAL_DECLARE_TEAR_AWAY_END()                                                                   \
  };                                                                                                          \
}

#define ZS_INTERNAL_DECLARE_TEAR_AWAY_TYPEDEF(xOriginalType, xTypeAlias)                                      \
    typedef xOriginalType xTypeAlias;


#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_0(xConst, xMethod)                                                                                                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_0(xConst, xMethod)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_1(xConst, xMethod,t1)                                                                                                            ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_1(xConst, xMethod,t1)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_2(xConst, xMethod,t1,t2)                                                                                                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_2(xConst, xMethod,t1,t2)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_3(xConst, xMethod,t1,t2,t3)                                                                                                      ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_3(xConst, xMethod,t1,t2,t3)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_4(xConst, xMethod,t1,t2,t3,t4)                                                                                                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_4(xConst, xMethod,t1,t2,t3,t4)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_5(xConst, xMethod,t1,t2,t3,t4,t5)                                                                                                ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_5(xConst, xMethod,t1,t2,t3,t4,t5)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_6(xConst, xMethod,t1,t2,t3,t4,t5,t6)                                                                                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_6(xConst, xMethod,t1,t2,t3,t4,t5,t6)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_7(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7)                                                                                          ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_7(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_8(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8)                                                                                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_8(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_9(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)                                                                                    ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_9(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_10(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)                                                                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_10(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_11(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)                                                                           ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_11(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_12(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)                                                                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_12(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_13(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)                                                                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_13(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_14(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)                                                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_14(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_15(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)                                                           ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_15(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_16(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)                                                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_16(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_17(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)                                                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_17(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_18(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)                                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_18(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_19(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)                                           ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_19(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_20(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)                                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_20(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_21(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)                                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_21(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_22(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_22(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_23(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)                           ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_23(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_24(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)                       ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_24(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_25(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_25(xConst, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)

#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_0(xConst, xMethod,r1)                                                                                                     ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_0(xConst, xMethod,r1)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_1(xConst, xMethod,r1,t1)                                                                                                  ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_1(xConst, xMethod,r1,t1)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_2(xConst, xMethod,r1,t1,t2)                                                                                               ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_2(xConst, xMethod,r1,t1,t2)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_3(xConst, xMethod,r1,t1,t2,t3)                                                                                            ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_3(xConst, xMethod,r1,t1,t2,t3)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_4(xConst, xMethod,r1,t1,t2,t3,t4)                                                                                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_4(xConst, xMethod,r1,t1,t2,t3,t4)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_5(xConst, xMethod,r1,t1,t2,t3,t4,t5)                                                                                      ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_5(xConst, xMethod,r1,t1,t2,t3,t4,t5)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_6(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6)                                                                                   ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_6(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_7(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7)                                                                                ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_7(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_8(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8)                                                                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_8(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_9(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9)                                                                          ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_9(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_10(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)                                                                     ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_10(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_11(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)                                                                 ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_11(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_12(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)                                                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_12(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_13(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)                                                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_13(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_14(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)                                                     ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_14(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_15(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)                                                 ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_15(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_16(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)                                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_16(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_17(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)                                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_17(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_18(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)                                     ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_18(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_19(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)                                 ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_19(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_20(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)                             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_20(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_21(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)                         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_21(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_22(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)                     ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_22(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_23(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)                 ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_23(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_24(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)             ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_24(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)
#define ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_25(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)         ZS_INTERNAL_DECLARE_PROXY_METHOD_SYNC_RETURN_25(xConst, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)

#endif //ZSLIB_INTERNAL_TEAR_AWAY_H_b93ee5a7a6bee3a329b1e47d719ceccd154e44ec
