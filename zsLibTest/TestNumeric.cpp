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
#include <zsLib/helpers.h>
#include <zsLib/Stringize.h>


#include "testing.h"
#include "main.h"

using zsLib::BYTE;
using zsLib::LONGLONG;
using zsLib::ULONGLONG;

//using namespace zsLib;

TESTING_AUTO_TEST_SUITE(zsLibNumeric)

  TESTING_AUTO_TEST_CASE(TestNumeric)
  {
    if (!ZSLIB_TEST_NUMERIC) return;

    TESTING_EQUAL(0, (char)zsLib::Numeric<char>("0"));
    TESTING_EQUAL(127, (char)zsLib::Numeric<char>("127"));
    TESTING_EQUAL(127, (char)zsLib::Numeric<char>("+127"));
    TESTING_EQUAL(-128, (char)zsLib::Numeric<char>("-128"));
    TESTING_EQUAL(127, (char)zsLib::Numeric<char>("0x7F"));
    TESTING_EQUAL(127, (char)zsLib::Numeric<char>("0x7f"));
    TESTING_EQUAL(127, (char)zsLib::Numeric<char>(" 7F ", true, 16));
    TESTING_EQUAL(-128, (char)zsLib::Numeric<char>("-0x80"));
    TESTING_EQUAL(0x80, (unsigned char)zsLib::Numeric<unsigned char>("0x80"));

    TESTING_EQUAL(0, (unsigned char)zsLib::Numeric<unsigned char>("0"));
    TESTING_EQUAL(255, (unsigned char)zsLib::Numeric<unsigned char>("255"));

    TESTING_EQUAL(32767, (short)zsLib::Numeric<short>("32767"));
    TESTING_EQUAL(-32768, (short)zsLib::Numeric<short>("-32768"));

    TESTING_EQUAL(0, (unsigned short)zsLib::Numeric<unsigned short>("0"));
    TESTING_EQUAL(65535, (unsigned short)zsLib::Numeric<unsigned short>("65535"));

    TESTING_EQUAL(2147483647, (int)zsLib::Numeric<int>("2147483647"));
#pragma warning(push)
#pragma warning(disable:4146)
    TESTING_EQUAL(-2147483648, (int)zsLib::Numeric<int>("-2147483648"));
#pragma warning(pop)

    TESTING_EQUAL(0, (unsigned int)zsLib::Numeric<unsigned int>("0"));
    TESTING_EQUAL(4294967295, (unsigned int)zsLib::Numeric<unsigned int>("4294967295"));

#ifdef ZSLIB_64BIT
    TESTING_EQUAL(0x7FFFFFFFFFFFFFFFL, (LONGLONG)zsLib::Numeric<LONGLONG>("9223372036854775807"));
    TESTING_EQUAL(0x8000000000000000L, (LONGLONG)zsLib::Numeric<LONGLONG>("-9223372036854775808"));

    TESTING_EQUAL(0x7FFFFFFFFFFFFFFFL, (LONGLONG)zsLib::Numeric<LONGLONG>("0x7FFFFFFFFFFFFFFF"));
    TESTING_EQUAL(0x8000000000000000L, (LONGLONG)zsLib::Numeric<LONGLONG>("-0x8000000000000000"));
#endif //ZSLIB_64BIT

    TESTING_EQUAL(0x0, (ULONGLONG)zsLib::Numeric<ULONGLONG>("0"));
    TESTING_EQUAL(0x0, (ULONGLONG)zsLib::Numeric<ULONGLONG>("0x0"));

#ifdef ZSLIB_64BIT
    TESTING_EQUAL(0xFFFFFFFFFFFFFFFFL, (ULONGLONG)zsLib::Numeric<ULONGLONG>("18446744073709551615"));
    TESTING_EQUAL(0xFFFFFFFFFFFFFFFFL, (ULONGLONG)zsLib::Numeric<ULONGLONG>("0xFFFFFFFFFFFFFFFF"));
#endif //ZSLIB_64BIT

    TESTING_EQUAL(0.0f, (double)zsLib::Numeric<float>("0.0"));
    TESTING_EQUAL(-0.5f, (double)zsLib::Numeric<float>("-0.5"));
    TESTING_EQUAL(0.5f, (double)zsLib::Numeric<float>("0.5"));

    TESTING_EQUAL(0.0f, (double)zsLib::Numeric<double>("0.0"));
    TESTING_EQUAL(-123456.78, (double)zsLib::Numeric<double>("-123456.78"));
    TESTING_EQUAL(123456.78, (double)zsLib::Numeric<double>("123456.78"));

    zsLib::UUID uuid = zsLib::createUUID();
    zsLib::String uuidStr = zsLib::Stringize<zsLib::UUID>(uuid);
    TESTING_CHECK(!uuidStr.isEmpty());

    zsLib::UUID uuid2((zsLib::UUID)zsLib::Numeric<zsLib::UUID>(uuidStr));

    bool result1 = (uuid == uuid2);
    TESTING_CHECK(result1)
    if (!result1) {
      TESTING_EQUAL(zsLib::string(uuid), zsLib::string(uuid2));
    }

    uuidStr = "{" + uuidStr + "}";
    zsLib::UUID uuid3((zsLib::UUID)zsLib::Numeric<zsLib::UUID>(uuidStr));

    bool result2 = (uuid == uuid3);
    TESTING_CHECK(result2)
    if (!result2) {
      TESTING_EQUAL(zsLib::string(uuid), zsLib::string(uuid3));
    }

    { bool thrown = false; try { auto val = (char)zsLib::Numeric<char>("128"); (void)val; } catch(zsLib::Numeric<char>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }
    { bool thrown = false; try { auto val = (char)zsLib::Numeric<char>("-129"); (void)val; } catch(zsLib::Numeric<char>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }
    { bool thrown = false; try { auto val = (char)zsLib::Numeric<char>(" 7F ", false, 16); (void)val; } catch(zsLib::Numeric<char>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }

    { bool thrown = false; try { auto val = (unsigned char)zsLib::Numeric<unsigned char>("-1"); (void)val; } catch(zsLib::Numeric<unsigned char>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }
    { bool thrown = false; try { auto val = (unsigned char)zsLib::Numeric<unsigned char>("256"); (void)val; } catch(zsLib::Numeric<unsigned char>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }

    { bool thrown = false; try { auto val = (short)zsLib::Numeric<short>("32768"); (void)val; } catch(zsLib::Numeric<short>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }
    { bool thrown = false; try { auto val = (short)zsLib::Numeric<short>("-32769"); (void)val; } catch(zsLib::Numeric<short>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }

    { bool thrown = false; try { auto val = (unsigned short)zsLib::Numeric<unsigned short>("65536"); (void)val; } catch(zsLib::Numeric<unsigned short>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }
    { bool thrown = false; try { auto val = (unsigned short)zsLib::Numeric<unsigned short>("-1"); (void)val; } catch(zsLib::Numeric<unsigned short>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }

    { bool thrown = false; try { auto val = (int)zsLib::Numeric<int>("2147483648"); (void)val; } catch(zsLib::Numeric<int>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }
    { bool thrown = false; try { auto val = (int)zsLib::Numeric<int>("-2147483649"); (void)val; } catch(zsLib::Numeric<int>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }

    { bool thrown = false; try { auto val = (unsigned int)zsLib::Numeric<unsigned int>("4294967296"); (void)val; } catch(zsLib::Numeric<unsigned int>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }
    { bool thrown = false; try { auto val = (unsigned int)zsLib::Numeric<unsigned int>("-1"); (void)val; } catch(zsLib::Numeric<unsigned int>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }

    { bool thrown = false; try { auto val = (LONGLONG)zsLib::Numeric<LONGLONG>("9223372036854775808"); (void)val; } catch(zsLib::Numeric<LONGLONG>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }
    { bool thrown = false; try { auto val = (LONGLONG)zsLib::Numeric<LONGLONG>("-9223372036854775809"); (void)val; } catch(zsLib::Numeric<LONGLONG>::ValueOutOfRange &) {thrown = true;} TESTING_CHECK(thrown); }

    {
      zsLib::String durStr = "10";
      zsLib::Hours result = zsLib::Numeric<zsLib::Hours>(durStr);

      zsLib::String full = zsLib::string(result);

      TESTING_EQUAL(full, "10");
    }

    {
      zsLib::String durStr = "123";
      zsLib::Minutes result = zsLib::Numeric<zsLib::Minutes>(durStr);

      zsLib::String full = zsLib::string(result);

      TESTING_EQUAL(full, "123");
    }

    {
      zsLib::String durStr = "99";
      zsLib::Seconds result = zsLib::Numeric<zsLib::Seconds>(durStr);

      zsLib::String full = zsLib::string(result);

      TESTING_EQUAL(full, "99");
    }

    {
      zsLib::String durStr = "45.67";
      zsLib::Milliseconds result = zsLib::Numeric<zsLib::Milliseconds>(durStr);

      zsLib::String full = zsLib::string(result);

      TESTING_EQUAL(full, "45.670");
    }
    
    {
      zsLib::String durStr = "45.6789";
      zsLib::Milliseconds result = zsLib::Numeric<zsLib::Milliseconds>(durStr);

      zsLib::String full = zsLib::string(result);

      TESTING_EQUAL(full, "45.678");
    }

    {
      zsLib::String durStr = "45.6789";
      zsLib::Microseconds result = zsLib::Numeric<zsLib::Microseconds>(durStr);

      zsLib::String full = zsLib::string(result);

      TESTING_EQUAL(full, "45.678900");
    }

    {
      zsLib::String timeStr = "1970-01-01 00:00:05.000";
      zsLib::Time result = zsLib::Numeric<zsLib::Time>(timeStr);

      zsLib::Microseconds sinceEpoch = zsLib::timeSinceEpoch<zsLib::Microseconds>(result);

      zsLib::String full = zsLib::string(result);

      zsLib::Microseconds fromStr = zsLib::Numeric<zsLib::Microseconds>(full);

      TESTING_EQUAL(zsLib::string(result), "5");
      TESTING_EQUAL(full, "5");
      TESTING_EQUAL(zsLib::string(sinceEpoch), "5.000000");
      TESTING_EQUAL(zsLib::string(sinceEpoch), zsLib::string(fromStr));
    }

    {
      zsLib::String timeStr = "1970-01-01 00:00:01.400";
      zsLib::Time result = zsLib::Numeric<zsLib::Time>(timeStr);

      zsLib::Microseconds sinceEpoch = zsLib::timeSinceEpoch<zsLib::Microseconds>(result);

      zsLib::String full = zsLib::string(result);

      zsLib::Microseconds fromStr = zsLib::Numeric<zsLib::Microseconds>(full);

      TESTING_EQUAL(full, "1.400000");
      TESTING_EQUAL(zsLib::string(sinceEpoch), "1.400000");
      TESTING_EQUAL(full, zsLib::string(fromStr));
    }

    {
      zsLib::String timeStr = "1970-01-01 05:32:59.455121";
      zsLib::ULONG seconds = (5*60*60)+(32*60)+59;
      zsLib::Time result = zsLib::Numeric<zsLib::Time>(timeStr);

      zsLib::Microseconds sinceEpoch = zsLib::timeSinceEpoch<zsLib::Microseconds>(result);
      zsLib::Time convertedBack = zsLib::timeSinceEpoch(sinceEpoch);

      zsLib::String part1 = zsLib::string(seconds);
      zsLib::String full = part1 + ".455121";

      zsLib::Microseconds fromStr = zsLib::Numeric<zsLib::Microseconds>(full);

      TESTING_EQUAL(zsLib::string(result), full);
      TESTING_EQUAL(zsLib::string(sinceEpoch), full);
      TESTING_EQUAL(zsLib::string(sinceEpoch), zsLib::string(convertedBack));
      TESTING_EQUAL(full, zsLib::string(fromStr));
    }

    {
      zsLib::Time result = zsLib::now();
      zsLib::Microseconds sinceEpoch = zsLib::timeSinceEpoch<zsLib::Microseconds>(result);
      zsLib::Time convertedBack = zsLib::timeSinceEpoch<zsLib::Microseconds>(sinceEpoch);

      zsLib::String full = zsLib::string(result);

      zsLib::Microseconds fromStr1 = zsLib::Numeric<zsLib::Microseconds>(full);
      zsLib::Time fromStr2 = zsLib::Numeric<zsLib::Time>(full);

      TESTING_EQUAL(zsLib::string(sinceEpoch), zsLib::string(convertedBack));
      TESTING_EQUAL(full, zsLib::string(fromStr1));
      TESTING_EQUAL(full, zsLib::string(fromStr2));
    }
  }

TESTING_AUTO_TEST_SUITE_END()
