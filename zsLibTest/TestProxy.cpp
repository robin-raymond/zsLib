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

#include <zsLib/Proxy.h>
#include <zsLib/ProxySubscriptions.h>
#include <zsLib/IMessageQueueThread.h>
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

  ZS_DECLARE_INTERACTION_PTR(Subscription)
  ZS_DECLARE_INTERACTION_PTR(ITestProxyDelegate)

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

  ZS_DECLARE_INTERACTION_PROXY(ITestProxyDelegate)

  ZS_DECLARE_INTERACTION_PROXY(ITestSubscriptionProxyDelegate)
  ZS_DECLARE_INTERACTION_PROXY_SUBSCRIPTION(Subscription, ITestSubscriptionProxyDelegate)

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
  ZS_DECLARE_CLASS_PTR(TestProxyCallback)

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
      mThread = zsLib::IMessageQueueThread::createBasic();
      mThreadNeverCalled = zsLib::IMessageQueueThread::createBasic();

      TestProxyCallbackPtr testObject = TestProxyCallback::create(mThread);

      ITestProxyDelegatePtr delegate = ITestProxyDelegateProxy::create(testObject);
      ITestSubscriptionProxyDelegatePtr subscriptionDelegate = ITestSubscriptionProxyDelegateProxy::create(testObject);

      ITestSubscriptionProxyDelegateSubscriptions subscriptions;

      SubscriptionPtr subscription = subscriptions.subscribe(subscriptionDelegate);

      TESTING_CHECK(delegate.get() != (ITestProxyDelegatePtr(testObject)).get())
      TESTING_CHECK(! zsLib::Proxy<ITestProxyDelegate>::isProxy(testObject))
      TESTING_CHECK(zsLib::Proxy<ITestProxyDelegate>::isProxy(delegate))

      ITestProxyDelegatePtr original = zsLib::Proxy<ITestProxyDelegate>::original(delegate);
      TESTING_EQUAL(original.get(), (ITestProxyDelegatePtr(testObject)).get())

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
          std::this_thread::yield();
      } while (count > 0);
      mThread->waitForShutdown();
      mThreadNeverCalled->waitForShutdown();

      TESTING_EQUAL(zsLib::proxyGetTotalConstructed(), 0);

      TESTING_EQUAL(getCheck().mCalledFunc3, "func3");
      TESTING_EQUAL(getCheck().mCalledFunc2, 1000);
      TESTING_CHECK(getCheck().mCalledFunc1);
      TESTING_CHECK(getCheck().mCalledSub1);
      TESTING_EQUAL(getCheck().mCalledSub2, "sub2");
      TESTING_CHECK(getCheck().mDestroyedTestProxyCallback);
    }

    zsLib::IMessageQueueThreadPtr mThread;
    zsLib::IMessageQueueThreadPtr mThreadNeverCalled;
  };

}

void testProxy()
{
  if (!ZSLIB_TEST_PROXY) return;

  testing::TestProxy test;
}
