//
//  zsLib_Test_iosTests.m
//  zsLib-Test-iosTests
//
//  Created by Robin Raymond on 2016-09-28.
//  Copyright © 2016 Robin Raymond. All rights reserved.
//

#import <XCTest/XCTest.h>

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

@interface zsLib_Test_iosTests : XCTestCase

@end

@implementation zsLib_Test_iosTests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)test_IPAddress {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
  unsigned int total = Testing::getGlobalFailedVar();

  Testing::setup();

  testIPAddress();

  XCTAssertEqual(total, (unsigned int)Testing::getGlobalFailedVar());
}

- (void)test_Numeric {
  unsigned int total = Testing::getGlobalFailedVar();

  Testing::setup();

  testNumeric();

  XCTAssertEqual(total, (unsigned int)Testing::getGlobalFailedVar());
}

- (void)test_Promise {
  unsigned int total = Testing::getGlobalFailedVar();

  Testing::setup();

  testPromise();

  XCTAssertEqual(total, (unsigned int)Testing::getGlobalFailedVar());
}

- (void)test_Proxy {
  unsigned int total = Testing::getGlobalFailedVar();

  Testing::setup();

  testProxy();

  XCTAssertEqual(total, (unsigned int)Testing::getGlobalFailedVar());
}

- (void)test_Socket {
  unsigned int total = Testing::getGlobalFailedVar();

  Testing::setup();

  testSocket();

  XCTAssertEqual(total, (unsigned int)Testing::getGlobalFailedVar());
}

- (void)test_SocketAsync {
  unsigned int total = Testing::getGlobalFailedVar();

  Testing::setup();

  testSocketAsync();

  XCTAssertEqual(total, (unsigned int)Testing::getGlobalFailedVar());
}

- (void)test_String {
  unsigned int total = Testing::getGlobalFailedVar();

  Testing::setup();

  testString();

  XCTAssertEqual(total, (unsigned int)Testing::getGlobalFailedVar());
}

- (void)test_Stringize {
  unsigned int total = Testing::getGlobalFailedVar();

  Testing::setup();

  testStringize();

  XCTAssertEqual(total, (unsigned int)Testing::getGlobalFailedVar());
}

- (void)test_TearAway {
  unsigned int total = Testing::getGlobalFailedVar();

  Testing::setup();

  testTearAway();

  XCTAssertEqual(total, (unsigned int)Testing::getGlobalFailedVar());
}

- (void)test_Timer {
  unsigned int total = Testing::getGlobalFailedVar();

  Testing::setup();

  testTimer();

  XCTAssertEqual(total, (unsigned int)Testing::getGlobalFailedVar());
}

- (void)test_XML {
  unsigned int total = Testing::getGlobalFailedVar();

  Testing::setup();

  testXML();

  XCTAssertEqual(total, (unsigned int)Testing::getGlobalFailedVar());
}

- (void)test_ProxyUsingGUIThread {
  unsigned int total = Testing::getGlobalFailedVar();

  Testing::setup();

  testProxyUsingGUIThread();

  XCTAssertEqual(total, (unsigned int)Testing::getGlobalFailedVar());
}


/*
- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}
 */

@end
