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

#include <zsLib/internal/zsLib_Log.h>

// forward declare a subsystem (for use in header files); wrap inside the namespace where it should be defined
#define ZS_DECLARE_FORWARD_SUBSYSTEM(xSubsystem)                        ZS_INTERNAL_DECLARE_FORWARD_SUBSYSTEM(xSubsystem)

// declare a subsystem; wrap inside the namespace where it should be defined
#define ZS_DECLARE_SUBSYSTEM(xSubsystem)                                ZS_INTERNAL_DECLARE_SUBSYSTEM(xSubsystem)

// implement the code for a subsystem; wrap inside the same namespace as all the other declations
#define ZS_IMPLEMENT_SUBSYSTEM(xSubsystem)                              ZS_INTERNAL_IMPLEMENT_SUBSYSTEM(xSubsystem)

// return a reference to the current Subsystem
#define ZS_GET_SUBSYSTEM()                                              ZS_INTERNAL_GET_SUBSYSTEM()

/// get a subsystem that is not part of this subsystem
#define ZS_GET_OTHER_SUBSYSTEM(xNamespace, xSubsystem)                  ZS_INTERNAL_GET_OTHER_SUBSYSTEM(xNamespace, xSubsystem)

#define ZS_LOG_PARAMS(xMsg)                                             ZS_INTERNAL_LOG_PARAMS(xMsg)
#define ZS_PARAM(xName, xValue)                                         ZS_INTERNAL_PARAM(xName, xValue)
#define ZS_PARAMIZE(xValueName)                                         ZS_INTERNAL_PARAMIZE(xValueName)

#define ZS_GET_LOG_LEVEL()                                              ZS_INTERNAL_GET_LOG_LEVEL()
#define ZS_GET_SUBSYSTEM_LOG_LEVEL(xSubsystem)                          ZS_INTERNAL_GET_SUBSYSTEM_LOG_LEVEL(xSubsystem)
#define ZS_IS_LOGGING(xLevel)                                           ZS_INTERNAL_IS_LOGGING(xLevel)
#define ZS_IS_SUBSYSTEM_LOGGING(xSubsystem, xLevel)                     ZS_INTERNAL_IS_SUBSYSTEM_LOGGING(xSubsystem, xLevel)

#define ZS_LOG_FORCED(xSeverity, xLevel, xMsg)                          ZS_INTERNAL_LOG_FORCED(xSeverity, xLevel, xMsg)

#define ZS_LOG_BASIC(xMsg)                                              ZS_INTERNAL_LOG_BASIC(xMsg)
#define ZS_LOG_DETAIL(xMsg)                                             ZS_INTERNAL_LOG_DETAIL(xMsg)
#define ZS_LOG_DEBUG(xMsg)                                              ZS_INTERNAL_LOG_DEBUG(xMsg)
#define ZS_LOG_TRACE(xMsg)                                              ZS_INTERNAL_LOG_TRACE(xMsg)
#define ZS_LOG_INSANE(xMsg)                                             ZS_INTERNAL_LOG_INSANE(xMsg)

#define ZS_LOG_WITH_SEVERITY(xSeverityValue, xLevelValue, xMsg)         ZS_INTERNAL_LOG_WITH_SEVERITY(xSeverityValue, xLevelValue, xMsg)

#define ZS_LOG_BASIC_WITH_SEVERITY(xSeverityValue, xMsg)                ZS_INTERNAL_LOG_BASIC_WITH_SEVERITY(xSeverityValue, xMsg)
#define ZS_LOG_DETAIL_WITH_SEVERITY(xSeverityValue, xMsg)               ZS_INTERNAL_LOG_DETAIL_WITH_SEVERITY(xSeverityValue, xMsg)
#define ZS_LOG_DEBUG_WITH_SEVERITY(xSeverityValue, xMsg)                ZS_INTERNAL_LOG_DEBUG_WITH_SEVERITY(xSeverityValue, xMsg)
#define ZS_LOG_TRACE_WITH_SEVERITY(xSeverityValue, xMsg)                ZS_INTERNAL_LOG_TRACE_WITH_SEVERITY(xSeverityValue, xMsg)
#define ZS_LOG_INSANE_WITH_SEVERITY(xSeverityValue, xMsg)               ZS_INTERNAL_LOG_INSANE_WITH_SEVERITY(xSeverityValue, xMsg)

