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

#pragma once

#ifndef ZSLIB_ZSTYPES_H_53fa543745f508d26ae5baeec4bf3770
#define ZSLIB_ZSTYPES_H_53fa543745f508d26ae5baeec4bf3770

#include <zsLib/internal/types.h>

#define ZS_DECLARE_PTR(xExistingType)                                               ZS_INTERNAL_DECLARE_PTR(xExistingType)
#define ZS_DECLARE_USING_PTR(xNamespace, xExistingType)                             ZS_INTERNAL_DECLARE_USING_PTR(xNamespace, xExistingType)
#define ZS_DECLARE_CLASS_PTR(xClassName)                                            ZS_INTERNAL_DECLARE_CLASS_PTR(xClassName)
#define ZS_DECLARE_STRUCT_PTR(xStructName)                                          ZS_INTERNAL_DECLARE_STRUCT_PTR(xStructName)
#define ZS_DECLARE_INTERACTION_PTR(xInteractionName)                                ZS_INTERNAL_DECLARE_STRUCT_PTR(xInteractionName)
#define ZS_DECLARE_TYPEDEF_PTR(xOriginalType, xNewTypeName)                         ZS_INTERNAL_DECLARE_TYPEDEF_PTR(xOriginalType, xNewTypeName)
#define ZS_DYNAMIC_PTR_CAST(xType, xObject)                                         ZS_INTERNAL_DYNAMIC_PTR_CAST(xType, xObject)


namespace zsLib
{
  using std::make_shared;
  
  ZS_DECLARE_TYPEDEF_PTR(std::thread, Thread)
  ZS_DECLARE_TYPEDEF_PTR(std::mutex, Lock)
  ZS_DECLARE_TYPEDEF_PTR(std::recursive_mutex, RecursiveLock)

  typedef std::lock_guard<Lock> AutoLock;
  typedef std::lock_guard<RecursiveLock> AutoRecursiveLock;

  typedef std::chrono::system_clock::time_point Time;
  typedef std::chrono::hours Hours;
  typedef std::chrono::minutes Minutes;
  typedef std::chrono::seconds Seconds;
  typedef std::chrono::milliseconds Milliseconds;
  typedef std::chrono::microseconds Microseconds;
  typedef std::chrono::nanoseconds Nanoseconds;

  typedef CHAR ZsDeclareCHAR;
  typedef UCHAR ZsDeclareUCHAR;
  typedef SHORT ZsDeclareSHORT;
  typedef USHORT ZsDeclareUSHORT;
  typedef INT ZsDeclareINT;
  typedef UINT ZsDeclareUINT;
  typedef ULONG ZsDeclareULONG;
  typedef LONGLONG ZsDeclareLONGLONG;
  typedef ULONGLONG ZsDeclareULONGLONG;

  typedef FLOAT ZsDeclareFLOAT;
  typedef DOUBLE ZsDeclareDOUBLE;

  typedef BYTE ZsDeclareBYTE;
  typedef WORD ZsDeclareWORD;
  typedef DWORD ZsDeclareDWORD;
  typedef QWORD ZsDeclareQWORD;

  typedef PTRNUMBER ZsDeclarePTRNUMBER;
  typedef USERPARAM ZsDeclareUSERPARAM;

  typedef PUID ZsDeclarePUID;
  typedef UUID ZsDeclareUUID;

  typedef TCHAR ZsDeclareTCHAR;
  typedef WCHAR ZsDeclareWCHAR;

  typedef STR ZsDeclareSTR;
  typedef CSTR ZsDeclareCSTR;
  typedef TSTR ZsDeclareTSTR;
  typedef CTSTR ZsDeclareCTSTR;
  typedef WSTR ZsDeclareWSTR;
  typedef CWSTR ZsDeclareCWSTR;

  typedef LONGEST zsDeclareLONGEST;
  typedef ULONGEST zsDeclareULONGEST;

  class PrivateGlobalLock;

  ZS_DECLARE_CLASS_PTR(Event)
  ZS_DECLARE_CLASS_PTR(Exception)
  ZS_DECLARE_CLASS_PTR(IPAddress)

