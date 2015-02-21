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

#include <zsLib/Promise.h>
#include <zsLib/MessageQueueThread.h>
#include <zsLib/Stringize.h>
#include <iostream>


#include "testing.h"
#include "main.h"

using zsLib::ULONG;
using zsLib::String;
using zsLib::MessageQueueAssociator;

namespace testing
{
  ZS_DECLARE_USING_PTR(zsLib, IMessageQueue)
  ZS_DECLARE_USING_PTR(zsLib, Promise)
  ZS_DECLARE_USING_PTR(zsLib, IPromiseDelegate)

  struct check_promise {

    std::atomic<int> mSettledCalled {};
    std::atomic<int> mResolvedCalled {};
    std::atomic<int> mRejectedCalled {};

    String mValueAny1;
    String mValueAny2;

    String mReasonAny1;
    String mReasonAny2;

    std::atomic<int> mDestroyedCallback {};
  };

  static check_promise &getCheck()
  {
    static check_promise gCheck;
    return gCheck;
  }

  ZS_DECLARE_CLASS_PTR(TestPromiseCallback)

  ZS_DECLARE_STRUCT_PTR(Any1)
  ZS_DECLARE_STRUCT_PTR(Any2)

  struct Any1 : public zsLib::Any
  {
    String mValue;

    Any1(const char *value) : mValue(value) {}
    static Any1Ptr create(const char *value) {return Any1Ptr(new Any1(value));}
  };

  struct Any2 : public zsLib::Any
  {
    String mValue;

    Any2(const char *value) : mValue(value) {}
    static Any2Ptr create(const char *value) {return Any2Ptr(new Any2(value));}
  };
  
  class TestPromiseCallback : public IPromiseDelegate
  {
  private:
    TestPromiseCallback()
    {
    }
  public:
    static TestPromiseCallbackPtr create()
    {
      return TestPromiseCallbackPtr(new TestPromiseCallback());
    }

    virtual void onPromiseSettled(PromisePtr promise)
    {
      ++getCheck().mSettledCalled;
    }

    virtual void onPromiseResolved(PromisePtr promise)
    {
      ++getCheck().mResolvedCalled;

      {
        auto any = promise->value<Any1>();
        if (any) {
          getCheck().mValueAny1 = any->mValue;
        }
      }
      {
        auto any = promise->value<Any2>();
        if (any) {
          getCheck().mValueAny2 = any->mValue;
        }
      }
    }

    virtual void onPromiseRejected(PromisePtr promise)
    {
      ++getCheck().mRejectedCalled;

      {
        auto any = promise->reason<Any1>();
        if (any) {
          getCheck().mReasonAny1 = any->mValue;
        }
      }
      {
        auto any = promise->reason<Any2>();
        if (any) {
          getCheck().mReasonAny2 = any->mValue;
        }
      }
    }

    ~TestPromiseCallback()
    {
      ++getCheck().mDestroyedCallback;
    }
  };

  ZS_DECLARE_CLASS_PTR(TestPromiseCallbackWithQueue)

  class TestPromiseCallbackWithQueue : public MessageQueueAssociator,
                                       public IPromiseDelegate
  {
  private:
    TestPromiseCallbackWithQueue(IMessageQueuePtr queue) :
      MessageQueueAssociator(queue)
    {
    }

  public:
    static TestPromiseCallbackWithQueuePtr create(IMessageQueuePtr queue)
    {
      return TestPromiseCallbackWithQueuePtr(new TestPromiseCallbackWithQueue(queue));
    }

    virtual void onPromiseSettled(PromisePtr promise)
    {
      ++getCheck().mSettledCalled;
    }

    virtual void onPromiseResolved(PromisePtr promise)
    {
      ++getCheck().mResolvedCalled;

      {
        auto any = promise->value<Any1>();
        if (any) {
          getCheck().mValueAny1 = any->mValue;
        }
      }
      {
        auto any = promise->value<Any2>();
        if (any) {
          getCheck().mValueAny2 = any->mValue;
        }
      }
    }

    virtual void onPromiseRejected(PromisePtr promise)
    {
      ++getCheck().mRejectedCalled;

      {
        auto any = promise->reason<Any1>();
        if (any) {
          getCheck().mReasonAny1 = any->mValue;
        }
      }
      {
        auto any = promise->reason<Any2>();
        if (any) {
          getCheck().mReasonAny2 = any->mValue;
        }
      }
    }

    ~TestPromiseCallbackWithQueue()
    {
      ++getCheck().mDestroyedCallback;
    }
  };
  
