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

#include <zsLib/Log.h>
#include <zsLib/helpers.h>
#include <zsLib/Exception.h>
#include <zsLib/XML.h>

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
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
    Log::notifyNewSubsystem(this);
  }

  //---------------------------------------------------------------------------
  void Subsystem::setOutputLevel(Log::Level inLevel)
  {
    mLevel = inLevel;
  }

  //---------------------------------------------------------------------------
  Log::Level Subsystem::getOutputLevel() const
  {
    return mLevel;
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
  Log::Severity Log::toSeverity(const char *inSeverityStr)
  {
    static Severity severityArray[] = {
      Warning,
      Error,
      Fatal,

      Informational
    };

    String severityStr(inSeverityStr);

    for (int index = 0; Informational != severityArray[index]; ++index) {
      if (severityStr == toString(severityArray[index])) {
        return severityArray[index];
      }
    }

    return Informational;
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
  Log::Level Log::toLevel(const char *inLevelStr)
  {
    static Level levelArray[] = {
      Basic,
      Detail,
      Debug,
      Trace,
      Insane,

      None
    };

    String levelStr(inLevelStr);

    for (int index = 0; None != levelArray[index]; ++index) {
      if (levelStr == toString(levelArray[index])) {
        return levelArray[index];
      }
    }

    return None;
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
    static SingletonLazySharedPtr<Log> singleton(create());
    return singleton.singleton();
  }

  //---------------------------------------------------------------------------
  LogPtr Log::create()
  {
    return LogPtr(new Log);
  }

  //---------------------------------------------------------------------------
  void Log::addListener(ILogDelegatePtr delegate)
  {
    LogPtr log = Log::singleton();
    if (!log) return;

    Log &refThis = (*log);

    SubsystemList notifyList;
    // copy the list but notify without the lock
    {
      AutoRecursiveLock lock(refThis.mLock);

      ListenerListPtr replaceList(new ListenerList);

      if (refThis.mListeners) {
        (*replaceList) = (*refThis.mListeners);
      }

      replaceList->push_back(delegate);

      refThis.mListeners = replaceList;

      notifyList = refThis.mSubsystems;
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
    LogPtr log = Log::singleton();
    if (!log) return;

    Log &refThis = (*log);

    AutoRecursiveLock lock(refThis.mLock);

    ListenerListPtr replaceList(new ListenerList);

    if (refThis.mListeners) {
      (*replaceList) = (*refThis.mListeners);
    }

    for (ListenerList::iterator iter = replaceList->begin(); iter != replaceList->end(); ++iter)
    {
      if (delegate.get() == (*iter).get())
      {
        replaceList->erase(iter);

        refThis.mListeners = replaceList;
        return;
      }
    }

    ZS_THROW_INVALID_ARGUMENT("cound not remove log listener as it was not found")
  }

  //---------------------------------------------------------------------------
  void Log::notifyNewSubsystem(Subsystem *inSubsystem)
  {
    LogPtr log = Log::singleton();
    if (!log) return;

    Log &refThis = (*log);

    ListenerListPtr notifyList;

    // scope: remember the subsystem
    {
      AutoRecursiveLock lock(refThis.mLock);
      refThis.mSubsystems.push_back(inSubsystem);
      notifyList = refThis.mListeners;
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

    LogPtr log = Log::singleton();
    if (!log) return;

    Log &refThis = (*log);

    ListenerListPtr notifyList;

    {
      AutoRecursiveLock lock(refThis.mLock);
      notifyList = refThis.mListeners;
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

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, bool value) :
    Param(name, value ? "true" : "false", true)
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

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, const Time &value)
  {
    if (Time() == value) return;

    Param tmp(name, string(value), true);

    mParam = tmp.mParam;
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, const Hours &value)
  {
    if (Hours() == value) return;
    Param temp(name, string(value));
    mParam = temp.mParam;
    return;
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, const Minutes &value)
  {
    if (Hours() == value) return;
    Param temp(name, string(value));
    mParam = temp.mParam;
    return;
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, const Seconds &value)
  {
    if (Hours() == value) return;
    Param temp(name, string(value));
    mParam = temp.mParam;
    return;
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, const Milliseconds &value)
  {
    if (Hours() == value) return;
    Param temp(name, string(value));
    mParam = temp.mParam;
    return;
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, const Microseconds &value)
  {
    if (Hours() == value) return;
    Param temp(name, string(value));
    mParam = temp.mParam;
    return;
  }

  //---------------------------------------------------------------------------
  Log::Param::Param(const char *name, const Nanoseconds &value)
  {
    if (Hours() == value) return;
    Param temp(name, string(value));
    mParam = temp.mParam;
    return;
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
