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

#include <zsLib/Numeric.h>
#include <zsLib/types.h>
#include <zsLib/Exception.h>
#include <zsLib/helpers.h>

#include <math.h>

#ifdef _WIN32
#include <objbase.h>
#endif //_WIN32

#pragma warning(push)
#pragma warning(disable:4290)

char *strptime(const char *buf, const char *fmt, struct tm *tm);

namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib)}

using namespace std;

namespace zsLib
{
  namespace compatibility {
#ifdef _WIN32
    using zsLib::internal::uuid_wrapper;

    int uuid_parse(const char *input, uuid_wrapper::raw_uuid_type &output) {

      String str(input);
      if (str.isEmpty()) return -1;
      if ('{' != *(str.c_str())) str = "{" + str + "}";

      std::wstring convertedStr = str.wstring();
      CLSID clsID {};
      auto result = CLSIDFromString(convertedStr.c_str(), &clsID);

      memcpy(&output, &clsID, sizeof(output));

      if (NOERROR == result) return 0;
      return -1;
    }
#endif //_WIN32
  }

  using namespace compatibility;

  template<typename t_type>
  void Numeric<t_type>::get(t_type &outValue) const throw (ValueOutOfRange)
  {
    if (std::numeric_limits<t_type>::is_signed) {
      LONGLONG result = 0;
      bool converted = internal::convert(mData, result, sizeof(t_type), mIngoreWhitespace, mBase);
      if (!converted)
        ZS_THROW_CUSTOM(ValueOutOfRange, "Cannot convert value (generic): " + mData)
      outValue = static_cast<t_type>(result);
      return;
    }
    ULONGLONG result = 0;
    bool converted = internal::convert(mData, result, sizeof(t_type), mIngoreWhitespace, mBase);
    if (!converted)
      ZS_THROW_CUSTOM(ValueOutOfRange, "Cannot convert value: " + mData)
    outValue = static_cast<t_type>(result);
  }

  template<>
  void Numeric<bool>::get(bool &outValue) const throw (ValueOutOfRange)
  {
    bool result = 0;
    bool converted = internal::convert(mData, result, mIngoreWhitespace);
    if (!converted)
      ZS_THROW_CUSTOM(ValueOutOfRange, "Cannot convert value (bool): " + mData)
    outValue = result;
  }

  template<>
  void Numeric<float>::get(float &outValue) const throw (ValueOutOfRange)
  {
    float result = 0;
    bool converted = internal::convert(mData, result, mIngoreWhitespace);
    if (!converted)
      ZS_THROW_CUSTOM(ValueOutOfRange, "Cannot convert value (float): " + mData)
    outValue = result;
  }

  template<>
  void Numeric<double>::get(double &outValue) const throw (ValueOutOfRange)
  {
    double result = 0;
    bool converted = internal::convert(mData, result, mIngoreWhitespace);
    if (!converted)
      ZS_THROW_CUSTOM(ValueOutOfRange, "Cannot convert value (double): " + mData)
    outValue = result;
  }

  template<>
  void Numeric<UUID>::get(UUID &outValue) const throw (ValueOutOfRange)
  {
    UUID result;
    bool converted = internal::convert(mData, result, mIngoreWhitespace);
    if (!converted)
      ZS_THROW_CUSTOM(ValueOutOfRange, "Cannot convert value (UUID): " + mData)
    outValue = result;
  }

  template<>
  void Numeric<Time>::get(Time &outValue) const throw (ValueOutOfRange)
  {
    Time result;
    bool converted = internal::convert(mData, result, mIngoreWhitespace);
    if (!converted)
      ZS_THROW_CUSTOM(ValueOutOfRange, "Cannot convert value (Time): " + mData)
    outValue = result;
  }

  template<class Converter>
  void simpleDurationConvert(
                             typename Converter::NumericType &outValue,
                             const String &data,
                             bool ignoreWhiteSpace,
                             CSTR failureMessage
                             ) throw (typename Converter::ValueOutOfRange)
  {
    typedef typename Converter::ValueOutOfRange CustomValueOutOfRange;
    typename Converter::NumericType::rep value = 0;

    try {
      value = Numeric<typename Converter::NumericType::rep>(data, ignoreWhiteSpace);
    } catch(typename Numeric<typename Converter::NumericType::rep>::ValueOutOfRange &) {
      ZS_THROW_CUSTOM(CustomValueOutOfRange, failureMessage + data)
    }
    outValue = typename Converter::NumericType(value);
  }

