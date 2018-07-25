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

#include "testing.h"
#include "main.h"

using zsLib::BYTE;
using zsLib::DWORD;
using zsLib::CSTR;
using zsLib::CWSTR;
using zsLib::WCHAR;
using zsLib::WORD;

#define TEST_UTF8_1_DIGIT "A"
#define TEST_UTF8_2_DIGIT "\xC2\xA9"
#define TEST_UTF8_3_DIGIT "\xE2\x89\xA0"

#define TEST_UNICODE_1_DIGIT 0x0041
#define TEST_UNICODE_2_DIGIT 0x00A9
#define TEST_UNICODE_3_DIGIT 0x2260

static void testReverseEndian(wchar_t &value)
{
  if (sizeof(WORD) != sizeof(wchar_t)) return;

  BYTE *buffer = (BYTE *)&value;
  BYTE tmp = buffer[0];
  buffer[0] = buffer[1];
  buffer[1] = tmp;
}

void testString1()

{
  if (!ZSLIB_TEST_STRING) return;

  class NullTestString
  {
  public:
    NullTestString(const char *value = NULL) :
      mValue(value)
    {
    }

    zsLib::String getValue() const {return mValue;}
  private:
    zsLib::String mValue;
  };

  std::string stdstr_empty;

  std::string stdstr1("this is a std::test1");
  std::wstring wstdstr1(L"this is a wide std::test1");

  zsLib::String str1;

  const char *forceNullCSTR = NULL;
  zsLib::String forceNull(forceNullCSTR);
  std::string forceNullCopied(forceNull);
  NullTestString nullStrObj;
  zsLib::String str2("this is a test2");
  zsLib::String wstr2(L"this is a wide test2");
  zsLib::String str4(stdstr1);
  zsLib::String str5(wstdstr1);

  const char *buffer = "0123456789012345678901234567890!!!!!!!!!!";
  const char *buffer7 = TEST_UTF8_1_DIGIT TEST_UTF8_2_DIGIT TEST_UTF8_3_DIGIT;

  // test all the edge values for UTF-8 encoding
  // 0x01 0x7F 0x80 0x7FF 0x800  0xFFFF 0x10000  0x1FFFFF 0x200000   0x3FFFFFF  0x4000000     0x7FFFFFFF
  // 01   7F   C280 DFBF  E0A080 EFBFBF F0908080 F7BFBFBF F888808080 FBBFBFBFBF FC8480808080  FDBFBFBFBFBF
  const char *buffer8 = "\x01\x7F\xC2\x80\xDF\xBF\xE0\xA0\x80\xEF\xBF\xBF\xF0\x90\x80\x80\xF7\xBF\xBF\xBF\xF8\x88\x80\x80\x80\xFB\xBF\xBF\xBF\xBF\xFC\x84\x80\x80\x80\x80\xFD\xBF\xBF\xBF\xBF\xBF";
  wchar_t wbuffer8[13] = {
    0x01,
    0x7F,
    0x80,
    0x7FF,
    0x800,
    0xFFFF,
    (wchar_t)(zsLib::DWORD)0x10000,
    (wchar_t)(zsLib::DWORD)0x1FFFFF,
    (wchar_t)(zsLib::DWORD)0x200000,
    (wchar_t)(zsLib::DWORD)0x3FFFFFF,
    (wchar_t)(zsLib::DWORD)0x4000000,
    (wchar_t)(zsLib::DWORD)0x7FFFFFFF,
    0x0
  };

  // test all the edge values for UTF-8 encoding
  // 0x01 0x7F 0x80 0x7FF 0x800  0xFFFF 0x10000  0x10FFFF
  // 01   7F   C280 DFBF  E0A080 EFBFBF F0908080 F48FBFBF
  const char *buffer8_utf16 = "\x01\x7F\xC2\x80\xDF\xBF\xE0\xA0\x80\xEF\xBF\xBF\xF0\x90\x80\x80\xF4\x8F\xBF\xBF";
  wchar_t wbuffer8_utf16[11] = {
    0x01,
    0x7F,
    0x80,
    0x7FF,
    0x800,
    0xFFFF,

    // represents 0x10000
    0x0000 | 0xD800, // high
    0x0000 | 0xDC00, // low

    // represents 0x10FFFF; 0x10FFFF - 0x10000 = 0xFFFFF or 1111 11111111 11111111 (20 bits)
    0x3FF | 0xD800, // low
    0x3FF | 0xDC00, // high

    0x0
  };

#if 0
  {
    zsLib::WORD value {1};
    zsLib::BYTE checkE[sizeof(value)];
    memcpy(&(checkE[0]), &value, sizeof(value));
    if (1 == checkE[0]) {
      testReverseEndian(wbuffer8_utf16[6]);
      testReverseEndian(wbuffer8_utf16[7]);
      testReverseEndian(wbuffer8_utf16[8]);
      testReverseEndian(wbuffer8_utf16[9]);
    }
  }
#endif //0

  zsLib::String str6("Τη γλώσσα μου έδωσαν ελληνική");
  zsLib::String str7(buffer7);
  zsLib::String str8(sizeof(wchar_t) == sizeof(WORD) ? buffer8_utf16 : buffer8);

  std::wstring wstr7(str7.wstring());
  std::wstring wstr8(str8.wstring());

  zsLib::String str9(wstr8);

  zsLib::String str10("hello");
  str10 = stdstr1;
  zsLib::String str11("goodbye");
  str11 = wstdstr1;

  zsLib::String str12("whatever");
  str12 = str6;

  zsLib::String str13("foobar 13");
  str13 = "this is the proper value";

  zsLib::String str14("foobar 14");
  str14 = L"this is the proper wide value";

  zsLib::String strLower("ThIs Is A tEsT");
  strLower.toLower();

  zsLib::String strUpper("ThIs Is A tEsT");
  strUpper.toUpper();

  zsLib::String strTrim(" \t\r\n\t\v  This string is:\t fantastic\t \r\v\n \t");

  zsLib::String strTrim1(strTrim);
  strTrim1.trim();

  zsLib::String strTrimLeft(strTrim);
  strTrimLeft.trimLeft();

  zsLib::String strTrimRight(strTrim);
  strTrimRight.trimRight();

  zsLib::String replaceAll("chili, hotdogs, popcorn, chicken, chili, sour cream, vinegar, cream");

  zsLib::String replace1(replaceAll);
  replace1.replaceAll("chili", "chilli");

  zsLib::String replace2(replaceAll);
  replace2.replaceAll(", ", "\n");

  zsLib::String replace3(replaceAll);
  replace3.replaceAll(", ", "\n", 1);

  zsLib::String replace4(replaceAll);
  replace4.replaceAll(", ", "\n", 0);

  zsLib::String replace5(replaceAll);
  replace5.replaceAll(", ", ", x1, ");

  zsLib::String replace6(replaceAll);
  replace6.replaceAll("cream", "");
  replace6.replaceAll(" ,", ",");

  TESTING_CHECK(forceNull.isEmpty());
  TESTING_EQUAL(zsLib::String(), forceNull);
  TESTING_EQUAL(forceNullCopied, forceNull);
  TESTING_EQUAL(zsLib::String(), nullStrObj.getValue())
  TESTING_CHECK(stdstr_empty.empty());
  TESTING_CHECK(str1.empty());
  TESTING_CHECK(str1.isEmpty());
  TESTING_EQUAL(str2, "this is a test2");
  TESTING_EQUAL(wstr2, "this is a wide test2");
  TESTING_EQUAL(str4, "this is a std::test1");
  TESTING_EQUAL(str5, "this is a wide std::test1");
  TESTING_EQUAL(zsLib::String::copyFrom(buffer, 15), "012345678901234");
  bool equal6 = (0 == strcmp(str6, "Τη γλώσσα μου έδωσαν ελληνική"));  // I have no idea what this says so if it is bad or insulting, my apologies!
  TESTING_CHECK(equal6);
  TESTING_EQUAL(str7, buffer7);
  if (sizeof(wchar_t) == sizeof(zsLib::WORD)) {
    TESTING_EQUAL(wstr7[0], TEST_UNICODE_1_DIGIT);
    TESTING_EQUAL(wstr7[1], TEST_UNICODE_2_DIGIT);
    TESTING_EQUAL(wstr7[2], TEST_UNICODE_3_DIGIT);

    TESTING_EQUAL(wstr8[0], wbuffer8_utf16[0]);
    TESTING_EQUAL(wstr8[1], wbuffer8_utf16[1]);
    TESTING_EQUAL(wstr8[2], wbuffer8_utf16[2]);
    TESTING_EQUAL(wstr8[3], wbuffer8_utf16[3]);
    TESTING_EQUAL(wstr8[4], wbuffer8_utf16[4]);
    TESTING_EQUAL(wstr8[5], wbuffer8_utf16[5]);
    TESTING_EQUAL(wstr8[6], wbuffer8_utf16[6]);
    TESTING_EQUAL(wstr8[7], wbuffer8_utf16[7]);
    TESTING_EQUAL(wstr8[8], wbuffer8_utf16[8]);
    TESTING_EQUAL(wstr8[9], wbuffer8_utf16[9]);
    TESTING_EQUAL(wstr8[10], wbuffer8_utf16[10]);
  }
  if (sizeof(wchar_t) == sizeof(zsLib::DWORD)) {
    for (size_t loop = 0; loop < 13; ++loop) {
      bool equal = (wstr8[loop] == wbuffer8[loop]);
      TESTING_CHECK(equal)
    }
  }
  bool equal9 = (str9 == (sizeof(WORD) == sizeof(wchar_t) ? buffer8_utf16 : buffer8));
  TESTING_CHECK(equal9);
  bool equal10 = (0 == strcmp(((CSTR)str9), (sizeof(WORD) == sizeof(wchar_t) ? buffer8_utf16 : buffer8)));
  TESTING_CHECK(equal10);
  bool equal11 = (str9 == str8);
  TESTING_CHECK(equal11);
  TESTING_EQUAL(str10, stdstr1);
  TESTING_EQUAL(str11, zsLib::String(wstdstr1));
  bool equal12 = (str12 == str6);
  TESTING_CHECK(equal12);
  TESTING_EQUAL(str13, "this is the proper value");
  TESTING_EQUAL(str14, "this is the proper wide value");
  TESTING_EQUAL(strLower, "this is a test");
  TESTING_EQUAL(strUpper, "THIS IS A TEST");
  TESTING_EQUAL(0, strLower.compareNoCase(strUpper));
  TESTING_EQUAL(0, strLower.compareNoCase(strUpper.c_str()));
  TESTING_EQUAL(strTrim1, "This string is:\t fantastic");
  TESTING_CHECK(strTrimLeft == "This string is:\t fantastic\t \r\v\n \t");
  TESTING_CHECK(strTrimRight == " \t\r\n\t\v  This string is:\t fantastic");
  TESTING_EQUAL(replace1, "chilli, hotdogs, popcorn, chicken, chilli, sour cream, vinegar, cream");
  TESTING_CHECK(replace2 == "chili\nhotdogs\npopcorn\nchicken\nchili\nsour cream\nvinegar\ncream");
  TESTING_CHECK(replace3 == "chili\nhotdogs, popcorn, chicken, chili, sour cream, vinegar, cream");
  TESTING_EQUAL(replace4, "chili, hotdogs, popcorn, chicken, chili, sour cream, vinegar, cream");
  TESTING_EQUAL(replace5, "chili, x1, hotdogs, x1, popcorn, x1, chicken, x1, chili, x1, sour cream, x1, vinegar, x1, cream");
  TESTING_EQUAL(replace6, "chili, hotdogs, popcorn, chicken, chili, sour, vinegar, ");
}


