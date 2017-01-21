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

#ifndef ZSLIB_TEAR_AWAY_H_1b92b54dd4fd3fc7f9059b2fa3c3156c8dea29a1
#define ZSLIB_TEAR_AWAY_H_1b92b54dd4fd3fc7f9059b2fa3c3156c8dea29a1

#include <zsLib/types.h>

namespace zsLib
{
  template <typename XINTERFACE, typename XDATATYPE>
  class TearAway
  {
  public:

    ZS_DECLARE_TYPEDEF_PTR(XINTERFACE, TearAwayInterface);
    ZS_DECLARE_TYPEDEF_PTR(XDATATYPE, TearAwayData);
    ZS_DECLARE_TYPEDEF_PTR(TearAway, TearAwayType);

  public:
    //------------------------------------------------------------------------
    // PURPOSE: Create an object tear away wrapper to wrap an original
    //          interface with optional additional associated data
    static TearAwayInterfacePtr create(
                                       TearAwayInterfacePtr original,
                                       TearAwayDataPtr data = TearAwayDataPtr()
                                       )                                                                                                                      {return original->tear_away_implementation_for_this_interface_is_not_defined();}

    //------------------------------------------------------------------------
    // PURPOSE: Get the tear away interface associated to the tear away
    //          instance. Functionally identical to static original(...)
    //          method.
    TearAwayInterfacePtr getDelegate() const                                                                                                                  { return TearAwayInterfacePtr()->tear_away_implementation_for_this_interface_is_not_defined(); }

    //------------------------------------------------------------------------
    // PURPOSE: Set the tear away interface associated to the tear away
    //          instance.
    void setDelegate(TearAwayInterfacePtr original)                                                                                                           { original->tear_away_implementation_for_this_interface_is_not_defined(); }

    //------------------------------------------------------------------------
    // PURPOSE: Gets the data associated with the tear away instance.
    TearAwayDataPtr getData() const                                                                                                                           { TearAwayDataPtr()->tear_away_implementation_for_this_interface_is_not_defined(); }

    //------------------------------------------------------------------------
    // PURPOSE: Returns true if the interface passed in is a tear away
    static bool isTearAway(TearAwayInterfacePtr tearAway)                                                                                                     {return tearAway->tear_away_implementation_for_this_interface_is_not_defined();}

    //------------------------------------------------------------------------
    // PURPOSE: Returns the original inteface from a tear away (or if the
    //          object is not a tear away just returns the object passed in
    //          since it is the original).
    static TearAwayInterfacePtr original(TearAwayInterfacePtr tearAway)                                                                                       {return tearAway->tear_away_implementation_for_this_interface_is_not_defined();}

    //------------------------------------------------------------------------
    // PURPOSE: Returns pointer to data structure if the interface passed
    //          in is a tear away otherwise returns TearAwayDataPtr()
    static TearAwayDataPtr data(TearAwayInterfacePtr tearAway)                                                                                                {return tearAway->tear_away_implementation_for_this_interface_is_not_defined();}

    //------------------------------------------------------------------------
    // PURPOSE: Returns the tear away inteface from an interface type (or if
    //          the object is not a tear away then returns a nullptr).
    static TearAwayTypePtr tearAway(TearAwayInterfacePtr tearAway)                                                                                            { return tearAway->tear_away_implementation_for_this_interface_is_not_defined(); }
  };
}

#include <zsLib/internal/zsLib_TearAway.h>

#define ZS_DECLARE_INTERACTION_TEAR_AWAY(xInteractionName, xDataType)                                                                                         ZS_INTERNAL_DECLARE_INTERACTION_TEAR_AWAY(xInteractionName, xDataType)
#define ZS_DECLARE_TYPEDEF_TEAR_AWAY(xOriginalType, xNewTypeName, xDataType)                                                                                  ZS_INTERNAL_DECLARE_TYPEDEF_TEAR_AWAY(xOriginalType, xNewTypeName, xDataType)
#define ZS_DECLARE_USING_TEAR_AWAY(xNamespace, xExistingType)                                                                                                 ZS_INTERNAL_DECLARE_USING_TEAR_AWAY(xNamespace, xExistingType)

#define ZS_DECLARE_TEAR_AWAY_BEGIN(xInterface, xDataType)                                                                                                     ZS_INTERNAL_DECLARE_TEAR_AWAY_BEGIN(xInterface, xDataType)
#define ZS_DECLARE_TEAR_AWAY_END()                                                                                                                            ZS_INTERNAL_DECLARE_TEAR_AWAY_END()

