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

#include <zsLib/internal/zsLib_SocketMonitor.h>

#ifndef ZSLIB_EVENTING_NOOP
#include <zsLib/internal/zsLib.events.h>
#else
#include <zsLib/eventing/noop.h>
#endif //ndef ZSLIB_EVENTING_NOOP

#include <zsLib/Socket.h>
#include <zsLib/IPAddress.h>
#include <zsLib/Stringize.h>

#include <zsLib/SafeInt.h>

#include <fcntl.h>
#include <signal.h>

#ifndef _WIN32
#include <unistd.h>
#endif //ndef _WIN32

#pragma warning(push)
#pragma warning(disable:4290)

namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib_socket)}

namespace zsLib
{
  ZS_EVENTING_TASK(Socket);
  ZS_EVENTING_TASK_OPCODE(Socket, Exception);
  ZS_EVENTING_TASK_OPCODE(Socket, Orphan);
  ZS_EVENTING_TASK_OPCODE(Socket, Adopt);
  ZS_EVENTING_TASK_OPCODE(Socket, Event);
  ZS_EVENTING_TASK_OPCODE(Socket, Bind);
  ZS_EVENTING_TASK_OPCODE(Socket, Listen);
  ZS_EVENTING_TASK_OPCODE(Socket, Accept);
  ZS_EVENTING_TASK_OPCODE(Socket, Connect);
  ZS_EVENTING_TASK_OPCODE(Socket, Shutdown);
  ZS_EVENTING_TASK_OPCODE(Socket, Option);

#ifndef _WIN32
  typedef linger LINGER;

  int WSAGetLastError()
  {
    return errno;
  }

  int closesocket(SOCKET socket)
  {
    return close(socket);
  }
#else
  typedef int socklen_t;
#endif //_WIN32

  namespace internal
  {
    //-----------------------------------------------------------------------
    static zsLib::Log::Params slog(const char *message)
    {
      return zsLib::Log::Params(message, "Socket");
    }

#if defined(__QNX__) || defined(__APPLE__)
    //-------------------------------------------------------------------------
    static pthread_once_t &getIgnoreSigTermKeyOnce()
    {
      static pthread_once_t ignoreSigTermKeyOnce = PTHREAD_ONCE_INIT;
      return ignoreSigTermKeyOnce;
    }

    //-------------------------------------------------------------------------
    static pthread_key_t &getIgnoreSigTermKey()
    {
      static pthread_key_t ignoreSigTermKey {};
      return ignoreSigTermKey;
    }

    //-------------------------------------------------------------------------
    static void ignoreSigTermKeyDestructor(void* value) {
      delete (bool*) value;
      pthread_setspecific(getIgnoreSigTermKey(), NULL);
    }

    //-------------------------------------------------------------------------
    static void makeIgnoreSigTermKeyOnce() {
      pthread_key_create(&(getIgnoreSigTermKey()), ignoreSigTermKeyDestructor);
    }
#endif //defined(__QNX__) || defined(__APPLE__)

    //-------------------------------------------------------------------------
    static void ignoreSigTermOnThread()
    {
#ifndef _WIN32

#if defined(__QNX__) || defined(__APPLE__)
      pthread_once(& (getIgnoreSigTermKeyOnce()), makeIgnoreSigTermKeyOnce);

      if (!pthread_getspecific(getIgnoreSigTermKey())) {
        pthread_setspecific(getIgnoreSigTermKey(), new bool {});

#else
      static thread_local bool alreadyIgnored {};

      if (!alreadyIgnored) {
        alreadyIgnored = true;

#endif //__QNX__
        struct sigaction act;
        memset(&act, 0, sizeof(act));

        act.sa_handler=SIG_IGN;
        sigemptyset(&act.sa_mask);
        act.sa_flags=0;
        sigaction(SIGPIPE, &act, NULL);
      }
#endif //ndef _WIN32
    }

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4297)

    class SocketInit
    {
    public:
      //-----------------------------------------------------------------------
      SocketInit()
      {
        WSADATA data;
        memset(&data, 0, sizeof(data));
        int result = WSAStartup(MAKEWORD(2, 2), &data);
        if (0 != result) {
          ZS_THROW_CUSTOM_PROPERTIES_1(zsLib::Socket::Exceptions::Unspecified, result, String("WSAStartup failed with error code: ") + string(result))
        }
      }

      //-----------------------------------------------------------------------
      ~SocketInit()
      {
        int result = WSACleanup();
        if (0 != result) {
          ZS_THROW_CUSTOM_PROPERTIES_1(zsLib::Socket::Exceptions::Unspecified, result, String("WSACleanup failed with error code: ") + string(result))
        }
      }
    };
#pragma warning(pop)
#else
    class SocketInit
    {
    public:
      SocketInit() {}
      ~SocketInit() {}
    };
#endif //_WIN32

    //-------------------------------------------------------------------------
    static void socketInit()
    {
      static SocketInit gInit;
    }

    //-------------------------------------------------------------------------
    static void prepareRawIPAddress(const IPAddress &inAddress, sockaddr_in &inIPv4, sockaddr_in6 &inIPv6, sockaddr * &outAddress, socklen_t &outSize)
    {
      outAddress = NULL;
      outSize = 0;

      if (inAddress.isIPv6()) {
        memset(&inIPv6, 0, sizeof(inIPv6));
        inAddress.getIPv6(inIPv6);
        outAddress = (sockaddr *)&inIPv6;
        outSize = sizeof(inIPv6);
      } else {
        memset(&inIPv4, 0, sizeof(inIPv4));
        inAddress.getIPv4(inIPv4);
        outAddress = (sockaddr *)&inIPv4;
        outSize = sizeof(inIPv4);
      }
    }

    //-------------------------------------------------------------------------
    void Socket::notifyReadReady()
    {
      SocketPtr socket;
      ISocketDelegatePtr delegate;
      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;
        socket = mThis.lock();
      }
      if (!delegate)
        return;

      ZS_EVENTING_1(x, i, Insane, SocketReadReadyEvent, zs, Socket, Event, this, this, this);

      delegate->onReadReady(socket);
    }

    //-------------------------------------------------------------------------
    void Socket::notifyWriteReady()
    {
      SocketPtr socket;
      ISocketDelegatePtr delegate;
      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;
        socket = mThis.lock();
      }
      if (!delegate)
        return;

      ZS_EVENTING_1(x, i, Insane, SocketWriteReadyEvent, zs, Socket, Event, this, this, this);