#ifdef ZS_TARGET_WCHAR_IS_UTF16

size_t gTEST_ARRAY_1_LINE_START = __LINE__;
static wchar_t gWArray1[] =
{
  0x01,
  0x7F,
  0x80,
  0xBF,
  0xC0,
  0xC1,
  0xC2,
  0xDF,
  0xD0,
  0xDF,
  0xE0,
  0xEF,
  0xF0,
  0xF4,
  0xF5,
  0xFF,
  0x100,
  0x7FF,
  0x800,
  0xFFFF,
  0xD800, 0xDC00,
  0xDBFF, 0xDC00,
  0xD800, 0xDFFF,
  0xDBFF, 0xDFFF,
  0xE000,
  0xEFFF,
  0xF000,
  0xFFFF,
  'A',
  'B',
  'C',
  'D',
  'E',
  'F',
  0x0
};
size_t gTEST_ARRAY_1_LINE_END = __LINE__;

#else //ZS_TARGET_WCHAR_IS_UTF16

#ifdef ZS_TARGET_WCHAR_IS_UTF32
size_t gTEST_ARRAY_1_LINE_START = __LINE__;
static wchar_t gWArray1[] =
{
  0x01,
  0x7F,
  0x80,
  0xBF,
  0xC0,
  0xC1,
  0xC2,
  0xDF,
  0xD0,
  0xDF,
  0xE0,
  0xEF,
  0xF0,
  0xF4,
  0xF5,
  0xFF,
  0x100,
  0x7FF,
  0x800,
  0xFFFF,
  0x10000,
  0x10FC00,
  0x103FF,
  0x10FFFF,
  0xE000,
  0xEFFF,
  0xF000,
  0xFFFF,
  'A',
  'B',
  'C',
  'D',
  'E',
  'F',
  0x0
};
size_t gTEST_ARRAY_1_LINE_END = __LINE__;
#endif //ZS_TARGET_WCHAR_IS_UTF32