#define ZS_LOG_SUBSYSTEM_BASIC(xSubsystem, xMsg)                        ZS_INTERNAL_LOG_SUBSYSTEM_BASIC(xSubsystem, xMsg)
#define ZS_LOG_SUBSYSTEM_DETAIL(xSubsystem, xMsg)                       ZS_INTERNAL_LOG_SUBSYSTEM_DETAIL(xSubsystem, xMsg)
#define ZS_LOG_SUBSYSTEM_DEBUG(xSubsystem, xMsg)                        ZS_INTERNAL_LOG_SUBSYSTEM_DEBUG(xSubsystem, xMsg)
#define ZS_LOG_SUBSYSTEM_TRACE(xSubsystem,xMsg)                         ZS_INTERNAL_LOG_SUBSYSTEM_TRACE(xSubsystem, xMsg)
#define ZS_LOG_SUBSYSTEM_INSANE(xSubsystem, xMsg)                       ZS_INTERNAL_LOG_SUBSYSTEM_INSANE(xSubsystem, xMsg)

#define ZS_TRACE()                                                      ZS_INTERNAL_LOG_TRACE("[TRACE]")
#define ZS_TRACE_THIS()                                                 ZS_INTERNAL_LOG_TRACE("[TRACE THIS=" + (::zsLib::string((::zsLib::PTRNUMBER)this)) + "]")

#define ZS_TRACE_INSANE()                                               ZS_INTERNAL_LOG_INSANE("[TRACE]")
#define ZS_TRACE_INSANE_THIS()                                          ZS_INTERNAL_LOG_INSANE("[TRACE THIS=" + (::zsLib::string((::zsLib::PTRNUMBER)this)) + "]")

#define ZS_LOG(xLevel, xMsg)                                            ZS_INTERNAL_LOG(xLevel, xMsg)
#define ZS_LOG_WARNING(xLevel, xMsg)                                    ZS_INTERNAL_LOG_WARNING(xLevel, xMsg)
#define ZS_LOG_ERROR(xLevel, xMsg)                                      ZS_INTERNAL_LOG_ERROR(xLevel, xMsg)
#define ZS_LOG_FATAL(xLevel, xMsg)                                      ZS_INTERNAL_LOG_FATAL(xLevel, xMsg)

#define ZS_LOG_SUBSYSTEM(xSubsystem, xLevel, xMsg)                      ZS_INTERNAL_LOG_SUBSYSTEM(xSubsystem, xLevel, xMsg)
#define ZS_LOG_SUBSYSTEM_WARNING(xSubsystem, xLevel, xMsg)              ZS_INTERNAL_LOG_SUBSYSTEM_WARNING(xSubsystem, xLevel, xMsg)
#define ZS_LOG_SUBSYSTEM_ERROR(xSubsystem, xLevel, xMsg)                ZS_INTERNAL_LOG_SUBSYSTEM_ERROR(xSubsystem, xLevel, xMsg)
#define ZS_LOG_SUBSYSTEM_FATAL(xSubsystem, xLevel, xMsg)                ZS_INTERNAL_LOG_SUBSYSTEM_FATAL(xSubsystem, xLevel, xMsg)

#define ZS_LOG_IF(xCond, xLevel, xMsg)                                  ZS_INTERNAL_LOG_IF(xCond, xLevel, xMsg)
#define ZS_LOG_WARNING_IF(xCond, xLevel, xMsg)                          ZS_INTERNAL_LOG_WARNING_IF(xCond, xLevel, xMsg)
#define ZS_LOG_ERROR_IF(xCond, xLevel, xMsg)                            ZS_INTERNAL_LOG_ERROR_IF(xCond, xLevel, xMsg)
#define ZS_LOG_FATAL_IF(xCond, xLevel, xMsg)                            ZS_INTERNAL_LOG_FATAL_IF(xCond, xLevel, xMsg)

namespace zsLib
{
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark Log
  #pragma mark

  class Log : public internal::Log
  {
    friend class internal::Log;

  public:
    typedef InternalAtomIndex EventingAtomIndex;
    typedef InternalAtomData EventingAtomData;
    typedef EventingAtomData * EventingAtomDataArray;
    typedef InternalKeywordBitmaskType KeywordBitmaskType;

    typedef const zsLib::eventing::USE_EVENT_DESCRIPTOR * EVENT_DESCRIPTOR_HANDLE;
    typedef const zsLib::eventing::USE_EVENT_DATA_DESCRIPTOR * EVENT_DATA_DESCRIPTOR_HANDLE;
    typedef const zsLib::eventing::USE_EVENT_PARAMETER_DESCRIPTOR * EVENT_PARAMETER_DESCRIPTOR_HANDLE;

    typedef uint64_t ProviderHandle;

  public:
    enum Severity
    {
      Severity_First      = 0,

      Informational       = Severity_First,
      Info                = Informational,
      Warning             = 1,
      Error               = 2,
      Fatal               = 3,

      Severity_Last       = Fatal
    };

    static const char *toString(Severity severity);
    static Severity toSeverity(const char *severityStr); // throw (Exceptions::InvalidArgument);

    enum Level : LevelBaseType
    {
      Level_First         = 0,

