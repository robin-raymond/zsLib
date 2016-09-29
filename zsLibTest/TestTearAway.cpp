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

#include <zsLib/TearAway.h>
#include <zsLib/Stringize.h>
#include <iostream>


#include "testing.h"
#include "main.h"

using zsLib::ULONG;
using zsLib::IMessageQueue;

namespace testing
{
  struct check {
    check()
    {
    }
    bool mCalledFunc1 {};
    ULONG mCalledFunc2 {};
    zsLib::String mCalledFunc3;
    int mCalledFunc4 {};
    int mCalledFunc5 {};
    int mCalledFunc6 {};

    bool mDestroyedTestProxyCallback {};
  };

  static check &getCheck()
  {
    static check gCheck;
    return gCheck;
  }

  ZS_DECLARE_INTERACTION_PTR(Subscription)
  ZS_DECLARE_INTERACTION_PTR(ITestProxyDelegate)

  struct MyData
  {
    int mValue {};
  };

  ZS_DECLARE_STRUCT_PTR(MyData)

  interaction ITestTearAway
  {
    virtual void func1() = 0;
    virtual void func2() const = 0;
    virtual void func3(zsLib::String) = 0;
    virtual void func4(int value) = 0;
    virtual zsLib::String func5(ULONG value1, ULONG value2) = 0;
    virtual zsLib::String func6(ULONG value1, ULONG value2) const = 0;
  };

  ZS_DECLARE_INTERACTION_TEAR_AWAY(ITestTearAway, MyData)

}


ZS_DECLARE_TEAR_AWAY_BEGIN(testing::ITestTearAway, testing::MyData)
ZS_DECLARE_TEAR_AWAY_METHOD_0(func1)
ZS_DECLARE_TEAR_AWAY_METHOD_CONST_0(func2)
ZS_DECLARE_TEAR_AWAY_METHOD_1(func3, zsLib::String)
ZS_DECLARE_TEAR_AWAY_METHOD_1(func4, int)
ZS_DECLARE_TEAR_AWAY_METHOD_RETURN_2(func5, zsLib::String, ULONG, ULONG)
ZS_DECLARE_TEAR_AWAY_METHOD_CONST_RETURN_2(func6, zsLib::String, ULONG, ULONG)
ZS_DECLARE_TEAR_AWAY_END()

namespace testing
{
  ZS_DECLARE_CLASS_PTR(TestTearAwayCallback)

  class TestTearAwayCallback : public ITestTearAway
  {
  private:
    TestTearAwayCallback()
    {
    }
  public:
    static TestTearAwayCallbackPtr create()
    {
      return TestTearAwayCallbackPtr(new TestTearAwayCallback());
    }

    virtual void func1()
    {
      getCheck().mCalledFunc1 = true;
    }
    virtual void func2() const
    {
      ++(getCheck().mCalledFunc2);
    }
    virtual void func3(zsLib::String value)
    {
      getCheck().mCalledFunc3 = value;
    }
    virtual void func4(int value)
    {
      getCheck().mCalledFunc4 = value;
    }
    virtual zsLib::String func5(ULONG value1, ULONG value2)
    {
      return zsLib::Stringize<ULONG>(value1,16).string() + " " + zsLib::Stringize<ULONG>(value2,16).string();
    }
    virtual zsLib::String func6(ULONG value1, ULONG value2) const
    {
      return zsLib::Stringize<ULONG>(value1,16).string() + " " + zsLib::Stringize<ULONG>(value2,16).string();
    }

    ~TestTearAwayCallback()
    {
      getCheck().mDestroyedTestProxyCallback = true;
    }
  };

  class TestTearAway
  {
  public:

    TestTearAway()
    {
      TestTearAwayCallbackPtr testObject = TestTearAwayCallback::create();

      ITestTearAwayPtr delegate = ITestTearAwayTearAway::create(testObject);

      TESTING_CHECK(delegate.get() != (ITestTearAwayPtr(testObject)).get())
      TESTING_CHECK(! ITestTearAwayTearAway::isTearAway(testObject))
      TESTING_CHECK(ITestTearAwayTearAway::isTearAway(delegate))

      ITestTearAwayPtr original = ITestTearAwayTearAway::original(delegate);
      TESTING_EQUAL(original.get(), (ITestTearAwayPtr(testObject)).get())

      MyDataPtr data = ITestTearAwayTearAway::data(testObject);
      TESTING_CHECK(MyDataPtr() == data)

      delegate->func1();
      for (int i = 0; i < 1000; ++i)
      {
        delegate->func2();
      }
      zsLib::String str("func3");
      delegate->func3(str);
      str = "bogus3";

      delegate->func4(0xFFFF);
      TESTING_EQUAL(getCheck().mCalledFunc4, 0xFFFF);

      TESTING_EQUAL(delegate->func5(0xABC, 0xDEF), "abc def");
      TESTING_EQUAL(delegate->func6(0xDEF, 0xABC), "def abc");

      checkData();
    }

    void checkData()
    {
      MyDataPtr myData(new MyData);
      myData->mValue = 1001;

      TestTearAwayCallbackPtr testObject = TestTearAwayCallback::create();

      ITestTearAwayPtr delegate = ITestTearAwayTearAway::create(testObject, myData);

      MyDataPtr data = ITestTearAwayTearAway::data(delegate);
      TESTING_CHECK(data != MyDataPtr())

      TESTING_EQUAL(data->mValue, 1001)
    }

    ~TestTearAway()
    {
      TESTING_EQUAL(getCheck().mCalledFunc3, "func3");
      TESTING_EQUAL(getCheck().mCalledFunc2, 1000);
      TESTING_CHECK(getCheck().mCalledFunc1);
      TESTING_CHECK(getCheck().mDestroyedTestProxyCallback);
    }
  };

}

void testTearAway()
{
    if (!ZSLIB_TEST_TEAR_AWAY) return;

    testing::TestTearAway test;
}
