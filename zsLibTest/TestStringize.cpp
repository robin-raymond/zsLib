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

#include <zsLib/helpers.h>
#include <zsLib/Stringize.h>
#include <zsLib/Numeric.h>
#include <zsLib/IPAddress.h>


#include "testing.h"
#include "main.h"

using zsLib::BYTE;
using zsLib::LONGLONG;
using zsLib::ULONGLONG;
using zsLib::IPAddress;

TESTING_AUTO_TEST_SUITE(zsLibStringize)

  TESTING_AUTO_TEST_CASE(TestStringize)
  {
    if (!ZSLIB_TEST_STRINGIZE) return;

    TESTING_EQUAL("0", zsLib::Stringize<char>(0).string());
    TESTING_EQUAL("-128", zsLib::Stringize<char>(-128).string());
    TESTING_EQUAL("127", zsLib::Stringize<char>(127).string());
    TESTING_EQUAL("80", zsLib::Stringize<char>(-128, 16).string());

    TESTING_EQUAL("0", zsLib::Stringize<unsigned char>(0).string());
    TESTING_EQUAL("255", zsLib::Stringize<unsigned char>(255).string());
    TESTING_EQUAL("ff", zsLib::Stringize<unsigned char>(0xFF, 16).string());

    TESTING_EQUAL("ffff", zsLib::Stringize<unsigned short>(0xFFFF, 16).string());
    TESTING_EQUAL("ffffffff", zsLib::Stringize<unsigned int>(0xFFFFFFFF, 16).string());
#ifdef ZSLIBG_64BIT
    TESTING_EQUAL("ffffffffffffffff", zsLib::Stringize<ULONGLONG>(0xFFFFFFFFFFFFFFFF, 16).string());
#endif // ZSLIBG_64BIT

    TESTING_EQUAL("f0f0", zsLib::Stringize<unsigned short>(0xF0F0, 16).string());
    TESTING_EQUAL("f0f0f0f0", zsLib::Stringize<unsigned int>(0xF0F0F0F0, 16).string());
#ifdef ZSLIB_64BIT
    TESTING_EQUAL("f0f0f0f0f0f0f0f0", zsLib::Stringize<ULONGLONG>(0xF0F0F0F0F0F0F0F0, 16).string());
#endif // ZSLIBG_64BIT

    TESTING_EQUAL("5", zsLib::Stringize<float>(5.0f).string());
    TESTING_EQUAL("0", zsLib::Stringize<float>(0.0f).string());
    TESTING_EQUAL("0.5", zsLib::Stringize<float>(0.5).string());
    TESTING_EQUAL("0.05", zsLib::Stringize<float>(0.05).string());
    TESTING_EQUAL("-0.5", zsLib::Stringize<float>(-0.5).string());
    TESTING_EQUAL("-0.05", zsLib::Stringize<float>(-0.05).string());

    TESTING_EQUAL("0", zsLib::Stringize<double>(0.0).string());
    TESTING_EQUAL("123456.789", zsLib::Stringize<double>(123456.789).string());
    TESTING_EQUAL("123456.0789", zsLib::Stringize<double>(123456.0789).string());
    TESTING_EQUAL("-123456.789", zsLib::Stringize<double>(-123456.789).string());

    zsLib::UUID uuid = zsLib::Numeric<zsLib::UUID>("b0a01e87-2be5-4daa-8155-1380c98400a1");
    zsLib::String uuidStr = zsLib::Stringize<zsLib::UUID>(uuid);
    TESTING_EQUAL("b0a01e87-2be5-4daa-8155-1380c98400a1", uuidStr);

    TESTING_EQUAL(IPAddress("192.168.1.10:5060").string(), string(IPAddress("192.168.1.10:5060")))

    {
      zsLib::Hours duration {};
      duration = zsLib::Hours(5);
      zsLib::ULONG hours = 5;
      zsLib::String result = zsLib::string(hours);

      TESTING_EQUAL(result, zsLib::Stringize<zsLib::Hours>(duration).string());
      TESTING_EQUAL(result, zsLib::string(duration));
    }

    {
      zsLib::Seconds duration {};
      duration = zsLib::Hours(5) + zsLib::Minutes(1) + zsLib::Seconds(44);
      zsLib::ULONG seconds = (5*60*60) + (1*60) + 44;
      zsLib::String result = zsLib::string(seconds);

      TESTING_EQUAL(result, zsLib::Stringize<zsLib::Seconds>(duration).string());
      TESTING_EQUAL(result, zsLib::string(duration));
    }

    {
      zsLib::Microseconds duration {};
      duration = zsLib::Hours(75) + zsLib::Minutes(47) + zsLib::Seconds(3) + zsLib::Milliseconds(453) + zsLib::Microseconds(912);
      zsLib::ULONG seconds = (75*60*60) + (47*60) + 3;
      zsLib::ULONG micro = (453 * 1000) + 912;
      zsLib::String result = zsLib::string(seconds) + "." + zsLib::string(micro);

      zsLib::String result2 = zsLib::Stringize<zsLib::Microseconds>(duration).string();
      zsLib::String result3 = zsLib::string(duration);

      TESTING_EQUAL(result, result2);
      TESTING_EQUAL(result, result3);
    }

    {
      zsLib::Microseconds duration {};
      duration = zsLib::Hours(75) + zsLib::Minutes(47) + zsLib::Seconds(3) + zsLib::Milliseconds(53) + zsLib::Microseconds(2);
      zsLib::ULONG seconds = (75*60*60) + (47*60) + 3;
      zsLib::ULONG micro = (53 * 1000) + 2;
      zsLib::String result = zsLib::string(seconds) + ".0" + zsLib::string(micro);

      TESTING_EQUAL(result, zsLib::Stringize<zsLib::Microseconds>(duration).string());
      TESTING_EQUAL(result, zsLib::string(duration));
    }
  }

TESTING_AUTO_TEST_SUITE_END()