      None                = Level_First,
      Basic               = 1,
      Detail              = 2,
      Debug               = 3,
      Trace               = 4,
      Insane              = 5,

      Level_Last          = Insane,
    };

    static const char *toString(Level level);
    static Level toLevel(const char *levelStr); // throw (Exceptions::InvalidArgument);

    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark Log::Param
    #pragma mark

    class Param
    {
    public:
      Param(const Param &param);
      Param(const char *name, const char *value, bool isNumber = false);
      Param(const char *name, const String &value, bool isNumber = false);
      Param(const char *name, XML::ElementPtr value);
      Param(const char *name, bool value);
      Param(const char *name, CHAR value);
      Param(const char *name, UCHAR value);
      Param(const char *name, SHORT value);
      Param(const char *name, USHORT value);
      Param(const char *name, INT value);
      Param(const char *name, UINT value);
      Param(const char *name, LONG value);
      Param(const char *name, ULONG value);
      Param(const char *name, LONGLONG value);
      Param(const char *name, ULONGLONG value);
      Param(const char *name, FLOAT value);
      Param(const char *name, DOUBLE value);
      Param(const char *name, const Time &value);
      Param(const char *name, const Hours &value);
      Param(const char *name, const Minutes &value);
      Param(const char *name, const Seconds &value);
      Param(const char *name, const Milliseconds &value);
      Param(const char *name, const Microseconds &value);
      Param(const char *name, const Nanoseconds &value);

      const XML::ElementPtr &param() const;

    private:
      XML::ElementPtr mParam;
    };

    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark Log::Params
    #pragma mark

    class Params
    {
    public:
      Params();
      Params(const char *message, XML::ElementPtr object  = XML::ElementPtr());
      Params(const String &message, XML::ElementPtr object = XML::ElementPtr());
      Params(const char *message, const char *staticObjectName);
      Params(const String &message, const char *staticObjectName);
      Params(const Params &params);

      Params &operator<<(const XML::ElementPtr &param);
      Params &operator<<(const Param &param);
      Params &operator<<(const Params &params);

      Params &operator+(const XML::ElementPtr &param) {return *this << param;}
      Params &operator+(const Param &param)           {return *this << param;}

      const String &message() const;
      const XML::ElementPtr &object() const;
      const XML::ElementPtr &params() const;

    protected:
      XML::ElementPtr mObject;
      XML::ElementPtr mParams;
      String mMessage;
    };

  public:
    ~Log();

    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark Log => (output methods)
    #pragma mark

    static void addOutputListener(ILogOutputDelegatePtr delegate);
    static void removeOutputListener(ILogOutputDelegatePtr delegate);

    static void notifyNewSubsystem(Subsystem *inSubsystem);

    static void setOutputLevelByName(
                                     const char *subsystemName,
                                     Level level
                                     );
    static void log(
                    const Subsystem &subsystem,
                    Severity severity,
                    Level level,
                    const String &message,
                    CSTR function,
                    CSTR filePath,
                    ULONG lineNumber
                    );

    static void log(
                    const Subsystem &subsystem,
                    Severity severity,
                    Level level,
                    const Params &params,
                    CSTR function,
                    CSTR filePath,
                    ULONG lineNumber
                    );

    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark Log => (eventing methods)
    #pragma mark

    static void addEventingListener(ILogEventingDelegatePtr delegate);
    static void removeEventingListener(ILogEventingDelegatePtr delegate);

    static void addEventingProviderListener(ILogEventingProviderDelegatePtr delegate);
    static void removeEventingProviderListener(ILogEventingProviderDelegatePtr delegate);

    static EventingAtomIndex registerEventingAtom(const char *atomNamespace); // a result of "0" is an error

    static ProviderHandle registerEventingWriter(
                                                 const char *providerID,
                                                 const char *providerName,
                                                 const char *uniqueProviderHash
                                                 );
    static ProviderHandle registerEventingWriter(
                                                 const UUID &providerID,
                                                 const char *providerName,
                                                 const char *uniqueProviderHash
                                                 );
    static void unregisterEventingWriter(ProviderHandle providerHandle);

    static bool getEventingWriterInfo(
                                      ProviderHandle handle,
                                      UUID &outProviderID,
                                      String &outProviderName,
                                      String &outUniqueProviderHash,
                                      EventingAtomDataArray *outArray = NULL
                                      );
    
    static void setEventingLevelByName(
                                       const char *subsystemName,
                                       Level level
                                       );

    static void writeEvent(
                           ProviderHandle handle,
                           Severity severity,
                           Level level,
                           EVENT_DESCRIPTOR_HANDLE descriptor,
                           EVENT_PARAMETER_DESCRIPTOR_HANDLE paramDescriptor,
                           EVENT_DATA_DESCRIPTOR_HANDLE dataDescriptor,
                           size_t dataDescriptorCount
                           );