  class TestPromise
  {
  public:
    TestPromise()
    {
      mThreadService = zsLib::MessageQueueThread::createBasic("service");
      mThreadUser = zsLib::MessageQueueThread::createBasic("user");

      TestPromiseCallbackPtr callback = TestPromiseCallback::create();
      TestPromiseCallbackWithQueuePtr callbackWithQueue = TestPromiseCallbackWithQueue::create(mThreadUser);

      TestPromiseCallbackPtr callbackIgnored = TestPromiseCallback::create();

      {
        PromisePtr promise = Promise::create();
        TESTING_CHECK(promise)
      }

      {
        PromisePtr promise = Promise::create();
        TESTING_CHECK(promise)
        promise->then(callback);
        promise->resolve();
      }

      {
        PromisePtr promise = Promise::create(mThreadUser);
        TESTING_CHECK(promise)
        promise->then(callback);
        promise->resolve();
      }

      {
        PromisePtr promise = Promise::create(mThreadUser);
        TESTING_CHECK(promise)
        promise->then(callback);
        promise->reject();
      }

      {
        PromisePtr promise = Promise::create();
        TESTING_CHECK(promise)
        promise->then(callbackWithQueue);
        promise->reject();
      }

      {
        PromisePtr promise = Promise::create();
        TESTING_CHECK(promise)
        promise->resolve();
        promise->then(callbackWithQueue);
      }

      {
        PromisePtr promise = Promise::create();
        TESTING_CHECK(promise)
        promise->reject();
        promise->then(callback);
      }

      {
        PromisePtr promise = Promise::create();
        TESTING_CHECK(promise)
        promise->resolve(Any1::create("any1resolve"));
        promise->then(callbackWithQueue);
      }

      {
        PromisePtr promise = Promise::create();
        TESTING_CHECK(promise)
        promise->resolve(Any2::create("any2resolve"));
        promise->then(callback);
      }

      {
        PromisePtr promise = Promise::create();
        TESTING_CHECK(promise)
        promise->reject(Any1::create("any1reject"));
        promise->then(callback);
      }

      {
        PromisePtr promise = Promise::create();
        TESTING_CHECK(promise)
        promise->reject(Any1::create("any1reject-b"));
        promise->then(callback);
      }

      {
        PromisePtr promise = Promise::create();
        TESTING_CHECK(promise)
        promise->reject(Any2::create("any2reject"));
        promise->then(callbackWithQueue);
      }

      {
        std::list<PromisePtr> promises;

        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->reject();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }

        PromisePtr all = Promise::all(promises);
        all->then(callback);
      }

      {
        std::list<PromisePtr> promises;

        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->reject();
        }

        PromisePtr any = Promise::race(promises);
        any->then(callbackWithQueue);
      }

      {
        std::list<PromisePtr> promises;

        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->reject();
        }

        PromisePtr any = Promise::race(promises);
        any->then(callback);
      }

      {
        std::list<PromisePtr> promises;

        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->reject();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }

        PromisePtr any = Promise::race(promises);
        any->then(callback);
      }

      {
        std::list<PromisePtr> promises;

        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }

        PromisePtr all = Promise::all(promises);
        all->then(callback);
      }

      {
        std::list<PromisePtr> promises;

        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }

        PromisePtr all = Promise::all(promises);
        all->then(callback);
      }

      {
        std::list<PromisePtr> promises;

        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }

        PromisePtr all = Promise::all(promises);
        all->then(callbackIgnored);
      }

      {
        PromiseWeakPtr weakPromise;

        {
          PromisePtr promise = Promise::create();
          promise->then(callback);
          promise->background();

          weakPromise = promise;
        }

        PromisePtr laterPromise = weakPromise.lock();
        TESTING_CHECK(laterPromise)

        laterPromise->resolve();
      }

      {
        PromisePtr promise = Promise::create();

        PromisePtr chainedPromise = Promise::create();
        chainedPromise->then(callback);

        promise->then(chainedPromise);
        promise->resolve();
      }

      {
        PromisePtr promise = Promise::create();

        PromisePtr chainedPromise = Promise::create();
        chainedPromise->then(callback);

        promise->then(chainedPromise);
        promise->reject();
      }

      {
        std::list<PromisePtr> promises;
        {
          PromisePtr promise = Promise::create();
          promise->then(callback);
          promises.push_back(promise);
        }
        {
          PromisePtr promise = Promise::create();
          promise->then(callback);
          promises.push_back(promise);
        }
        {
          PromisePtr promise = Promise::create();
          promise->then(callback);
          promises.push_back(promise);
        }

        PromisePtr broadcastPromise = Promise::broadcast(promises);
        broadcastPromise->resolve();
      }
      {
        std::list<PromisePtr> promises;
        {
          PromisePtr promise = Promise::create();
          promise->then(callback);
          promises.push_back(promise);
        }
        {
          PromisePtr promise = Promise::create();
          promise->then(callback);
          promises.push_back(promise);
        }

        PromisePtr broadcastPromise = Promise::broadcast(promises);
        broadcastPromise->reject();
      }

      {
        std::list<PromisePtr> promises;

        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->reject();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }

        PromisePtr all = Promise::allSettled(promises);
        all->then(callback);
      }

      {
        std::list<PromisePtr> promises;

        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->reject();
        }

        PromisePtr all = Promise::allSettled(promises);
        all->then(callback);
      }

      {
        std::list<PromisePtr> promises;

        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->reject();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
          promise->resolve();
        }
        {
          PromisePtr promise = Promise::create();
          promises.push_back(promise);
        }

        PromisePtr all = Promise::allSettled(promises);
        all->then(callbackIgnored);
      }
    }

    ~TestPromise()
    {
      IMessageQueue::size_type count = 0;
      do
      {
        count = mThreadService->getTotalUnprocessedMessages();
        count += mThreadUser->getTotalUnprocessedMessages();
        if (0 != count)
          std::this_thread::yield();
      } while (count > 0);
      mThreadService->waitForShutdown();
      mThreadUser->waitForShutdown();

      TESTING_EQUAL(zsLib::proxyGetTotalConstructed(), 0);

      TESTING_EQUAL(3, getCheck().mDestroyedCallback)
      TESTING_EQUAL(27, getCheck().mSettledCalled)
      TESTING_EQUAL(15, getCheck().mResolvedCalled)
      TESTING_EQUAL(12, getCheck().mRejectedCalled)

      TESTING_EQUAL("any1resolve", getCheck().mValueAny1)
      TESTING_EQUAL("any2resolve", getCheck().mValueAny2)

      TESTING_EQUAL("any1reject-b", getCheck().mReasonAny1)
      TESTING_EQUAL("any2reject", getCheck().mReasonAny2)
    }

    zsLib::MessageQueueThreadPtr mThreadService;
    zsLib::MessageQueueThreadPtr mThreadUser;
  };

}


void testPromise()
{
  {
    testing::TestPromise test;
  }
}