      delegate->onWriteReady(socket);
    }

    //-------------------------------------------------------------------------
    void Socket::notifyException()
    {
      SocketPtr socket;
      ISocketDelegatePtr delegate;
      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;
        socket = mThis.lock();
      }
      if (!delegate)
        return;

      ZS_EVENTING_1(x, e, Insane, SocketExceptionEvent, zs, Socket, Exception, this, this, this);

      delegate->onException(socket);
    }

    //-------------------------------------------------------------------------
    void Socket::linkSocketMonitor()
    {
      if (mMonitor) return;

      mMonitor = SocketMonitor::link();
    }

    //-------------------------------------------------------------------------
    void Socket::unlinkSocketMonitor()
    {
      if (mMonitor) {
        mMonitor->unlink();
        mMonitor.reset();
      }
      mMonitorReadReady = false;
      mMonitorWriteReady = false;
      mMonitorException = false;
    }
  }

  //---------------------------------------------------------------------------
  void Socket::ignoreSIGPIPEOnThisThread()
  {
    internal::ignoreSigTermOnThread();
  }

  //---------------------------------------------------------------------------
  SocketPtr Socket::create() throw(Exceptions::Unspecified)
  {
    SocketPtr object(new Socket);
    object->mThis = object;
    return object;
  }

  //---------------------------------------------------------------------------
  SocketPtr Socket::createUDP(Create::Family inFamily) throw(Exceptions::Unspecified)
  {
    return create(inFamily, Create::Datagram, Create::UDP);
  }

  //---------------------------------------------------------------------------
  SocketPtr Socket::createTCP(Create::Family inFamily) throw(Exceptions::Unspecified)
  {
    return create(inFamily, Create::Stream, Create::TCP);
  }

  //---------------------------------------------------------------------------
  SocketPtr Socket::create(Create::Family inFamily, Create::Type inType, Create::Protocol inProtocol) throw(Exceptions::Unspecified)
  {
    internal::socketInit();
    SOCKET socket = ::socket(inFamily, inType, inProtocol);

    SocketPtr object = create();
    object->mThis = object;

    ZS_EVENTING_5(x, i, Detail, SocketCreate, zs, Socket, Start, this, this, object.get(), socket, socket, static_cast<uint64_t>(socket), enum, family, to_underlying(inFamily), enum, type, to_underlying(inType), enum, protocol, to_underlying(inProtocol));

    if (INVALID_SOCKET == socket)
    {
      int error = WSAGetLastError();
      ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(socket), int, error, error);
      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Unspecified, error, "Could not create socket due to an unexpected error, where error=" + (string(error)))
    }

    ZS_LOG_TRACE(internal::slog("creating socket") + ZS_PARAM("socket", (PTRNUMBER)socket))

    object->adopt(socket);
    return object;
  }

  //---------------------------------------------------------------------------
  Socket::Socket() throw(Exceptions::Unspecified) :
    mSocket(INVALID_SOCKET)
  {
    linkSocketMonitor();

    internal::socketInit();
    internal::ignoreSigTermOnThread();
  }

  //---------------------------------------------------------------------------
  Socket::~Socket() throw(
                          Exceptions::WouldBlock,
                          Exceptions::Unspecified
                          )
  {
    try {
      close();
    } catch (...) {
      unlinkSocketMonitor();
      throw;
    }
    unlinkSocketMonitor();
  }

  //---------------------------------------------------------------------------
  bool Socket::isValid() const
  {
    AutoRecursiveLock lock(mLock);
    return INVALID_SOCKET != mSocket;
  }

  //---------------------------------------------------------------------------
  SOCKET Socket::getSocket() const
  {
    AutoRecursiveLock lock(mLock);
    return mSocket;
  }

  //---------------------------------------------------------------------------
  SOCKET Socket::orphan()
  {
    setDelegate();    // clear out the delegate

    AutoRecursiveLock lock(mLock);
    SOCKET temp = mSocket;
    mSocket = INVALID_SOCKET;

    ZS_EVENTING_1(x, i, Debug, SocketOrphan, zs, Socket, Orphan, socket, socket, static_cast<uint64_t>(temp));

    return temp;
  }

  //---------------------------------------------------------------------------
  void Socket::adopt(SOCKET inSocket)
  {
    close();

    AutoRecursiveLock lock(mLock);
    ZS_LOG_TRACE(internal::slog("adopting socket") + ZS_PARAM("socket", (PTRNUMBER)inSocket))
    mSocket = inSocket;

    ZS_EVENTING_1(x, i, Debug, SocketAdopt, zs, Socket, Adopt, socket, socket, static_cast<uint64_t>(mSocket));
  }

  //---------------------------------------------------------------------------
  void Socket::setDelegate(ISocketDelegatePtr originalDelegate) throw (Socket::Exceptions::InvalidSocket)
  {
    ISocketDelegatePtr delegate = ISocketDelegateProxy::createWeak(originalDelegate);

    if (delegate) {
      // only supported if using a proxy mechanism
      ZS_THROW_INVALID_USAGE_IF(!ISocketDelegateProxy::isProxy(delegate))
    }

    bool remove = false;

    {
      AutoRecursiveLock lock(mLock);
      if (mDelegate)
        remove = true;
      mDelegate.reset();
    }

    if (remove)
      mMonitor->monitorEnd(*this);

    bool monitorRead = true;
    bool monitorWrite = true;
    bool monitorException = true;

    {
      AutoRecursiveLock lock(mLock);
      if (INVALID_SOCKET == mSocket)
        return;

      mDelegate = delegate;
      monitorRead = mMonitorReadReady;
      monitorWrite = mMonitorWriteReady;
      monitorException = mMonitorException;
    }

    if (delegate)
      mMonitor->monitorBegin(mThis.lock(), monitorRead, monitorWrite, monitorException);
  }

  //---------------------------------------------------------------------------
  void Socket::monitor(Monitor::Options options)
  {
    ISocketDelegatePtr delegate;
    bool monitorRead = true;
    bool monitorWrite = true;
    bool monitorException = true;

    {
      AutoRecursiveLock lock(mLock);

      bool oldMonitorRead = mMonitorReadReady;
      bool oldMonitorWrite = mMonitorWriteReady;
      bool oldMonitorException = mMonitorException;

      mMonitorReadReady = (0 != (options & Monitor::Read));
      mMonitorWriteReady = (0 != (options & Monitor::Write));
      mMonitorException = (0 != (options & Monitor::Exception));

      if ((oldMonitorRead == mMonitorReadReady) &&
          (oldMonitorWrite == mMonitorWriteReady) &&
          (oldMonitorException == mMonitorException)) {
        return;
      }

      monitorRead = mMonitorReadReady;
      monitorWrite = mMonitorWriteReady;
      monitorException = mMonitorException;
      delegate = mDelegate;

      if (!mDelegate) return;
    }

    mMonitor->monitorEnd(*this);
    mMonitor->monitorBegin(mThis.lock(), monitorRead, monitorWrite, monitorException);
  }

  //---------------------------------------------------------------------------
  void Socket::close() throw(Exceptions::WouldBlock, Exceptions::Unspecified)
  {
    internal::ignoreSigTermOnThread();

    setDelegate();

    AutoRecursiveLock lock(mLock);
    if (INVALID_SOCKET == mSocket)
      return;

    ZS_LOG_TRACE(internal::slog("closing socket") + ZS_PARAM("socket", (PTRNUMBER)mSocket))

    int result = closesocket(mSocket);

    ZS_EVENTING_2(x, i, Debug, SocketClose, zs, Socket, Stop, socket, socket, static_cast<uint64_t>(mSocket), int, result, result);

    mSocket = INVALID_SOCKET;
    if (SOCKET_ERROR == result)
    {
      int error = WSAGetLastError();
      ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, error);
      switch (error)
      {
        case WSAEWOULDBLOCK: ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::WouldBlock, error, "closesocket on an asynchronous socket would block, where socket id=" + (string((PTRNUMBER)mSocket))); break;
        default:             ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Unspecified, error, "closesocket returned an unexpected error, where socket id=" + (string((PTRNUMBER)mSocket)) + ", error=" + (string(error)))
      }
    }
  }

  //---------------------------------------------------------------------------
  IPAddress Socket::getLocalAddress() const throw (Exceptions::InvalidSocket, Exceptions::Unspecified)
  {
    internal::ignoreSigTermOnThread();

    AutoRecursiveLock lock(mLock);
    ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())

    sockaddr_in6 address;
    memset(&address, 0, sizeof(address));
    socklen_t size = sizeof(address);
    int result = getsockname(
                             mSocket,
                             (sockaddr *)&address,
                             &size
                             );

    ZS_EVENTING_4(x, i, Debug, SocketGetLocalAddress, zs, Socket, Info, socket, socket, static_cast<uint64_t>(mSocket), int, result, result, buffer, address, &address, size, size, size);

    if (SOCKET_ERROR == result)
    {
      int error = WSAGetLastError();
      ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, error);
      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Unspecified, error, "getsockname returned an unexpected error, where socket id=" + (string((PTRNUMBER)mSocket)) + ", error=" + (string(error)))
    }

    IPAddress localIP;
    switch (size)
    {
      case sizeof(sockaddr_in):  localIP = IPAddress((sockaddr_in &)address); break;
      case sizeof(sockaddr_in6):
      default:                   localIP = IPAddress(address); break;
    }

    return localIP;
  }

  //---------------------------------------------------------------------------
  IPAddress Socket::getRemoteAddress() const throw (Exceptions::InvalidSocket, Exceptions::Unspecified)
  {
    internal::ignoreSigTermOnThread();

    AutoRecursiveLock lock(mLock);
    ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())

    sockaddr_in6 address;
    memset(&address, 0, sizeof(address));
    socklen_t size = sizeof(address);
    int result = getpeername(
                             mSocket,
                             (sockaddr *)&address,
                             &size
                             );
    ZS_EVENTING_4(x, i, Debug, SocketGetRemoteAddress, zs, Socket, Info, socket, socket, static_cast<uint64_t>(mSocket), int, result, result, buffer, address, &address, size, size, size);

    if (SOCKET_ERROR == result)
    {
      int error = WSAGetLastError();
      ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, error);
      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Unspecified, error, "getpeername returned an unexpected error, where socket id=" + (string((PTRNUMBER)mSocket)) + ", error=" + (string(error)))
    }

    IPAddress localIP;
    switch (size)
    {
      case sizeof(sockaddr_in):  localIP = IPAddress((sockaddr_in &)address); break;
      case sizeof(sockaddr_in6):
      default:                   localIP = IPAddress(address); break;
    }

    return localIP;
  }

  //---------------------------------------------------------------------------
  static int handleError(bool *outWouldBlock)
  {
    int error = WSAGetLastError();

    if (outWouldBlock) {
      *outWouldBlock = false;
      if ((WSAEWOULDBLOCK == error) ||
          (WSAEINPROGRESS == error)) {
        *outWouldBlock = true;
        return 0;
      }
    }
    
    return error;
  }

  //---------------------------------------------------------------------------
  static int handleError(
                         int error,
                         int *outNoThrowErrorResult
                         )
  {
    if (0 == error) {
      error = WSAGetLastError();
    }

    if (outNoThrowErrorResult) {
      *outNoThrowErrorResult = error;
      return 0;
    }
    
    return error;
  }

  //---------------------------------------------------------------------------
  void Socket::bind(
                    const IPAddress &inBindIP,
                    int *outNoThrowErrorResult
                    ) const throw(
                                  Exceptions::InvalidSocket,
                                  Exceptions::AddressInUse,
                                  Exceptions::Unspecified
                                  )
  {
    internal::ignoreSigTermOnThread();

    {
      AutoRecursiveLock lock(mLock);
      ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())

      sockaddr_in addressv4 {};
      sockaddr_in6 addressv6 {};
      sockaddr *address = NULL;
      socklen_t size = 0;
      internal::prepareRawIPAddress(inBindIP, addressv4, addressv6, address, size);
#ifdef _WIN32
//      if (0 != addressv6.sin6_scope_id) {
//        addressv6.sin6_scope_struct.Level = ScopeLevelLink;
//      }
#endif //_WIN32

      int result = ::bind(mSocket, address, size);
      ZS_EVENTING_4(x, i, Debug, SocketBind, zs, Socket, Info, socket, socket, static_cast<uint64_t>(mSocket), int, result, result, buffer, address, address, size, size, size);
      if (SOCKET_ERROR == result)
      {
        int error = handleError(0, outNoThrowErrorResult);
        ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, error);
        if (0 == error) return;

        switch (error)
        {
          case WSAEADDRINUSE: ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::AddressInUse, error, "Cannot bind socket as address is already in use, where socket id=" + (string((PTRNUMBER)mSocket)) + ", bind IP=" + inBindIP.string()); break;
          default:            ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Unspecified, error, "Unexpected error binding a socket, where socket id=" + (string((PTRNUMBER)mSocket)) + ", bind ip=" + inBindIP.string() + ", error=" + (string(error))); break;
        }
      }
    }

    if (mMonitorException)
      mMonitor->monitorException(*this);
  }

  //---------------------------------------------------------------------------
  void Socket::listen() const throw(
                                    Exceptions::InvalidSocket,
                                    Exceptions::AddressInUse,
                                    Exceptions::Unspecified
                                   )
  {
    internal::ignoreSigTermOnThread();

    {
      AutoRecursiveLock lock(mLock);
      ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())

      int result = ::listen(mSocket, SOMAXCONN);
      ZS_EVENTING_2(x, i, Detail, SocketListen, zs, Socket, Listen, socket, socket, static_cast<uint64_t>(mSocket), int, result, result);
      if (SOCKET_ERROR == result)
      {
        int error = WSAGetLastError();
        ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, error);
        switch (error)
        {
          case WSAEADDRINUSE: ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::AddressInUse, error, "Cannot listen on socket as address is already in use, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          default:            ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Unspecified, error, "Unexpected error listening on a socket, where socket id=" + (string((PTRNUMBER)mSocket)) + ", error=" + (string(error))); break;
        }
      }
    }

    if (mMonitorReadReady)
      mMonitor->monitorRead(*this);
    if (mMonitorWriteReady)
      mMonitor->monitorWrite(*this);
    if (mMonitorException)
      mMonitor->monitorException(*this);
  }

  //---------------------------------------------------------------------------
  SocketPtr Socket::accept(
                           IPAddress &outRemoteIP,
                           bool *outWouldBlock,
                           int *outNoThrowErrorResult
                           ) const throw(
                                         Exceptions::InvalidSocket,
                                         Exceptions::ConnectionReset,
                                         Exceptions::Unspecified
                                         )
  {
    internal::ignoreSigTermOnThread();

    SOCKET acceptSocket = INVALID_SOCKET;
    if (outNoThrowErrorResult)
      *outNoThrowErrorResult = 0;

    {
      AutoRecursiveLock lock(mLock);
      ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())

      sockaddr_in6 address;
      memset(&address, 0, sizeof(address));
      address.sin6_family = AF_INET6;
      socklen_t size = sizeof(address);
      acceptSocket = ::accept(mSocket, (sockaddr *)(&address), &size);
      ZS_EVENTING_4(x, i, Debug, SocketAccept, zs, Socket, Accept, socket, listenSocket, static_cast<uint64_t>(mSocket), socket, acceptSocket, static_cast<uint64_t>(acceptSocket), buffer, address, &address, size, size, size);
      if (INVALID_SOCKET == acceptSocket)
      {
        int error = handleError(outWouldBlock);
        ZS_EVENTING_2(x, i, Trace, SocketWouldBlock, zs, Socket, Info, socket, socket, static_cast<uint64_t>(mSocket), bool, wouldBlock, NULL == outWouldBlock ? false : *outWouldBlock);
        if (0 == error) return SocketPtr();

        error = handleError(error, outNoThrowErrorResult);
        ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, NULL == outNoThrowErrorResult ? error : *outNoThrowErrorResult);
        if (0 == error) return SocketPtr();

        switch (error)
        {
          case WSAECONNRESET: ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::ConnectionReset, error, "New connection was indicated but connection was reset before it could be accepted, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          default:            ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Unspecified, error, "Unexpected error accepting new connection, where socket id=" + (string((PTRNUMBER)mSocket)) + ", error=" + (string(error))); break;
        }
      }

      switch (size)
      {
        case sizeof(sockaddr_in):  outRemoteIP = IPAddress((sockaddr_in &)address); break;
        case sizeof(sockaddr_in6):
        default:                   outRemoteIP = IPAddress(address); break;
      }
    }

    if (mMonitorReadReady)
      mMonitor->monitorRead(*this);
    if (mMonitorWriteReady)
      mMonitor->monitorWrite(*this);
    if (mMonitorException)
      mMonitor->monitorException(*this);

    SocketPtr result = create();
    result->adopt(acceptSocket);
    return result;
  }

  //---------------------------------------------------------------------------
  void Socket::connect(
                       const IPAddress &inDestination,
                       bool *outWouldBlock,
                       int *outNoThrowErrorResult
                       ) const throw(
                                     Exceptions::InvalidSocket,
                                     Exceptions::WouldBlock,
                                     Exceptions::AddressInUse,
                                     Exceptions::NetworkNotReachable,
                                     Exceptions::HostNotReachable,
                                     Exceptions::Timeout,
                                     Exceptions::Unspecified
                                     )
  {
    internal::ignoreSigTermOnThread();

    {
      if (outNoThrowErrorResult)
        *outNoThrowErrorResult = 0;

      if (NULL != outWouldBlock)
        *outWouldBlock = false;

      AutoRecursiveLock lock(mLock);
      ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())

      sockaddr_in addressv4;
      sockaddr_in6 addressv6;
      sockaddr *address = NULL;
      socklen_t size = 0;
      internal::prepareRawIPAddress(inDestination, addressv4, addressv6, address, size);

      int result = ::connect(mSocket, address, size);
      ZS_EVENTING_3(x, i, Debug, SocketConnect, zs, Socket, Connect, socket, socket, static_cast<uint64_t>(mSocket), buffer, address, address, size, size, size);

      if (SOCKET_ERROR == result)
      {
        int error = handleError(outWouldBlock);
        ZS_EVENTING_2(x, i, Trace, SocketWouldBlock, zs, Socket, Info, socket, socket, static_cast<uint64_t>(mSocket), bool, wouldBlock, NULL == outWouldBlock ? false : *outWouldBlock);
        if (0 == error) goto connect_final;

        error = handleError(error, outNoThrowErrorResult);
        ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, NULL == outNoThrowErrorResult ? error : *outNoThrowErrorResult);
        if (0 == error) return;

        switch (error)
        {
          case WSAEINPROGRESS:
          case WSAEWOULDBLOCK:
          {
            ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::WouldBlock, error, "The connection will block, where socket id=" + (string((PTRNUMBER)mSocket)) + ", destination ip=" + inDestination.string());
            break;
          }
          case WSAEADDRINUSE:     ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::AddressInUse, error, "Cannot connect socket as address is already in use, where socket id" + (string((PTRNUMBER)mSocket)) + ", destination ip=" + inDestination.string()); break;
          case WSAECONNREFUSED:   ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::ConnectionRefused, error, "Cannot connect socket as connection was refused, where socket id=" + (string((PTRNUMBER)mSocket)) + ", destination ip=" + inDestination.string()); break;
          case WSAENETUNREACH:    ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::NetworkNotReachable, error, "Cannot connect socket as network was not able to reach destination, where socket id=" + (string((PTRNUMBER)mSocket)) + ", destination ip=" + inDestination.string()); break;
          case WSAEHOSTUNREACH:   ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::HostNotReachable, error, "Cannot connect socket as host was not reachable, where socket id=" + (string((PTRNUMBER)mSocket)) + ", destination ip=" + inDestination.string()); break;
          case WSAETIMEDOUT:      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Timeout, error, "Cannot connect socket as connection timed out, where socket id=" + (string((PTRNUMBER)mSocket)) + ", destination ip=" + inDestination.string()); break;
          default:                ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Unspecified, error, "Unexpected error connecting to destination, where socket id=" + (string((PTRNUMBER)mSocket)) + ", destination ip=" + inDestination.string() + ", error=" + (string(error))); break;
        }
      }
    }

  connect_final:

    if (mMonitorReadReady)
      mMonitor->monitorRead(*this);
    if (mMonitorWriteReady)
      mMonitor->monitorWrite(*this);
    if (mMonitorException)
      mMonitor->monitorException(*this);
  }

  //---------------------------------------------------------------------------
  size_t Socket::receive(
                         BYTE *ioBuffer,
                         size_t inBufferLengthInBytes,
                         bool *outWouldBlock,
                         ULONG inFlags,
                         int *outNoThrowErrorResult
                         ) const throw(
                                       Exceptions::InvalidSocket,
                                       Exceptions::WouldBlock,
                                       Exceptions::Shutdown,
                                       Exceptions::ConnectionReset,
                                       Exceptions::ConnectionAborted,
                                       Exceptions::Timeout,
                                       Exceptions::BufferTooSmall,
                                       Exceptions::Unspecified
                                       )
  {
    internal::ignoreSigTermOnThread();

    ssize_t result = 0;
    if (outNoThrowErrorResult)
      *outNoThrowErrorResult = 0;

    if (NULL != outWouldBlock)
      *outWouldBlock = false;

    // scope:
    {
      AutoRecursiveLock lock(mLock);
      ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())

      if (0 == inBufferLengthInBytes)
        return 0;

      ZS_THROW_INVALID_ARGUMENT_IF(NULL == ioBuffer)

      result = ::recv(
                      mSocket,
                      (char *)ioBuffer,
                      SafeInt<int>(inBufferLengthInBytes),
                      SafeInt<ULONG>(inFlags)
                      );

      ZS_EVENTING_5(x, i, Trace, SocketRecv, zs, Socket, Receive, socket, socket, static_cast<uint64_t>(mSocket), ssize_t, result, result, ulong, flags, inFlags, buffer, buffer, ioBuffer, size, size, inBufferLengthInBytes);

      if (SOCKET_ERROR == result)
      {
        result = 0;

        int error = handleError(outWouldBlock);
        ZS_EVENTING_2(x, i, Trace, SocketWouldBlock, zs, Socket, Info, socket, socket, static_cast<uint64_t>(mSocket), bool, wouldBlock, NULL == outWouldBlock ? false : *outWouldBlock);
        if (0 == error) goto receive_final;

        error = handleError(error, outNoThrowErrorResult);
        ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, NULL == outNoThrowErrorResult ? error : *outNoThrowErrorResult);
        if (0 == error) return 0;

        switch (error)
        {
          case WSAEINPROGRESS:
          case WSAEWOULDBLOCK:
          {
            ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::WouldBlock, error, "Cannot receive socket data as socket would block, where socket id=" + (string((PTRNUMBER)mSocket)));
            break;
          }
          case WSAESHUTDOWN:      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Shutdown, error, "Cannot receive socket data as connection was shutdown, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAECONNRESET:     ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::ConnectionReset, error, "Cannot receive socket data as socket was abruptly closed, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAECONNABORTED:   ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::ConnectionAborted, error, "Cannot receive socket data as socket connection was aborted, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAENETRESET:      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Timeout, error, "Cannot receive socket data as connection keep alive timed out, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAETIMEDOUT:      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Timeout, error, "Cannot receive socket data as socket timed out, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAEMSGSIZE:       ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::BufferTooSmall, error, "Cannot receive socket data as buffer provided was too small, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          default:                ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Unspecified, error, "The receive unexpectedly closed, where socket id=" + (string((PTRNUMBER)mSocket)) + ", error=" + (string(error))); break;
        }
      }
    }

  receive_final:

    if (mMonitorReadReady)
      mMonitor->monitorRead(*this);

    return static_cast<size_t>(result);
  }

  //---------------------------------------------------------------------------
  size_t Socket::receiveFrom(
                             IPAddress &outRemoteIP,
                             BYTE *ioBuffer,
                             size_t inBufferLengthInBytes,
                             bool *outWouldBlock,
                             ULONG inFlags,
                             int *outNoThrowErrorResult
                             ) const throw(
                                           Exceptions::InvalidSocket,
                                           Exceptions::WouldBlock,
                                           Exceptions::Shutdown,
                                           Exceptions::ConnectionReset,
                                           Exceptions::Timeout,
                                           Exceptions::BufferTooSmall,
                                           Exceptions::Unspecified
                                           )
  {
    internal::ignoreSigTermOnThread();

    ssize_t result = 0;
    if (outNoThrowErrorResult)
      *outNoThrowErrorResult = 0;

    if (NULL != outWouldBlock)
      *outWouldBlock = false;

    // scope:
    {
      AutoRecursiveLock lock(mLock);
      ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())

      sockaddr_in6 address;
      memset(&address, 0, sizeof(address));
      address.sin6_family = AF_INET6;
      socklen_t size = sizeof(address);
      result = recvfrom(
                        mSocket,
                        (char *)ioBuffer,
                        SafeInt<int>(inBufferLengthInBytes),
                        SafeInt<int>(inFlags),
                        (sockaddr *)&address,
                        &size
                        );

      ZS_EVENTING_7(x, i, Trace, SocketRecvFrom, zs, Socket, Receive, socket, socket, static_cast<uint64_t>(mSocket), ssize_t, result, result, ulong, flags, inFlags, buffer, buffer, ioBuffer, size, size, inBufferLengthInBytes, binary, address, &address, size, addressSize, size);

      if (SOCKET_ERROR == result)
      {
        result = 0;

        int error = handleError(outWouldBlock);
        ZS_EVENTING_2(x, i, Trace, SocketWouldBlock, zs, Socket, Info, socket, socket, static_cast<uint64_t>(mSocket), bool, wouldBlock, NULL == outWouldBlock ? false : *outWouldBlock);
        if (0 == error) goto recvfrom_final;

        error = handleError(error, outNoThrowErrorResult);
        ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, NULL == outNoThrowErrorResult ? error : *outNoThrowErrorResult);
        if (0 == error) return 0;

        switch (error)
        {
          case WSAEINPROGRESS:
          case WSAEWOULDBLOCK:
          {
            ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::WouldBlock, error, "Cannot receive socket data as socket would block, where socket id=" + (string((PTRNUMBER)mSocket)));
            break;
          }
          case WSAESHUTDOWN:      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Shutdown, error, "Cannot receive socket data as connection was shutdown, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAECONNRESET:     ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::ConnectionReset, error, "Cannot receive socket data as socket was abruptly closed, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAENETRESET:      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Timeout, error, "Cannot receive socket data as connection keep alive timed out, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAETIMEDOUT:      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Timeout, error, "Cannot receive socket data as socket timed out, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAEMSGSIZE:       ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::BufferTooSmall, error, "Cannot receive socket data as buffer provided was too small, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          default:                ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Unspecified, error, "The receive unexpectedly closed, where socket id=" + (string((PTRNUMBER)mSocket)) + ", error=" + (string(error))); break;
        }
      } else {
        switch (size)
        {
          case sizeof(sockaddr_in):  outRemoteIP = IPAddress((sockaddr_in &)address); break;
          case sizeof(sockaddr_in6):
          default:                   outRemoteIP = IPAddress(address); break;
        }
      }
    }
  recvfrom_final:

    if (mMonitorReadReady)
      mMonitor->monitorRead(*this);

    return static_cast<size_t>(result);
  }

  //---------------------------------------------------------------------------
  size_t Socket::send(
                      const BYTE *inBuffer,
                      size_t inBufferLengthInBytes,
                      bool *outWouldBlock,
                      ULONG inFlags,
                      int *outNoThrowErrorResult
                      ) const throw(
                                    Exceptions::InvalidSocket,
                                    Exceptions::WouldBlock,
                                    Exceptions::Shutdown,
                                    Exceptions::HostNotReachable,
                                    Exceptions::ConnectionAborted,
                                    Exceptions::ConnectionReset,
                                    Exceptions::Timeout,
                                    Exceptions::BufferTooSmall,
                                    Exceptions::Unspecified
                                    )
  {
    internal::ignoreSigTermOnThread();

    ssize_t result = 0;
    if (outNoThrowErrorResult)
      *outNoThrowErrorResult = 0;

    if (NULL != outWouldBlock)
      *outWouldBlock = false;

    // scope:
    {
      AutoRecursiveLock lock(mLock);
      ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())

      result = ::send(
                      mSocket,
                      (const char *)inBuffer,
                      SafeInt<int>(inBufferLengthInBytes),
                      static_cast<int>(inFlags)
                      );

      ZS_EVENTING_5(x, i, Trace, SocketSend, zs, Socket, Send, socket, socket, static_cast<uint64_t>(mSocket), ssize_t, result, result, ulong, flags, inFlags, buffer, buffer, inBuffer, size, size, inBufferLengthInBytes);

      if (SOCKET_ERROR == result)
      {
        result = 0;

        int error = handleError(outWouldBlock);
        ZS_EVENTING_2(x, i, Trace, SocketWouldBlock, zs, Socket, Info, socket, socket, static_cast<uint64_t>(mSocket), bool, wouldBlock, NULL == outWouldBlock ? false : *outWouldBlock);
        if (0 == error) goto send_final;

        error = handleError(error, outNoThrowErrorResult);
        ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, NULL == outNoThrowErrorResult ? error : *outNoThrowErrorResult);
        if (0 == error) return 0;

        switch (error)
        {
          case WSAEINPROGRESS:
          case WSAEWOULDBLOCK:
          {
            ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::WouldBlock, error, "Cannot send socket data as socket would block, where socket id=" + (string((PTRNUMBER)mSocket)));
            break;
          }
          case WSAENETRESET:      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Timeout, error, "Cannot send socket data as connection keep alive timed out, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAESHUTDOWN:      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Shutdown, error, "Cannot send socket data as connection was shutdown, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAEHOSTUNREACH:   ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::HostNotReachable, error, "Cannot send socket data as host is unreachable at this time, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAECONNABORTED:   ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::ConnectionAborted, error, "Cannot send socket data as socket connection was aborted, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAECONNRESET:     ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::ConnectionReset, error, "Cannot send socket data as socket was abruptly closed, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAETIMEDOUT:      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Timeout, error, "Cannot send socket data as socket timed out, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAEMSGSIZE:       ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::BufferTooSmall, error, "Cannot send socket data as buffer provided was too big, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          default:                ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Unspecified, error, "The send unexpectedly closed, where socket id=" + (string((PTRNUMBER)mSocket)) + ", error=" + (string(error))); break;
        }
      }
    }
  send_final:

    if (mMonitorWriteReady)
      mMonitor->monitorWrite(*this);

    return static_cast<size_t>(result);
  }

  //---------------------------------------------------------------------------
  size_t Socket::sendTo(
                        const IPAddress &inDestination,
                        const BYTE *inBuffer,
                        size_t inBufferLengthInBytes,
                        bool *outWouldBlock,
                        ULONG inFlags,
                        int *outNoThrowErrorResult
                        ) const throw(
                                      Exceptions::InvalidSocket,
                                      Exceptions::WouldBlock,
                                      Exceptions::Shutdown,
                                      Exceptions::Timeout,
                                      Exceptions::HostNotReachable,
                                      Exceptions::ConnectionAborted,
                                      Exceptions::ConnectionReset,
                                      Exceptions::Timeout,
                                      Exceptions::BufferTooSmall,
                                      Exceptions::Unspecified
                                      )
  {
    internal::ignoreSigTermOnThread();

    ssize_t result = 0;
    if (outNoThrowErrorResult)
      *outNoThrowErrorResult = 0;

    if (NULL != outWouldBlock)
      *outWouldBlock = false;

    // scope:
    {
      AutoRecursiveLock lock(mLock);
      ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())

      sockaddr_in addressv4;
      sockaddr_in6 addressv6;
      sockaddr *address = NULL;
      socklen_t size = 0;
      internal::prepareRawIPAddress(inDestination, addressv4, addressv6, address, size);

      result = ::sendto(
                        mSocket,
                        (const char *)inBuffer,
                        SafeInt<int>(inBufferLengthInBytes),
                        static_cast<int>(inFlags),
                        address,
                        SafeInt<int>(size)
                        );

      ZS_EVENTING_7(x, i, Trace, SocketSendTo, zs, Socket, Send, socket, socket, static_cast<uint64_t>(mSocket), ssize_t, result, result, ulong, flags, inFlags, buffer, buffer, inBuffer, size, size, inBufferLengthInBytes, binary, address, address, size, addressSize, size);

      if (SOCKET_ERROR == result)
      {
        result = 0;

        int error = handleError(outWouldBlock);
        ZS_EVENTING_2(x, i, Trace, SocketWouldBlock, zs, Socket, Info, socket, socket, static_cast<uint64_t>(mSocket), bool, wouldBlock, NULL == outWouldBlock ? false : *outWouldBlock);
        if (0 == error) goto sendto_final;

        error = handleError(error, outNoThrowErrorResult);
        ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, NULL == outNoThrowErrorResult ? error : *outNoThrowErrorResult);
        if (0 == error) return 0;

        switch (error)
        {
          case WSAEINPROGRESS:
          case WSAEWOULDBLOCK:
          {
            ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::WouldBlock, error, "Cannot send socket data as socket would block, where socket id=" + (string((PTRNUMBER)mSocket)));
            break;
          }
          case WSAENETRESET:      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Timeout, error, "Cannot send socket data as connection keep alive timed out, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAESHUTDOWN:      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Shutdown, error, "Cannot send socket data as connection was shutdown, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAEHOSTUNREACH:   ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::HostNotReachable, error, "Cannot send socket data as host is unreachable at this time, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAECONNABORTED:   ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::ConnectionAborted, error, "Cannot send socket data as socket connection was aborted, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAECONNRESET:     ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::ConnectionReset, error, "Cannot send socket data as socket was abruptly closed, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAETIMEDOUT:      ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Timeout, error, "Cannot send socket data as socket timed out, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          case WSAEMSGSIZE:       ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::BufferTooSmall, error, "Cannot send socket data as buffer provided was too big, where socket id=" + (string((PTRNUMBER)mSocket))); break;
          default:                ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Unspecified, error, "The send unexpectedly closed, where socket id=" + (string((PTRNUMBER)mSocket)) + ", error=" + (string(error))); break;
        }
      }
    }

  sendto_final:

    if (mMonitorWriteReady)
      mMonitor->monitorWrite(*this);

    return static_cast<size_t>(result);
  }

  //---------------------------------------------------------------------------
  void Socket::shutdown(Shutdown::Options inOptions) const throw(Exceptions::InvalidSocket, Exceptions::Unspecified)
  {
    internal::ignoreSigTermOnThread();

    {
      AutoRecursiveLock lock(mLock);
      ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())

      int result = ::shutdown(mSocket, inOptions);
      ZS_EVENTING_3(x, i, Debug, SocketShutdown, zs, Socket, Shutdown, socket, socket, static_cast<uint64_t>(mSocket), int, result, result, enum, options, to_underlying(inOptions));

      if (SOCKET_ERROR == result)
      {
        int error = WSAGetLastError();
        ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, error);
        ZS_THROW_CUSTOM_PROPERTIES_1(Exceptions::Unspecified, error, "Failed to perform a shutdown on the socket, where socket id=" + (string((PTRNUMBER)mSocket)) + ", error=" + (string(error)))
      }
    }

    if (mMonitorReadReady)
      mMonitor->monitorRead(*this);
    if (mMonitorWriteReady)
      mMonitor->monitorWrite(*this);
    if (mMonitorException)
      mMonitor->monitorException(*this);
  }

  namespace internal
  {
    //-------------------------------------------------------------------------
    static void setSocketOptions(
                                 SOCKET inSocket,
                                 int inLevel,
                                 int inOptionName,
                                 BYTE *inOptionValue,
                                 socklen_t inOptionLength
                                 )
    {
      int result = ::setsockopt(inSocket, inLevel, inOptionName, (const char *)inOptionValue, inOptionLength);
      ZS_EVENTING_6(x, i, Debug, SocketSetOption, zs, Socket, Option, socket, socket, static_cast<uint64_t>(inSocket), int, result, result, int, level, inLevel, int, optionName, inOptionName, binary, optionValue, inOptionValue, size, optionValueSize, inOptionLength);

      if (SOCKET_ERROR == result)
      {
        int error = WSAGetLastError();
        ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(inSocket), int, error, error);
        ZS_THROW_CUSTOM_PROPERTIES_1(zsLib::Socket::Exceptions::Unspecified, error, "setsockopt failed, where socket id=" + (string((PTRNUMBER)inSocket)) + ", socket option=" + (string(inOptionName)) + ", error=" + (string(error)))
      }
    }

    //-------------------------------------------------------------------------
    static void getSocketOptions(
                                 SOCKET inSocket,
                                 int inLevel,
                                 int inOptionName,
                                 BYTE *outOptionValue,
                                 socklen_t inOptionLength
                                 )
    {
      socklen_t length = inOptionLength;
      int result = ::getsockopt(inSocket, inLevel, inOptionName, (char *)outOptionValue, &length);
      ZS_EVENTING_6(x, i, Debug, SocketGetOptions, zs, Socket, Option, socket, socket, static_cast<uint64_t>(inSocket), int, result, result, int, level, inLevel, int, optionName, inOptionName, binary, optionValue, outOptionValue, size, optionValueSize, inOptionLength);

      if (SOCKET_ERROR == result)
      {
        int error = WSAGetLastError();
        ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(inSocket), int, error, error);
        ZS_THROW_CUSTOM_PROPERTIES_1(zsLib::Socket::Exceptions::Unspecified, error, "getsockopt failed, where socket id=" + (string((PTRNUMBER)inSocket)) + ", socket option=" + (string(inOptionName)) + ", error=" + (string(error)))
      }
      ZS_THROW_BAD_STATE_IF(length != inOptionLength)
    }

  } // namespace internal

  //---------------------------------------------------------------------------
  void Socket::setOptionFlag(Socket::SetOptionFlag::Options inOption, bool inEnabled) const throw(
                                                                                                  Exceptions::InvalidSocket,
                                                                                                  Exceptions::UnsupportedSocketOption,
                                                                                                  Exceptions::Unspecified
                                                                                                  )
  {
    internal::ignoreSigTermOnThread();

    AutoRecursiveLock lock(mLock);
    ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())
    ZS_THROW_CUSTOM_PROPERTIES_1_IF(Socket::Exceptions::UnsupportedSocketOption, -1 == inOption, ENOSYS)

    if (SetOptionFlag::NonBlocking == inOption)
    {
#ifdef _WIN32
      u_long value = inEnabled ? 1 : 0;
      int result = ioctlsocket(mSocket, inOption, &value);
#else
      int mode = inEnabled ? 1 : 0;
      int result = ioctl(mSocket, FIONBIO, &mode);
      result = fcntl(mSocket, F_GETFL, 0);
      if (SOCKET_ERROR != result) {
        if (inEnabled) {
          if (0 == (O_NONBLOCK & result)) {
            result = fcntl(mSocket, F_SETFL, O_NONBLOCK | result);   // turn on the flag with bitwise ORing the nonblocking
          }
        } else {
          if (0 != (O_NONBLOCK & result)) {
            result = fcntl(mSocket, F_SETFL, (result ^ O_NONBLOCK)); // turn off the flag with xor since it was on
          }
        }
      }
#endif //WIN32
      ZS_EVENTING_4(x, i, Debug, SocketSetOptionFlag, zs, Socket, Option, socket, socket, static_cast<uint64_t>(mSocket), int, result, result, enum, option, to_underlying(inOption), bool, enabled, inEnabled);
      if (SOCKET_ERROR == result)
      {
        int error = WSAGetLastError();
        ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, error);
        ZS_THROW_CUSTOM_PROPERTIES_1(Socket::Exceptions::Unspecified, error, "Failed to set the socket to non-blocking, where socket id=" + (string((PTRNUMBER)mSocket)) + ", error=" + (string(error)))
      }
      return;
    }

    int level = SOL_SOCKET;
    if (SetOptionFlag::TCPNoDelay == inOption)
      level = IPPROTO_TCP;