#endif //ZS_TARGET_WCHAR_IS_UTF16

size_t gTEST_ARRAY_1_LINE_LENGTH = (gTEST_ARRAY_1_LINE_END - gTEST_ARRAY_1_LINE_START - 5);

static BYTE gByteArray1[] =
{
  0x01,
  0x7F,
  0xC2, 0x80, // 0x80,
  0xC2, 0xBF, // 0xBF,
  0xC3, 0x80, // 0xC0,
  0xC3, 0x81, // 0xC1,
  0xC3, 0x82, // 0xC2,
  0xC3, 0x9F, // 0xDF,
  0xC3, 0x90, // 0xD0,
  0xC3, 0x9F, // 0xDF,
  0xC3, 0xA0, // 0xE0,
  0xC3, 0xAF, // 0xEF,
  0xC3, 0xB0, // 0xF0,
  0xC3, 0xB4, // 0xF4,
  0xC3, 0xB5, // 0xF5,
  0xC3, 0xBF, // 0xFF,
  0xC4, 0x80, // 0x100,
  0xDF, 0xBF, // 0x7FF,
  0xE0, 0xA0, 0x80, // 0x800,
  0xEF, 0xBF, 0xBF, // 0xFFFF,
  0xF0, 0x90, 0x80, 0x80, // 0xD800, 0xDC00, //F0908080 - 0x10000
  0xF4, 0x8F, 0xB0, 0x80, // 0xDBFF, 0xDC00, //F48FB080 - 0x10FC00
  0xF0, 0x90, 0x8F, 0xBF, // 0xD800, 0xDFFF, //F0908FBF - 0x103FF
  0xF4, 0x8F, 0xBF, 0xBF, // 0xDBFF, 0xDFFF, //F48FBFBF - 0x10FFFF
  0xEE, 0x80, 0x80, // 0xE000,
  0xEE, 0xBF, 0xBF, // 0xEFFF,
  0xEF, 0x80, 0x80, // 0xF000,
  0xEF, 0xBF, 0xBF, // 0xFFFF,
  'A',
  'B',
  'C',
  'D',
  'E',
  'F',
  0x0
};

