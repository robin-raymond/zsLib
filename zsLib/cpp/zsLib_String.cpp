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

#include <zsLib/String.h>
#include <zsLib/Log.h>
#include <zsLib/Exception.h>

#include <algorithm>

#define ZS_INTERNAL_UTF8_MAX_CHARACTER_ENCODED_BYTE_SIZE (sizeof(BYTE)*6)

//namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

using namespace std;

namespace zsLib
{
  namespace internal
  {
    typedef WORD UTF16;
    typedef DWORD UTF32;

    typedef UTF16 * UTF16STR;
    typedef UTF32 * UTF32STR;

    typedef const UTF16 * CUTF16STR;
    typedef const UTF32 * CUTF32STR;

    static UTF32 utf8ToUTF32(CSTR &ioUTF8);
    static void utf32ToUTF8(UTF32 utf32Char, STR &outUTF8);

    std::string convertToString(CWSTR value);
    std::wstring convertToWString(CSTR value);

    std::string convertToString(const std::wstring &value);
    std::wstring convertToWString(const std::string &value);

    struct ci_char_traits : public std::char_traits<char> {
      static bool eq(char c1, char c2) { return tolower(c1) == tolower(c2); }
      static bool ne(char c1, char c2) { return tolower(c1) != tolower(c2); }
      static bool lt(char c1, char c2) { return tolower(c1) <  tolower(c2); }
      static int compare(const char* s1, const char* s2, size_t n) {
        while( n-- != 0 ) {
          if( tolower(*s1) < tolower(*s2) ) return -1;
          if( tolower(*s1) > tolower(*s2) ) return 1;
          ++s1; ++s2;
        }

        return 0;
      }

      static const char* find(const char* s, int n, char a) {
        while( n-- > 0 && toupper(*s) != toupper(a) ) {
          ++s;
        }
        return s;
      }
    };

    typedef std::basic_string<char, ci_char_traits> ci_string;
  } // namespace internal

  String::String() : std::string()
  {
  }

  String::String(CSTR value) : std::string(value ? value : std::string())
  {
  }

  String::String(CWSTR value) : std::string(internal::convertToString(value))
  {
  }

  String::String(CSTR value, size_t length) : std::string(value ? value : static_cast<const char *>(NULL), length)
  {
  }

  String::String(CWSTR value, size_t length)
  {
    std::wstring temp(value, length);
    (*this) = String(temp);
  }

  String::String(const std::string &value) : std::string(value)
  {
  }

  String::String(const std::wstring &value) : std::string(internal::convertToString(value))
  {
  }

  String::String(const String &value) : std::string(value)
  {
  }

  String String::copyFrom(CSTR source, size_t maxCharacters)
  {
    if (!source) return String();

    std::unique_ptr<char[]> copy(new char[maxCharacters+1]);

    STR dest = copy.get();
    for (; *source && (maxCharacters > 0); ++source, --maxCharacters, ++dest)
    {
      *dest = *source;
    }
    *dest = 0;
    return String((CSTR)copy.get());
  }

  String String::copyFromUnicodeSafe(CSTR source, size_t maxCharacters)
  {
    if (!source) return String();

    std::unique_ptr<char[]> copy(new char[(maxCharacters*ZS_INTERNAL_UTF8_MAX_CHARACTER_ENCODED_BYTE_SIZE)+1]);

    STR dest = copy.get();
    for (; *source && (maxCharacters > 0); --maxCharacters)
    {
      internal::utf32ToUTF8(internal::utf8ToUTF32(source), dest);
    }
    *dest = 0;
    return String((CSTR)copy.get());
  }

  std::wstring String::wstring() const
  {
    return internal::convertToWString(*this);
  }

  bool String::isEmpty() const
  {
    return empty();
  }

  bool String::hasData() const
  {
    return !empty();
  }

  size_t String::getLength() const
  {
    return (*this).size();
  }

  String::operator CSTR() const
  {
    return (*this).c_str();
  }

  String &String::operator=(const std::string &value)
  {
    if (this == &value)
      return *this;

    (*(dynamic_cast<std::string *>(this))) = (*(dynamic_cast<const std::string *>(&value)));
    return *this;
  }

  String &String::operator=(const std::wstring &value)
  {
    return (*this) = String(value);
  }

  String &String::operator=(const String &value)
  {
    if (this == &value)
      return *this;

    (*(dynamic_cast<std::string *>(this))) = (*(dynamic_cast<const std::string *>(&value)));
    return *this;
  }

