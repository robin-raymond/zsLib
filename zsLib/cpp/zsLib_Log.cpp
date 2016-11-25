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
#include <zsLib/eventing/Log.h>
#include <zsLib/helpers.h>
#include <zsLib/Exception.h>
#include <zsLib/XML.h>
#include <zsLib/String.h>
#include <zsLib/Singleton.h>

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
  Subsystem::Subsystem(
                       CSTR inName,
                       Log::Level inOutputLevel,
                       Log::Level inEventingLevel
                       ) :
    mSubsystem(inName),
    mOutputLevel(static_cast<Subsystem::LevelType>(inOutputLevel)),
    mEventingLevel(static_cast<Subsystem::LevelType>(inEventingLevel))
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
    mOutputLevel = inLevel;
  }

  //---------------------------------------------------------------------------
  Log::Level Subsystem::getOutputLevel() const
  {
    return mOutputLevel;
  }

  //---------------------------------------------------------------------------
  void Subsystem::setEventingLevel(Log::Level inLevel)
  {
    mEventingLevel = inLevel;
  }

  //---------------------------------------------------------------------------
  Log::Level Subsystem::getEventingLevel() const
  {
    return mEventingLevel;
  }

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark Log
  #pragma mark

  namespace internal
  {
    String Log::paramize(const char *name)
    {
      if (!name) return String();

      auto orignalLength = strlen(name);
      auto maxLength = orignalLength * 2;

      char buffer[256] {};
      char *useBuffer = &(buffer[0]);

      char *allocatedBuffer = NULL;

      if (maxLength >= sizeof(buffer)) {
        allocatedBuffer = new char[maxLength+1] {};
        useBuffer = allocatedBuffer;
      }

      bool lastWasSpace = true;
      bool lastWasUpper = true;

      const char *source = name;
      char *dest = useBuffer;

      while (*source) {
        char gliph = *source;

        if (!isalnum(gliph)) {
          if (lastWasSpace) {
            ++source;
            continue;
          }
          *dest = ' ';
          ++dest;
          ++source;

          lastWasSpace = true;
          continue;
        }

        if (isupper(gliph)) {
          if (!lastWasUpper) {
            if (!lastWasSpace) {
              *dest = ' ';
              ++dest;
            }
          }

          *dest = tolower(gliph);
          ++dest;
          ++source;

          lastWasSpace = false;
          lastWasUpper = true;
          continue;
        }

        lastWasUpper = false;
        lastWasSpace = false;

        *dest = gliph;
        ++dest;
        ++source;
      }

      String result((const char *)useBuffer);

      delete [] allocatedBuffer;
      allocatedBuffer = NULL;

      return result;
    }
  }

  //---------------------------------------------------------------------------
  const char *Log::toString(Severity severity)
  {
    switch (severity) {
      case Informational: return "Informational";
      case Warning:       return "Warning";
      case Error:         return "Error";
      case Fatal:         return "Fatal";
    }
    return "undefined";
  }

  //---------------------------------------------------------------------------
  Log::Severity Log::toSeverity(const char *inSeverityStr)
  {
    String severityStr(inSeverityStr);

    if (0 == (severityStr.compareNoCase("Info"))) {
      severityStr = "Informational";
    }

    for (Log::Severity index = Log::Severity_First; index <= Log::Severity_Last; index = static_cast<Log::Severity>(static_cast<std::underlying_type<Log::Severity>::type>(index) + 1)) {
      if (0 == severityStr.compareNoCase(toString(index))) {
        return index;
      }
    }

    ZS_THROW_INVALID_ARGUMENT(String("Invalid severity: ") + severityStr);
    return Informational;
  }
  
  //---------------------------------------------------------------------------
  const char *Log::toString(Level level)
  {
    switch (level) {
      case None:          return "None";
      case Basic:         return "Basic";
      case Detail:        return "Detail";
      case Debug:         return "Debug";
      case Trace:         return "Trace";
      case Insane:        return "Insane";
    }
    return "undefined";
  }

  //---------------------------------------------------------------------------
  Log::Level Log::toLevel(const char *inLevelStr)
  {
    String levelStr(inLevelStr);

    for (Log::Level index = Log::Level_First; index <= Log::Level_Last; index = static_cast<Log::Level>(static_cast<std::underlying_type<Log::Level>::type>(index) + 1)) {
      if (0 == levelStr.compareNoCase(toString(index))) {
        return index;
      }
    }

    ZS_THROW_INVALID_ARGUMENT(String("Invalid level: ") + levelStr);
    return None;
  }

  //---------------------------------------------------------------------------
  Log::Log(const make_private &ignore) :
    internal::Log(ignore)
  {
  }

  //---------------------------------------------------------------------------
  Log::~Log()
  {
    for (auto iter = mSubsystems.begin(); iter != mSubsystems.end(); ++iter)
    {
      auto &subsystem = (*iter);
      subsystem->setEventingLevel(None);
    }

    for (auto iter = mCleanUpWriters.begin(); iter != mCleanUpWriters.end(); ++iter)
    {
      auto writer = (*iter);
      delete writer;
    }
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
    return make_shared<Log>(make_private{});
  }

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark (output methods)
  #pragma mark

  //---------------------------------------------------------------------------
  void Log::addOutputListener(ILogOutputDelegatePtr delegate)
  {
    if (!delegate) return;

    LogPtr log = Log::singleton();
    if (!log) return;

    Log &refThis = (*log);

    SubsystemList notifyList;
    // copy the list but notify without the lock
    {
      AutoRecursiveLock lock(refThis.mLock);

      OutputListenerListPtr replaceList(make_shared<OutputListenerList>());

      size_t originalSize = refThis.mOutputListeners->size();

      (*replaceList) = (*refThis.mOutputListeners);

      replaceList->push_back(delegate);

      refThis.mOutputListeners = replaceList;

      notifyList = refThis.mSubsystems;

      if (0 == originalSize) {
        Optional<Level> defaultLevel;

        {
          auto found = refThis.mDefaultOutputSubsystemLevels.find(String());
          if (found != refThis.mDefaultOutputSubsystemLevels.end()) defaultLevel = static_cast<Level>((*found).second);
        }

        for (auto iter = refThis.mSubsystems.begin(); iter != refThis.mSubsystems.end(); ++iter)
        {
          auto &subsystem = (*iter);
          auto name = subsystem->getName();
          auto found = refThis.mDefaultOutputSubsystemLevels.find(String(name));
          if (found != refThis.mDefaultOutputSubsystemLevels.end()) {
            auto level = static_cast<Level>((*found).second);
            subsystem->setOutputLevel(level);
          } else if (defaultLevel.hasValue()) {
            subsystem->setOutputLevel(defaultLevel.value());
          }
        }
      }
    }

    for (SubsystemList::iterator iter = notifyList.begin(); iter != notifyList.end(); ++iter)
    {
      delegate->onNewSubsystem(*(*iter));
    }
  }

  //---------------------------------------------------------------------------
  void Log::removeOutputListener(ILogOutputDelegatePtr delegate)
  {
    if (!delegate) return;

    LogPtr log = Log::singleton();
    if (!log) return;

    Log &refThis = (*log);

    AutoRecursiveLock lock(refThis.mLock);

    OutputListenerListPtr replaceList(make_shared<OutputListenerList>());

    (*replaceList) = (*refThis.mOutputListeners);

    for (OutputListenerList::iterator iter = replaceList->begin(); iter != replaceList->end(); ++iter)
    {
      if (delegate.get() == (*iter).get())
      {
        replaceList->erase(iter);

        if (0 == replaceList->size()) {
          if (refThis.mDefaultOutputSubsystemLevels.size() > 0) {
            for (auto iter = refThis.mSubsystems.begin(); iter != refThis.mSubsystems.end(); ++iter) {
              auto &subsystem = (*iter);
              subsystem->setOutputLevel(None);
            }
          }
        }

        refThis.mOutputListeners = replaceList;
        return;
      }
    }

    ZS_THROW_INVALID_ARGUMENT("cound not remove log listener as it was not found");
  }

  //---------------------------------------------------------------------------
  void Log::notifyNewSubsystem(Subsystem *inSubsystem)
  {
    LogPtr log = Log::singleton();
    if (!log) return;

    Log &refThis = (*log);

    OutputListenerListPtr notifyList;

    // scope: remember the subsystem
    {
      AutoRecursiveLock lock(refThis.mLock);
      refThis.mSubsystems.push_back(inSubsystem);
      notifyList = refThis.mOutputListeners;

      {
        auto found = refThis.mDefaultOutputSubsystemLevels.find(String(inSubsystem->getName()));
        if (found != refThis.mDefaultOutputSubsystemLevels.end()) {
          inSubsystem->setOutputLevel(static_cast<Level>((*found).second));
        } else {
          auto foundDefault = refThis.mDefaultOutputSubsystemLevels.find(String());
          if (foundDefault != refThis.mDefaultOutputSubsystemLevels.end()) {
            inSubsystem->setOutputLevel(static_cast<Level>((*foundDefault).second));
          }
        }
      }
    }

    for (auto iter = notifyList->begin(); iter != notifyList->end(); ++iter)
    {
      (*iter)->onNewSubsystem(*inSubsystem);
    }
  }

  //---------------------------------------------------------------------------
  void Log::setOutputLevelByName(
                                 const char *subsystemName,
                                 Level level
                                 )
  {
    String subsystemNameStr(subsystemName);
    if (subsystemNameStr.isEmpty()) {
      subsystemNameStr = String();
    }

    LogPtr log = Log::singleton();
    if (!log) return;

    Log &refThis = (*log);

    bool defaultAll = subsystemNameStr.isEmpty();

    {
      AutoRecursiveLock lock(refThis.mLock);
      refThis.mDefaultOutputSubsystemLevels[subsystemNameStr] = static_cast<decltype(level)>(static_cast<std::underlying_type<decltype(level)>::type>(level));

      // if no listeners attached then there's no reason to adjust the subsystem's current level
      if (refThis.mOutputListeners->size() < 1) return;

      for (auto iter = refThis.mSubsystems.begin(); iter != refThis.mSubsystems.end(); ++iter)
      {
        auto &subsystem = *(*iter);

        if (defaultAll) {
          subsystem.setOutputLevel(level);
          continue;
        }

        if (subsystemNameStr == subsystem.getName()) {
          subsystem.setOutputLevel(level);
        }
      }
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

    OutputListenerListPtr notifyList;

    {
      AutoRecursiveLock lock(refThis.mLock);
      notifyList = refThis.mOutputListeners;
    }

    for (auto iter = notifyList->begin(); iter != notifyList->end(); ++iter)
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
  #pragma mark (eventing methods)
  #pragma mark

  //---------------------------------------------------------------------------
  void Log::addEventingListener(ILogEventingDelegatePtr delegate)
  {
    if (!delegate) return;

    LogPtr log = Log::singleton();
    if (!log) return;

    Log &refThis = (*log);

    SubsystemList notifyList;

    // copy the list but notify without the lock
    {
      AutoRecursiveLock lock(refThis.mLock);

      EventingListenerListPtr replaceList(make_shared<EventingListenerList>());

      size_t originalSize = refThis.mEventingListeners->size();

      (*replaceList) = (*refThis.mEventingListeners);

      replaceList->push_back(delegate);

      refThis.mEventingListeners = replaceList;

      notifyList = refThis.mSubsystems;

      if (0 == originalSize) {
        Optional<Level> defaultLevel;

        {
          auto found = refThis.mDefaultEventingSubsystemLevels.find(String());
          if (found != refThis.mDefaultEventingSubsystemLevels.end()) defaultLevel = static_cast<Level>((*found).second);
        }

        for (auto iter = refThis.mSubsystems.begin(); iter != refThis.mSubsystems.end(); ++iter)
        {
          auto &subsystem = (*iter);
          auto name = subsystem->getName();
          auto found = refThis.mDefaultEventingSubsystemLevels.find(String(name));
          if (found != refThis.mDefaultEventingSubsystemLevels.end()) {
            auto level = static_cast<Level>((*found).second);
            subsystem->setEventingLevel(level);
          }
          else if (defaultLevel.hasValue()) {
            subsystem->setEventingLevel(defaultLevel.value());
          }
        }
      }
    }

    for (SubsystemList::iterator iter = notifyList.begin(); iter != notifyList.end(); ++iter)
    {
      delegate->onNewSubsystem(*(*iter));
    }
  }

  //---------------------------------------------------------------------------
  void Log::removeEventingListener(ILogEventingDelegatePtr delegate)
  {
    if (!delegate) return;

    LogPtr log = Log::singleton();
    if (!log) return;

    Log &refThis = (*log);

    AutoRecursiveLock lock(refThis.mLock);

    EventingListenerListPtr replaceList(make_shared<EventingListenerList>());

    (*replaceList) = (*refThis.mEventingListeners);

    for (EventingListenerList::iterator iter = replaceList->begin(); iter != replaceList->end(); ++iter)
    {
      if (delegate.get() == (*iter).get())
      {
        replaceList->erase(iter);

        if (0 == replaceList->size()) {
          if (refThis.mDefaultEventingSubsystemLevels.size() > 0) {
            for (auto iter = refThis.mSubsystems.begin(); iter != refThis.mSubsystems.end(); ++iter) {
              auto &subsystem = (*iter);
              subsystem->setEventingLevel(None);
            }
          }
        }

        refThis.mEventingListeners = replaceList;
        return;
      }
    }

    ZS_THROW_INVALID_ARGUMENT("cound not remove log listener as it was not found");
  }

  //---------------------------------------------------------------------------
  uintptr_t Log::registerEventingWriter(
                                        const UUID &providerID,
                                        const char *providerName,
                                        const char *uniqueProviderHash
                                        )
  {
    LogPtr log = Log::singleton();
    if (!log) return 0;

    Log &refThis = (*log);

    EventingWriter *writer = NULL;

    {
      AutoRecursiveLock lock(refThis.mLock);

      {
        auto found = refThis.mEventWriters.find(providerID);
        if (found != refThis.mEventWriters.end()) {
          auto existingWriter = (*found).second;
          return reinterpret_cast<uintptr_t>(existingWriter);
        }
      }

      writer = new EventingWriter;
      writer->mProviderID = providerID;
      writer->mProviderName = String(providerName);
      writer->mUniqueProviderHash = String(uniqueProviderHash);
      writer->mLog = log.get();

      refThis.mEventWriters[providerID] = writer;
    }

    return reinterpret_cast<uintptr_t>(writer);
  }

  //---------------------------------------------------------------------------
  void Log::unregisterEventingWriter(uintptr_t handle)
  {
    if (0 == handle) return;

    LogPtr log = Log::singleton();
    if (!log) return;

    Log &refThis = (*log);

    {
      AutoRecursiveLock lock(refThis.mLock);

      EventingWriter *writer = reinterpret_cast<EventingWriter *>(handle);
      
      for (auto iter = refThis.mEventWriters.begin(); iter != refThis.mEventWriters.end(); ++iter)
      {
        uintptr_t found = reinterpret_cast<uintptr_t>((*iter).second);
        if (found != handle) continue;

        refThis.mEventWriters.erase(iter);
        break;
      }

      refThis.mCleanUpWriters.insert(writer);
    }

  }

  //---------------------------------------------------------------------------
  bool Log::getEventingWriterInfo(
                                  uintptr_t handle,
                                  UUID &outProviderID,
                                  String &outProviderName,
                                  String &outUniqueProviderHash
                                  )
  {
    if (0 == handle) return false;

    EventingWriter *writer = reinterpret_cast<EventingWriter *>(handle);

    outProviderID = writer->mProviderID;
    outProviderName = writer->mProviderName;
    outUniqueProviderHash = writer->mUniqueProviderHash;
    return true;
  }

  //---------------------------------------------------------------------------
  void Log::setEventingLevelByName(
                                   const char *subsystemName,
                                   Level level
                                   )
  {
    String subsystemNameStr(subsystemName);
    if (subsystemNameStr.isEmpty()) {
      subsystemNameStr = String();
    }

    LogPtr log = Log::singleton();
    if (!log) return;

    Log &refThis = (*log);

    bool defaultAll = subsystemNameStr.isEmpty();

    {
      AutoRecursiveLock lock(refThis.mLock);
      refThis.mDefaultEventingSubsystemLevels[subsystemNameStr] = static_cast<decltype(level)>(static_cast<std::underlying_type<decltype(level)>::type>(level));

      // if no listeners attached then there's no reason to adjust the subsystem's current level
      if (refThis.mEventingListeners->size() < 1) return;

      for (auto iter = refThis.mSubsystems.begin(); iter != refThis.mSubsystems.end(); ++iter)
      {
        auto &subsystem = *(*iter);

        if (defaultAll) {
          subsystem.setEventingLevel(level);
          continue;
        }

        if (subsystemNameStr == subsystem.getName()) {
          subsystem.setEventingLevel(level);
        }
      }
    }
  }

  //---------------------------------------------------------------------------
  void Log::writeEvent(
                       uintptr_t handle,
                       Severity severity,
                       Level level,
                       const char *subsystemName,
                       const char *functionName,
                       ULONG lineNumber,
                       size_t mValue,
                       const BYTE *buffer,
                       size_t bufferSize,
                       const BYTE * const* buffers,
                       const size_t *buffersSizes,
                       size_t totalBuffers
                       )
  {
    if (0 == handle) return;

    EventingWriter *writer = reinterpret_cast<EventingWriter *>(handle);

    Log *log = writer->mLog;

    EventingListenerListPtr notifyList;

    {
      AutoRecursiveLock lock(log->mLock);
      notifyList = log->mEventingListeners;
    }

    EventingListenerList &useList = *notifyList;

    for (auto iter = useList.begin(); iter != useList.end(); ++iter)
    {
      (*iter)->onWriteEvent(handle, severity, level, subsystemName, functionName, lineNumber, mValue, buffer, bufferSize, buffers, buffersSizes, totalBuffers);
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