#define ZS_DECLARE_TEAR_AWAY_TYPEDEF(xOriginalType, xTypeAlias)                                                                                               ZS_INTERNAL_DECLARE_TEAR_AWAY_TYPEDEF(xOriginalType, xTypeAlias)

#define ZS_DECLARE_TEAR_AWAY_METHOD_0(xMethod)                                                                                                                ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_0(ZS_LIB_NO_CONST, xMethod)
#define ZS_DECLARE_TEAR_AWAY_METHOD_1(xMethod,t1)                                                                                                             ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_1(ZS_LIB_NO_CONST, xMethod,t1)
#define ZS_DECLARE_TEAR_AWAY_METHOD_2(xMethod,t1,t2)                                                                                                          ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_2(ZS_LIB_NO_CONST, xMethod,t1,t2)
#define ZS_DECLARE_TEAR_AWAY_METHOD_3(xMethod,t1,t2,t3)                                                                                                       ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_3(ZS_LIB_NO_CONST, xMethod,t1,t2,t3)
#define ZS_DECLARE_TEAR_AWAY_METHOD_4(xMethod,t1,t2,t3,t4)                                                                                                    ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_4(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4)
#define ZS_DECLARE_TEAR_AWAY_METHOD_5(xMethod,t1,t2,t3,t4,t5)                                                                                                 ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_5(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5)
#define ZS_DECLARE_TEAR_AWAY_METHOD_6(xMethod,t1,t2,t3,t4,t5,t6)                                                                                              ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_6(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6)
#define ZS_DECLARE_TEAR_AWAY_METHOD_7(xMethod,t1,t2,t3,t4,t5,t6,t7)                                                                                           ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_7(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7)
#define ZS_DECLARE_TEAR_AWAY_METHOD_8(xMethod,t1,t2,t3,t4,t5,t6,t7,t8)                                                                                        ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_8(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8)
#define ZS_DECLARE_TEAR_AWAY_METHOD_9(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)                                                                                     ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_9(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)
#define ZS_DECLARE_TEAR_AWAY_METHOD_10(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)                                                                                ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_10(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)
#define ZS_DECLARE_TEAR_AWAY_METHOD_11(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)                                                                            ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_11(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)
#define ZS_DECLARE_TEAR_AWAY_METHOD_12(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)                                                                        ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_12(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)
#define ZS_DECLARE_TEAR_AWAY_METHOD_13(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)                                                                    ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_13(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)
#define ZS_DECLARE_TEAR_AWAY_METHOD_14(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)                                                                ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_14(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)
#define ZS_DECLARE_TEAR_AWAY_METHOD_15(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)                                                            ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_15(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)
#define ZS_DECLARE_TEAR_AWAY_METHOD_16(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)                                                        ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_16(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)
#define ZS_DECLARE_TEAR_AWAY_METHOD_17(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)                                                    ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_17(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)
#define ZS_DECLARE_TEAR_AWAY_METHOD_18(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)                                                ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_18(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)
#define ZS_DECLARE_TEAR_AWAY_METHOD_19(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)                                            ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_19(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)
#define ZS_DECLARE_TEAR_AWAY_METHOD_20(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)                                        ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_20(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)
#define ZS_DECLARE_TEAR_AWAY_METHOD_21(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)                                    ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_21(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)
#define ZS_DECLARE_TEAR_AWAY_METHOD_22(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)                                ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_22(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)
#define ZS_DECLARE_TEAR_AWAY_METHOD_23(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)                            ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_23(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)
#define ZS_DECLARE_TEAR_AWAY_METHOD_24(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)                        ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_24(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)
#define ZS_DECLARE_TEAR_AWAY_METHOD_25(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)                    ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_25(ZS_LIB_NO_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)