  String &String::operator=(CSTR value)
  {
    (*(dynamic_cast<std::string *>(this))) = value;
    return *this;
  }

  String &String::operator=(CWSTR value)
  {
    return (*this) = String(value);
  }

  int String::compareNoCase(CSTR value) const
  {
    internal::ci_string str1(*this);
    internal::ci_string str2(value);
    return str1.compare(str2);
  }

  int String::compareNoCase(const String &value) const
  {
    internal::ci_string str1(*this);
    internal::ci_string str2(value);
    return str1.compare(str2);
  }

  void String::toLower()
  {
#ifdef __QNX__
    char *buffer = new char[length()+1];
    memset(&(buffer[0]), 0, sizeof(char)*(length()+1));

    const char *source = c_str();
    char *dest = buffer;
    for (; *source != '\0'; ++source, ++dest)
    {
      *dest = (char)tolower(*source);
    }

    (*this) = (const char *)buffer;

    delete [] buffer;
    buffer = NULL;
#else
    for (auto & c: *this) c = tolower((UCHAR)c);
#endif //__QNX__
  }

  void String::toUpper()
  {
#ifdef __QNX__
    char *buffer = new char[length()+1];
    memset(&(buffer[0]), 0, sizeof(char)*(length()+1));

    const char *source = c_str();
    char *dest = buffer;
    for (; *source != '\0'; ++source, ++dest)
    {
      *dest = (char)toupper(*source);
    }

    (*this) = (const char *)buffer;

    delete [] buffer;
    buffer = NULL;
#else
    for (auto & c: *this) c = toupper((UCHAR)c);
#endif //__QNX__
  }

  namespace internal {
    struct one_of_check : public std::unary_function<char,bool>{
      one_of_check(CSTR strip) : mStrip(strip) {}

      bool operator()(const char &c)
      {
        return NULL != strchr(mStrip, (UCHAR)c);
      }

      CSTR mStrip;
    };
  }

  void String::trim(CSTR chars)
  {
    const std::string &s = (*this);

    auto  wsfront=std::find_if_not(s.begin(),s.end(), internal::one_of_check(chars));
    *this = std::string(wsfront,std::find_if_not(s.rbegin(),std::string::const_reverse_iterator(wsfront), internal::one_of_check(chars)).base());
  }

  void String::trimLeft(CSTR chars)
  {
    const std::string &s = (*this);
    *this = std::string(std::find_if_not(s.begin(), s.end(), internal::one_of_check(chars)), s.end());
  }

  void String::trimRight(CSTR chars)
  {
    const std::string &s = (*this);
    *this = std::string(s.begin(),std::find_if_not(s.rbegin(),std::string::const_reverse_iterator(s.begin()), internal::one_of_check(chars)).base());
  }

  size_t String::lengthUnicodeSafe() const
  {
    CSTR str = this->c_str();
    if (NULL == str)
      return 0;

    size_t size = 0;
    while (0 != *str)
    {
      internal::utf8ToUTF32(str);
      ++size;
    }
    return size;
  }

  WCHAR String::atUnicodeSafe(size_t pos) const
  {
    CSTR str = this->c_str();
    if (NULL == str) {
      if (0 == pos)
        return 0;
      throw std::out_of_range("String::atUnicodeSafe passed an index value too large");
    }

    size_t size = 0;
    while (0 != *str)
    {
      WCHAR value = (WCHAR)internal::utf8ToUTF32(str);
      if (size == pos)
        return value;
      ++size;
    }
    throw std::out_of_range("String::atUnicodeSafe passed an index value too large");
    return 0;
  }

  String String::substrUnicodeSafe(size_t from, size_t n) const
  {
    CSTR startOfString = (*this).c_str();
    CSTR start = startOfString;

    if (NULL == start)
      return String();

    while ((from > 0) && (0 != *start))
    {
      internal::utf8ToUTF32(start);
      --from;
    }

    if (std::string::npos == n) {
      return String(this->substr(start - startOfString, n));
    }

    CSTR to = start;
    while ((n > 0) && (0 != *to)) {
      internal::utf8ToUTF32(to);
      --n;
    }

    return String(this->substr(start - startOfString, to - start));
  }