static wchar_t gWGetAtArray1[] =
{
  0x01,
  0x7F,
  0x80,
  0xBF,
  0xC0,
  0xC1,
  0xC2,
  0xDF,
  0xD0,
  0xDF,
  0xE0,
  0xEF,
  0xF0,
  0xF4,
  0xF5,
  0xFF,
  0x100,
  0x7FF,
  0x800,
  0xFFFF,
  (wchar_t)(DWORD)0x10000, //  0x10000 // 0xD800, 0xDC00,
  (wchar_t)(DWORD)0x10FC00, // 0x10FC00 //0xDBFF, 0xDC00,
  (wchar_t)(DWORD)0x103FF, //  0x103FF // 0xD800, 0xDFFF,
  (wchar_t)(DWORD)0x10FFFF, // 0x10FFFF // 0xDBFF, 0xDFFF,
  0xE000,
  0xEFFF,
  0xF000,
  0xFFFF,
  'A',
  'B',
  'C',
  'D',
  'E',
  'F',
  0x0
};

struct TestMidStruct
{
  int mPos;
  int mCount;
  BYTE *mContents;
  size_t mLength;
};

static BYTE gMid1ByteArray1[] =
{
  0x01,
  0x0
};

static BYTE gMid2ByteArray1[] =
{
  0x01,
  0x7F,
  0x0
};