#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_0(xMethod)                                                                                                          ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_0(ZS_LIB_CONST, xMethod)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_1(xMethod,t1)                                                                                                       ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_1(ZS_LIB_CONST, xMethod,t1)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_2(xMethod,t1,t2)                                                                                                    ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_2(ZS_LIB_CONST, xMethod,t1,t2)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_3(xMethod,t1,t2,t3)                                                                                                 ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_3(ZS_LIB_CONST, xMethod,t1,t2,t3)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_4(xMethod,t1,t2,t3,t4)                                                                                              ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_4(ZS_LIB_CONST, xMethod,t1,t2,t3,t4)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_5(xMethod,t1,t2,t3,t4,t5)                                                                                           ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_5(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_6(xMethod,t1,t2,t3,t4,t5,t6)                                                                                        ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_6(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_7(xMethod,t1,t2,t3,t4,t5,t6,t7)                                                                                     ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_7(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_8(xMethod,t1,t2,t3,t4,t5,t6,t7,t8)                                                                                  ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_8(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_9(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)                                                                               ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_9(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_10(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)                                                                          ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_10(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_11(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)                                                                      ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_11(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_12(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)                                                                  ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_12(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_13(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)                                                              ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_13(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_14(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)                                                          ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_14(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_15(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)                                                      ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_15(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_16(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)                                                  ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_16(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_17(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)                                              ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_17(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_18(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)                                          ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_18(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_19(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)                                      ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_19(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_20(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)                                  ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_20(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_21(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)                              ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_21(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_22(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)                          ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_22(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_23(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)                      ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_23(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_24(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)                  ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_24(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_25(xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)              ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_25(ZS_LIB_CONST, xMethod,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)

#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_0(xMethod,r1)                                                                                                      ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_0(ZS_LIB_NO_CONST, xMethod,r1)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_1(xMethod,r1,t1)                                                                                                   ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_1(ZS_LIB_NO_CONST, xMethod,r1,t1)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_2(xMethod,r1,t1,t2)                                                                                                ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_2(ZS_LIB_NO_CONST, xMethod,r1,t1,t2)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_3(xMethod,r1,t1,t2,t3)                                                                                             ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_3(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_4(xMethod,r1,t1,t2,t3,t4)                                                                                          ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_4(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_5(xMethod,r1,t1,t2,t3,t4,t5)                                                                                       ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_5(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_6(xMethod,r1,t1,t2,t3,t4,t5,t6)                                                                                    ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_6(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_7(xMethod,r1,t1,t2,t3,t4,t5,t6,t7)                                                                                 ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_7(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_8(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8)                                                                              ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_8(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_9(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9)                                                                           ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_9(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_10(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)                                                                      ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_10(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_11(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)                                                                  ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_11(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_12(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)                                                              ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_12(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_13(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)                                                          ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_13(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_14(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)                                                      ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_14(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_15(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)                                                  ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_15(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_16(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)                                              ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_16(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_17(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)                                          ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_17(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_18(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)                                      ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_18(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_19(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)                                  ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_19(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_20(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)                              ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_20(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_21(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)                          ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_21(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_22(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)                      ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_22(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_23(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)                  ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_23(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_24(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)              ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_24(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)
#define ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_25(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)          ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_25(ZS_LIB_NO_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)

#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_0(xMethod,r1)                                                                                                ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_0(ZS_LIB_CONST, xMethod,r1)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_1(xMethod,r1,t1)                                                                                             ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_1(ZS_LIB_CONST, xMethod,r1,t1)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_2(xMethod,r1,t1,t2)                                                                                          ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_2(ZS_LIB_CONST, xMethod,r1,t1,t2)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_3(xMethod,r1,t1,t2,t3)                                                                                       ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_3(ZS_LIB_CONST, xMethod,r1,t1,t2,t3)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_4(xMethod,r1,t1,t2,t3,t4)                                                                                    ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_4(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_5(xMethod,r1,t1,t2,t3,t4,t5)                                                                                 ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_5(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_6(xMethod,r1,t1,t2,t3,t4,t5,t6)                                                                              ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_6(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_7(xMethod,r1,t1,t2,t3,t4,t5,t6,t7)                                                                           ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_7(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_8(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8)                                                                        ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_8(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_9(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9)                                                                     ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_9(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_10(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)                                                                ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_10(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_11(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)                                                            ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_11(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_12(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)                                                        ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_12(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_13(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)                                                    ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_13(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_14(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)                                                ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_14(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_15(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)                                            ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_15(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_16(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)                                        ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_16(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_17(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)                                    ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_17(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_18(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)                                ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_18(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_19(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)                            ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_19(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_20(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)                        ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_20(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_21(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)                    ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_21(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_22(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)                ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_22(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_23(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)            ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_23(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_24(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)        ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_24(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24)
#define ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_25(xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)    ZS_INTERNAL_DECLARE_TEAR_AWAY_METHOD_RETURN_25(ZS_LIB_CONST, xMethod,r1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25)

#endif //ZSLIB_TEAR_AWAY_H_1b92b54dd4fd3fc7f9059b2fa3c3156c8dea29a1