  void String::replaceAll(CSTR findStr, CSTR replaceStr, size_t totalOccurances)
  {
    if (NULL == findStr)
      findStr = "";
    if (NULL == replaceStr)
      replaceStr = "";

    size_t findSize = strlen(findStr);
    size_t replaceSize = strlen(replaceStr);
    if (findSize < 1)
      return;

    String::size_type position = (*this).find(findStr); // find first space
    while ((position != std::string::npos) && (0 != totalOccurances))
    {
      (*this).replace(position, findSize, replaceStr);
      position = (*this).find(findStr, position + replaceSize);
      if (npos != totalOccurances)
        --totalOccurances;
    }
  }

  namespace internal
  {
#define ZS_INTERNAL_UTF16_MASK_FLAG_BITS 0xFC00
#define ZS_INTERNAL_UTF16_MASK_10_BITS_VALUE 0x3FF
#define ZS_INTERNAL_UTF16_HIGH_BITS_FLAG 0xD800
#define ZS_INTERNAL_UTF16_LOW_BITS_FLAG 0xDC00
#define ZS_INTERNAL_UTF16_BASE_INDEX 0x10000

#define ZS_INTERNAL_UTF32_NO_SURROGATE_BELOW (0xD7FF+1)
#define ZS_INTERNAL_UTF32_NO_SURROGATE_BETEWEEN_MIN (0xE000-1)
#define ZS_INTERNAL_UTF32_NO_SURROGATE_BETEWEEN_MAX (0xFFFF+1)

#define ZS_INTERNAL_UTF32_MAKE_SURROGATE_BETEWEEN_MIN (0x10000-1)
#define ZS_INTERNAL_UTF32_MAKE_SURROGATE_BETEWEEN_MAX (0x10FFFF+1)

    static UTF32 utf16ToUTF32(CUTF16STR &ioUTF16, bool be)
    {
      const BYTE *ptr = (const BYTE *)(&(ioUTF16[0]));
      UTF16 surrogate1 {};
      UTF16 surrogate2 {};
      if (be) {
        surrogate1 = (((UTF16)(ptr[0])) << 8) | ((UTF16)(ptr[1]));
        surrogate2 = (((UTF16)(ptr[2])) << 8) | ((UTF16)(ptr[3]));
      } else {
        surrogate1 = (((UTF16)(ptr[1])) << 8) | ((UTF16)(ptr[0]));
        surrogate2 = (((UTF16)(ptr[3])) << 8) | ((UTF16)(ptr[2]));
      }

      if (0 == surrogate2) {
        ++ioUTF16;
        return (UTF32)surrogate1;
      }

      if ((ZS_INTERNAL_UTF16_HIGH_BITS_FLAG == (surrogate1 & ZS_INTERNAL_UTF16_MASK_FLAG_BITS)) &&
          (ZS_INTERNAL_UTF16_LOW_BITS_FLAG == (surrogate2 & ZS_INTERNAL_UTF16_MASK_FLAG_BITS))) {
        surrogate1 = surrogate1 & ZS_INTERNAL_UTF16_MASK_10_BITS_VALUE;
        surrogate2 = surrogate2 & ZS_INTERNAL_UTF16_MASK_10_BITS_VALUE;

        UTF32 result = (((UTF32)surrogate1) << 10) | ((UTF32)surrogate2);
        result += ZS_INTERNAL_UTF16_BASE_INDEX;
        ioUTF16 += 2;
        return result;
      }

      ++ioUTF16;
      return surrogate1;
    }

    static void utf32ToUTF16(UTF32 utf32Char, UTF16STR &outResult)
    {
      // direct copy of the value to the result
      if ((utf32Char < ZS_INTERNAL_UTF32_NO_SURROGATE_BELOW) ||
          ((utf32Char > ZS_INTERNAL_UTF32_NO_SURROGATE_BETEWEEN_MIN) &&
           (utf32Char < ZS_INTERNAL_UTF32_NO_SURROGATE_BETEWEEN_MAX))) {
        UTF16 value = (UTF16)utf32Char;
        memcpy(outResult, &value, sizeof(value));
        ++outResult;
        return;
      }

      // make a surrogate pair out of the value
      if ((utf32Char > ZS_INTERNAL_UTF32_MAKE_SURROGATE_BETEWEEN_MIN) &&
          (utf32Char < ZS_INTERNAL_UTF32_MAKE_SURROGATE_BETEWEEN_MAX)) {
        utf32Char -= ZS_INTERNAL_UTF16_BASE_INDEX;
        UTF16 surrogate1 = (ZS_INTERNAL_UTF16_HIGH_BITS_FLAG | (utf32Char >> 10) & ZS_INTERNAL_UTF16_MASK_10_BITS_VALUE);
        UTF16 surrogate2 = (ZS_INTERNAL_UTF16_LOW_BITS_FLAG | (utf32Char & ZS_INTERNAL_UTF16_MASK_10_BITS_VALUE));
        memcpy(outResult, &surrogate1, sizeof(surrogate1));
        ++outResult;
        memcpy(outResult, &surrogate2, sizeof(surrogate2));
        ++outResult;
        return;
      }

      // this is an illegal pairing, shove the result in as best possible
      UTF16 surrogate1 = (0xFFFF & (utf32Char >> 16));
      UTF16 surrogate2 = (utf32Char & 0xFFFF);

      if (0 != surrogate1) {
        memcpy(outResult, &surrogate1, sizeof(surrogate1));
        ++outResult;
      }
      memcpy(outResult, &surrogate2, sizeof(surrogate2));
      ++outResult;
    }

