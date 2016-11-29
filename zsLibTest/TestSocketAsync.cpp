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
#include <zsLib/IMessageQueueThread.h>


#include "testing.h"
#include "main.h"

using zsLib::BYTE;
using zsLib::ULONG;
using zsLib::IMessageQueue;

namespace async_socket
{
  ZS_DECLARE_CLASS_PTR(SocketServer)

  class SocketServer : public zsLib::MessageQueueAssociator,
                       public zsLib::ISocketDelegate
  {
  private:
    SocketServer(zsLib::IMessageQueuePtr queue) : zsLib::MessageQueueAssociator(queue) { }

    void setup()
    {
      mReadReadyCalled = 0;
      mWriteReadyCalled = 0;
      mExceptionCalled = 0;
      mAddress = zsLib::IPAddress(zsLib::IPAddress::loopbackV4(), 43216);
      mSocket = zsLib::Socket::createUDP();
      mSocket->setOptionFlag(zsLib::Socket::SetOptionFlag::NonBlocking, true);
      mSocket->setDelegate(mThis.lock());
      mSocket->bind(mAddress);
    }

  public:
    static SocketServerPtr create(zsLib::IMessageQueuePtr queue)
    {
      SocketServerPtr object(new SocketServer(queue));
      object->mThis = object;
      object->setup();
      return object;
    }

    virtual void onReadReady(zsLib::SocketPtr socket)
    {
      TESTING_STDOUT() << "ON READ READY\n";
      ++mReadReadyCalled;

      zsLib::IPAddress address;
      BYTE buffer[1024];
      size_t total = socket->receiveFrom(
                                         address,
                                         buffer,
                                         sizeof(buffer)
                                         );
      mReadData.push_back((const char *)buffer);
      mReadAddresses.push_back(address);
      TESTING_STDOUT() << "READ " << total << " BYTES.\n";
    }

    virtual void onWriteReady(zsLib::SocketPtr socket)
    {
      TESTING_STDOUT() << "ON WRITE READY\n";
      ++mWriteReadyCalled;
    }

    virtual void onException(zsLib::SocketPtr socket)
    {
      TESTING_STDOUT() << "ONEXCEPTION\n";
      ++mExceptionCalled;
    }

    const zsLib::IPAddress &getAddress() {return mAddress;}

  public:
    ULONG mReadReadyCalled;
    ULONG mWriteReadyCalled;
    ULONG mExceptionCalled;
    std::vector<zsLib::IPAddress> mReadAddresses;
    std::vector<std::string> mReadData;

  private:
    SocketServerWeakPtr mThis;
    zsLib::SocketPtr mSocket;
    zsLib::IPAddress mAddress;
  };

  ZS_DECLARE_CLASS_PTR(ListenServer)

  class ListenServer : public zsLib::MessageQueueAssociator,
                       public zsLib::ISocketDelegate
  {
  private:
    ListenServer(zsLib::IMessageQueuePtr queue) : zsLib::MessageQueueAssociator(queue) { }

    void setup()
    {
      mReadReadyCalled = 0;
      mWriteReadyCalled = 0;
      mExceptionCalled = 0;
      mAddress = zsLib::IPAddress(zsLib::IPAddress(zsLib::IPAddress::loopbackV4(), 43218));
      mSocket = zsLib::Socket::createTCP();
      mSocket->setOptionFlag(zsLib::Socket::SetOptionFlag::NonBlocking, true);
      mSocket->setDelegate(mThis.lock());
      mSocket->bind(mAddress);
      mSocket->listen();
    }

  public:
    static ListenServerPtr create(zsLib::IMessageQueuePtr queue)
    {
      ListenServerPtr object(new ListenServer(queue));
      object->mThis = object;
      object->setup();
      return object;
    }

    virtual void onReadReady(zsLib::SocketPtr socket)
    {
      TESTING_STDOUT() << "ON READ READY\n";
      ++mReadReadyCalled;

      if (socket == mSocket) {
        zsLib::IPAddress address;
        mSocketIncoming = mSocket->accept(address);
        mSocketIncoming->setDelegate(mThis.lock());
        mReadAddresses.push_back(address);
        return;
      }

      BYTE buffer[1024];
      size_t total = socket->receive(
        buffer,
        sizeof(buffer)
        );
      mReadData.push_back((const char *)buffer);
      TESTING_STDOUT() << "READ " << total << " BYTES.\n";
    }

    virtual void onWriteReady(zsLib::SocketPtr socket)
    {
      TESTING_STDOUT() << "ON WRITE READY\n";
      ++mWriteReadyCalled;
    }

    virtual void onException(zsLib::SocketPtr socket)
    {
      TESTING_STDOUT() << "ONEXCEPTION\n";
      ++mExceptionCalled;
    }

    const zsLib::IPAddress &getAddress() { return mAddress; }

  public:
    ULONG mReadReadyCalled;
    ULONG mWriteReadyCalled;
    ULONG mExceptionCalled;
    std::vector<zsLib::IPAddress> mReadAddresses;
    std::vector<std::string> mReadData;

  private:
    ListenServerWeakPtr mThis;
    zsLib::SocketPtr mSocket;
    zsLib::SocketPtr mSocketIncoming;
    zsLib::IPAddress mAddress;
    zsLib::IPAddress mAddressIncoming;
  };

