/*
 *  Created by Robin Raymond.
 *  Copyright 2009-2013. Robin Raymond. All rights reserved.
 *
 * This file is part of zsLib.
 *
 * zsLib is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License (LGPL) as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * zsLib is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with zsLib; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */

#include <zsLib/helpers.h>
#include <zsLib/Stringize.h>

//#include <boost/test/unit_test_suite.hpp>
//#include <boost/test/unit_test.hpp>
//#include <boost/test/test_tools.hpp>

#include "boost_replacement.h"
#include "main.h"

static int get99()
{
  return 99;
}

BOOST_AUTO_TEST_SUITE(zsLibHelperTest)

  BOOST_AUTO_TEST_CASE(Test_AUTO)
  {
    if (!ZSLIB_TEST_HELPER) return;

    zsLib::AutoBool testBool;
    BOOST_CHECK(!testBool)

    zsLib::AutoBool autoBool;

    zsLib::String resultAutoBool = zsLib::string(autoBool);
    BOOST_EQUAL("false", resultAutoBool);

    boost::value_initialized<int> value;

    BOOST_CHECK(0 == value)

    zsLib::AutoDWORD autoDword;
    BOOST_EQUAL("0", zsLib::string(autoDword));

    get(autoDword) = 15;
    BOOST_EQUAL("15", zsLib::string(autoDword));

    zsLib::PUID puid1 = zsLib::createPUID();

    zsLib::AutoPUID autoPuid1;

    BOOST_CHECK(get(autoPuid1) == (puid1 + 1))

    BOOST_EQUAL(zsLib::string(autoPuid1), zsLib::Stringize<zsLib::PUID>(puid1+1).string())

    BOOST_EQUAL("99", zsLib::string(get99()));
    BOOST_EQUAL("99", zsLib::Stringize<int>(get99()).string());
  }

  BOOST_AUTO_TEST_CASE(Test_PUID_GUID)
  {
    if (!ZSLIB_TEST_HELPER) return;

    zsLib::PUID puid1 = zsLib::createPUID();
    zsLib::PUID puid2 = zsLib::createPUID();

    zsLib::UUID uuid1 = zsLib::createUUID();
    zsLib::UUID uuid2 = zsLib::createUUID();

    BOOST_CHECK(sizeof(puid1) == sizeof(zsLib::PUID))
    BOOST_CHECK(puid1 != puid2)

    BOOST_CHECK(sizeof(uuid1) == sizeof(zsLib::UUID))
    BOOST_CHECK(uuid1 != uuid2)
  }

  BOOST_AUTO_TEST_CASE(Test_atomic_inc_dec)
  {
    if (!ZSLIB_TEST_HELPER) return;

    zsLib::ULONG value = 0;
    zsLib::ULONG value1 = zsLib::atomicIncrement(value);
    BOOST_EQUAL(1, value);
    BOOST_EQUAL(1, value1);
    BOOST_EQUAL(1, zsLib::atomicGetValue(value));
    zsLib::ULONG value2 = zsLib::atomicIncrement(value);
    BOOST_EQUAL(2, value);
    BOOST_EQUAL(2, value2);
    BOOST_EQUAL(2, zsLib::atomicGetValue(value));
    zsLib::ULONG value3 = zsLib::atomicDecrement(value);
    BOOST_EQUAL(1, value);
    BOOST_EQUAL(1, value3);
    BOOST_EQUAL(1, zsLib::atomicGetValue(value));
    zsLib::ULONG value4 = zsLib::atomicDecrement(value);
    BOOST_EQUAL(0, value);
    BOOST_EQUAL(0, value4);
    BOOST_EQUAL(0, zsLib::atomicGetValue(value));
  }

  BOOST_AUTO_TEST_CASE(TestHelper_atomic_get_set)
  {
    if (!ZSLIB_TEST_HELPER) return;

    zsLib::DWORD value = 0;
    BOOST_EQUAL(0, zsLib::atomicGetValue32(value))
    zsLib::atomicSetValue32(value, 1);
    BOOST_EQUAL(1, zsLib::atomicGetValue32(value))
    zsLib::atomicSetValue32(value, 0xFFFFFFFF);
    BOOST_EQUAL(0xFFFFFFFF, zsLib::atomicGetValue32(value))
  }

BOOST_AUTO_TEST_SUITE_END()
