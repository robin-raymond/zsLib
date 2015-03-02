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

#include <zsLib/IPAddress.h>
#ifndef _WIN32
#include <arpa/inet.h>
#endif //_WIN32



#include "testing.h"
#include "main.h"

#define HlZeroStruct(xValue) memset(&(xValue), 0, sizeof(xValue))

using zsLib::BYTE;
using zsLib::DWORD;

class TestIPAddress
{
public:
  TestIPAddress()
  {
    testConstructors();
    testOtherMethods();
  }

  void testConstructors()
  {
    {
      zsLib::IPAddress ip;
      TESTING_CHECK(ip.isEmpty())
      TESTING_CHECK(ip.string() == "0.0.0.0")
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(ip.isIPv4Compatible())
      TESTING_CHECK(!ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(ip.isAddressEmpty())
      TESTING_CHECK(ip.isPortEmpty())

      zsLib::IPAddress ip2(ip);
      TESTING_CHECK(ip2.isEmpty())
      TESTING_CHECK(ip2.string() == "0.0.0.0")
      TESTING_CHECK(ip2.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(ip2.isIPv4Compatible())
      TESTING_CHECK(!ip2.isIPv4Mapped())
      TESTING_CHECK(!ip2.isIPv46to4())
      TESTING_CHECK(ip2.isAddressEmpty())
      TESTING_CHECK(ip2.isPortEmpty())

      zsLib::IPAddress ip3(ip, 5060);
      TESTING_CHECK(!ip3.isEmpty())
      TESTING_CHECK(5060 == ip3.getPort())
      TESTING_CHECK(ip3.string() == "0.0.0.0:5060")
      TESTING_CHECK(ip3.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(ip3.isAddressEmpty())
      TESTING_CHECK(!ip3.isPortEmpty())
    }
    {
      zsLib::IPAddress ipOriginal(192,168,1,17);
      zsLib::IPAddress ip(ipOriginal);
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(ip.isPortEmpty())
      TESTING_CHECK(ip.string() == "192.168.1.17")
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(ip == ipOriginal)
    }
    {
      zsLib::IPAddress ipOriginal(192,168,1,17, 5060);
      zsLib::IPAddress ip(ipOriginal);
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(ip.string() == "192.168.1.17:5060")
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(ip == ipOriginal)
    }
    {
      BYTE address[4] = {192,168,1,17};
      DWORD value = ntohl(*((DWORD *)&(address[0])));
      zsLib::IPAddress ipOriginal(value);
      zsLib::IPAddress ip(ipOriginal);
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(ip.isPortEmpty())
      TESTING_CHECK(ip.string() == "192.168.1.17")
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(ip == ipOriginal)
    }
    {
      BYTE address[4] = {192,168,1,17};
      DWORD value = ntohl(*((DWORD *)&(address[0])));
      zsLib::IPAddress ipOriginal(value, 5060);
      zsLib::IPAddress ip(ipOriginal);
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(ip.string() == "192.168.1.17:5060")
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(ip == ipOriginal)
    }
    {
      zsLib::IPv6PortPair ipRaw;
      memset(&(ipRaw), 0, sizeof(ipRaw));

      ipRaw.mIPAddress.dw[2] = htonl(0xFFFF);
      ipRaw.mIPAddress.by[12] = 192;
      ipRaw.mIPAddress.by[13] = 168;
      ipRaw.mIPAddress.by[14] = 1;
      ipRaw.mIPAddress.by[15] = 17;
      ipRaw.mPort = htons(5060);

      zsLib::IPAddress ipOriginal(ipRaw);
      zsLib::IPAddress ip(ipOriginal);
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(ip.string() == "192.168.1.17:5060")
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(ip == ipOriginal)
    }
    {
      sockaddr_in address;
      address.sin_family = AF_INET;
#ifdef _WIN32
#if !(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
      inet_pton(AF_INET, "192.168.1.17", &(address.sin_addr));
#else
      // argh - windows phone is missing this method for testing so putting in
      // values manually since this method is not the point of the test
      //address.sin_addr.s_addr = inet_addr("192.168.1.17");
      address.sin_addr.S_un.S_un_b.s_b1 = 192;
      address.sin_addr.S_un.S_un_b.s_b2 = 168;
      address.sin_addr.S_un.S_un_b.s_b3 = 1;
      address.sin_addr.S_un.S_un_b.s_b4 = 17;
#endif //!(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
#else
      inet_pton(AF_INET, "192.168.1.17", &(address.sin_addr));
#endif //_WIN32
      address.sin_port = htons(5060);

      zsLib::IPAddress ipOriginal(address);
      zsLib::IPAddress ip(ipOriginal);
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(ip.string() == "192.168.1.17:5060")
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(ip == ipOriginal)
    }
    {
      sockaddr_in6 address;
      address.sin6_family = AF_INET6;
#ifdef _WIN32
      address.sin6_addr.u.Word[0] = htons(0x1020);
      address.sin6_addr.u.Word[1] = htons(0xa1b1);
      address.sin6_addr.u.Word[2] = htons(0xc2d2);
      address.sin6_addr.u.Word[3] = htons(0xe3f3);
      address.sin6_addr.u.Word[4] = htons(0xa4b4);
      address.sin6_addr.u.Word[5] = htons(0xc5d5);
      address.sin6_addr.u.Word[6] = htons(0xe6f6);
      address.sin6_addr.u.Word[7] = htons(0xa7f7);
#elif defined __linux__
      address.sin6_addr.s6_addr[0] = htons(0x1020);
      address.sin6_addr.s6_addr[1] = htons(0xa1b1);
      address.sin6_addr.s6_addr[2] = htons(0xc2d2);
      address.sin6_addr.s6_addr[3] = htons(0xe3f3);
      address.sin6_addr.s6_addr[4] = htons(0xa4b4);
      address.sin6_addr.s6_addr[5] = htons(0xc5d5);
      address.sin6_addr.s6_addr[6] = htons(0xe6f6);
      address.sin6_addr.s6_addr[7] = htons(0xa7f7);
#else
      address.sin6_addr.__u6_addr.__u6_addr16[0] = htons(0x1020);
      address.sin6_addr.__u6_addr.__u6_addr16[1] = htons(0xa1b1);
      address.sin6_addr.__u6_addr.__u6_addr16[2] = htons(0xc2d2);
      address.sin6_addr.__u6_addr.__u6_addr16[3] = htons(0xe3f3);
      address.sin6_addr.__u6_addr.__u6_addr16[4] = htons(0xa4b4);
      address.sin6_addr.__u6_addr.__u6_addr16[5] = htons(0xc5d5);
      address.sin6_addr.__u6_addr.__u6_addr16[6] = htons(0xe6f6);
      address.sin6_addr.__u6_addr.__u6_addr16[7] = htons(0xa7f7);
#endif //_WIN32
      address.sin6_port = htons(5060);

      zsLib::IPAddress ipOriginal(address);
      zsLib::IPAddress ip(ipOriginal);
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(0 == (ip.string()).compareNoCase("[1020:a1b1:c2d2:e3f3:a4b4:c5d5:e6f6:a7f7]:5060"))
      TESTING_CHECK(!ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(ip == ipOriginal)
    }
    {
      zsLib::IPAddress ipOriginal(zsLib::String("192.168.1.17"), 5060);
      zsLib::IPAddress ip(ipOriginal);
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(ip.string() == "192.168.1.17:5060")
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(ip == ipOriginal)
    }
    {
      zsLib::IPAddress ipOriginal(zsLib::String("192.168.1.17:5060"));
      zsLib::IPAddress ip(ipOriginal);
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(ip.string() == "192.168.1.17:5060")
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(ip == ipOriginal)
    }
    {
      zsLib::IPAddress ipOriginal(zsLib::String("1020:a1b1:c2d2:e3f3:a4b4:c5d5:e6f6:a7f7"),5060);
      zsLib::IPAddress ip(ipOriginal);
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(0 == (ip.string()).compareNoCase("[1020:a1b1:c2d2:e3f3:a4b4:c5d5:e6f6:a7f7]:5060"))
      TESTING_CHECK(!ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(ip == ipOriginal)
    }
    {
      zsLib::IPAddress ipOriginal(zsLib::String("[1020:a1b1:c2d2:e3f3:a4b4:c5d5:e6f6:a7f7]:5060"));
      zsLib::IPAddress ip(ipOriginal);
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(0 == (ip.string()).compareNoCase("[1020:a1b1:c2d2:e3f3:a4b4:c5d5:e6f6:a7f7]:5060"))
      TESTING_CHECK(!ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(ip == ipOriginal)
    }
    {
      zsLib::IPAddress ipOriginal(zsLib::String("::ffff:192.168.1.17"),5060);
      zsLib::IPAddress ip(ipOriginal);
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(0 == (ip.string()).compareNoCase("192.168.1.17:5060"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(ip == ipOriginal)
    }
    {
      zsLib::IPAddress ipOriginal(zsLib::String("2002:c000:022a::"), 5060);
      zsLib::IPAddress ip(ipOriginal);
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(0 == (ip.string()).compareNoCase("192.0.2.42:5060"))
      TESTING_CHECK(0 == (ip.stringAsIPv6()).compareNoCase("[2002:c000:22a::]:5060"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(ip.isIPv46to4())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(ip == ipOriginal)
    }
    {
      zsLib::IPAddress ipOriginal(zsLib::String("[2002:c000:022a::]:5060"));
      zsLib::IPAddress ip(ipOriginal);
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(0 == (ip.string()).compareNoCase("192.0.2.42:5060"))
      TESTING_CHECK(0 == (ip.stringAsIPv6()).compareNoCase("[2002:c000:22a::]:5060"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(ip.isIPv46to4())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(ip == ipOriginal)
    }

    {int i = 0; ++i;}
  }

  void testOtherMethods()
  {
    {
      zsLib::IPAddress ip(zsLib::IPAddress::anyV4());
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(ip.isPortEmpty())
      TESTING_CHECK(ip.isAddrAny())
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(0 == (ip.string()).compareNoCase("0.0.0.0"))
    }
    {
      zsLib::IPAddress ip(zsLib::IPAddress::anyV6());
      TESTING_CHECK(ip.isEmpty())
      TESTING_CHECK(ip.isAddressEmpty())
      TESTING_CHECK(ip.isPortEmpty())
      TESTING_CHECK(ip.isAddrAny())
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(!ip.isIPv4Mapped())
      TESTING_CHECK(ip.isIPv4Compatible())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(0 == (ip.string()).compareNoCase("0.0.0.0"))
    }
    {
      zsLib::IPAddress ip(zsLib::IPAddress::loopbackV4());
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(ip.isPortEmpty())
      TESTING_CHECK(ip.isLoopback())
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(0 == (ip.string()).compareNoCase("127.0.0.1"))
    }
    {
      zsLib::IPAddress ip(zsLib::IPAddress::loopbackV6());
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(ip.isPortEmpty())
      TESTING_CHECK(ip.isLoopback())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(0 == (ip.string()).compareNoCase("::1"))
    }
    {
      zsLib::IPAddress ipDifferent("[2010:b1a1:d2c2:f3e3:b4a4:d5c5:f6e6:f7a7]:5060");
      TESTING_CHECK(0 == (ipDifferent.string()).compareNoCase("[2010:b1a1:d2c2:f3e3:b4a4:d5c5:f6e6:f7a7]:5060"))

      zsLib::IPAddress ipOriginal(zsLib::String("[1020:a1b1:c2d2:e3f3:a4b4:c5d5:e6f6:a7f7]:5060"));
      zsLib::IPAddress ip;
      ip = ipOriginal;
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(0 == (ip.string()).compareNoCase("[1020:a1b1:c2d2:e3f3:a4b4:c5d5:e6f6:a7f7]:5060"))
      TESTING_CHECK(!ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(ip == ipOriginal)
      TESTING_CHECK(ip != ipDifferent)
    }
    {
      zsLib::IPAddress ipDifferent(zsLib::String("192.168.1.17:5060"));
      ipDifferent.convertIPv46to4();
      TESTING_CHECK(!ipDifferent.isIPv4Compatible())
      TESTING_CHECK(!ipDifferent.isIPv4Mapped())
      TESTING_CHECK(ipDifferent.isIPv46to4())
      TESTING_CHECK(ipDifferent.string() == "192.168.1.17:5060")

      zsLib::IPv6PortPair ipRaw;
      memset(&(ipRaw), 0, sizeof(ipRaw));

      ipRaw.mIPAddress.dw[2] = htonl(0xFFFF);
      ipRaw.mIPAddress.by[12] = 192;
      ipRaw.mIPAddress.by[13] = 168;
      ipRaw.mIPAddress.by[14] = 1;
      ipRaw.mIPAddress.by[15] = 17;
      ipRaw.mPort = htons(5060);

      zsLib::IPAddress ipOriginal(ipRaw);
      zsLib::IPAddress ip;
      ip = ipOriginal;
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(ip.string() == "192.168.1.17:5060")
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(ip == ipOriginal)
      TESTING_CHECK(ip != ipDifferent)
      TESTING_CHECK(ip.isEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ip.isAddressEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ip.isAddressEqual(ipRaw))
      TESTING_CHECK(!ipDifferent.isAddressEqual(ipRaw))
      TESTING_CHECK(ipDifferent.isAddressEqualIgnoringIPv4Format(ipRaw))

      ipDifferent.setPort(5061);
      TESTING_CHECK(ipDifferent.string() == "192.168.1.17:5061")
      TESTING_CHECK(!ipDifferent.isIPv4Compatible())
      TESTING_CHECK(!ipDifferent.isIPv4Mapped())
      TESTING_CHECK(ipDifferent.isIPv46to4())
      TESTING_CHECK(!ip.isEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ip.isAddressEqualIgnoringIPv4Format(ipDifferent))
    }
    {
      zsLib::IPAddress ipDifferent(zsLib::String("[2010:b1a1:d2c2:f3e3:b4a4:d5c5:f6e6:f7a7]:5061"));
      TESTING_CHECK(0 == (ipDifferent.string()).compareNoCase("[2010:b1a1:d2c2:f3e3:b4a4:d5c5:f6e6:f7a7]:5061"))

      zsLib::IPAddress ipOriginal(zsLib::String("[1020:a1b1:c2d2:e3f3:a4b4:c5d5:e6f6:a7f7]:5060"));
      zsLib::IPAddress ip;
      ip = ipOriginal;
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(0 == (ip.string()).compareNoCase("[1020:a1b1:c2d2:e3f3:a4b4:c5d5:e6f6:a7f7]:5060"))
      TESTING_CHECK(!ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(ip == ipOriginal)
      TESTING_CHECK(ip != ipDifferent)
      TESTING_CHECK(!ip.isAddressEqual(ipDifferent))
      TESTING_CHECK(!ipDifferent.isEmpty())
      TESTING_CHECK(!ipDifferent.isAddressEmpty())
      TESTING_CHECK(!ipDifferent.isPortEmpty())

      ipDifferent = ipOriginal;
      TESTING_CHECK(0 == (ipDifferent.string()).compareNoCase("[1020:a1b1:c2d2:e3f3:a4b4:c5d5:e6f6:a7f7]:5060"))
      ipDifferent.setPort(5061);
      TESTING_CHECK(0 == (ipDifferent.string()).compareNoCase("[1020:a1b1:c2d2:e3f3:a4b4:c5d5:e6f6:a7f7]:5061"))
      TESTING_CHECK(ip != ipDifferent)
      TESTING_CHECK(ip.isAddressEqual(ipDifferent))

      ipDifferent.clear();
      TESTING_CHECK(0 == (ipDifferent.string()).compareNoCase("0.0.0.0"))
      TESTING_CHECK(0 == (ipDifferent.stringAsIPv6()).compareNoCase("::"))
      TESTING_CHECK(ipDifferent.isAddrAny())
      TESTING_CHECK(ipDifferent.isEmpty())
      TESTING_CHECK(ipDifferent.isAddressEmpty())
      TESTING_CHECK(ipDifferent.isPortEmpty())
      TESTING_CHECK(ipDifferent.isIPv4())
      TESTING_CHECK(ipDifferent.isIPv6())
      TESTING_CHECK(ip != ipDifferent)
      TESTING_CHECK(!ip.isAddressEqual(ipDifferent))

      ip.clear();
      TESTING_CHECK(ip == ipDifferent)
      TESTING_CHECK(ip.isAddressEqual(ipDifferent))
    }
    {
      zsLib::IPAddress ipDifferent("192.168.1.17:5060");
      ipDifferent.convertIPv46to4();
      TESTING_CHECK(!ipDifferent.isIPv4Compatible())
      TESTING_CHECK(!ipDifferent.isIPv4Mapped())
      TESTING_CHECK(ipDifferent.isIPv46to4())
      TESTING_CHECK(ipDifferent.string() == "192.168.1.17:5060")

      zsLib::IPv6PortPair ipRaw;
      memset(&(ipRaw), 0, sizeof(ipRaw));

      ipRaw.mIPAddress.dw[2] = htonl(0xFFFF);
      ipRaw.mIPAddress.by[12] = 192;
      ipRaw.mIPAddress.by[13] = 168;
      ipRaw.mIPAddress.by[14] = 1;
      ipRaw.mIPAddress.by[15] = 17;
      ipRaw.mPort = htons(5060);

      zsLib::IPAddress ipOriginal(ipRaw);
      zsLib::IPAddress ip;
      ip = ipOriginal;
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(ip.string() == "192.168.1.17:5060")
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(ip == ipOriginal)
      TESTING_CHECK(ip != ipDifferent)
      TESTING_CHECK(ip.isEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ip.isAddressEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ip.isAddressEqual(ipRaw))
      TESTING_CHECK(!ipDifferent.isAddressEqual(ipRaw))
      TESTING_CHECK(ipDifferent.isAddressEqualIgnoringIPv4Format(ipRaw))

      ipOriginal = ipDifferent;
      TESTING_CHECK(!ipDifferent.isIPv4Compatible())
      TESTING_CHECK(!ipDifferent.isIPv4Mapped())
      TESTING_CHECK(ipDifferent.isIPv46to4())
      TESTING_CHECK(ipOriginal == ipDifferent)

      ipDifferent.convertIPv4Mapped();
      TESTING_CHECK(ipDifferent.string() == "192.168.1.17:5060")
      TESTING_CHECK(!ipDifferent.isIPv4Compatible())
      TESTING_CHECK(ipDifferent.isIPv4Mapped())
      TESTING_CHECK(!ipDifferent.isIPv46to4())
      TESTING_CHECK(ip == ipDifferent)
      TESTING_CHECK(ip.isEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ip.isAddressEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ipDifferent.isAddressEqual(ipRaw))
      TESTING_CHECK(ipDifferent.isAddressEqualIgnoringIPv4Format(ipRaw))

      ipDifferent.convertIPv4Compatible();
      TESTING_CHECK(ipDifferent.string() == "192.168.1.17:5060")
      TESTING_CHECK(ipDifferent.isIPv4Compatible())
      TESTING_CHECK(!ipDifferent.isIPv4Mapped())
      TESTING_CHECK(!ipDifferent.isIPv46to4())
      TESTING_CHECK(ip != ipDifferent)
      TESTING_CHECK(ip.isEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ip.isAddressEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(!ipDifferent.isAddressEqual(ipRaw))
      TESTING_CHECK(ipDifferent.isAddressEqualIgnoringIPv4Format(ipRaw))

      ipDifferent.convertIPv46to4();
      TESTING_CHECK(ipDifferent.string() == "192.168.1.17:5060")
      TESTING_CHECK(!ipDifferent.isIPv4Compatible())
      TESTING_CHECK(!ipDifferent.isIPv4Mapped())
      TESTING_CHECK(ipDifferent.isIPv46to4())
      TESTING_CHECK(ipOriginal == ipDifferent)
      TESTING_CHECK(ip != ipDifferent)
      TESTING_CHECK(ip.isEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ip.isAddressEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(!ipDifferent.isAddressEqual(ipRaw))
      TESTING_CHECK(ipDifferent.isAddressEqualIgnoringIPv4Format(ipRaw))

      ipDifferent.convertIPv46to4();   // this should be a NOOP
      TESTING_CHECK(ipDifferent.string() == "192.168.1.17:5060")
      TESTING_CHECK(!ipDifferent.isIPv4Compatible())
      TESTING_CHECK(!ipDifferent.isIPv4Mapped())
      TESTING_CHECK(ipDifferent.isIPv46to4())
      TESTING_CHECK(ipOriginal == ipDifferent)
      TESTING_CHECK(ip != ipDifferent)
      TESTING_CHECK(ip.isEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ip.isAddressEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(!ipDifferent.isAddressEqual(ipRaw))
      TESTING_CHECK(ipDifferent.isAddressEqualIgnoringIPv4Format(ipRaw))
    }
    {
      zsLib::IPAddress ipDifferent("192.168.1.17:5060");
      ipDifferent.convertIPv46to4();
      constTest(ipDifferent);
    }
    {
      TESTING_CHECK(zsLib::IPAddress::anyV4().isAddrAny())
      TESTING_CHECK(zsLib::IPAddress::anyV6().isAddrAny())

      zsLib::IPAddress ip("0.0.0.0:5060");
      TESTING_CHECK(0 == ip.string().compareNoCase("0.0.0.0:5060"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(ip.isAddrAny())
      TESTING_CHECK(!ip.isLoopback())
      TESTING_CHECK(zsLib::IPAddress::anyV4() != ip)
      TESTING_CHECK(zsLib::IPAddress::anyV4().isAddressEqual(ip))

      ip.clear();
      TESTING_CHECK(0 == ip.string().compareNoCase("0.0.0.0"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(ip.isAddrAny())
      TESTING_CHECK(!ip.isLoopback())
      TESTING_CHECK(zsLib::IPAddress::anyV6() == ip)

      ip = zsLib::IPAddress("192.168.1.17");
      TESTING_CHECK(0 == ip.string().compareNoCase("192.168.1.17"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isAddrAny())
      TESTING_CHECK(!ip.isLoopback())
    }
    {
      TESTING_CHECK(zsLib::IPAddress::loopbackV4().isLoopback())
      TESTING_CHECK(zsLib::IPAddress::loopbackV6().isLoopback())

      zsLib::IPAddress ip("127.0.0.1:5060");
      TESTING_CHECK(0 == ip.string().compareNoCase("127.0.0.1:5060"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isAddrAny())
      TESTING_CHECK(ip.isLoopback())
      TESTING_CHECK(zsLib::IPAddress::loopbackV4() != ip)
      TESTING_CHECK(zsLib::IPAddress::loopbackV4().isAddressEqual(ip))

      ip = zsLib::IPAddress("::1");
      TESTING_CHECK(0 == ip.string().compareNoCase("::1"))
      TESTING_CHECK(!ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(!ip.isAddrAny())
      TESTING_CHECK(ip.isLoopback())
      TESTING_CHECK(zsLib::IPAddress::loopbackV6() == ip)

      ip = zsLib::IPAddress("192.168.1.17");
      TESTING_CHECK(0 == ip.string().compareNoCase("192.168.1.17"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isAddrAny())
      TESTING_CHECK(!ip.isLoopback())
    }
    {
      zsLib::IPAddress ip("127.0.0.1:5060");
      TESTING_CHECK(0 == ip.string().compareNoCase("127.0.0.1:5060"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(ip.isLoopback())

      ip = zsLib::IPAddress("::1");
      TESTING_CHECK(0 == ip.string().compareNoCase("::1"))
      TESTING_CHECK(!ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(ip.isLoopback())
    }
    {
      // test isLinkLocal, isPrivate

      zsLib::IPAddress ip("169.254.0.0:5060");
      TESTING_CHECK(0 == ip.string().compareNoCase("169.254.0.0:5060"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(ip.isLinkLocal())
      TESTING_CHECK(ip.isPrivate())

      sockaddr_in raw;
      ip.getIPv4(raw);
      TESTING_CHECK(raw.sin_family == AF_INET)
#ifdef _WIN32
      TESTING_CHECK(raw.sin_addr.S_un.S_addr == htonl(0xA9FE0000))
#else
      TESTING_CHECK(raw.sin_addr.s_addr == htonl(0xA9FE0000))
#endif //_WIN32
      TESTING_CHECK(raw.sin_port == htons(5060))

      ip = zsLib::IPAddress("192.168.1.1:5060");
      TESTING_CHECK(0 == ip.string().compareNoCase("192.168.1.1:5060"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isLinkLocal())
      TESTING_CHECK(ip.isPrivate())

      HlZeroStruct(raw);
      ip.getIPv4(raw);
      TESTING_CHECK(raw.sin_family == AF_INET)
#ifdef _WIN32
      TESTING_CHECK(raw.sin_addr.S_un.S_addr == htonl(0xC0A80101))
#else
      TESTING_CHECK(raw.sin_addr.s_addr == htonl(0xC0A80101))
#endif //_WIN32
      TESTING_CHECK(raw.sin_port == htons(5060))

      ip = zsLib::IPAddress("10.1.2.3:5060");
      TESTING_CHECK(0 == ip.string().compareNoCase("10.1.2.3:5060"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isLinkLocal())
      TESTING_CHECK(ip.isPrivate())

      HlZeroStruct(raw);
      ip.getIPv4(raw);
      TESTING_CHECK(raw.sin_family == AF_INET)
#ifdef _WIN32
      TESTING_CHECK(raw.sin_addr.S_un.S_addr == htonl(0x0A010203))
#else
      TESTING_CHECK(raw.sin_addr.s_addr == htonl(0x0A010203))
#endif //_WIN32
      TESTING_CHECK(raw.sin_port == htons(5060))

      // FE80::/10 or 11111110 10XXXXXX (link local)
      ip = zsLib::IPAddress("[fe80::abcd]:5060");
      TESTING_CHECK(0 == ip.string().compareNoCase("[fe80::abcd]:5060"))
      TESTING_CHECK(!ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(ip.isLinkLocal())
      TESTING_CHECK(ip.isPrivate())

      HlZeroStruct(raw);
      bool caught = false;
      try {ip.getIPv4(raw);} catch(zsLib::IPAddress::Exceptions::NotIPv4) {caught = true;}
      TESTING_CHECK(caught)

      sockaddr_in6 raw6;
      ip.getIPv6(raw6);
      TESTING_CHECK(raw6.sin6_family == AF_INET6)
#ifdef _WIN32
      TESTING_CHECK(raw6.sin6_addr.u.Word[0] == htons(0xfe80))
      TESTING_CHECK(raw6.sin6_addr.u.Word[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[7] == htons(0xabcd))
#elif defined __linux__
      TESTING_CHECK(raw6.sin6_addr.s6_addr[0] == htons(0xfe80))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[7] == htons(0xabcd))
#else
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[0] == htons(0xfe80))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[7] == htons(0xabcd))
#endif //WIN32
      TESTING_CHECK(raw6.sin6_port == htons(5060))

      // make bit pattern is FEC0::/10 or 11111110 11XXXXXX (NOT link local)
      ip = zsLib::IPAddress("fec0::abcd");
      TESTING_CHECK(0 == ip.string().compareNoCase("fec0::abcd"))
      TESTING_CHECK(!ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(!ip.isLinkLocal())
      TESTING_CHECK(!ip.isPrivate())

      HlZeroStruct(raw6);
      ip.getIPv6(raw6);
      TESTING_CHECK(raw6.sin6_family == AF_INET6)
#ifdef _WIN32
      TESTING_CHECK(raw6.sin6_addr.u.Word[0] == htons(0xfec0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[7] == htons(0xabcd))
#elif defined __linux__
      TESTING_CHECK(raw6.sin6_addr.s6_addr[0] == htons(0xfec0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[7] == htons(0xabcd))
#else
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[0] == htons(0xfec0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[7] == htons(0xabcd))
#endif //_WIN32
      TESTING_CHECK(raw6.sin6_port == htons(0))

      // bit pattern is FC00::/7 or 1111110X (private)
      ip = zsLib::IPAddress("fc00::abcd");
      TESTING_CHECK(0 == ip.string().compareNoCase("fc00::abcd"))
      TESTING_CHECK(!ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(!ip.isLinkLocal())
      TESTING_CHECK(ip.isPrivate())

      HlZeroStruct(raw6);
      ip.getIPv6(raw6);
      TESTING_CHECK(raw6.sin6_family == AF_INET6)
#ifdef _WIN32
      TESTING_CHECK(raw6.sin6_addr.u.Word[0] == htons(0xfc00))
      TESTING_CHECK(raw6.sin6_addr.u.Word[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[7] == htons(0xabcd))
#elif defined __linux__
      TESTING_CHECK(raw6.sin6_addr.s6_addr[0] == htons(0xfc00))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[7] == htons(0xabcd))
#else
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[0] == htons(0xfc00))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[7] == htons(0xabcd))
#endif //_WIN32
      TESTING_CHECK(raw6.sin6_port == htons(0))

      // make bit pattern 1111111X (not private)
      ip = zsLib::IPAddress("fe00::abcd");
      TESTING_CHECK(0 == ip.string().compareNoCase("fe00::abcd"))
      TESTING_CHECK(!ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(!ip.isLinkLocal())
      TESTING_CHECK(!ip.isPrivate())

      HlZeroStruct(raw6);
      ip.getIPv6(raw6);
      TESTING_CHECK(raw6.sin6_family == AF_INET6)
#ifdef _WIN32
      TESTING_CHECK(raw6.sin6_addr.u.Word[0] == htons(0xfe00))
      TESTING_CHECK(raw6.sin6_addr.u.Word[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[7] == htons(0xabcd))
#elif defined __linux__
      TESTING_CHECK(raw6.sin6_addr.s6_addr[0] == htons(0xfe00))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[7] == htons(0xabcd))
#else
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[0] == htons(0xfe00))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[7] == htons(0xabcd))
#endif //_WIN32
      TESTING_CHECK(raw6.sin6_port == htons(0))

      ip = zsLib::IPAddress::loopbackV4();
      TESTING_CHECK(0 == ip.string().compareNoCase("127.0.0.1"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(ip.isLoopback())
      TESTING_CHECK(!ip.isLinkLocal())
      TESTING_CHECK(!ip.isPrivate())

      HlZeroStruct(raw6);
      ip.getIPv6(raw6);
      TESTING_CHECK(raw6.sin6_family == AF_INET6)
#ifdef _WIN32
      TESTING_CHECK(raw6.sin6_addr.u.Word[0] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[5] == htons(0xFFFF))
      TESTING_CHECK(raw6.sin6_addr.u.Word[6] == htons(0x7F00))
      TESTING_CHECK(raw6.sin6_addr.u.Word[7] == htons(0x0001))
#elif defined __linux__
      TESTING_CHECK(raw6.sin6_addr.s6_addr[0] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[5] == htons(0xFFFF))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[6] == htons(0x7F00))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[7] == htons(0x0001))
#else
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[0] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[5] == htons(0xFFFF))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[6] == htons(0x7F00))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[7] == htons(0x0001))
#endif //_WIN32
      TESTING_CHECK(raw6.sin6_port == htons(0))

      ip = zsLib::IPAddress::loopbackV6();
      TESTING_CHECK(0 == ip.string().compareNoCase("::1"))
      TESTING_CHECK(!ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(!ip.isLinkLocal())
      TESTING_CHECK(!ip.isPrivate())

      HlZeroStruct(raw6);
      ip.getIPv6(raw6);
      TESTING_CHECK(raw6.sin6_family == AF_INET6)
#ifdef _WIN32
      TESTING_CHECK(raw6.sin6_addr.u.Word[0] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.u.Word[7] == htons(0x0001))
#elif defined __linux__
      TESTING_CHECK(raw6.sin6_addr.s6_addr[0] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.s6_addr[7] == htons(0x0001))
#else
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[0] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[1] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[2] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[3] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[4] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[5] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[6] == htons(0))
      TESTING_CHECK(raw6.sin6_addr.__u6_addr.__u6_addr16[7] == htons(0x0001))
#endif //_WIN32
      TESTING_CHECK(raw6.sin6_port == htons(0))

      ip = zsLib::IPAddress::anyV4();
      TESTING_CHECK(0 == ip.string().compareNoCase("0.0.0.0"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isLinkLocal())
      TESTING_CHECK(!ip.isPrivate())

      HlZeroStruct(raw);
      ip.getIPv4(raw);
      TESTING_CHECK(raw.sin_family == AF_INET)
#ifdef _WIN32
      TESTING_CHECK(raw.sin_addr.S_un.S_addr == htonl(0))
#else
      TESTING_CHECK(raw.sin_addr.s_addr == htonl(0))
#endif //_WIN32
      TESTING_CHECK(raw.sin_port == htons(0))

      ip = zsLib::IPAddress::anyV6();
      TESTING_CHECK(0 == ip.string().compareNoCase("0.0.0.0"))
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(ip.isIPv6())
      TESTING_CHECK(!ip.isLinkLocal())
      TESTING_CHECK(!ip.isPrivate())

      HlZeroStruct(raw);
      ip.getIPv4(raw);
      TESTING_CHECK(raw.sin_family == AF_INET)
#ifdef _WIN32
      TESTING_CHECK(raw.sin_addr.S_un.S_addr == htonl(0))
#else
      TESTING_CHECK(raw.sin_addr.s_addr == htonl(0))
#endif //_WIN32
      TESTING_CHECK(raw.sin_port == htons(0))
    }
    {int i = 0; ++i;}
  }

  void constTest(const zsLib::IPAddress &constIP)
  {
    {
      TESTING_CHECK(!constIP.isIPv4Compatible())
      TESTING_CHECK(!constIP.isIPv4Mapped())
      TESTING_CHECK(constIP.isIPv46to4())
      TESTING_CHECK(constIP.string() == "192.168.1.17:5060")
      TESTING_CHECK(!constIP.isEmpty())
      TESTING_CHECK(!constIP.isAddressEmpty())
      TESTING_CHECK(!constIP.isPortEmpty())
      TESTING_CHECK(!constIP.isAddrAny())
      TESTING_CHECK(!constIP.isLoopback())
      TESTING_CHECK(constIP.isIPv4())
      TESTING_CHECK(!constIP.isIPv6())
      TESTING_CHECK(!constIP.isLinkLocal())
      TESTING_CHECK(constIP.isPrivate())
      BYTE byteIP[4] = {192,168,1,17};
      DWORD value = htonl(constIP.getIPv4AddressAsDWORD());
      TESTING_CHECK(0 == memcmp(&value, &(byteIP[0]), sizeof(value)))
      TESTING_CHECK(5060 == constIP.getPort())
      sockaddr_in ipv4;
      constIP.getIPv4(ipv4);
      TESTING_CHECK(AF_INET == ipv4.sin_family)
#ifdef _WIN32
      TESTING_CHECK(0 == memcmp(&(ipv4.sin_addr.S_un.S_addr), &(byteIP[0]), sizeof(ipv4.sin_addr.S_un.S_addr)))
#else
      TESTING_CHECK(0 == memcmp(&(ipv4.sin_addr.s_addr), &(byteIP[0]), sizeof(ipv4.sin_addr.s_addr)))
#endif //_WIN32
      TESTING_CHECK(5060 == ntohs(ipv4.sin_port))

      sockaddr_in6 ipv6;
      constIP.getIPv6(ipv6);
      TESTING_CHECK(AF_INET6 == ipv6.sin6_family)
#ifdef _WIN32
      TESTING_CHECK(0x2002 == ntohs(ipv6.sin6_addr.u.Word[0]))
      TESTING_CHECK(0 == ipv6.sin6_addr.u.Word[3])
      TESTING_CHECK(0 == ipv6.sin6_addr.u.Word[4])
      TESTING_CHECK(0 == ipv6.sin6_addr.u.Word[5])
      TESTING_CHECK(0 == ipv6.sin6_addr.u.Word[6])
      TESTING_CHECK(0 == ipv6.sin6_addr.u.Word[7])
      TESTING_CHECK(0 == memcmp(&(ipv6.sin6_addr.u.Word[1]), &(byteIP[0]), sizeof(byteIP)))
#elif defined __linux__
      TESTING_CHECK(0x2002 == ntohs(ipv6.sin6_addr.s6_addr[0]))
      TESTING_CHECK(0 == ipv6.sin6_addr.s6_addr[3])
      TESTING_CHECK(0 == ipv6.sin6_addr.s6_addr[4])
      TESTING_CHECK(0 == ipv6.sin6_addr.s6_addr[5])
      TESTING_CHECK(0 == ipv6.sin6_addr.s6_addr[6])
      TESTING_CHECK(0 == ipv6.sin6_addr.s6_addr[7])
      TESTING_CHECK(0 == memcmp(&(ipv6.sin6_addr.s6_addr[1]), &(byteIP[0]), sizeof(byteIP)))
#else
      TESTING_CHECK(0x2002 == ntohs(ipv6.sin6_addr.__u6_addr.__u6_addr16[0]))
      TESTING_CHECK(0 == ipv6.sin6_addr.__u6_addr.__u6_addr16[3])
      TESTING_CHECK(0 == ipv6.sin6_addr.__u6_addr.__u6_addr16[4])
      TESTING_CHECK(0 == ipv6.sin6_addr.__u6_addr.__u6_addr16[5])
      TESTING_CHECK(0 == ipv6.sin6_addr.__u6_addr.__u6_addr16[6])
      TESTING_CHECK(0 == ipv6.sin6_addr.__u6_addr.__u6_addr16[7])
      TESTING_CHECK(0 == memcmp(&(ipv6.sin6_addr.__u6_addr.__u6_addr16[1]), &(byteIP[0]), sizeof(byteIP)))
#endif //_WIN32
      TESTING_CHECK(5060 == ntohs(ipv6.sin6_port))

      zsLib::IPAddress ipDifferent("192.168.1.17:5060");
      ipDifferent.convertIPv46to4();
      (void)(ipDifferent == constIP);
      TESTING_CHECK(!ipDifferent.isIPv4Compatible())
      TESTING_CHECK(!ipDifferent.isIPv4Mapped())
      TESTING_CHECK(ipDifferent.isIPv46to4())
      TESTING_CHECK(ipDifferent.string() == "192.168.1.17:5060")

      TESTING_CHECK(constIP == ipDifferent)
      TESTING_CHECK(!(constIP != ipDifferent))
      TESTING_CHECK(constIP.isAddressEqual(ipDifferent))
      TESTING_CHECK(constIP.isAddressEqual(ipDifferent))
      TESTING_CHECK(constIP.isEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(constIP.isAddressEqualIgnoringIPv4Format(ipDifferent))

      zsLib::IPv6PortPair ipRaw;
      memset(&(ipRaw), 0, sizeof(ipRaw));

      ipRaw.mIPAddress.dw[2] = htonl(0xFFFF);
      ipRaw.mIPAddress.by[12] = 192;
      ipRaw.mIPAddress.by[13] = 168;
      ipRaw.mIPAddress.by[14] = 1;
      ipRaw.mIPAddress.by[15] = 17;
      ipRaw.mPort = htons(5060);

      TESTING_CHECK(!(constIP == ipRaw))
      TESTING_CHECK(constIP != ipRaw)
      TESTING_CHECK(!constIP.isAddressEqual(ipRaw))
      TESTING_CHECK(constIP.isEqualIgnoringIPv4Format(ipRaw))
      TESTING_CHECK(constIP.isAddressEqualIgnoringIPv4Format(ipRaw))

      zsLib::IPAddress ipOriginal(ipRaw);
      zsLib::IPAddress ip;
      ip = ipOriginal;
      TESTING_CHECK(!ip.isEmpty())
      TESTING_CHECK(!ip.isPortEmpty())
      TESTING_CHECK(ip.string() == "192.168.1.17:5060")
      TESTING_CHECK(ip.isIPv4())
      TESTING_CHECK(!ip.isIPv6())
      TESTING_CHECK(!ip.isAddressEmpty())
      TESTING_CHECK(!ip.isIPv4Compatible())
      TESTING_CHECK(ip.isIPv4Mapped())
      TESTING_CHECK(!ip.isIPv46to4())
      TESTING_CHECK(ip == ipOriginal)
      TESTING_CHECK(ip != ipDifferent)
      TESTING_CHECK(ip.isEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ip.isAddressEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ip.isAddressEqual(ipRaw))
      TESTING_CHECK(!ipDifferent.isAddressEqual(ipRaw))
      TESTING_CHECK(ipDifferent.isAddressEqualIgnoringIPv4Format(ipRaw))

      ipOriginal = ipDifferent;
      TESTING_CHECK(!ipDifferent.isIPv4Compatible())
      TESTING_CHECK(!ipDifferent.isIPv4Mapped())
      TESTING_CHECK(ipDifferent.isIPv46to4())
      TESTING_CHECK(ipOriginal == ipDifferent)

      ipDifferent = constIP.convertIPv4Mapped();
      TESTING_CHECK(ipDifferent.string() == "192.168.1.17:5060")
      TESTING_CHECK(!ipDifferent.isIPv4Compatible())
      TESTING_CHECK(ipDifferent.isIPv4Mapped())
      TESTING_CHECK(!ipDifferent.isIPv46to4())
      TESTING_CHECK(ip == ipDifferent)
      TESTING_CHECK(ip.isEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ip.isAddressEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ipDifferent.isAddressEqual(ipRaw))
      TESTING_CHECK(ipDifferent.isAddressEqualIgnoringIPv4Format(ipRaw))

      ipDifferent = constIP.convertIPv4Compatible();
      TESTING_CHECK(ipDifferent.string() == "192.168.1.17:5060")
      TESTING_CHECK(ipDifferent.isIPv4Compatible())
      TESTING_CHECK(!ipDifferent.isIPv4Mapped())
      TESTING_CHECK(!ipDifferent.isIPv46to4())
      TESTING_CHECK(ip != ipDifferent)
      TESTING_CHECK(ip.isEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ip.isAddressEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(!ipDifferent.isAddressEqual(ipRaw))
      TESTING_CHECK(ipDifferent.isAddressEqualIgnoringIPv4Format(ipRaw))

      ipDifferent = constIP.convertIPv46to4();
      TESTING_CHECK(ipDifferent.string() == "192.168.1.17:5060")
      TESTING_CHECK(!ipDifferent.isIPv4Compatible())
      TESTING_CHECK(!ipDifferent.isIPv4Mapped())
      TESTING_CHECK(ipDifferent.isIPv46to4())
      TESTING_CHECK(ipOriginal == ipDifferent)
      TESTING_CHECK(ip != ipDifferent)
      TESTING_CHECK(ip.isEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(ip.isAddressEqualIgnoringIPv4Format(ipDifferent))
      TESTING_CHECK(!ipDifferent.isAddressEqual(ipRaw))
      TESTING_CHECK(ipDifferent.isAddressEqualIgnoringIPv4Format(ipRaw))
    }
    {int i = 0; ++i;}
  }
};


void testIPAddress()
{
  if (!ZSLIB_TEST_IP_ADDRESS) return;

  TestIPAddress test;
}