  template<>
  void Numeric<Hours>::get(Hours &outValue) const throw (ValueOutOfRange)
  {
    simpleDurationConvert< Numeric<Hours> >(outValue, mData, mIngoreWhitespace, "Cannot convert value (Hours): ");
  }

  template<>
  void Numeric<Minutes>::get(Minutes &outValue) const throw (ValueOutOfRange)
  {
    simpleDurationConvert< Numeric<Minutes> >(outValue, mData, mIngoreWhitespace, "Cannot convert value (Minutes): ");
  }

  template<>
  void Numeric<Seconds>::get(Seconds &outValue) const throw (ValueOutOfRange)
  {
    simpleDurationConvert< Numeric<Seconds> >(outValue, mData, mIngoreWhitespace, "Cannot convert value (Seconds): ");
  }

  template<class Converter>
  void fractionalDurationConvert(
                                 typename Converter::NumericType &outValue,
                                 const String &data,
                                 bool ignoreWhiteSpace,
                                 CSTR failureMessage
                                 ) throw (typename Converter::ValueOutOfRange)
  {
    typedef typename Converter::ValueOutOfRange CustomValueOutOfRange;

    typename Converter::NumericType::rep value = 0;

    String temp = data;
    if (ignoreWhiteSpace) {
      temp.trim();
    }

    size_t requiredDigits = static_cast<size_t>(log10(Converter::NumericType::period::den));

    std::size_t found = data.find('.');

    String fractional;

    if (std::string::npos != found) {
      fractional = temp.substr(found+1);
      temp.erase(found);
    }

    if (fractional.length() < requiredDigits) {
      fractional += std::string(requiredDigits - fractional.length(), '0');
    } else if (fractional.length() > requiredDigits) {
      fractional.erase(requiredDigits);
    }

    temp += fractional;

    try {
      value = Numeric<typename Converter::NumericType::rep>(temp, false);
    } catch(typename Numeric<typename Converter::NumericType::rep>::ValueOutOfRange &) {
      ZS_THROW_CUSTOM(CustomValueOutOfRange, failureMessage + data)
    }
    outValue = typename Converter::NumericType(value);
  }

  template<>
  void Numeric<Milliseconds>::get(Milliseconds &outValue) const throw (ValueOutOfRange)
  {
    fractionalDurationConvert< Numeric<Milliseconds> >(outValue, mData, mIngoreWhitespace, "Cannot convert value (Milliseconds): ");
  }

  template<>
  void Numeric<Microseconds>::get(Microseconds &outValue) const throw (ValueOutOfRange)
  {
    fractionalDurationConvert< Numeric<Microseconds> >(outValue, mData, mIngoreWhitespace, "Cannot convert value (Microseconds): ");
  }

  template<>
  void Numeric<Nanoseconds>::get(Nanoseconds &outValue) const throw (ValueOutOfRange)
  {
    fractionalDurationConvert< Numeric<Nanoseconds> >(outValue, mData, mIngoreWhitespace, "Cannot convert value (Nanoseconds): ");
  }

  // force templates to be generated for these types
  template void Numeric<CHAR>::get(CHAR &) const throw (ValueOutOfRange);
  template void Numeric<UCHAR>::get(UCHAR &) const throw (ValueOutOfRange);
  template void Numeric<SHORT>::get(SHORT &) const throw (ValueOutOfRange);
  template void Numeric<USHORT>::get(USHORT &) const throw (ValueOutOfRange);
  template void Numeric<INT>::get(INT &) const throw (ValueOutOfRange);
  template void Numeric<UINT>::get(UINT &) const throw (ValueOutOfRange);
  template void Numeric<LONG>::get(LONG &) const throw (ValueOutOfRange);
  template void Numeric<ULONG>::get(ULONG &) const throw (ValueOutOfRange);
  template void Numeric<LONGLONG>::get(LONGLONG &) const throw (ValueOutOfRange);
  template void Numeric<ULONGLONG>::get(ULONGLONG &) const throw (ValueOutOfRange);
#ifndef _WIN32
  template void Numeric<bool>::get(bool &) const throw (ValueOutOfRange);
  template void Numeric<float>::get(float &) const throw (ValueOutOfRange);
  template void Numeric<double>::get(double &) const throw (ValueOutOfRange);
  template void Numeric<UUID>::get(UUID &) const throw (ValueOutOfRange);
#endif //_WIN32