  class SocketTest
  {
  public:
    SocketTest()
    {
      srand(static_cast<signed int>(time(NULL)));
      testUDP();
      testTCP();
    }

    void testUDP()
    {
      srand(static_cast<signed int>(time(NULL)));
      zsLib::WORD port1 = (rand()%(65550-5000))+5000;

      zsLib::IMessageQueueThreadPtr thread(zsLib::IMessageQueueThread::createBasic());

      SocketServerPtr server(SocketServer::create(thread));

      TESTING_SLEEP(1000)
      zsLib::IPAddress address = zsLib::IPAddress(zsLib::IPAddress::loopbackV4(), port1);
      zsLib::SocketPtr socket = zsLib::Socket::createUDP();
      socket->bind(address);

      socket->sendTo(server->getAddress(), (BYTE *)"HELLO1", sizeof("HELLO1") + sizeof(char));
      TESTING_SLEEP(5000)

      socket->sendTo(server->getAddress(), (BYTE *)"HELLO2", sizeof("HELLO2") + sizeof(char));

      TESTING_SLEEP(10000)

      TESTING_EQUAL(2, server->mReadReadyCalled);
      TESTING_EQUAL(1, server->mWriteReadyCalled);
      TESTING_EQUAL(0, server->mExceptionCalled);
      TESTING_EQUAL(2, server->mReadData.size());
      TESTING_EQUAL(2, server->mReadAddresses.size());

      TESTING_EQUAL("HELLO1", server->mReadData[0]);
      TESTING_EQUAL("HELLO2", server->mReadData[1]);

      TESTING_CHECK(address == server->mReadAddresses[0]);
      TESTING_CHECK(address == server->mReadAddresses[1]);

      server.reset();

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

    void testTCP()
    {
      zsLib::WORD port1 = (rand() % (65550 - 5000)) + 5000;

      zsLib::IMessageQueueThreadPtr thread(zsLib::IMessageQueueThread::createBasic());

      ListenServerPtr server(ListenServer::create(thread));

      TESTING_SLEEP(1000)
      zsLib::IPAddress address = zsLib::IPAddress(zsLib::IPAddress::loopbackV4(), port1);
      zsLib::SocketPtr socket = zsLib::Socket::createTCP();
      socket->bind(address);
      socket->connect(server->getAddress());

      socket->send((BYTE *)"HELLO1", sizeof("HELLO1") + sizeof(char));
      TESTING_SLEEP(5000)

      socket->send((BYTE *)"HELLO2", sizeof("HELLO2") + sizeof(char));

      TESTING_SLEEP(10000)

      TESTING_EQUAL(3, server->mReadReadyCalled);
      TESTING_EQUAL(1, server->mWriteReadyCalled);
      TESTING_EQUAL(0, server->mExceptionCalled);
      TESTING_EQUAL(2, server->mReadData.size());
      TESTING_EQUAL(1, server->mReadAddresses.size());

      TESTING_EQUAL("HELLO1", server->mReadData[0]);
      TESTING_EQUAL("HELLO2", server->mReadData[1]);

      TESTING_CHECK(address == server->mReadAddresses[0]);

      server.reset();

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
  };
}

void testSocketAsync()
{
  if (!ZSLIB_TEST_SOCKET_ASYNC) return;

    async_socket::SocketTest test;
}
