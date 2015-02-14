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

#ifndef TESTING_AUTO_TEST_SUITE

#include <zsLib/types.h>
#include <iostream>
#include <sstream>

#ifdef __QNX__
#include <QDebug>
#endif //__QNX__

namespace Testing
{
  std::atomic_uint &getGlobalPassedVar();
  std::atomic_uint &getGlobalFailedVar();

  void passed();
  void failed();
  void output();

  void runAllTests();
}

#ifdef __QNX__
#define TESTING_STDOUT() (qDebug())
#else
#define TESTING_STDOUT() (std::cout)
#endif //__QNX___

#define TESTING_AUTO_TEST_SUITE(xParam) namespace xParam {

#define TESTING_AUTO_TEST_SUITE_END() }

#define TESTING_AUTO_TEST_CASE(xTestCase) static struct Test_##xTestCase {   \
    Test_##xTestCase()                                              \
    {                                                               \
      TESTING_STDOUT() << "STARTING:     " #xTestCase "\n";         \
      try                                                           \
      { test_func(); }                                              \
      catch(...)                                                    \
      { TESTING_STDOUT() << "***UNCAUGHT EXCEPTION IN***: " #xTestCase "\n"; Testing::failed(); }   \
      std::cout << "ENDING:       " #xTestCase "\n\n";              \
    }                                                               \
    void test_func();                                               \
  } g_Test_##xTestCase;                                             \
\
  void Test_##xTestCase::test_func()

#define TESTING_CHECK(xValue)                                         \
  {                                                                   \
    if (!(xValue))                                                    \
    { TESTING_STDOUT() << "***FAILED***: " #xValue "\n"; Testing::failed(); } \
    else                                                                             \
    { TESTING_STDOUT() << "PASSED:       " #xValue "\n"; Testing::passed(); } \
  }

#define TESTING_EQUAL(xValue1, xValue2)                             \
  if (!((xValue1) == (xValue2)))                                    \
  { std::stringstream sv1, sv2; sv1 << (xValue1); sv2 << (xValue2); TESTING_STDOUT() << "***FAILED***: " #xValue1 " == " #xValue2 ", V1=" << (sv1.str().c_str()) << ", V2=" << (sv2.str().c_str()) << "\n"; Testing::failed(); }                   \
  else                                                              \
  { std::stringstream sv1, sv2; sv1 << (xValue1); sv2 << (xValue2); TESTING_STDOUT() << "PASSED:       " #xValue1 " == " #xValue2 ", V1=" << (sv1.str().c_str()) << ", V2=" << (sv2.str().c_str()) << "\n"; Testing::passed(); }

#endif //TESTING_AUTO_TEST_SUITE