    // U+000000-U+00007F    0xxxxxxx
    // U+000080-U+0007FF    110yyyxx 10xxxxxx                   110-00010  10-000000 -> 110-11111 10-111111
    // U+000800-U+00FFFF    1110yyyy 10yyyyxx 10xxxxxx
    // U+010000-U+10FFFF    11110zzz 10zzyyyy 10yyyyxx 10xxxxxx

    static size_t gUTF8LengthLookupTable[256] =
    {
      // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 00 - 0F
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 10 - 1F
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 20 - 2F
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 30 - 3F
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 40 - 4F
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 50 - 5F
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 60 - 6F
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 70 - 7F  // direct conversion to this point
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 80 - 8F
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 90 - 9F
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A0 - AF
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B0 - BF
         1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // C0 - CF // C0->C1 = illegal, C2->DF = 2 character encoding range
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // D0 - DF
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // E0 - EF // E0->EF = 3 character encoding range
         4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1  // F0 - FF // F0->F7 = 4 character encoding range,  F8-FB = 5 characters,  FC->FD = 6 characters, FE-FF = invalid
    };

    static BYTE gUTF8FirstByteValueMask[7] =
    {
      0,    // 0
      0x7F, // 1
      0x1F, // 2
      0xF,  // 3
      0x7,  // 4
      0x3,  // 5
      0x1   // 6
    };

    static BYTE gUTF8FirstByteIndicator[7] =
    {
      0,    // 0
      0,    // 1
      0xC0, // 2
      0xE0, // 3
      0xF0, // 4
      0xF8, // 5
      0xFC  // 6
    };

#define ZS_INTERNAL_UTF8_OTHER_DIGITS_MASK         0xC0
#define ZS_INTERNAL_UTF8_OTHER_DIGITS_INDICATOR    0x80

#define ZS_INTERNAL_UTF8_OTHER_DIGITS_VALUE_MASK   0x3F


    static UTF32 utf8ToUTF32(CSTR &ioUTF8)
    {
      CSTR start = ioUTF8;
      size_t byteLength = gUTF8LengthLookupTable[((BYTE)*ioUTF8)];

      if (byteLength > 1) {
        UTF32 result = (((BYTE)*ioUTF8) & gUTF8FirstByteValueMask[byteLength]);
        ++ioUTF8;

        while (byteLength > 1)
        {
          if (ZS_INTERNAL_UTF8_OTHER_DIGITS_INDICATOR != (*ioUTF8 & ZS_INTERNAL_UTF8_OTHER_DIGITS_MASK))
          {
            // this is illegal utf-8 sequence, treat as ASCII sequence instead by rewinding to start and just converting to unicode directly from ASCII character
            result = (UTF32)((BYTE)(*start));
            ioUTF8 = (start + 1);
            return result;
          }

          result <<= 6;
          result = result | ((((BYTE)(*ioUTF8)) & ZS_INTERNAL_UTF8_OTHER_DIGITS_VALUE_MASK));
          ++ioUTF8;
          --byteLength;
        }
        return result;
      }

      UTF32 result = (UTF32)((BYTE)(*ioUTF8));
      ++ioUTF8;
      return result;
    }

#define ZS_INTERNAL_UTF8_OTHER_DIGITS_MASK         0xC0
#define ZS_INTERNAL_UTF8_OTHER_DIGITS_INDICATOR    0x80

