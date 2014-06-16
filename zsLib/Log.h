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

#ifndef ZSLIB_LOG_H_47d9d31085744072b865483d323d7b02
#define ZSLIB_LOG_H_47d9d31085744072b865483d323d7b02

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
    enum Severity
    {
      Informational,
      Warning,
      Error,
      Fatal
    };

    static const char *toString(Severity severity);
    static Severity toSeverity(const char *severityStr);

    enum Level
    {
      None,
      Basic,
      Detail,
      Debug,
      Trace,
      Insane
    };

    static const char *toString(Level level);
    static Level toLevel(const char *levelStr);

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
      Param(const char *name, const Duration &value);

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
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark Log => (methods)
    #pragma mark

    ~Log();

    static void addListener(ILogDelegatePtr delegate);
    static void removeListener(ILogDelegatePtr delegate);

    static void notifyNewSubsystem(Subsystem *inSubsystem);

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

  protected:
    Log();

    static LogPtr singleton();
    static LogPtr create();
  };

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark ILogDelegate
  #pragma mark

  interaction ILogDelegate
  {
  public:
    // notification that a new subsystem exists
    virtual void onNewSubsystem(zsLib::Subsystem &inSubsystem) {}

    // notification of a log event
    virtual void onLog(
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
  #pragma mark Subsystem
  #pragma mark

  class Subsystem
  {
  public:
    typedef DWORD LevelType;

  public:
    Subsystem(CSTR inName, Log::Level inLevel = Log::Basic);
    CSTR getName() const {return mSubsystem;}

    void setOutputLevel(Log::Level inLevel);
    Log::Level getOutputLevel() const;

  protected:
    virtual void notifyNewSubsystem();

  private:
    CSTR mSubsystem;
    mutable LevelType mLevel;
  };

} // namespace zsLib

#endif //ZSLIB_LOG_H_47d9d31085744072b865483d323d7b02
