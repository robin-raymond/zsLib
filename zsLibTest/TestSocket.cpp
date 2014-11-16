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

#include <zsLib/Socket.h>
#include <zsLib/IPAddress.h>


#include "testing.h"
#include "main.h"

#define HlZeroStruct(xValue) memset(&(xValue), 0, sizeof(xValue))
#define HlZeroMemory(xValue, xSize) memset((xValue), 0, xSize)

#ifndef _WIN32
using zsLib::INVALID_SOCKET;
#endif //ndef _WIN32
using zsLib::BYTE;

class TestSocket
{
public:
  TestSocket()
  {
    if (!ZSLIB_TEST_SOCKET) return;
    
    srand(static_cast<signed int>(time(NULL)));
    zsLib::WORD port1 = (rand()%(65550-5000))+5000;
    zsLib::WORD port2 = (rand()%(65550-5000))+5000;
    if (port1 == port2) {
      port2 = port1 + 1;
    }

    {
      zsLib::SocketPtr socket = zsLib::Socket::createUDP();
      TESTING_CHECK(INVALID_SOCKET != socket->getSocket())
    }
    {
      zsLib::SocketPtr socket = zsLib::Socket::createTCP();
      TESTING_CHECK(INVALID_SOCKET != socket->getSocket())
    }
    {
      zsLib::SocketPtr socket = zsLib::Socket::create(zsLib::Socket::Create::IPv6, zsLib::Socket::Create::Datagram, zsLib::Socket::Create::UDP);
      TESTING_CHECK(INVALID_SOCKET != socket->getSocket())
    }
    {
      zsLib::SocketPtr socket = zsLib::Socket::create(zsLib::Socket::Create::IPv6, zsLib::Socket::Create::Datagram, zsLib::Socket::Create::UDP);
      TESTING_CHECK(INVALID_SOCKET != socket->getSocket())
    }
    {
      zsLib::IPAddress address1(zsLib::IPAddress::loopbackV4(), port1);
      zsLib::IPAddress address2(zsLib::IPAddress::loopbackV4(), port2);
      zsLib::SocketPtr socket1 = zsLib::Socket::createUDP();
      socket1->bind(address1);

      zsLib::SocketPtr socket2 = zsLib::Socket::createUDP();
      socket2->bind(address2);

      socket1->sendTo(address2, (BYTE *)"HELLO", sizeof("HELLO"));

      zsLib::IPAddress address3;
      BYTE buffer[1024];
      HlZeroMemory(&(buffer[0]), sizeof(buffer));
      size_t length = socket2->receiveFrom(address3, buffer, sizeof(buffer));
      TESTING_CHECK(sizeof("HELLO") == length)
      TESTING_CHECK(0 == memcmp(&(buffer[0]), "HELLO", sizeof("HELLO")))
      TESTING_CHECK(address3 == address1)

      zsLib::IPAddress getaddress1 = socket1->getLocalAddress();
      zsLib::IPAddress getaddress2 = socket2->getLocalAddress();
      TESTING_CHECK(address1 == getaddress1)
      TESTING_CHECK(address2 == getaddress2)
      socket1->connect(address2);
      socket2->connect(address1);
      zsLib::IPAddress remoteaddress1 = socket1->getRemoteAddress();
      zsLib::IPAddress remoteaddress2 = socket2->getRemoteAddress();
      TESTING_CHECK(address1 == remoteaddress2)
      TESTING_CHECK(address2 == remoteaddress1)

      socket1->send((BYTE *)"HELLO", sizeof("HELLO"));

      HlZeroMemory(&(buffer[0]), sizeof(buffer));
      length = socket2->receive(buffer, sizeof(buffer));
      TESTING_CHECK(sizeof("HELLO") == length)
      TESTING_CHECK(0 == memcmp(&(buffer[0]), "HELLO", sizeof("HELLO")))
    }
    {
      zsLib::IPAddress address1(zsLib::IPAddress::loopbackV6(), port1);
      zsLib::IPAddress address2(zsLib::IPAddress::loopbackV6(), port2);
      zsLib::SocketPtr socket1 = zsLib::Socket::createUDP(zsLib::Socket::Create::IPv6);
      socket1->bind(address1);

      zsLib::SocketPtr socket2 = zsLib::Socket::createUDP(zsLib::Socket::Create::IPv6);
      socket2->bind(address2);

      socket1->sendTo(address2, (BYTE *)"HELLO", sizeof("HELLO"));

      zsLib::IPAddress address3;
      BYTE buffer[1024];
      HlZeroMemory(&(buffer[0]), sizeof(buffer));
      size_t length = socket2->receiveFrom(address3, buffer, sizeof(buffer));
      TESTING_CHECK(sizeof("HELLO") == length)
      TESTING_CHECK(0 == memcmp(&(buffer[0]), "HELLO", sizeof("HELLO")))
      TESTING_CHECK(address3 == address1)

      zsLib::IPAddress getaddress1 = socket1->getLocalAddress();
      zsLib::IPAddress getaddress2 = socket2->getLocalAddress();
      TESTING_CHECK(address1 == getaddress1)
      TESTING_CHECK(address2 == getaddress2)
    }
    {
      zsLib::IPAddress address1(zsLib::IPAddress::loopbackV6(), port1);
      zsLib::IPAddress address2(zsLib::IPAddress::loopbackV6(), port2);
      zsLib::SocketPtr socket1 = zsLib::Socket::createTCP(zsLib::Socket::Create::IPv6);
      zsLib::SocketPtr socket2 = zsLib::Socket::createTCP(zsLib::Socket::Create::IPv6);

      socket1->bind(address1);
      socket1->listen();
      socket2->connect(address1);

      zsLib::IPAddress remoteIP;
      zsLib::SocketPtr socket3 = socket1->accept(remoteIP);
      TESTING_CHECK(socket3)
      TESTING_CHECK(address2.isAddressEqual(remoteIP))

      socket3->send((BYTE *)"HELLO", sizeof("HELLO"));

      BYTE buffer[1024];
      HlZeroMemory(&(buffer[0]), sizeof(buffer));
      size_t length = socket2->receive(buffer, sizeof(buffer));
      TESTING_CHECK(sizeof("HELLO") == length)
      TESTING_CHECK(0 == memcmp(&(buffer[0]), "HELLO", sizeof("HELLO")))

      zsLib::IPAddress getaddress1 = socket1->getLocalAddress();
      zsLib::IPAddress getaddress2 = socket2->getLocalAddress();
      zsLib::IPAddress getaddress3 = socket3->getLocalAddress();
      TESTING_CHECK(address1 == getaddress1)
      TESTING_CHECK(address2.isAddressEqual(getaddress2))
      TESTING_CHECK(remoteIP == getaddress2)

      zsLib::IPAddress remoteaddress1 = socket3->getRemoteAddress();
      zsLib::IPAddress remoteaddress2 = socket2->getRemoteAddress();
      TESTING_CHECK(address2.isAddressEqual(remoteaddress1))
      TESTING_CHECK(getaddress2 == remoteaddress1)
      TESTING_CHECK(getaddress3 == remoteaddress2)
      TESTING_CHECK(address1 == remoteaddress2)
    }

    {int i = 0; ++i;}
  }
};



TESTING_AUTO_TEST_SUITE(zsLibSocket)

  TESTING_AUTO_TEST_CASE(TestSocket)
  {
    TestSocket test;
  }

TESTING_AUTO_TEST_SUITE_END()