  ZS_DECLARE_CLASS_PTR(Log)
  ZS_DECLARE_INTERACTION_PTR(ILogDelegate)

  ZS_DECLARE_CLASS_PTR(SingletonManager)
  ZS_DECLARE_INTERACTION_PTR(ISingletonManagerDelegate)

  ZS_DECLARE_INTERACTION_PTR(IMessageQueueMessage)
  ZS_DECLARE_INTERACTION_PTR(IMessageQueueNotify)
  ZS_DECLARE_INTERACTION_PTR(IMessageQueue)
  ZS_DECLARE_INTERACTION_PTR(IMessageQueueThread)

  ZS_DECLARE_CLASS_PTR(MessageQueue)
  ZS_DECLARE_CLASS_PTR(MessageQueueThread)
  ZS_DECLARE_CLASS_PTR(MessageQueueThreadPool)

  ZS_DECLARE_CLASS_PTR(Promise)
  ZS_DECLARE_INTERACTION_PTR(IPromiseDelegate)
  ZS_DECLARE_INTERACTION_PTR(IPromiseSettledDelegate)
  ZS_DECLARE_INTERACTION_PTR(IPromiseResolutionDelegate)
  ZS_DECLARE_INTERACTION_PTR(IPromiseCatchDelegate)

  ZS_DECLARE_INTERACTION_PTR(ISocketDelegate)

  ZS_DECLARE_CLASS_PTR(Socket)
  ZS_DECLARE_CLASS_PTR(String)


  ZS_DECLARE_INTERACTION_PTR(ITimerDelegate)

  ZS_DECLARE_CLASS_PTR(Timer)

  ZS_DECLARE_STRUCT_PTR(Any)

  struct Any
  {
    virtual ~Any() {}
  };

  struct Noop
  {
    Noop(bool noop = false) : mNoop(noop) {}
    Noop(const Noop &noop) : mNoop(noop.mNoop) {}

    bool isNoop() const {return mNoop;}

    bool mNoop;
  };

  class AutoInitializedPUID
  {
  public:
    AutoInitializedPUID();

    operator PUID() const {return mValue;}

  private:
    PUID mValue;
  };

  typedef AutoInitializedPUID AutoPUID;

  template <typename type>
  class Optional
  {
  public:
    typedef type UseType;
  public:
    Optional() {}

    Optional(const UseType &value) :
      mHasValue(true),
      mType(value)
    {}

    Optional(const Optional &op2) :
      mHasValue(op2.mHasValue),
      mType(op2.mType)
    {}

    Optional &operator=(const Optional &op2)
    {
      mHasValue = op2.mHasValue;
      mType = op2.mType;
      return *this;
    }

    Optional &operator=(const UseType &op2)
    {
      mHasValue = true;
      mType = op2;
      return *this;
    }

    bool hasValue() const {return mHasValue;}
    UseType &value() {return mType;}
    const UseType &value() const {return mType;}
    operator UseType() const {return mType;}

  public:
    bool mHasValue {false};
    UseType mType {};
  };

  namespace XML
  {
    ZS_DECLARE_CLASS_PTR(Node)
    ZS_DECLARE_CLASS_PTR(Document)
    ZS_DECLARE_CLASS_PTR(Element)
    ZS_DECLARE_CLASS_PTR(Attribute)
    ZS_DECLARE_CLASS_PTR(Text)
    ZS_DECLARE_CLASS_PTR(Comment)
    ZS_DECLARE_CLASS_PTR(Declaration)
    ZS_DECLARE_CLASS_PTR(Unknown)
    ZS_DECLARE_CLASS_PTR(Parser)
    ZS_DECLARE_CLASS_PTR(Generator)

    class ParserPos;
    class ParserWarning;
    class WalkSink;
  }

  namespace JSON = zsLib::XML;

} // namespace zsLib

#endif //ZSLIB_ZSTYPES_H_53fa543745f508d26ae5baeec4bf3770