static BYTE gMid3ByteArray1[] =
{
  0x7F,
  0xC2, 0x80, // 0x80,
  0x0
};

static BYTE gMid4ByteArray1[] =
{
  0xE0, 0xA0, 0x80, // 0x800,
  0xEF, 0xBF, 0xBF, // 0xFFFF,
  0xF0, 0x90, 0x80, 0x80, // 0xD800, 0xDC00,
  0xF4, 0x8F, 0xB0, 0x80, // 0xDBFF, 0xDC00,
  0xF0, 0x90, 0x8F, 0xBF, // 0xD800, 0xDFFF,
  0xF4, 0x8F, 0xBF, 0xBF, // 0xDBFF, 0xDFFF,
  0xEE, 0x80, 0x80, // 0xE000,
  0xEE, 0xBF, 0xBF, // 0xEFFF,
  0xEF, 0x80, 0x80, // 0xF000,
  0xEF, 0xBF, 0xBF, // 0xFFFF,
  'A',
  'B',
  'C',
  'D',
  'E',
  'F',
  0x0
};

static BYTE gMid5ByteArray1[] =
{
  0xE0, 0xA0, 0x80, // 0x800,
  0xEF, 0xBF, 0xBF, // 0xFFFF,
  0xF0, 0x90, 0x80, 0x80, // 0xD800, 0xDC00,
  0xF4, 0x8F, 0xB0, 0x80, // 0xDBFF, 0xDC00,
  0xF0, 0x90, 0x8F, 0xBF, // 0xD800, 0xDFFF,
  0xF4, 0x8F, 0xBF, 0xBF, // 0xDBFF, 0xDFFF,
  0xEE, 0x80, 0x80, // 0xE000,
  0xEE, 0xBF, 0xBF, // 0xEFFF,
  0xEF, 0x80, 0x80, // 0xF000,
  0xEF, 0xBF, 0xBF, // 0xFFFF,
  'A',
  'B',
  'C',
  'D',
  'E',
  0x0
};

static TestMidStruct gMidTests[] =
{
  {0, 1, &(gMid1ByteArray1[0]), sizeof(gMid1ByteArray1)},
  {0, 2, &(gMid2ByteArray1[0]), sizeof(gMid2ByteArray1)},
  {1, 2, &(gMid3ByteArray1[0]), sizeof(gMid3ByteArray1)},
  {18, 16, &(gMid4ByteArray1[0]), sizeof(gMid4ByteArray1)},
  {18, 17, &(gMid4ByteArray1[0]), sizeof(gMid4ByteArray1)},
  {18, -1, &(gMid4ByteArray1[0]), sizeof(gMid4ByteArray1)},
  {18, 15, &(gMid5ByteArray1[0]), sizeof(gMid5ByteArray1)},
  {0, 0, NULL, 0}
};

static size_t gTEST_ARRAY_2_LINE_START = __LINE__;
static wchar_t gWArray2[] =
{
  0xDC00,           // not legal surrogate
  0xD800, 0xDC00,   // legal surrogate
  0xDBFF, 0xDFFF,   // legal surrogate
  0xD800, 0xDFFF,   // legal surrogate
          0xDBFF,   // not legal surrogate
  0x0
};
static size_t gTEST_ARRAY_2_LINE_END = __LINE__;
static size_t gTEST_ARRAY_2_LINE_LENGTH = (gTEST_ARRAY_2_LINE_END - gTEST_ARRAY_2_LINE_START - 5);

