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

#include <zsLib/Proxy.h>
#include <zsLib/ProxySubscriptions.h>
#include <zsLib/MessageQueueThread.h>
#include <zsLib/Stringize.h>
#include <iostream>

//#include <boost/test/unit_test_suite.hpp>
//#include <boost/test/unit_test.hpp>
//#include <boost/test/test_tools.hpp>


#include "boost_replacement.h"

using zsLib::ULONG;
using zsLib::IMessageQueue;

namespace testing
{
  struct check {
    check()
    {
      mCalledFunc1 = false;
      mCalledFunc2 = 0;
      mCalledSub1 = false;
      mDestroyedTestProxyCallback = false;
    }
    bool mCalledFunc1;
    ULONG mCalledFunc2;
    zsLib::String mCalledFunc3;
    int mCalledFunc4;

    bool mCalledSub1;
    zsLib::String mCalledSub2;

    bool mDestroyedTestProxyCallback;
  };

  static check &getCheck()
  {
    static check gCheck;
    return gCheck;
  }

  interaction ITestProxyDelegate;
  typedef boost::shared_ptr<ITestProxyDelegate> ITestProxyDelegatePtr;
  typedef zsLib::Proxy<ITestProxyDelegate> ITestProxyDelegateProxy;

  interaction ITestProxyDelegate
  {
    virtual void func1() = 0;
    virtual void func2() = 0;
    virtual void func3(zsLib::String) = 0;
    virtual void func4(int value) = 0;
    virtual zsLib::String func5(ULONG value1, ULONG value2) = 0;
  };

  interaction ITestSubscriptionProxyDelegate
  {
    virtual void sub1() = 0;
    virtual void sub2(zsLib::String) = 0;
  };

  interaction Subscription
  {
    virtual void cancel() = 0;
  };

  typedef boost::shared_ptr<ITestSubscriptionProxyDelegate> ITestSubscriptionProxyDelegatePtr;
  typedef zsLib::Proxy<ITestSubscriptionProxyDelegate> ITestSubscriptionProxyDelegateWeakPtr;
  typedef zsLib::Proxy<ITestSubscriptionProxyDelegate> ITestSubscriptionProxyDelegateProxy;
  typedef zsLib::ProxySubscriptions<ITestSubscriptionProxyDelegate, Subscription> ITestSubscriptionProxyDelegateProxySubscriptions;

  typedef boost::shared_ptr<Subscription> SubscriptionPtr;
  typedef boost::weak_ptr<Subscription> SubscriptionWeakPtr;
}


ZS_DECLARE_PROXY_BEGIN(testing::ITestProxyDelegate)
ZS_DECLARE_PROXY_METHOD_0(func1)
ZS_DECLARE_PROXY_METHOD_0(func2)
ZS_DECLARE_PROXY_METHOD_1(func3, zsLib::String)
ZS_DECLARE_PROXY_METHOD_SYNC_1(func4, int)
ZS_DECLARE_PROXY_METHOD_SYNC_RETURN_2(func5, zsLib::String, ULONG, ULONG)
ZS_DECLARE_PROXY_END()

ZS_DECLARE_PROXY_BEGIN(testing::ITestSubscriptionProxyDelegate)
ZS_DECLARE_PROXY_METHOD_0(sub1)
ZS_DECLARE_PROXY_METHOD_1(sub2, zsLib::String)
ZS_DECLARE_PROXY_END()

ZS_DECLARE_PROXY_SUBSCRIPTIONS_BEGIN(testing::ITestSubscriptionProxyDelegate, testing::Subscription)
ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_0(sub1)
ZS_DECLARE_PROXY_SUBSCRIPTIONS_METHOD_1(sub2, zsLib::String)
ZS_DECLARE_PROXY_SUBSCRIPTIONS_END()

namespace testing
{
  class TestProxyCallback;
  typedef boost::shared_ptr<TestProxyCallback> TestProxyCallbackPtr;

