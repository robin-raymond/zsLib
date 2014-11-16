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

#include "testing.h"
#include "main.h"

static int get99()
{
  return 99;
}

TESTING_AUTO_TEST_SUITE(zsLibHelperTest)

  TESTING_AUTO_TEST_CASE(Test_AUTO)
  {
    if (!ZSLIB_TEST_HELPER) return;

    bool testBool {};
    TESTING_CHECK(!testBool)

    bool autoBool {};

    zsLib::String resultAutoBool = zsLib::string(autoBool);
    TESTING_EQUAL("false", resultAutoBool);

    int value {};

    TESTING_CHECK(0 == value)

    zsLib::DWORD autoDword {};
    TESTING_EQUAL("0", zsLib::string(autoDword));

    autoDword = 15;
    TESTING_EQUAL("15", zsLib::string(autoDword));

    zsLib::PUID puid1 = zsLib::createPUID();

    zsLib::AutoPUID autoPuid1;

    TESTING_CHECK(autoPuid1 == (puid1 + 1))

    TESTING_EQUAL(zsLib::string(autoPuid1), zsLib::Stringize<zsLib::PUID>(puid1+1).string())

    TESTING_EQUAL("99", zsLib::string(get99()));
    TESTING_EQUAL("99", zsLib::Stringize<int>(get99()).string());
  }

  TESTING_AUTO_TEST_CASE(Test_PUID_GUID)
  {
    if (!ZSLIB_TEST_HELPER) return;

    zsLib::PUID puid1 = zsLib::createPUID();
    zsLib::PUID puid2 = zsLib::createPUID();

    zsLib::UUID uuid1 = zsLib::createUUID();
    zsLib::UUID uuid2 = zsLib::createUUID();

    TESTING_CHECK(sizeof(puid1) == sizeof(zsLib::PUID))
    TESTING_CHECK(puid1 != puid2)

    TESTING_CHECK(sizeof(uuid1) == sizeof(zsLib::UUID))
    TESTING_CHECK(uuid1 != uuid2)
  }

  TESTING_AUTO_TEST_CASE(Test_atomic_inc_dec)
  {
    if (!ZSLIB_TEST_HELPER) return;

    std::atomic_ulong value {};
    zsLib::ULONG value1 = (++value);
    TESTING_EQUAL(1, value);
    TESTING_EQUAL(1, value1);
    zsLib::ULONG value2 = (++value);
    TESTING_EQUAL(2, value);
    TESTING_EQUAL(2, value2);
    zsLib::ULONG value3 = (--value);
    TESTING_EQUAL(1, value);
    TESTING_EQUAL(1, value3);
    zsLib::ULONG value4 = (--value);
    TESTING_EQUAL(0, value);
    TESTING_EQUAL(0, value4);
  }

  TESTING_AUTO_TEST_CASE(TestHelper_atomic_get_set)
  {
    if (!ZSLIB_TEST_HELPER) return;

    std::atomic_ulong value {};
    TESTING_EQUAL(0, value)
    value = 1;
    TESTING_EQUAL(1, value)
    value = 0xFFFFFFFF;
    TESTING_EQUAL(0xFFFFFFFF, value)
  }

TESTING_AUTO_TEST_SUITE_END()