static BYTE gByteArray2[] =
{
  0xED, 0xB0, 0x80, // 0xDC00 = 1101 110000 000000 = 1110xxxx 10xxxxxx 10xxxxxx = 111011011011000010000000 = EDB080
  0xF0, 0x90, 0x80, 0x80, // 0xD800, 0xDC00, 0x10000 = 000 010000 000000 000000 = 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx = 11110000100100001000000010000000 = F0908080
  0xF4, 0x8F, 0xBF, 0xBF, // 0xDBFF, 0xDFFF, 0x10FFFF = 100 001111 111111 111111 = 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx = 11110100100011111011111110111111 = F48FBFBF
  0xF0, 0x90, 0x8F, 0xBF, // 0xD800, 0xDFFF, 0x103FF = 000 010000 001111 111111 = 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx = 11110000100100001000111110111111 = F0908FBF
  0xED, 0xAF, 0xBF, // 0xDBFF = 1101 101111 111111 = 1110xxxx 10xxxxxx 10xxxxxx = 111011011010111110111111 = EDAFBF
  0x0
};

static size_t gTEST_ARRAY_3_LINE_START = __LINE__;
static wchar_t gWArray3[] =
{
  0xD800, 0xDC00,
  0xDBFF, 0xDC00,
  0xD800, 0xDFFF,
  0xDBFF, 0xDFFF,
  0x0
};
static size_t gTEST_ARRAY_3_LINE_END = __LINE__;
static size_t gTEST_ARRAY_3_LINE_LENGTH = (gTEST_ARRAY_3_LINE_END - gTEST_ARRAY_3_LINE_START - 5);

static BYTE gByteArray3[] =
{
  0xF0, 0x90, 0x80, 0x80, // 0xD800, 0xDC00, 0x10000 = 000 010000 000000 000000 = 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx = 11110000100100001000000010000000 = F0908080
  0xF4, 0x8F, 0xB0, 0x80, // 0xDBFF, 0xDC00, 0x10FC00 =  100 001111 110000 000000 = 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx = 11110100100011111011000010000000 = F48FB080
  0xF0, 0x90, 0x8F, 0xBF, // 0xD800, 0xDFFF, 0x103FF = 000 010000 001111 111111 = 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx = 11110000100100001000111110111111 = F0908FBF
  0xF4, 0x8F, 0xBF, 0xBF, // 0xDBFF, 0xDFFF, 0x10FFFF = 100 001111 111111 111111 = 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx = 11110100100011111011111110111111 = F48FBFBF
  0x0
};

// these are all illegal UTF16 sequences
static wchar_t gWArray4[] =
{
  0xD800, 0x0001,
  0xDBFF, 0xFFFF,
  0xD800, 0xD800,
  0xD8FF, 0xD800,
  0xDBFF, 0xD8FF,
  0xDBFF, 0xDBFF,
  0x0
};

static size_t gTEST_ARRAY_4_LINE_START = __LINE__;
static BYTE gByteArray4[] =
{
  0xED, 0xA0,
  0x80, 0x01, // 0xD800, 0x0001,
  0xED, 0xAF, 0xBF,
  0xEF, 0xBF, 0xBF, // 0xDBFF, 0xFFFF,
  0xED, 0xA0, 0x80,
  0xED, 0xA0, 0x80, // 0xD800, 0xD800,
  0xED, 0xA3, 0xBF,
  0xED, 0xA0, 0x80, // 0xD8FF, 0xD800,
  0xED, 0xAF, 0xBF,
  0xED, 0xA3, 0xBF, // 0xDBFF, 0xD8FF,
  0xED, 0xAF, 0xBF,
  0xED, 0xAF, 0xBF, // 0xDBFF, 0xDBFF,
  0x0
};
static size_t gTEST_ARRAY_4_LINE_END = __LINE__;
static size_t gTEST_ARRAY_4_LINE_LENGTH = (gTEST_ARRAY_4_LINE_END - gTEST_ARRAY_4_LINE_START - 5);

static BYTE gByteArray5[] =
{
  0xC0, // illegal UTF8 start sequences
  0xC1,
  0xF5,
  0xF6,
  0xF7,
  0xF8,
  0xF9,
  0xFA,
  0xFB,
  0xFC,
  0xFD,
  0xFE,
  0xFF,
  0xC2, 0x01,
  0xC2, 0xC0,
  0xC2, 0xFF,
  0x00
};