  namespace internal
  {
    // 0    1    2    3    4   5    6    7    8    9    A    B    C    D    E    F
    //NUL  SOH  STX  ETX  EOT  ENQ  ACK  BEL  BS   HT   LF   VT   FF   CR   SO   SI   // 0
    //DLE  DC1  DC2  DC3  DC4  NAK  SYN  ETB  CAN  EM   SUB  ESC  FS   GS   RS   US   // 1
    // SP   !    "    #    $    %    &    '    (    )    *    +    ,    -    .    /   // 2
    // 0    1    2    3    4    5    6    7    8    9    :    ;    <    =    >    ?   // 3
    // @    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O   // 4
    // P    Q    R    S    T    U    V    W    X    Y    Z    [    \    ]    ^    _   // 5
    // `    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o   // 6
    // p    q    r    s    t    u    v    w    x    y    z    {    |    }    ~   DEL  // 7
    static BYTE gDigitToValue[256] = {
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 0
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 1
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 2
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 3
      0xFF,10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,   // 4
      25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  0xFF,0xFF,0xFF,0xFF,0xFF, // 5
      0xFF,10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,   // 6
      25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  0xFF,0xFF,0xFF,0xFF,0xFF, // 7

      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 8
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 9
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 10
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 11
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 12
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 13
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 14
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 15
    };

    static ULONGLONG gMinLongLongs[] =
    {
      0xFFFFFFFFFFFFFF80ULL,  // 1 bytes
      0xFFFFFFFFFFFF8000ULL,  // 2 bytes
      0xFFFFFFFFFF800000ULL,  // 3 bytes
      0xFFFFFFFF80000000ULL,  // 4 bytes
      0xFFFFFF8000000000ULL,  // 5 bytes
      0xFFFF800000000000ULL,  // 6 bytes
      0xFF80000000000000ULL,  // 7 bytes
      0x8000000000000000ULL   // 8 bytes
    };

    static LONGLONG gMaxLongLongs[] =
    {
      0x000000000000007FLL,  // 1 bytes
      0x0000000000007FFFLL,  // 2 bytes
      0x00000000007FFFFFLL,  // 3 bytes
      0x000000007FFFFFFFLL,  // 4 bytes
      0x0000007FFFFFFFFFLL,  // 5 bytes
      0x00007FFFFFFFFFFFLL,  // 6 bytes
      0x007FFFFFFFFFFFFFLL,  // 7 bytes
      0x7FFFFFFFFFFFFFFFLL   // 8 bytes
    };

    static ULONGLONG gMaxULongLongs[] =
    {
      0x00000000000000FFULL,  // 1 bytes
      0x000000000000FFFFULL,  // 2 bytes
      0x0000000000FFFFFFULL,  // 3 bytes
      0x00000000FFFFFFFFULL,  // 4 bytes
      0x000000FFFFFFFFFFULL,  // 5 bytes
      0x0000FFFFFFFFFFFFULL,  // 6 bytes
      0x00FFFFFFFFFFFFFFULL,  // 7 bytes
      0xFFFFFFFFFFFFFFFFULL   // 8 bytes
    };

    void skipSpace(CSTR &ioStr)
    {
      while (isspace(*ioStr)) {
        ++ioStr;
      }
    }

    bool isPositive(CSTR &ioStr, bool &hadSign)
    {
      hadSign = false;
      if ('-' == *ioStr) {
        ++ioStr;
        hadSign = true;
        return false;
      }
      if ('+' == *ioStr) {
        ++ioStr;
        hadSign = true;
        return true;
      }
      return true;
    }

    size_t shouldForceBase(CSTR &ioStr, size_t base, bool hadSign)
    {
      if ('x' == ioStr[1]) {
        ioStr += 2;
        return 16;
      }
      return base;
    }

    bool convert(const String &input, LONGLONG &outResult, size_t size, bool ignoreWhiteSpace, size_t base)
    {
      ZS_THROW_INVALID_USAGE_IF((0 == size) || (size > sizeof(QWORD)))

      CSTR str = input;
      if (NULL == str)
        return false;

      if (ignoreWhiteSpace)
        skipSpace(str);

      bool hadSign = false;
      bool positive = isPositive(str, hadSign);
      base = shouldForceBase(str, base, hadSign);

      LONGLONG result = 0;

      BYTE digit = 0;
      if (positive) {
        while ((digit = gDigitToValue[*str]) < base) {
          LONGLONG lastResult = result;

          result *= base;
          result += static_cast<LONGLONG>(digit);
          if (result < lastResult)
            return false; // value had to flip
          ++str;
        }
        if (result > gMaxLongLongs[size-1])
          return false;
      } else {
        while ((digit = gDigitToValue[*str]) < base) {
          LONGLONG lastResult = result;

          result *= base;
          result -= static_cast<LONGLONG>(digit);
          if (lastResult < result)
            return false; // value had to flip
          ++str;
        }
        if (result < static_cast<LONGLONG>(gMinLongLongs[size-1]))
          return false;
      }

      if (ignoreWhiteSpace)
        skipSpace(str);

      if (0 != *str)
        return false;

      outResult = result;
      return true;
    }

