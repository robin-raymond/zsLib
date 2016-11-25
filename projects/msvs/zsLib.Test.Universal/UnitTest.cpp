#include "pch.h"
#include "CppUnitTest.h"

#include <zsLibTest/testing.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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

namespace zsLib_Test_Universal
{
    TEST_CLASS(UnitTest_zsLib)
    {
    public:
        TEST_METHOD(Test_IPAddress)
        {
          Testing::setup();

          unsigned int totalFailures = Testing::getGlobalFailedVar();

          testIPAddress();

          if (totalFailures != Testing::getGlobalFailedVar()) {
            Assert::Fail(L"IP address tests have failed", LINE_INFO());
          }
        }

        TEST_METHOD(Test_Numeric)
        {
          Testing::setup();

          unsigned int totalFailures = Testing::getGlobalFailedVar();

          testNumeric();

          if (totalFailures != Testing::getGlobalFailedVar()) {
            Assert::Fail(L"Numeric tests have failed", LINE_INFO());
          }
        }

        TEST_METHOD(Test_Promise)
        {
          Testing::setup();

          unsigned int totalFailures = Testing::getGlobalFailedVar();

          testPromise();

          if (totalFailures != Testing::getGlobalFailedVar()) {
            Assert::Fail(L"Promise tests have failed", LINE_INFO());
          }
        }

        TEST_METHOD(Test_Proxy)
        {
          Testing::setup();

          unsigned int totalFailures = Testing::getGlobalFailedVar();

          testProxy();

          if (totalFailures != Testing::getGlobalFailedVar()) {
            Assert::Fail(L"Proxy tests have failed", LINE_INFO());
          }
        }

        TEST_METHOD(Test_Socket)
        {
          Testing::setup();

          unsigned int totalFailures = Testing::getGlobalFailedVar();

          testSocket();

          if (totalFailures != Testing::getGlobalFailedVar()) {
            Assert::Fail(L"Socket tests have failed", LINE_INFO());
          }
        }

        TEST_METHOD(Test_SocketAsync)
        {
          Testing::setup();

          unsigned int totalFailures = Testing::getGlobalFailedVar();

          testSocketAsync();

          if (totalFailures != Testing::getGlobalFailedVar()) {
            Assert::Fail(L"SocketAsync tests have failed", LINE_INFO());
          }
        }

        TEST_METHOD(Test_String)
        {
          Testing::setup();

          unsigned int totalFailures = Testing::getGlobalFailedVar();

          testString();

          if (totalFailures != Testing::getGlobalFailedVar()) {
            Assert::Fail(L"String tests have failed", LINE_INFO());
          }
        }

        TEST_METHOD(Test_Stringize)
        {
          Testing::setup();

          unsigned int totalFailures = Testing::getGlobalFailedVar();

          testStringize();

          if (totalFailures != Testing::getGlobalFailedVar()) {
            Assert::Fail(L"Stringize tests have failed", LINE_INFO());
          }
        }

        TEST_METHOD(Test_TearAway)
        {
          Testing::setup();

          unsigned int totalFailures = Testing::getGlobalFailedVar();

          testTearAway();

          if (totalFailures != Testing::getGlobalFailedVar()) {
            Assert::Fail(L"TearAway tests have failed", LINE_INFO());
          }
        }

        TEST_METHOD(Test_Timer)
        {
          Testing::setup();

          unsigned int totalFailures = Testing::getGlobalFailedVar();

          testTimer();

          if (totalFailures != Testing::getGlobalFailedVar()) {
            Assert::Fail(L"Timer tests have failed", LINE_INFO());
          }
        }

        TEST_METHOD(Test_XML)
        {
          Testing::setup();

          unsigned int totalFailures = Testing::getGlobalFailedVar();

          testXML();

          if (totalFailures != Testing::getGlobalFailedVar()) {
            Assert::Fail(L"XML tests have failed", LINE_INFO());
          }
        }

        TEST_METHOD(Test_ProxyUsingGUIThread)
        {
          Testing::setup();

          unsigned int totalFailures = Testing::getGlobalFailedVar();

          testProxyUsingGUIThread();

          if (totalFailures != Testing::getGlobalFailedVar()) {
            Assert::Fail(L"proxyUsingGUIThread tests have failed", LINE_INFO());
          }
        }
    };
}