static size_t gTEST_ARRAY_5_LINE_START = __LINE__;
static wchar_t gWArray5[] =
{
  0xC0,
  0xC1,
  0xF5,
  0xF6,
  0xF7,
  0xF8,
  0xF9,
  0xFA,
  0xFB,
  0xFC,
  0xFD,
  0xFE,
  0xFF,
  0xC2,
  0x01,
  0xC2,
  0xC0,
  0xC2,
  0xFF,
  0x00
};
static size_t gTEST_ARRAY_5_LINE_END = __LINE__;
static size_t gTEST_ARRAY_5_LINE_LENGTH = (gTEST_ARRAY_5_LINE_END - gTEST_ARRAY_5_LINE_START - 5);

static BYTE gUTFByteArray5[] =
{
  0xC3, 0x80, // 0xC0,
  0xC3, 0x81, // 0xC1,
  0xC3, 0xB5, // 0xF5,
  0xC3, 0xB6, // 0xF6,
  0xC3, 0xB7, // 0xF7,
  0xC3, 0xB8, // 0xF8,
  0xC3, 0xB9, // 0xF9,
  0xC3, 0xBA, // 0xFA,
  0xC3, 0xBB, // 0xFB,
  0xC3, 0xBC, // 0xFC,
  0xC3, 0xBD, // 0xFD,
  0xC3, 0xBE, // 0xFE,
  0xC3, 0xBF, // 0xFF,
  0xC3, 0x82, // 0xC2,
  0x01,
  0xC3, 0x82, // 0xC2,
  0xC3, 0x80, // 0xC0,
  0xC3, 0x82, // 0xC2,
  0xC3, 0xBF, // 0xFF,
  0x00
};



static size_t gTEST_ARRAY_6_LINE_START = __LINE__;
static wchar_t gWArray6[] =
{
  0xD83D, 0xDE06,  // 0x1F606 = 0000111101 1000000110 = 110110xxxxxxxxxx 110111xxxxxxxxxxx = 1101100000111101 1101111000000110 = D83D DE06
  0x0
};
static size_t gTEST_ARRAY_6_LINE_END = __LINE__;
static size_t gTEST_ARRAY_6_LINE_LENGTH = (gTEST_ARRAY_6_LINE_END - gTEST_ARRAY_6_LINE_START - 5);

static BYTE gByteArray6[] =
{
  0xF0, 0x9F, 0x98, 0x86, // 0xD83D, 0xDE06, 0x1F606 = 000 011111 011000 000110 = 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx = 11110000100111111001100010000110 = F09F9886
  0x0
};

