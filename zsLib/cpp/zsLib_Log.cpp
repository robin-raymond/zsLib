
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

#include <zsLib/Log.h>
#include <zsLib/helpers.h>
#include <zsLib/Exception.h>
#include <zsLib/XML.h>

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{

  namespace internal
  {
    class GlobalLog;
    typedef boost::shared_ptr<GlobalLog> GlobalLogPtr;

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark GlobalLog
    #pragma mark

    class GlobalLog
    {
    private:
      GlobalLog() {mLog = zsLib::Log::create();}

    public:
      //-----------------------------------------------------------------------
      static GlobalLogPtr create() {return GlobalLogPtr(new GlobalLog);}

      LogPtr mLog;
    };

    //-----------------------------------------------------------------------
    static GlobalLogPtr &getGlobalLog()
    {
      static GlobalLogPtr log = GlobalLog::create();
      return log;
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark Log
    #pragma mark

    //-------------------------------------------------------------------------
    LogPtr Log::create()
    {
      return LogPtr(new zsLib::Log);
    }
  } // internal;

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark Subsystem
  #pragma mark

  //---------------------------------------------------------------------------
  Subsystem::Subsystem(CSTR inName, Log::Level inLevel) :
    mSubsystem(inName),
    mLevel(static_cast<Subsystem::LevelType>(inLevel))
  {
  }

  //---------------------------------------------------------------------------
  void Subsystem::notifyNewSubsystem()
  {
    Log::singleton()->notifyNewSubsystem(this);
  }

  //---------------------------------------------------------------------------
  void Subsystem::setOutputLevel(Log::Level inLevel)
  {
    ULONG value = (ULONG)inLevel;
    atomicSetValue32(mLevel, static_cast<Subsystem::LevelType>(value));
  }

  //---------------------------------------------------------------------------
  Log::Level Subsystem::getOutputLevel() const
  {
    return (Log::Level)atomicGetValue32(mLevel);
  }

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark Log
  #pragma mark

  //---------------------------------------------------------------------------
  const char *Log::toString(Severity severity)
  {
    switch (severity) {
      case Informational: return "info";
      case Warning:       return "warning";
      case Error:         return "error";
      case Fatal:         return "fatal";
    }
    return "undefined";
  }

  //---------------------------------------------------------------------------
  const char *Log::toString(Level level)
  {
    switch (level) {
      case None:          return "none";
      case Basic:         return "basic";
      case Detail:        return "detail";
      case Debug:         return "debug";
      case Trace:         return "trace";
      case Insane:        return "insane";
    }
    return "undefined";
  }

  //---------------------------------------------------------------------------
  Log::Log()
  {
  }

  //---------------------------------------------------------------------------
  Log::~Log()
  {
  }

  //---------------------------------------------------------------------------
  LogPtr Log::singleton()
  {
    return internal::getGlobalLog()->mLog;
  }

  //---------------------------------------------------------------------------
  void Log::addListener(ILogDelegatePtr delegate)
  {
    SubsystemList notifyList;
    // copy the list but notify without the lock
    {
      AutoRecursiveLock lock(mLock);

      ListenerListPtr replaceList(new ListenerList);

      if (mListeners) {
        (*replaceList) = (*mListeners);
      }

      replaceList->push_back(delegate);

      mListeners = replaceList;

      notifyList = mSubsystems;
    }

    for (SubsystemList::iterator iter = notifyList.begin(); iter != notifyList.end(); ++iter)
    {
      delegate->onNewSubsystem(
                               *(*iter)
                               );
    }
  }

  //---------------------------------------------------------------------------
  void Log::removeListener(ILogDelegatePtr delegate)
  {
    AutoRecursiveLock lock(mLock);

    ListenerListPtr replaceList(new ListenerList);

    if (mListeners) {
      (*replaceList) = (*mListeners);
    }

    for (ListenerList::iterator iter = replaceList->begin(); iter != replaceList->end(); ++iter)
    {
      if (delegate.get() == (*iter).get())
      {
        replaceList->erase(iter);

        mListeners = replaceList;
        return;
      }
    }
    ZS_THROW_INVALID_ARGUMENT("cound not remove log listener as it was not found")
  }

  //---------------------------------------------------------------------------
  void Log::notifyNewSubsystem(Subsystem *inSubsystem)
  {
    ListenerListPtr notifyList;

    // scope: remember the subsystem
    {
      AutoRecursiveLock lock(mLock);
      mSubsystems.push_back(inSubsystem);
      notifyList = mListeners;
    }

    if (!notifyList) return;

    for (ListenerList::iterator iter = notifyList->begin(); iter != notifyList->end(); ++iter)
    {
      (*iter)->onNewSubsystem(
                              *inSubsystem
                              );
    }
  }

  //---------------------------------------------------------------------------
  void Log::log(
                const Subsystem &inSubsystem,
                Severity inSeverity,
                Level inLevel,
                const String &inMessage,
                CSTR inFunction,
                CSTR inFilePath,
                ULONG inLineNumber
                )
  {
    log(inSubsystem, inSeverity, inLevel, Log::Params(inMessage), inFunction, inFilePath, inLineNumber);
  }

  //-------------------------------------------------------------------------
  void Log::log(
                const Subsystem &inSubsystem,
                Severity inSeverity,
                Level inLevel,
                const Log::Params &inParams,
                CSTR inFunction,
                CSTR inFilePath,
                ULONG inLineNumber
                )
  {
    if (inLevel > inSubsystem.getOutputLevel())
      return;

    ListenerListPtr notifyList;

    {
      AutoRecursiveLock lock(mLock);
      notifyList = mListeners;
    }

    if (!notifyList) return;

    for (ListenerList::iterator iter = notifyList->begin(); iter != notifyList->end(); ++iter)
    {
      (*iter)->onLog(
                     inSubsystem,
                     inSeverity,
                     inLevel,
                     inFunction,
                     inFilePath,
                     inLineNumber,
                     inParams
                     );
    }
  }

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark Log::Params
  #pragma mark

  //---------------------------------------------------------------------------
  Log::Param::Param(const Param &param) :
    mParam(param.mParam)
  {
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, const char *value, bool isNumber)
  {
    if (!value) return;
    if ('\0' == *value) return;

    mParam = XML::Element::create(name);

    XML::TextPtr tmpTxt = XML::Text::create();
    if (isNumber) {
      tmpTxt->setValue(value, XML::Text::Format_JSONNumberEncoded);
    } else {
      tmpTxt->setValueAndJSONEncode(value);
    }
    mParam->adoptAsFirstChild(tmpTxt);
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, const String &value, bool isNumber)
  {
    if (value.isEmpty()) return;

    mParam = XML::Element::create(name);

    XML::TextPtr tmpTxt = XML::Text::create();
    if (isNumber) {
      tmpTxt->setValue(value, XML::Text::Format_JSONNumberEncoded);
    } else {
      tmpTxt->setValueAndJSONEncode(value);
    }
    mParam->adoptAsFirstChild(tmpTxt);
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, XML::ElementPtr value)
  {
    if (!value) return;

    mParam = XML::Element::create(name);

    mParam->adoptAsLastChild(value);
  }

#ifndef _ANDROID
  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, bool value) :
    Param(name, "true", true)
  {
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, CHAR value) :
    Param(name, (INT)value)
  {
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, UCHAR value) :
    Param(name, (UINT)value)
  {
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, SHORT value) :
    Param(name, (INT)value)
  {
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, USHORT value) :
    Param(name, (UINT)value)
  {
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, INT value) :
    Param(name, string(value), true)
  {
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, UINT value) :
    Param(name, string(value), true)
  {
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, LONG value) :
    Param(name, string(value), true)
  {
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, ULONG value) :
    Param(name, string(value), true)
  {
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, LONGLONG value) :
    Param(name, string(value), true)
  {
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, ULONGLONG value) :
    Param(name, string(value), true)
  {
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, FLOAT value) :
    Param(name, string(value), true)
  {
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, DOUBLE value) :
    Param(name, string(value), true)
  {
  }
#else
  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, bool value) {
    Param(name, "true", true);
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, CHAR value) {
    Param(name, (INT)value);
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, UCHAR value) {
    Param(name, (UINT)value);
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, SHORT value) {
    Param(name, (INT)value);
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, USHORT value) {
    Param(name, (UINT)value);
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, INT value) {
    Param(name, string(value), true);
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, UINT value) {
    Param(name, string(value), true);
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, LONG value) {
    Param(name, string(value), true);
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, ULONG value) {
    Param(name, string(value), true);
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, LONGLONG value) {
    Param(name, string(value), true);
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, ULONGLONG value) {
    Param(name, string(value), true);
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, FLOAT value) {
    Param(name, string(value), true);
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, DOUBLE value) {
    Param(name, string(value), true);
  }