    static bool isEventingLogging(
                                  ProviderHandle handle,
                                  KeywordBitmaskType bitmask
                                  )                                            { return (0 != handle) && (0 != (bitmask & (reinterpret_cast<EventingWriter *>(handle)->mKeywordsBitmask))); }
    static void setEventingLogging(
                                   ProviderHandle handle,
                                   PUID enablingObjectID,
                                   bool enabled,
                                   KeywordBitmaskType bitmask = ZSLIB_LOG_PROVIDER_KEYWORDS_ALL
                                   );

  public:
    Log(const make_private &);

  protected:

    static LogPtr singleton();
    static LogPtr create();
  };

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark ILogOutputDelegate
  #pragma mark

  interaction ILogOutputDelegate
  {
  public:
    // notification that a new subsystem exists
    virtual void notifyNewSubsystem(zsLib::Subsystem &inSubsystem) {}

    // notification of a log event
    virtual void notifyLog(
                           const zsLib::Subsystem &inSubsystem,
                           zsLib::Log::Severity inSeverity,
                           zsLib::Log::Level inLevel,
                           zsLib::CSTR inFunction,
                           zsLib::CSTR inFilePath,
                           zsLib::ULONG inLineNumber,
                           const zsLib::Log::Params &params
                           ) {}
  };

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark ILogEventingProviderDelegate
  #pragma mark

  interaction ILogEventingProviderDelegate
  {
    typedef Log::EventingAtomDataArray EventingAtomDataArray;
    typedef Log::ProviderHandle ProviderHandle;
    typedef Log::EventingAtomData EventingAtomData;
    typedef Log::KeywordBitmaskType KeywordBitmaskType;

    // notification that a new subsystem exists
    virtual void notifyNewSubsystem(zsLib::Subsystem &inSubsystem) {}

    // notification of a log event
    virtual void notifyEventingProviderRegistered(
                                                  ProviderHandle handle,
                                                  EventingAtomDataArray eventingAtomDataArray
                                                  ) {}
    virtual void notifyEventingProviderUnregistered(
                                                    ProviderHandle handle,
                                                    EventingAtomDataArray eventingAtomDataArray
                                                    ) {}
    
    virtual void notifyEventingProviderLoggingStateChanged(
                                                           ProviderHandle handle,
                                                           EventingAtomDataArray eventingAtomDataArray,
                                                           KeywordBitmaskType keywords
                                                           ) {}
  };

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark ILogEventingDelegate
  #pragma mark

  interaction ILogEventingDelegate
  {
    typedef Log::EVENT_DESCRIPTOR_HANDLE EVENT_DESCRIPTOR_HANDLE;
    typedef Log::EVENT_PARAMETER_DESCRIPTOR_HANDLE EVENT_PARAMETER_DESCRIPTOR_HANDLE;
    typedef Log::EVENT_DATA_DESCRIPTOR_HANDLE EVENT_DATA_DESCRIPTOR_HANDLE;
    typedef Log::EventingAtomDataArray EventingAtomDataArray;
    typedef Log::ProviderHandle ProviderHandle;
    typedef Log::EventingAtomData EventingAtomData;

    typedef Log::Severity Severity;
    typedef Log::Level Level;

    // notification that a new subsystem exists
    virtual void notifyNewSubsystem(zsLib::Subsystem &inSubsystem) {}

    // notification of a log event
    virtual void notifyWriteEvent(
                                  ProviderHandle handle,
                                  EventingAtomDataArray eventingAtomDataArray,
                                  Severity severity,
                                  Level level,
                                  EVENT_DESCRIPTOR_HANDLE descriptor,
                                  EVENT_PARAMETER_DESCRIPTOR_HANDLE paramDescriptor,
                                  EVENT_DATA_DESCRIPTOR_HANDLE dataDescriptor,
                                  size_t dataDescriptorCount
                                  ) {}
  };

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark Subsystem
  #pragma mark

  class Subsystem
  {
  public:
    typedef Log::Level LevelType;

  public:
    Subsystem(
              CSTR inName,
              Log::Level inOutputLevel = Log::Basic,
              Log::Level inEventingLevel = Log::None
              );
    CSTR getName() const {return mSubsystem;}

    void setOutputLevel(Log::Level inLevel);
    Log::Level getOutputLevel() const;

    void setEventingLevel(Log::Level inLevel);
    Log::Level getEventingLevel() const;

  protected:
    virtual void notifyNewSubsystem();

  private:
    CSTR mSubsystem;
    mutable std::atomic<LevelType> mOutputLevel;
    mutable std::atomic<LevelType> mEventingLevel;
  };

} // namespace zsLib