void testString2()
{
  {
    // test convert to unicode then back
    CWSTR wStr = &(gWArray1[0]);
    zsLib::String string(wStr);

    CSTR utf8Str = string;
    TESTING_CHECK(0 == memcmp(utf8Str, &(gByteArray1[0]), sizeof(gByteArray1)))

    zsLib::String string2(utf8Str);
    std::wstring wstring2(string2.wstring());
    CWSTR utf16Str = wstring2.c_str();

    TESTING_CHECK(0 == memcmp((void *)utf16Str, wStr, sizeof(gWArray1)))

    TESTING_EQUAL(string.getLength(), strlen(utf8Str))
    TESTING_EQUAL(string.lengthUnicodeSafe(), gTEST_ARRAY_1_LINE_LENGTH)

    size_t length = (sizeof(gWGetAtArray1)/sizeof(WCHAR));
    for (size_t loop = 0; loop < length-1; ++loop)
    {
      TESTING_EQUAL(gWGetAtArray1[loop], string.atUnicodeSafe(loop))
    }

    for (size_t loop = 0; true; ++loop)
    {
      int pos = gMidTests[loop].mPos;
      int count = gMidTests[loop].mCount;
      if (NULL == gMidTests[loop].mContents)
        break;

      zsLib::String result = string.substrUnicodeSafe(pos, count);
      TESTING_CHECK(0 == memcmp((CSTR)result, gMidTests[loop].mContents, gMidTests[loop].mLength))
    }
  }
  if (sizeof(WORD) == sizeof(WCHAR))  // this test if for UTF-16 systems only, new test requried for UTF-32 systems
  {
    // test convert to unicode with right endian and back
    CWSTR wStr = &(gWArray3[0]);
    zsLib::String string(wStr);

    CSTR utf8Str = string;
    TESTING_CHECK(0 == memcmp(utf8Str, &(gByteArray3[0]), sizeof(gByteArray3)))

    zsLib::String string2(utf8Str);
    std::wstring wstring2(string2.wstring());
    CWSTR utf16Str = wstring2.c_str();

    TESTING_CHECK(0 == memcmp((void *)utf16Str, &(gWArray3[0]), sizeof(gWArray3)))
    TESTING_CHECK(string.lengthUnicodeSafe() == gTEST_ARRAY_3_LINE_LENGTH)
  }
  if (sizeof(WORD) == sizeof(WCHAR))  // this test if for UTF-16 systems only, new test requried for UTF-32 systems
  {
    // test convert to unicode with wrong endian and back
    CWSTR wStr = &(gWArray2[0]);
    zsLib::String string(wStr);

    CSTR utf8Str = string;
    TESTING_CHECK(0 == memcmp(utf8Str, &(gByteArray2[0]), sizeof(gByteArray2)))

    zsLib::String string2(utf8Str);
    std::wstring wstring2(string2.wstring());
    CWSTR utf16Str = wstring2.c_str();

    TESTING_CHECK(0 == memcmp((void *)utf16Str, &(gWArray2[0]), sizeof(gWArray2)))
    TESTING_CHECK(string.lengthUnicodeSafe() == gTEST_ARRAY_2_LINE_LENGTH)
  }
  if (sizeof(WORD) == sizeof(WCHAR))  // this test if for UTF-16 systems only, new test requried for UTF-32 systems
  {
    // test convert to unicode with right endian and back
    CWSTR wStr = &(gWArray6[0]);
    zsLib::String string(wStr);

    CSTR utf8Str = string;
    TESTING_CHECK(0 == memcmp(utf8Str, &(gByteArray6[0]), sizeof(gByteArray6)))

    zsLib::String string2(utf8Str);
    std::wstring wstring2(string2.wstring());
    CWSTR utf16Str = wstring2.c_str();

    TESTING_CHECK(0 == memcmp((void *)utf16Str, &(gWArray6[0]), sizeof(gWArray6)))
    TESTING_CHECK(string.lengthUnicodeSafe() == gTEST_ARRAY_6_LINE_LENGTH)
  }
  {
    // test illegal UTF-16 encoding
    CWSTR wStr = &(gWArray4[0]);
    zsLib::String string(wStr);

    CSTR utf8Str = string;
    TESTING_CHECK(0 == memcmp(utf8Str, &(gByteArray4[0]), sizeof(gByteArray4)))

    zsLib::String string2(utf8Str);
    std::wstring wstring2(string2.wstring());
    CWSTR utf16Str = wstring2.c_str();

    TESTING_CHECK(0 == memcmp((void *)utf16Str, wStr, sizeof(gWArray3)))
    TESTING_CHECK(string.lengthUnicodeSafe() == gTEST_ARRAY_4_LINE_LENGTH)
  }
  {
    // test illegal UTF-8 encoding
    zsLib::String string((CSTR)(&(gByteArray5[0])));

    CSTR utf8Str = string;
    TESTING_CHECK(0 == memcmp(utf8Str, &(gByteArray5[0]), sizeof(gByteArray5)))

    zsLib::String string2(utf8Str);
    std::wstring wstring2(string2.wstring());
    CWSTR utf16Str = wstring2.c_str();

    TESTING_CHECK(0 == memcmp((void *)utf16Str, &(gWArray5[0]), sizeof(gWArray5)))
    TESTING_CHECK(string.lengthUnicodeSafe() == gTEST_ARRAY_5_LINE_LENGTH)

    zsLib::String string3(utf16Str);
    CSTR utf8Str3 = string3;

    TESTING_CHECK(0 == memcmp(utf8Str3, &(gUTFByteArray5[0]), sizeof(gUTFByteArray5)))
  }
}


void testString()
{
    if (!ZSLIB_TEST_STRING) return;

    testString1();
    testString2();
}