#ifdef _WIN32
    BOOL value = (inEnabled ? 1 : 0);
#else
    int value = (inEnabled ? 1 : 0);
#endif //_WIN32
    internal::setSocketOptions(mSocket, level, static_cast<int>(inOption), (BYTE *)&value, sizeof(value));
  }

  //---------------------------------------------------------------------------
  void Socket::setOptionValue(Socket::SetOptionValue::Options inOption, ULONG inValue) const throw(
                                                                                                   Exceptions::InvalidSocket,
                                                                                                   Exceptions::UnsupportedSocketOption,
                                                                                                   Exceptions::Unspecified
                                                                                                   )
  {
    internal::ignoreSigTermOnThread();

    AutoRecursiveLock lock(mLock);
    ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())
    ZS_THROW_CUSTOM_PROPERTIES_1_IF(Socket::Exceptions::UnsupportedSocketOption, -1 == static_cast<int>(inOption), ENOSYS)

    if (SetOptionValue::LingerTimeInSeconds == inOption)
    {
      LINGER linger;
      memset(&linger, 0, sizeof(linger));
      linger.l_onoff = (inValue != 0 ? 1 : 0);
      linger.l_linger = (inValue != 0 ? (u_short)inValue : 0);
      internal::setSocketOptions(mSocket, SOL_SOCKET, SO_LINGER, (BYTE *)&linger, sizeof(linger));
      return;
    }

    int value = static_cast<int>(inValue);
    internal::setSocketOptions(mSocket, SOL_SOCKET, inOption, (BYTE *)&value, sizeof(value));
  }

  //---------------------------------------------------------------------------
  bool Socket::getOptionFlag(GetOptionFlag::Options inOption) const throw(
                                                                          Exceptions::InvalidSocket,
                                                                          Exceptions::UnsupportedSocketOption,
                                                                          Exceptions::Unspecified
                                                                          )
  {
    internal::ignoreSigTermOnThread();

    AutoRecursiveLock lock(mLock);
    ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())
    ZS_THROW_CUSTOM_PROPERTIES_1_IF(Socket::Exceptions::UnsupportedSocketOption, -1 == inOption, ENOSYS)

    if (GetOptionFlag::IsOOBAllRead == inOption)
    {
#ifdef _WIN32
      u_long value = 0;
      int result = ioctlsocket(mSocket, inOption, &value);
#else
      int value = 0;
      int result = ioctl(mSocket, inOption, &value);
#endif //_WIN32
      ZS_EVENTING_4(x, i, Debug, SocketGetOptionFlag, zs, Socket, Option, socket, socket, static_cast<uint64_t>(mSocket), int, result, result, enum, option, to_underlying(inOption), int, value, value);
      if (SOCKET_ERROR == result)
      {
        int error = WSAGetLastError();
        ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, error);
        ZS_THROW_CUSTOM_PROPERTIES_1(Socket::Exceptions::Unspecified, error, "Failed to determine if OOB data was available or not, where socket id=" + (string((PTRNUMBER)mSocket)) + ", error=" + (string(error)))
      }
      return 0 != value;
    }

    int level = SOL_SOCKET;
    if (GetOptionFlag::IsTCPNoDelay == inOption)
      level = IPPROTO_TCP;

