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

#include <zsLib/Timer.h>
#include <zsLib/MessageQueueThread.h>

#include "testing.h"
#include "main.h"

using zsLib::ULONG;
using zsLib::IMessageQueue;

ZS_DECLARE_CLASS_PTR(TestTimerCallback)

class TestTimerCallback : public zsLib::ITimerDelegate,
                          public zsLib::MessageQueueAssociator
{
private:
  TestTimerCallback(zsLib::IMessageQueuePtr queue) : zsLib::MessageQueueAssociator(queue), mCount(0)
  {
  }
public:
  static TestTimerCallbackPtr create(zsLib::IMessageQueuePtr queue)
  {
    return TestTimerCallbackPtr(new TestTimerCallback(queue));
  }

  virtual void onTimer(zsLib::TimerPtr timer)
  {
    ++mCount;
    std::cout << "ONTIMER:      " << ((zsLib::PTRNUMBER)timer.get()) << "\n";
  }

  ~TestTimerCallback()
  {
  }

public:
  ULONG mCount;
};


void testTimer()
{
  if (!ZSLIB_TEST_TIMER) return;

  zsLib::MessageQueueThreadPtr thread(zsLib::MessageQueueThread::createBasic());

  TestTimerCallbackPtr testObject = TestTimerCallback::create(thread);
  TestTimerCallbackPtr testObject2 = TestTimerCallback::create(thread);
  TestTimerCallbackPtr testObject3 = TestTimerCallback::create(thread);
  TestTimerCallbackPtr testObject4 = TestTimerCallback::create(thread);

  zsLib::TimerPtr timer1(zsLib::Timer::create(testObject, zsLib::Seconds(3)));
  zsLib::TimerPtr timer2(zsLib::Timer::create(testObject2, zsLib::Seconds(1), false));
  zsLib::TimerPtr timer3(zsLib::Timer::create(testObject3, zsLib::Seconds(4), false));
  zsLib::TimerPtr timer4(zsLib::Timer::create(testObject4, zsLib::Seconds(4), false));

  timer3.reset();         // this should cause the timer to be cancelled as if it fell out of scope before it has a chance to fire
  timer4->background();   // this should cause the timer to not be cancelled (but it will cancel itself after being fired)
  timer4.reset();

  TESTING_SLEEP(10000)
  timer1->cancel();

  TESTING_EQUAL(testObject->mCount, 3);
  TESTING_EQUAL(testObject2->mCount, 1);
  TESTING_EQUAL(testObject3->mCount, 0);
  TESTING_EQUAL(testObject4->mCount, 1);

  TESTING_STDOUT() << "WAITING:      To ensure the timers have truly stopped firing events.\n";
  TESTING_SLEEP(10000)

  timer1.reset();
  timer2.reset();
  timer3.reset();
  timer4.reset();

  TESTING_EQUAL(testObject->mCount, 3);
  TESTING_EQUAL(testObject2->mCount, 1);
  TESTING_EQUAL(testObject3->mCount, 0);
  TESTING_EQUAL(testObject4->mCount, 1);

  IMessageQueue::size_type count = 0;
  do
  {
    count = thread->getTotalUnprocessedMessages();
    if (0 != count)
      std::this_thread::yield();
  } while (count > 0);
  thread->waitForShutdown();

  TESTING_EQUAL(zsLib::proxyGetTotalConstructed(), 0);
}
