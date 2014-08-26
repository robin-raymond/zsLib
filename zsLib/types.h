/*
 *  Created by Robin Raymond.
 *  Copyright 2009-2013. Robin Raymond. All rights reserved.
 *
 * This file is part of zsLib.
 *
 * zsLib is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License (LGPL) as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * zsLib is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with zsLib; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
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
  ZS_DECLARE_TYPEDEF_PTR(boost::thread, Thread)
  ZS_DECLARE_TYPEDEF_PTR(boost::mutex, Lock)
  ZS_DECLARE_TYPEDEF_PTR(boost::recursive_mutex, RecursiveLock)

  typedef boost::lock_guard<Lock> AutoLock;
  typedef boost::lock_guard<RecursiveLock> AutoRecursiveLock;

  typedef boost::posix_time::ptime Time;
  typedef boost::posix_time::time_duration Duration;
  typedef boost::posix_time::seconds Seconds;
  typedef boost::posix_time::minutes Minutes;
  typedef boost::posix_time::hours Hours;
  typedef boost::posix_time::milliseconds Milliseconds;
  typedef boost::posix_time::microseconds Microseconds;
#if defined(BOOST_DATE_TIME_HAS_NANOSECONDS)
  typedef boost::posix_time::nanoseconds Nanoseconds;
#endif
  typedef boost::posix_time::time_period TimePeriod;
  typedef boost::posix_time::time_iterator TimeIterator;

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

  class PrivateGlobalLock;

  ZS_DECLARE_CLASS_PTR(Event)
  ZS_DECLARE_CLASS_PTR(Exception)
  ZS_DECLARE_CLASS_PTR(IPAddress)

  ZS_DECLARE_CLASS_PTR(Log)
  ZS_DECLARE_INTERACTION_PTR(ILogDelegate)

  ZS_DECLARE_INTERACTION_PTR(IMessageQueueMessage)
  ZS_DECLARE_INTERACTION_PTR(IMessageQueueNotify)
  ZS_DECLARE_INTERACTION_PTR(IMessageQueue)
  ZS_DECLARE_INTERACTION_PTR(IMessageQueueThread)

  ZS_DECLARE_CLASS_PTR(MessageQueue)
  ZS_DECLARE_CLASS_PTR(MessageQueueThread)


  ZS_DECLARE_INTERACTION_PTR(ISocketDelegate)

  ZS_DECLARE_CLASS_PTR(Socket)
  ZS_DECLARE_CLASS_PTR(String)


  ZS_DECLARE_INTERACTION_PTR(ITimerDelegate)

  ZS_DECLARE_CLASS_PTR(Timer)

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

  namespace JSON
  {
    using namespace XML;
  };

} // namespace zsLib

#endif //ZSLIB_ZSTYPES_H_53fa543745f508d26ae5baeec4bf3770
