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

#pragma once

#ifdef ZSLIB_EVENTING_NOOP

#ifndef ZSLIB_EVENTING_NOOP_H_986d96e7409b5a16c1d498406ec8fb135898b788
#define ZSLIB_EVENTING_NOOP_H_986d96e7409b5a16c1d498406ec8fb135898b788

#include <zsLib/types.h>

#define ZS_EVENTING_0(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode)
#define ZS_EVENTING_1(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1)
#define ZS_EVENTING_2(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2)
#define ZS_EVENTING_3(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3)
#define ZS_EVENTING_4(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4)
#define ZS_EVENTING_5(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5)
#define ZS_EVENTING_6(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6)
#define ZS_EVENTING_7(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7)
#define ZS_EVENTING_8(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8)
#define ZS_EVENTING_9(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9)
#define ZS_EVENTING_10(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10)
#define ZS_EVENTING_11(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11)
#define ZS_EVENTING_12(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11, xType12, xName12, xValue12)
#define ZS_EVENTING_13(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11, xType12, xName12, xValue12, xType13, xName13, xValue13)
#define ZS_EVENTING_14(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11, xType12, xName12, xValue12, xType13, xName13, xValue13, xType14, xName14, xValue14)
#define ZS_EVENTING_15(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11, xType12, xName12, xValue12, xType13, xName13, xValue13, xType14, xName14, xValue14, xType15, xName15, xValue15)
#define ZS_EVENTING_16(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11, xType12, xName12, xValue12, xType13, xName13, xValue13, xType14, xName14, xValue14, xType15, xName15, xValue15, xType16, xName16, xValue16)
#define ZS_EVENTING_17(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11, xType12, xName12, xValue12, xType13, xName13, xValue13, xType14, xName14, xValue14, xType15, xName15, xValue15, xType16, xName16, xValue16, xType17, xName17, xValue17)
#define ZS_EVENTING_18(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11, xType12, xName12, xValue12, xType13, xName13, xValue13, xType14, xName14, xValue14, xType15, xName15, xValue15, xType16, xName16, xValue16, xType17, xName17, xValue17, xType18, xName18, xValue18)
#define ZS_EVENTING_19(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11, xType12, xName12, xValue12, xType13, xName13, xValue13, xType14, xName14, xValue14, xType15, xName15, xValue15, xType16, xName16, xValue16, xType17, xName17, xValue17, xType18, xName18, xValue18, xType19, xName19, xValue19)
#define ZS_EVENTING_20(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11, xType12, xName12, xValue12, xType13, xName13, xValue13, xType14, xName14, xValue14, xType15, xName15, xValue15, xType16, xName16, xValue16, xType17, xName17, xValue17, xType18, xName18, xValue18, xType19, xName19, xValue19, xType20, xName20, xValue20)
#define ZS_EVENTING_21(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11, xType12, xName12, xValue12, xType13, xName13, xValue13, xType14, xName14, xValue14, xType15, xName15, xValue15, xType16, xName16, xValue16, xType17, xName17, xValue17, xType18, xName18, xValue18, xType19, xName19, xValue19, xType20, xName20, xValue20, xType21, xName21, xValue21)
#define ZS_EVENTING_22(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11, xType12, xName12, xValue12, xType13, xName13, xValue13, xType14, xName14, xValue14, xType15, xName15, xValue15, xType16, xName16, xValue16, xType17, xName17, xValue17, xType18, xName18, xValue18, xType19, xName19, xValue19, xType20, xName20, xValue20, xType21, xName21, xValue21, xType22, xName22, xValue22)
#define ZS_EVENTING_23(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11, xType12, xName12, xValue12, xType13, xName13, xValue13, xType14, xName14, xValue14, xType15, xName15, xValue15, xType16, xName16, xValue16, xType17, xName17, xValue17, xType18, xName18, xValue18, xType19, xName19, xValue19, xType20, xName20, xValue20, xType21, xName21, xValue21, xType22, xName22, xValue22, xType23, xName23, xValue23)
#define ZS_EVENTING_24(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11, xType12, xName12, xValue12, xType13, xName13, xValue13, xType14, xName14, xValue14, xType15, xName15, xValue15, xType16, xName16, xValue16, xType17, xName17, xValue17, xType18, xName18, xValue18, xType19, xName19, xValue19, xType20, xName20, xValue20, xType21, xName21, xValue21, xType22, xName22, xValue22, xType23, xName23, xValue23, xType24, xName24, xValue24)
#define ZS_EVENTING_25(xSubsystem, xSeverity, xLevel, xSymbol, xChannelID, xTaskID, xOpCode, xType1, xName1, xValue1, xType2, xName2, xValue2, xType3, xName3, xValue3, xType4, xName4, xValue4, xType5, xName5, xValue5, xType6, xName6, xValue6, xType7, xName7, xValue7, xType8, xName8, xValue8, xType9, xName9, xValue9, xType10, xName10, xValue10, xType11, xName11, xValue11, xType12, xName12, xValue12, xType13, xName13, xValue13, xType14, xName14, xValue14, xType15, xName15, xValue15, xType16, xName16, xValue16, xType17, xName17, xValue17, xType18, xName18, xValue18, xType19, xName19, xValue19, xType20, xName20, xValue20, xType21, xName21, xValue21, xType22, xName22, xValue22, xType23, xName23, xValue23, xType24, xName24, xValue24, xType25, xName25, xValue25)

#define ZS_EVENTING_REGISTER(xName)
#define ZS_EVENTING_UNREGISTER(xName)

#define ZS_EVENTING_PROVIDER(xUUID, xName, xSymbolName, xDescription, xResourceName)
#define ZS_EVENTING_ALIAS(xAliasInput, xAliasOuput)
#define ZS_EVENTING_INCLUDE(xSourceStr)
#define ZS_EVENTING_SOURCE(xSourceStr)
#define ZS_EVENTING_CHANNEL(xID, xNameStr, xOperationalType)
#define ZS_EVENTING_TASK(xName)
#define ZS_EVENTING_OPCODE(xName)
#define ZS_EVENTING_TASK_OPCODE(xTaskName, xOpCodeName)

namespace zsLib
{
  namespace eventing
  {
  }
}

#endif //ZSLIB_EVENTING_NOOP_H_986d96e7409b5a16c1d498406ec8fb135898b788

#endif //ZSLIB_EVENTING_NOOP
