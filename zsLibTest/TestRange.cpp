/*

 Copyright (c) 2017, Robin Raymond
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

#include <zsLib/RangeSelection.h>

#include "testing.h"
#include "main.h"

namespace testing_range
{
  template <typename RangeValueType>
  struct RangeTest
  {
    typedef std::pair<RangeValueType, RangeValueType> RangePair;
    typedef std::list<RangePair> RangeList;

    typedef std::pair<unsigned long long, RangeValueType> RandomRangePair;
    typedef std::list<RandomRangePair> RandomRangeList;

    void allow(RangeValueType start, RangeValueType stop)
    {
      range_.allow(start, stop);
    }

    void deny(RangeValueType start, RangeValueType stop)
    {
      range_.deny(start, stop);
    }

    void removeAllow(RangeValueType start, RangeValueType stop)
    {
      range_.removeAllow(start, stop);
    }

    void removeDeny(RangeValueType start, RangeValueType stop)
    {
      range_.removeDeny(start, stop);
    }

    void shouldAllow(RangeValueType start, RangeValueType stop)
    {
      shouldAllow_.push_back(RangePair(start, stop));
    }
    void shouldDeny(RangeValueType start, RangeValueType stop)
    {
      shouldDeny_.push_back(RangePair(start, stop));
    }
    void random(unsigned long long random, RangeValueType output)
    {
      randoms_.push_back(RandomRangePair(random, output));
    }

    void checkAllows(zsLib::RangeSelection<RangeValueType> &checkRange)
    {
      for (auto iter = shouldAllow_.begin(); iter != shouldAllow_.end(); ++iter) {
        auto &range = (*iter);

        RangeValueType loop = range.first;
        TESTING_EQUAL(loop, loop);
        for (; loop < range.second; ++loop) {
          if (!checkRange.isAllowed(loop)) {
            TESTING_EQUAL(loop, loop);
            TESTING_CHECK(false);
          }
        }
        TESTING_EQUAL(loop, loop);
        if (loop == range.second) {
          TESTING_CHECK(checkRange.isAllowed(loop));
        }
      }
    }

    void checkDenies(zsLib::RangeSelection<RangeValueType> &checkRange)
    {
      for (auto iter = shouldDeny_.begin(); iter != shouldDeny_.end(); ++iter) {
        auto &range = (*iter);

        RangeValueType loop = range.first;
        TESTING_EQUAL(loop, loop);
        for (; loop < range.second; ++loop) {
          if (checkRange.isAllowed(loop)) {
            TESTING_EQUAL(loop, loop);
            TESTING_CHECK(false);
          }
        }
        TESTING_EQUAL(loop, loop);
        if (loop == range.second) {
          TESTING_CHECK(!checkRange.isAllowed(loop));
        }
      }
    }

    void checkRandoms(zsLib::RangeSelection<RangeValueType> &checkRange)
    {
      for (auto iter = randoms_.begin(); iter != randoms_.end(); ++iter) {
        auto &range = (*iter);

        auto input = range.first;
        auto expectOutput = range.second;

        auto actualOutput = checkRange.getRandomPosition(input);

        TESTING_EQUAL(input, input);
        TESTING_EQUAL(expectOutput, actualOutput);
      }
    }

    void check(zsLib::RangeSelection<RangeValueType> &checkRange)
    {
      checkAllows(checkRange);
      checkDenies(checkRange);
      checkRandoms(checkRange);
    }

    void checkAll()
    {
      {
        check(range_);
      }

      {
        zsLib::RangeSelection<RangeValueType> range(range_);
        check(range);
      }

      {
        zsLib::RangeSelection<RangeValueType> range;
        range = (range_);
        check(range);
      }

      {
        auto exported = range_.exportToString();

        zsLib::RangeSelection<RangeValueType> range;
        range.importFromString(exported);
        check(range);
      }

      {
        zsLib::ISettings::clear("test_range");
        range_.exportToSetting("test_range");

        auto range = zsLib::RangeSelection<RangeValueType>::createFromSetting("test_range");
        check(range);
      }
    }

  protected:

    RangeList shouldAllow_;
    RangeList shouldDeny_;
    RandomRangeList randoms_;

    zsLib::RangeSelection<RangeValueType> range_;
  };

  void testRange1()
  {
    {
      RangeTest<WORD> range;
      range.shouldAllow(0, 65535);
      range.random(0, 0);
      range.random(65535, 65535);
      range.random(65536, 0);
      range.checkAll();
    }
    {
      RangeTest<WORD> range;
      range.allow(0, 65535);
      range.shouldAllow(0, 65535);
      range.random(0, 0);
      range.random(65535, 65535);
      range.random(65536, 0);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.deny(5000, 5000);
      range.shouldAllow(0, 4999);
      range.shouldDeny(5000, 5000);
      range.shouldAllow(5001, 65535);
      range.random(0, 0);
      range.random(4999, 4999);
      range.random(5000, 5001);
      range.random(5001, 5002);
      range.random(65535, 0);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.allow(1000, 5000);
      range.shouldAllow(1000, 5000);
      range.shouldDeny(0, 999);
      range.shouldDeny(5001, 65535);
      range.random(0, 1000);
      range.random(4000, 5000);
      range.random(4001, 1000);
      range.random(5001, 2000);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.allow(1000, 5000);
      range.deny(1500, 4500);
      range.shouldAllow(1000, 1499);
      range.shouldAllow(4501, 5000);
      range.shouldDeny(0, 999);
      range.shouldDeny(5001, 65535);
      range.random(0, 1000);
      range.random(499, 1499);
      range.random(500, 4501);
      range.random(999, 5000);
      range.random(1000, 1000);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.allow(1000, 5000);
      range.deny(500, 4500);
      range.shouldAllow(4501, 5000);
      range.shouldDeny(0, 4499);
      range.shouldDeny(5001, 65535);
      range.random(0, 4501);
      range.random(499, 5000);
      range.random(500, 4501);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.allow(1000, 5000);
      range.allow(5002, 9000);
      range.shouldAllow(1000, 5000);
      range.shouldAllow(5002, 9000);
      range.shouldDeny(0, 999);
      range.shouldDeny(5001, 5001);
      range.shouldDeny(9001, 65535);
      range.random(0, 1000);
      range.random(4000, 5000);
      range.random(4001, 5002);
      range.random(5001, 6002);
      range.random(6001, 7002);
      range.random(7001, 8002);
      range.random(7999, 9000);
      range.random(8000, 1000);
      range.random(8001, 1001);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.deny(443, 443);
      range.deny(500, 500);
      range.deny(1900, 1900);
      range.deny(2869, 2869);
      range.deny(3074, 3074);
      range.deny(3076, 3076);
      range.deny(4016, 4016);
      range.deny(4211, 4211);
      range.deny(4222, 4223);
      range.deny(4500, 4500);
      range.deny(4600, 4601);
      range.deny(5355, 5355);
      range.deny(49152, 57343);

      range.shouldAllow(0, 442);
      range.shouldAllow(444, 499);
      range.shouldAllow(501, 1899);
      range.shouldAllow(1901, 2868);
      range.shouldAllow(2870, 3073);
      range.shouldAllow(3075, 3075);
      range.shouldAllow(3077, 4015);
      range.shouldAllow(4017, 4210);
      range.shouldAllow(4212, 4221);
      range.shouldAllow(4224, 4499);
      range.shouldAllow(4501, 4599);
      range.shouldAllow(4602, 5354);
      range.shouldAllow(5356, 49151);
      range.shouldAllow(57344, 65535);

      range.shouldDeny(443, 443);
      range.shouldDeny(500, 500);
      range.shouldDeny(1900, 1900);
      range.shouldDeny(2869, 2869);
      range.shouldDeny(3074, 3074);
      range.shouldDeny(3076, 3076);
      range.shouldDeny(4016, 4016);
      range.shouldDeny(4211, 4211);
      range.shouldDeny(4222, 4223);
      range.shouldDeny(4500, 4500);
      range.shouldDeny(4600, 4601);
      range.shouldDeny(5355, 5355);
      range.shouldDeny(49152, 57343);
      range.checkAll();
    }


    {
      RangeTest<WORD> range;
      range.allow(5000, 65535);

      range.deny(443, 443);
      range.deny(500, 500);
      range.deny(1900, 1900);
      range.deny(2869, 2869);
      range.deny(3074, 3074);
      range.deny(3076, 3076);
      range.deny(4016, 4016);
      range.deny(4211, 4211);
      range.deny(4222, 4223);
      range.deny(4500, 4500);
      range.deny(4600, 4601);
      range.deny(5355, 5355);
      range.deny(49152, 57343);

      range.shouldAllow(5000, 5354);
      range.shouldAllow(5356, 49151);
      range.shouldAllow(57344, 65535);

      range.shouldDeny(0, 4999);

      range.shouldDeny(443, 443);
      range.shouldDeny(500, 500);
      range.shouldDeny(1900, 1900);
      range.shouldDeny(2869, 2869);
      range.shouldDeny(3074, 3074);
      range.shouldDeny(3076, 3076);
      range.shouldDeny(4016, 4016);
      range.shouldDeny(4211, 4211);
      range.shouldDeny(4222, 4223);
      range.shouldDeny(4500, 4500);
      range.shouldDeny(4600, 4601);
      range.shouldDeny(5355, 5355);
      range.shouldDeny(49152, 57343);

      range.random(0, 5000);
      range.random(354, 5354);
      range.random(355, 5356);
      range.random(1000, 6001);
      range.checkAll();
    }
  }

  void testRange2()
  {

    {
      RangeTest<WORD> range;
      range.allow(0, 65535);
      range.deny(0, 1000);
      range.deny(5, 900);
      range.deny(100, 9000);
      range.shouldAllow(9001, 65535);
      range.shouldDeny(0, 9000);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.allow(500, 65535);
      range.deny(0, 1000);
      range.deny(5, 900);
      range.deny(100, 9000);
      range.shouldAllow(9001, 65535);
      range.shouldDeny(0, 9000);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.allow(500, 1000);
      range.deny(0, 700);
      range.deny(800, 2000);
      range.shouldAllow(701, 799);
      range.shouldDeny(0, 700);
      range.shouldDeny(800, 65535);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.allow(500, 1000);
      range.allow(1500, 2000);
      range.allow(2500, 3000);
      range.deny(500, 501);
      range.deny(1700, 1800);
      range.deny(2800, 3000);

      range.shouldAllow(502, 1000);
      range.shouldAllow(1500, 1699);
      range.shouldAllow(1801, 2000);
      range.shouldAllow(2500, 2799);

      range.shouldDeny(0, 501);
      range.shouldDeny(1001, 1499);
      range.shouldDeny(1700, 1800);
      range.shouldDeny(2001, 2499);
      range.shouldDeny(2800, 65535);
      range.checkAll();
    }
  }

  void testRange3()
  {

    {
      RangeTest<WORD> range;
      range.allow(500, 1000);
      range.allow(750, 900);
      range.shouldAllow(500, 1000);
      range.shouldDeny(0, 499);
      range.shouldDeny(1001, 65535);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.allow(750, 900);
      range.allow(500, 1000);
      range.shouldAllow(500, 1000);
      range.shouldDeny(0, 499);
      range.shouldDeny(1001, 65535);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.allow(500, 1000);
      range.allow(400, 550);
      range.shouldAllow(400, 1000);
      range.shouldDeny(0, 399);
      range.shouldDeny(1001, 65535);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.allow(500, 1000);
      range.allow(750, 1100);
      range.shouldAllow(500, 1100);
      range.shouldDeny(0, 499);
      range.shouldDeny(1101, 65535);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.allow(500, 1000);
      range.allow(1500, 2000);
      range.allow(750, 1750);
      range.shouldAllow(500, 2000);
      range.shouldDeny(0, 499);
      range.shouldDeny(2001, 65535);
      range.checkAll();
    }
  }

  void testRange4()
  {

    {
      RangeTest<WORD> range;
      range.allow(500, 1000);
      range.allow(1500, 2000);
      range.deny(0, 499);
      range.deny(1001, 1499);
      range.deny(2001, 65535);

      range.shouldAllow(500, 1000);
      range.shouldAllow(1500, 2000);
      range.shouldDeny(0, 499);
      range.shouldDeny(1001, 1499);
      range.shouldDeny(2001, 65535);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.allow(500, 1000);
      range.allow(1500, 2000);

      range.deny(499, 1001);

      range.shouldAllow(1500, 2000);
      range.shouldDeny(0, 1499);
      range.shouldDeny(2001, 65535);
      range.checkAll();
    }

    {
      RangeTest<WORD> range;
      range.allow(500, 1000);
      range.allow(1500, 2000);

      range.deny(500, 1000);

      range.shouldAllow(1500, 2000);
      range.shouldDeny(0, 1499);
      range.shouldDeny(2001, 65535);
      range.checkAll();
    }
  }
}

void testRange()
{
  if (!ZSLIB_TEST_RANGE) return;

  testing_range::testRange1();
  testing_range::testRange2();
  testing_range::testRange3();
  testing_range::testRange4();
}