  class TestProxyCallback : public ITestProxyDelegate,
                            public ITestSubscriptionProxyDelegate,
                            public zsLib::MessageQueueAssociator
  {
  private:
    TestProxyCallback(zsLib::IMessageQueuePtr queue) : zsLib::MessageQueueAssociator(queue)
    {
    }
  public:
    static TestProxyCallbackPtr create(zsLib::IMessageQueuePtr queue)
    {
      return TestProxyCallbackPtr(new TestProxyCallback(queue));
    }

    virtual void func1()
    {
      getCheck().mCalledFunc1 = true;
    }
    virtual void func2()
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

    virtual void sub1()
    {
      getCheck().mCalledSub1 = true;
    }

    virtual void sub2(zsLib::String value)
    {
      getCheck().mCalledSub2 = value;
    }

    ~TestProxyCallback()
    {
      getCheck().mDestroyedTestProxyCallback = true;
    }
  };

  class TestProxy
  {
  public:
    TestProxy()
    {
      mThread = zsLib::MessageQueueThread::createBasic();
      mThreadNeverCalled = zsLib::MessageQueueThread::createBasic();

      TestProxyCallbackPtr testObject = TestProxyCallback::create(mThread);

      ITestProxyDelegatePtr delegate = ITestProxyDelegateProxy::create(testObject);
      ITestSubscriptionProxyDelegatePtr subscriptionDelegate = ITestSubscriptionProxyDelegateProxy::create(testObject);


      ITestSubscriptionProxyDelegateProxySubscriptions subscriptions;

      SubscriptionPtr subscription = subscriptions.subscribe(subscriptionDelegate);

      BOOST_CHECK(delegate.get() != (ITestProxyDelegatePtr(testObject)).get())
      BOOST_CHECK(! zsLib::Proxy<ITestProxyDelegate>::isProxy(testObject))
      BOOST_CHECK(zsLib::Proxy<ITestProxyDelegate>::isProxy(delegate))

      ITestProxyDelegatePtr original = zsLib::Proxy<ITestProxyDelegate>::original(delegate);
      BOOST_EQUAL(original.get(), (ITestProxyDelegatePtr(testObject)).get())

      delegate->func1();
      for (int i = 0; i < 1000; ++i)
      {
        delegate->func2();
      }
      zsLib::String str("func3");
      delegate->func3(str);
      str = "bogus3";

      delegate->func4(0xFFFF);
      BOOST_EQUAL(getCheck().mCalledFunc4, 0xFFFF);

      BOOST_EQUAL(delegate->func5(0xABC, 0xDEF), "abc def");

      subscriptions.delegate()->sub1();

      subscriptions.delegate()->sub2("sub2");

      subscription->cancel();
      subscriptions.delegate()->sub2("post_sub2_cancelled");
    }

    ~TestProxy()
    {
      IMessageQueue::size_type count = 0;
      do
      {
        count = mThread->getTotalUnprocessedMessages();
        count += mThreadNeverCalled->getTotalUnprocessedMessages();
        if (0 != count)
          boost::this_thread::yield();
      } while (count > 0);
      mThread->waitForShutdown();
      mThreadNeverCalled->waitForShutdown();

      BOOST_EQUAL(zsLib::proxyGetTotalConstructed(), 0);

      BOOST_EQUAL(getCheck().mCalledFunc3, "func3");
      BOOST_EQUAL(getCheck().mCalledFunc2, 1000);
      BOOST_CHECK(getCheck().mCalledFunc1);
      BOOST_CHECK(getCheck().mCalledSub1);
      BOOST_EQUAL(getCheck().mCalledSub2, "sub2");
      BOOST_CHECK(getCheck().mDestroyedTestProxyCallback);
    }

    zsLib::MessageQueueThreadPtr mThread;
    zsLib::MessageQueueThreadPtr mThreadNeverCalled;
  };

}

BOOST_AUTO_TEST_SUITE(zsLibProxy)

  BOOST_AUTO_TEST_CASE(TestProxy)
  {
    testing::TestProxy test;
  }

BOOST_AUTO_TEST_SUITE_END()