    static void utf32ToUTF8(UTF32 utf32Char, STR &outUTF8)
    {
      size_t length = 1;             // the assumed length
      if      (utf32Char > 0x03FFFFFF)
        length = 6;
      else if (utf32Char > 0x001FFFFF)
        length = 5;
      else if (utf32Char > 0x0000FFFF)
        length = 4;
      else if (utf32Char > 0x000007FF)
        length = 3;
      else if (utf32Char > 0x0000007F)
        length = 2;

      STR fillSpot = (outUTF8 + length - 1);

      size_t loop = length;
      while (loop > 1)
      {
        *fillSpot = (CHAR)((BYTE)((ZS_INTERNAL_UTF8_OTHER_DIGITS_VALUE_MASK & utf32Char) | ZS_INTERNAL_UTF8_OTHER_DIGITS_INDICATOR));
        utf32Char >>= 6;
        --fillSpot;
        --loop;
      }

      *fillSpot = (CHAR)((BYTE)((utf32Char & gUTF8FirstByteValueMask[length]) | gUTF8FirstByteIndicator[length]));

      // skip over the filled
      outUTF8 += length;
    }

    static std::unique_ptr<WCHAR[]> utf8ToUnicodeConvert(CSTR szInUTF8)
    {
      if (NULL == szInUTF8)
        return std::unique_ptr<WCHAR[]>();

      size_t actualLength = strlen(szInUTF8);

      // create a temporary buffer to convert into
      // heap allocated blocks do not require memset
      std::unique_ptr<WCHAR[]> result(new WCHAR[(actualLength*2)+1]);

      CSTR szSource = szInUTF8;
      WCHAR *pDest = result.get();

      if (sizeof(WORD) == sizeof(WCHAR))
      {
        while (0 != *szSource)
        {
          // requires UTF-32 to UTF-16 conversion
          utf32ToUTF16(utf8ToUTF32(szSource), (UTF16 * &)pDest);
        }
      } else {
        while (0 != *szSource)
        {
          *pDest = (WCHAR)utf8ToUTF32(szSource);
          ++pDest;
        }
      }
      *pDest = 0;

      return result;
    }

    static std::unique_ptr<CHAR[]> unicodeToUTF8Convert(CWSTR szInUnicodeString)
    {
      if (NULL == szInUnicodeString)
        return std::unique_ptr<CHAR[]>();

      size_t actualLength = wcslen(szInUnicodeString);

      // allocate enough room for the maximum encoding length (yah a bit
      // wasteful, but at least it doesn't have to double process
      // the input string)
      // ULONG = 4 bytes, should be plenty
      std::unique_ptr<CHAR[]> result(new char[(actualLength*ZS_INTERNAL_UTF8_MAX_CHARACTER_ENCODED_BYTE_SIZE)+1]);

      // convert to a unicode string for easier processing
      CWSTR szSource = szInUnicodeString;
      CHAR *pDest = result.get();

      if (sizeof(WORD) == sizeof(WCHAR))
      {
        UTF16 value = 1;
        BYTE checkBOM[sizeof(value)];
        memcpy(&(checkBOM[0]), &value, sizeof(value));
      
        bool be = (0 == checkBOM[0]);

        memcpy(&(checkBOM[0]), &(szSource[0]), sizeof(checkBOM));

        if (0xFE == checkBOM[0]) {
          if (0xFF == checkBOM[1]) {
            be = true;
            ++szSource;
          }
        } else if (0xFF == checkBOM[0]) {
          if (0xFE == checkBOM[1]) {
            be = false;
            ++szSource;
          }
        }

        while (0 != *szSource)
        {
          // this requires conversion from UTF 16 to UTF 32 then to UTF 8
          utf32ToUTF8(utf16ToUTF32((CUTF16STR &)szSource, be), pDest);
        }
      } else {
        while (0 != *szSource)
        {
          utf32ToUTF8((UTF32)(*szSource), pDest);
          ++szSource;
        }
      }

      *pDest = 0;

      return result;
    }
    
    std::string convertToString(CWSTR value)
    {
      if (!value) return std::string();
      std::unique_ptr<CHAR[]> result(unicodeToUTF8Convert(value));
      return std::string(result.get());
    }

    std::wstring convertToWString(CSTR value)
    {
      if (!value) return std::wstring();
      std::unique_ptr<WCHAR[]> result(utf8ToUnicodeConvert(value));
      return std::wstring(result.get());
    }

    std::string convertToString(const std::wstring &value)
    {
      return convertToString(value.c_str());
    }

    std::wstring convertToWString(const std::string &value)
    {
      return convertToWString(value.c_str());
    }

  } // namespace internal

}