#ifdef _WIN32
    BOOL value = 0;
#else
    int value = 0;
#endif //_WIN32
    internal::getSocketOptions(mSocket, level, inOption, (BYTE *)&value, sizeof(value));
    return 0 != value;
  }

  //---------------------------------------------------------------------------
  ULONG Socket::getOptionValue(GetOptionValue::Options inOption) const throw(
                                                                             Exceptions::InvalidSocket,
                                                                             Exceptions::UnsupportedSocketOption,
                                                                             Exceptions::Unspecified
                                                                             )
  {
    internal::ignoreSigTermOnThread();

    AutoRecursiveLock lock(mLock);
    ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())
    ZS_THROW_CUSTOM_PROPERTIES_1_IF(Socket::Exceptions::UnsupportedSocketOption, -1 == inOption, ENOSYS)

    if (GetOptionValue::ReadyToReadSizeInBytes == inOption)
    {
#ifdef _WIN32
      u_long value = 0;
      int result = ioctlsocket(mSocket, inOption, &value);
#else
      int value = 0;
      int result = ioctl(mSocket, inOption, &value);
#endif //_WIN32
      ZS_EVENTING_4(x, i, Debug, SocketGetOptionValue, zs, Socket, Option, socket, socket, static_cast<uint64_t>(mSocket), int, result, result, enum, option, to_underlying(inOption), int, value, value);

      if (SOCKET_ERROR == result)
      {
        int error = WSAGetLastError();
        ZS_EVENTING_2(x, e, Detail, SocketError, zs, Socket, Exception, socket, socket, static_cast<uint64_t>(mSocket), int, error, error);
        ZS_THROW_CUSTOM_PROPERTIES_1(Socket::Exceptions::Unspecified, error, "Failed to determine amount of data ready to read, where socket id=" + (string((PTRNUMBER)mSocket)) + ", error=" + (string(error)))
      }
      return value;
    }

    int level = SOL_SOCKET;
    int value_int = 0;
    unsigned int value_unsigned_int = 0;

    BYTE *pvalue = (BYTE *)&value_int;
    int size = sizeof(value_int);
    switch (inOption)
    {
      case GetOptionValue::MaxMessageSizeInBytes:  pvalue = ((BYTE *)&value_unsigned_int); size = sizeof(value_unsigned_int); break;
      default:                                     break;
    }

    internal::getSocketOptions(mSocket, level, inOption, pvalue, size);

    switch (inOption)
    {
      case GetOptionValue::MaxMessageSizeInBytes:  return value_unsigned_int;
      default:                                     break;
    }
    return value_int;
  }

  //---------------------------------------------------------------------------
  void Socket::onReadReadyReset() const throw(Exceptions::DelegateNotSet, Exceptions::InvalidSocket, Exceptions::Unspecified)
  {
    {
      AutoRecursiveLock lock(mLock);
      ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())
      ZS_THROW_CUSTOM_IF(Exceptions::DelegateNotSet, !mDelegate)
    }

    if (mMonitorReadReady) {
      ZS_EVENTING_1(x, i, Insane, SocketReadReadyReset, zs, Socket, Event, socket, socket, static_cast<uint64_t>(mSocket));

      //SocketNotifyEventReset(__func__, this, mSocket);
      mMonitor->monitorRead(*this);
    }
  }

  //---------------------------------------------------------------------------
  void Socket::onWriteReadyReset() const throw(Exceptions::DelegateNotSet, Exceptions::InvalidSocket, Exceptions::Unspecified)
  {
    {
      AutoRecursiveLock lock(mLock);
      ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())
      ZS_THROW_CUSTOM_IF(Exceptions::DelegateNotSet, !mDelegate)
    }

    if (mMonitorWriteReady) {
      ZS_EVENTING_1(x, i, Insane, SocketWriteReadyReset, zs, Socket, Event, socket, socket, static_cast<uint64_t>(mSocket));
      mMonitor->monitorWrite(*this);
    }
  }

  //---------------------------------------------------------------------------
  void Socket::onExceptionReset() const throw(Exceptions::DelegateNotSet, Exceptions::InvalidSocket, Exceptions::Unspecified)
  {
    {
      AutoRecursiveLock lock(mLock);
      ZS_THROW_CUSTOM_IF(Exceptions::InvalidSocket, !isValid())
      ZS_THROW_CUSTOM_IF(Exceptions::DelegateNotSet, !mDelegate)
    }

    if (mMonitorException) {
      ZS_EVENTING_1(x, e, Insane, SocketExceptionReset, zs, Socket, Event, socket, socket, static_cast<uint64_t>(mSocket));
      mMonitor->monitorException(*this);
    }
  }

} // namespace zsLib

#pragma warning(pop)
