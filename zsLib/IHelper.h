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

    static String toString(ElementPtr el);
    static ElementPtr toJSON(const char *str);

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
