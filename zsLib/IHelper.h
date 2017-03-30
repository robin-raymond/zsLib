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

#include <zsLib/types.h>
#include <zsLib/Log.h>

namespace zsLib
{
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark IHelper
  #pragma mark

  interaction IHelper
  {
    ZS_DECLARE_TYPEDEF_PTR(XML::Element, Element);
    ZS_DECLARE_TYPEDEF_PTR(XML::Document, Document);
    ZS_DECLARE_TYPEDEF_PTR(XML::Text, Text);

    typedef size_t Index;
    typedef std::map<Index, String> SplitMap;
    typedef std::list<String> StringList;
    typedef std::set<IPAddress> IPAddressSet;

    static void setup();
#ifdef WINRT
    static void setup(Windows::UI::Core::CoreDispatcher ^dispatcher);
#endif //WINRT

    static RecursiveLockPtr getGlobalLock();

    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, bool &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, CHAR &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, UCHAR &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, SHORT &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, USHORT &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, LONG &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, ULONG &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, LONGLONG &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, ULONGLONG &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, INT &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, UINT &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, FLOAT &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, DOUBLE &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, String &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Time &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Milliseconds &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Microseconds &outValue);

    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<bool> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<CHAR> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<UCHAR> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<SHORT> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<USHORT> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<LONG> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<ULONG> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<LONGLONG> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<ULONGLONG> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<INT> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<UINT> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<FLOAT> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<DOUBLE> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<String> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<Time> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<Milliseconds> &outValue);
    static void getElementValue(ElementPtr elem, const char *logObjectName, const char *subElementName, Optional<Microseconds> &outValue);

    static void adoptElementValue(ElementPtr elem, const char *subElementName, bool value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, CHAR value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, UCHAR value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, SHORT value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, USHORT value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, LONG value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, ULONG value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, LONGLONG value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, ULONGLONG value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, INT value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, UINT value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, FLOAT value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, DOUBLE value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const String &value, bool adoptEmptyValue);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Time &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Milliseconds &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Microseconds &value);

    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<bool> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<CHAR> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<UCHAR> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<SHORT> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<USHORT> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<LONG> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<ULONG> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<LONGLONG> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<ULONGLONG> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<INT> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<UINT> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<FLOAT> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<DOUBLE> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<String> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<Time> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<Milliseconds> &value);
    static void adoptElementValue(ElementPtr elem, const char *subElementName, const Optional<Microseconds> &value);

    static void debugAppend(ElementPtr &parentEl, const char *name, const char *value);
    static void debugAppend(ElementPtr &parentEl, const char *name, const String &value);
    static void debugAppendNumber(ElementPtr &parentEl, const char *name, const String &value);
    static void debugAppend(ElementPtr &parentEl, const char *name, bool value, bool ignoreIfFalse = true);
    static void debugAppend(ElementPtr &parentEl, const char *name, CHAR value, bool ignoreIfZero = true);
    static void debugAppend(ElementPtr &parentEl, const char *name, UCHAR value, bool ignoreIfZero = true);
    static void debugAppend(ElementPtr &parentEl, const char *name, SHORT value, bool ignoreIfZero = true);
    static void debugAppend(ElementPtr &parentEl, const char *name, USHORT value, bool ignoreIfZero = true);
    static void debugAppend(ElementPtr &parentEl, const char *name, INT value, bool ignoreIfZero = true);
    static void debugAppend(ElementPtr &parentEl, const char *name, UINT value, bool ignoreIfZero = true);
    static void debugAppend(ElementPtr &parentEl, const char *name, LONG value, bool ignoreIfZero = true);
    static void debugAppend(ElementPtr &parentEl, const char *name, ULONG value, bool ignoreIfZero = true);
    static void debugAppend(ElementPtr &parentEl, const char *name, LONGLONG value, bool ignoreIfZero = true);
    static void debugAppend(ElementPtr &parentEl, const char *name, ULONGLONG value, bool ignoreIfZero = true);
    static void debugAppend(ElementPtr &parentEl, const char *name, FLOAT value, bool ignoreIfZero = true);
    static void debugAppend(ElementPtr &parentEl, const char *name, DOUBLE value, bool ignoreIfZero = true);
    static void debugAppend(ElementPtr &parentEl, const char *name, const Time &value);
    static void debugAppend(ElementPtr &parentEl, const char *name, const Hours &value);
    static void debugAppend(ElementPtr &parentEl, const char *name, const Minutes &value);
    static void debugAppend(ElementPtr &parentEl, const char *name, const Seconds &value);
    static void debugAppend(ElementPtr &parentEl, const char *name, const Milliseconds &value);
    static void debugAppend(ElementPtr &parentEl, const char *name, const Microseconds &value);
    static void debugAppend(ElementPtr &parentEl, const char *name, const Nanoseconds &value);

    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<String> &value)        {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value());}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<bool> &value)          {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value(), false);}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<CHAR> &value)          {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value(), false);}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<UCHAR> &value)         {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value(), false);}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<SHORT> &value)         {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value(), false);}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<USHORT> &value)        {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value(), false);}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<INT> &value)           {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value(), false);}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<UINT> &value)          {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value(), false);}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<LONG> &value)          {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value(), false);}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<ULONG> &value)         {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value(), false);}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<LONGLONG> &value)      {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value(), false);}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<ULONGLONG> &value)     {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value(), false);}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<FLOAT> &value)         {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value(), false);}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<DOUBLE> &value)        {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value(), false);}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<Time> &value)          {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value());}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<Hours> &value)         {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value());}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<Minutes> &value)       {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value());}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<Seconds> &value)       {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value());}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<Milliseconds> &value)  {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value());}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<Microseconds> &value)  {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value());}
    static void debugAppend(ElementPtr &parentEl, const char *name, const Optional<Nanoseconds> &value)   {if (!value.hasValue()) return; debugAppend(parentEl, name, value.value());}

    static void debugAppend(ElementPtr &parentEl, const Log::Param &param);
    static void debugAppend(ElementPtr &parentEl, const char *name, ElementPtr childEl);
    static void debugAppend(ElementPtr &parentEl, ElementPtr childEl);

    static String toString(
                           ElementPtr el,
                           bool formatAsJson = true
                           );
    static ElementPtr toJSON(const char *str);
    static ElementPtr toXML(const char *str);

    static String getAttributeID(ElementPtr el);
    static void setAttributeIDWithText(ElementPtr el, const String &value);
    static void setAttributeIDWithNumber(ElementPtr el, const String &value);

    static String getAttribute(
                                ElementPtr el,
                                const String &attributeName
                                );

    static void setAttributeWithText(
                                      ElementPtr el,
                                      const String &attrName,
                                      const String &value
                                      );

    static void setAttributeWithNumber(
                                        ElementPtr el,
                                        const String &attrName,
                                        const String &value
                                        );

    static ElementPtr createElement(const String &elName);

    static ElementPtr createElementWithText(
                                            const String &elName,
                                            const String &textVal
                                            );
    static ElementPtr createElementWithNumber(
                                              const String &elName,
                                              const String &numberAsStringValue
                                              );
    static ElementPtr createElementWithTime(
                                            const String &elName,
                                            Time time
                                            );
    static ElementPtr createElementWithTextAndJSONEncode(
                                                          const String &elName,
                                                          const String &textVal
                                                          );
    static ElementPtr createElementWithTextID(
                                              const String &elName,
                                              const String &idValue
                                              );
    static ElementPtr createElementWithNumberID(
                                                const String &elName,
                                                const String &idValue
                                                );

    static TextPtr createText(const String &textVal);

    static String getElementText(ElementPtr el);
    static String getElementTextAndDecode(ElementPtr el);

    static String timeToString(const Time &value);
    static Time stringToTime(const String &str);

    static WORD getBE16(const void* memory);
    static DWORD getBE32(const void* memory);
    static QWORD getBE64(const void* memory);
    static void setBE16(void* memory, WORD v);
    static void setBE32(void* memory, DWORD v);
    static void setBE64(void* memory, QWORD v);

    static ElementPtr cloneAsCanonicalJSON(ElementPtr element);

    static void split(
                      const String &input,
                      SplitMap &outResult,
                      char splitChar
                      );

    static void split(
                      const String &input,
                      SplitMap &outResult,
                      const char *splitStr
                      );

    static void splitPruneEmpty(
                                SplitMap &ioResult,
                                bool reindex = true
                                );

    static void splitTrim(SplitMap &ioResult);

    static String combine(
                          const SplitMap &input,
                          const char *combineStr
                          );
    static String combine(
                          const StringList &input,
                          const char *combineStr
                          );

    static const String &get(
                              const SplitMap &inResult,
                              Index index
                              );

    static String getDebugString(
                                 const BYTE *buffer,
                                 size_t bufferSizeInBytes,
                                 size_t bytesPerGroup = 4,
                                 size_t maxLineLength = 160
                                 );

    static void parseIPs(
                         const String &ipList,
                         IPAddressSet &outIPs,
                         char splitChar = ','
                         );
    static bool containsIP(
                           const IPAddressSet &inMap,
                           const IPAddress &ip,
                           bool emptySetReturns = true
                           );
  };

} // namespace zsLib