#endif
  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, const Time &value)
  {
    if (Time() == value) return;

    Param tmp(name, string(value), true);

    mParam = tmp.mParam;
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, const Duration &value)
  {
    if (Duration() == value) return;

    if (name) {
      if (strstr(name, "(ms)")) {
        Param tmp(name, value.total_milliseconds());
        mParam = tmp.mParam;
        return;
      }
      if (strstr(name, "(s)")) {
        Param tmp(name, value.total_seconds());
        mParam = tmp.mParam;
        return;
      }
      if (strstr(name, "(seconds)")) {
        Param tmp(name, value.total_seconds());
        mParam = tmp.mParam;
        return;
      }
    }

    Param tmp(name, boost::posix_time::to_simple_string(value));
    mParam = tmp.mParam;
  }

  //---------------------------------------------------------------------------
  const XML::ElementPtr &Log::Param::param() const
  {
    return mParam;
  }

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark Log::Params
  #pragma mark

  //---------------------------------------------------------------------------
  Log::Params::Params()
  {
  }

  //---------------------------------------------------------------------------
  Log::Params::Params(const char *message, XML::ElementPtr object) :
    mMessage(message),
    mObject(object)
  {
  }

  //---------------------------------------------------------------------------
  Log::Params::Params(const String &message, XML::ElementPtr object) :
    mMessage(message),
    mObject(object)
  {
  }

  //---------------------------------------------------------------------------
  Log::Params::Params(const char *message, const char *staticObjectName) :
    mMessage(message),
    mObject(XML::Element::create(staticObjectName))
  {
    ZS_THROW_INVALID_ARGUMENT_IF(!staticObjectName)
  }

  //---------------------------------------------------------------------------
  Log::Params::Params(const String &message, const char *staticObjectName) :
    mMessage(message),
    mObject(XML::Element::create(staticObjectName))
  {
    ZS_THROW_INVALID_ARGUMENT_IF(!staticObjectName)
  }

  //---------------------------------------------------------------------------
  Log::Params::Params(const Params &params) :
    mMessage(params.mMessage),
    mObject(params.mObject),
    mParams(params.mParams)
  {
  }

  //---------------------------------------------------------------------------
  Log::Params &Log::Params::operator<<(const XML::ElementPtr &param)
  {
    if (!param) return *this;

    if (!mParams) {
      mParams = XML::Element::create("params");
    }

    mParams->adoptAsLastChild(param);

    return *this;
  }

  //---------------------------------------------------------------------------
  Log::Params &Log::Params::operator<<(const Param &param)
  {
    return (*this) << (param.param());
  }

  //---------------------------------------------------------------------------
  Log::Params &Log::Params::operator<<(const Params &params)
  {
    if (params.mMessage.hasData()) {
      mMessage = params.mMessage;
    }
    if (params.mObject) {
      mObject = params.mObject;
    }
    if (params.mParams) {
      mParams = params.mParams;
    }
    return (*this);
  }

  //---------------------------------------------------------------------------
  const String &Log::Params::message() const
  {
    return mMessage;
  }

  //---------------------------------------------------------------------------
  const XML::ElementPtr &Log::Params::object() const
  {
    return mObject;
  }

  //---------------------------------------------------------------------------
  const XML::ElementPtr &Log::Params::params() const
  {
    return mParams;
  }

} // namespace zsLib