    bool convert(const String &input, ULONGLONG &outResult, size_t size, bool ignoreWhiteSpace, size_t base)
    {
      ZS_THROW_INVALID_USAGE_IF((0 == size) || (size > sizeof(QWORD)))

      CSTR str = input;
      if (NULL == str)
        return false;

      if (ignoreWhiteSpace)
        skipSpace(str);

      bool hadSign = false;
      bool positive = isPositive(str, hadSign);
      if (!positive)
        return false;
      base = shouldForceBase(str, base, hadSign);

      ULONGLONG result = 0;

      BYTE digit = 0;
      while ((digit = gDigitToValue[*str]) < base) {
        ULONGLONG lastResult = result;

        result *= base;
        result += static_cast<ULONGLONG>(digit);
        if (result < lastResult)
          return false; // value had to flip
        ++str;
      }
      if (result > gMaxULongLongs[size-1])
        return false;

      if (ignoreWhiteSpace)
        skipSpace(str);

      if (0 != *str)
        return false;

      outResult = result;
      return true;
    }

    bool convert(const String &input, bool &outResult, bool ignoreWhiteSpace)
    {
      String temp = input;
      if (ignoreWhiteSpace)
        temp.trim();

      if ((0 == input.compareNoCase("true")) ||
          (0 == input.compareNoCase("t"))) {
        outResult = true;
        return true;
      }

      if ((0 == input.compareNoCase("false")) ||
          (0 == input.compareNoCase("f"))) {
        outResult = false;
        return true;
      }

      LONGLONG value = 0;
      bool result = convert(input, value, sizeof(LONGLONG), ignoreWhiteSpace, 10);
      if (!result)
        return false;
      outResult = (value == 0 ? false : true);
      return true;
    }

    bool convert(const String &input, float &outResult, bool ignoreWhiteSpace)
    {
      String temp = input;
      if (ignoreWhiteSpace)
        temp.trim();

      try {
        float result = std::stof(temp);
        outResult = result;
      } catch(...) {
        return false;
      }
      return true;
    }

    bool convert(const String &input, double &outResult, bool ignoreWhiteSpace)
    {
      String temp = input;
      if (ignoreWhiteSpace)
        temp.trim();

      try {
        double result = std::stod(temp);
        outResult = result;
      } catch(...) {
        return false;
      }
      return true;
    }

    bool convert(const String &input, UUID &outResult, bool ignoreWhiteSpace)
    {
      String temp = input;
      if (ignoreWhiteSpace)
        temp.trim();

#ifndef _WIN32
      temp.trimLeft("{");
      temp.trimRight("}");
#endif //ndef _WIN32

      if (0 == uuid_parse(temp.c_str(), outResult.mUUID)) return true;
      return false;
    }

    bool convert(const String &input, Time &outResult, bool ignoreWhiteSpace)
    {
      outResult = Time();

      String temp = input;
      if (ignoreWhiteSpace)
        temp.trim();

      String more;

      std::size_t posMore = temp.find('.');

      if (std::string::npos != posMore) {
        more = temp.substr(posMore+1);
        temp.erase(posMore);
      }

      try 
	  {
          zsLib::Seconds::rep seconds = Numeric<zsLib::Seconds::rep>(temp);
          outResult = zsLib::timeSinceEpoch(zsLib::Seconds(seconds));
      } catch(const Numeric<zsLib::Seconds::rep>::ValueOutOfRange &) {
        return false;
      }

      if (more.hasData()) {
        String fractionStr("0.");
        fractionStr += more;

        double x {};

        try {
          x = std::stod(fractionStr);
        } catch(...) {
          return false;
        }

        x = x * 1000000;

        std::chrono::microseconds fraction(static_cast<unsigned long>(round(x)));

        outResult += fraction;
      }

      return true;
    }

  }
}

#pragma warning(pop)
