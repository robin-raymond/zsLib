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

#include <iostream>

#include <zsLib/Singleton.h>
#include <zsLib/helpers.h>
#include "testing.h"

void testIPAddress();
void testNumeric();
void testPromise();
void testProxy();
void testProxyUsingGUIThread();
void testSocket();
void testSocketAsync();
void testString();
void testStringize();
void testTearAway();
void testTimer();
void testXML();

#ifdef _WIN32
debugostream &getDebugCout()
{
  static debugostream gdebug;
  return gdebug;
}
#endif //_WIN32

namespace Testing
{
  std::atomic_uint &getGlobalPassedVar()
  {
    static std::atomic_uint value {};
    return value;
  }

  std::atomic_uint &getGlobalFailedVar()
  {
    static std::atomic_uint value {};
    return value;
  }

  void passed()
  {
    ++(getGlobalPassedVar());
  }
  void failed()
  {
    ++(getGlobalFailedVar());
  }

  void output()
  {
    TESTING_STDOUT() << "PASSED:       [" << Testing::getGlobalPassedVar() << "]\n";
    if (0 != Testing::getGlobalFailedVar()) {
      TESTING_STDOUT() << "***FAILED***: [" << Testing::getGlobalFailedVar() << "]\n";
    }
  }

  class SetupInitializer
  {
  public:
    SetupInitializer()
    {
    }

    ~SetupInitializer()
    {
      output();
    }

  private:
    zsLib::SingletonManager::Initializer mInit;
  };

  static SetupInitializer &setupInitializer()
  {
    static SetupInitializer init;
    return init;
  }

  void setup()
  {
    setupInitializer();
  }

  void runAllTests()
  {
    setup();

    TESTING_RUN_TEST_CASE(testIPAddress)
    TESTING_RUN_TEST_CASE(testNumeric)
    TESTING_RUN_TEST_CASE(testPromise)
    TESTING_RUN_TEST_CASE(testProxy)
    TESTING_RUN_TEST_CASE(testSocket)
    TESTING_RUN_TEST_CASE(testSocketAsync)
    TESTING_RUN_TEST_CASE(testString)
    TESTING_RUN_TEST_CASE(testStringize)
    TESTING_RUN_TEST_CASE(testTearAway)
    TESTING_RUN_TEST_CASE(testTimer)
    TESTING_RUN_TEST_CASE(testXML)
    TESTING_RUN_TEST_CASE(testProxyUsingGUIThread)
  }
}
