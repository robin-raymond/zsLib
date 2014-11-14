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

#if !defined(ZSLIB_INTERNAL_LOG_H_bae05c798f0d8589029db417219553a9)
#define ZSLIB_INTERNAL_LOG_H_bae05c798f0d8589029db417219553a9

#include <zsLib/types.h>
#include <zsLib/Stringize.h>
#include <list>

namespace zsLib
{
  class String;
  class Exception;

  class Subsystem;

  namespace internal
  {
    class Log
    {
    protected:
      RecursiveLock mLock;
      typedef std::list<ILogDelegatePtr> ListenerList;
      ZS_DECLARE_PTR(ListenerList)

      ListenerListPtr mListeners;

      typedef std::list<Subsystem *> SubsystemList;
      SubsystemList mSubsystems;
    };
  }
}


#define ZS_INTERNAL_DECLARE_FORWARD_SUBSYSTEM(xSubsystem) \
  ::zsLib::Subsystem &get##xSubsystem##Subsystem();       \

#define ZS_INTERNAL_DECLARE_SUBSYSTEM(xSubsystem)         \
  ::zsLib::Subsystem &get##xSubsystem##Subsystem();       \
  static ::zsLib::Subsystem &getCurrentSubsystem() {      \
    return get##xSubsystem##Subsystem();                  \
  }

#define ZS_INTERNAL_IMPLEMENT_SUBSYSTEM(xSubsystem) \
  class xSubsystem##Subsystem : public ::zsLib::Subsystem \
  {                                                       \
  public:                                                 \
    xSubsystem##Subsystem(::zsLib::CSTR inName = #xSubsystem) : ::zsLib::Subsystem(inName) {notifyNewSubsystem();} \
  };                                                      \
  ::zsLib::Subsystem &get##xSubsystem##Subsystem() {      \
    static xSubsystem##Subsystem subsystem;               \
    return subsystem;                                     \
    }

#define ZS_INTERNAL_GET_SUBSYSTEM()                                     (getCurrentSubsystem())
#define ZS_INTERNAL_GET_OTHER_SUBSYSTEM(xNamespace, xSubsystem)         (xNamespace::get##xSubsystem##Subsystem())

#define ZS_INTERNAL_LOG_PARAMS(xMsg)                                    (::zsLib::Log::Params(xMsg))
#define ZS_INTERNAL_PARAM(xName, xValue)                                (::zsLib::Log::Param(xName, xValue))

#define ZS_INTERNAL_FUNCTION_FILE_LINE                                  __FUNCTION__, __FILE__, __LINE__

#define ZS_INTERNAL_GET_LOG_LEVEL()                                     ((ZS_GET_SUBSYSTEM()).getOutputLevel())
#define ZS_INTERNAL_GET_SUBSYSTEM_LOG_LEVEL(xSubsystem)                 ((xSubsystem).getOutputLevel())
#define ZS_INTERNAL_IS_LOGGING(xLevel)                                  (((ZS_GET_SUBSYSTEM()).getOutputLevel()) >= ::zsLib::Log::xLevel)
#define ZS_INTERNAL_IS_SUBSYSTEM_LOGGING(xSubsystem, xLevel)            (((xSubsystem).getOutputLevel()) >= ::zsLib::Log::xLevel)

#define ZS_INTERNAL_LOG_SUBSYSTEM_BASIC(xSubsystem, xMsg)               if (ZS_INTERNAL_IS_LOGGING(Basic))  {::zsLib::Log::log((xSubsystem), ::zsLib::Log::Informational, ::zsLib::Log::Basic, ::zsLib::Log::Params(xMsg), ZS_INTERNAL_FUNCTION_FILE_LINE);}
#define ZS_INTERNAL_LOG_SUBSYSTEM_DETAIL(xSubsystem, xMsg)              if (ZS_INTERNAL_IS_LOGGING(Detail)) {::zsLib::Log::log((xSubsystem), ::zsLib::Log::Informational, ::zsLib::Log::Detail, ::zsLib::Log::Params(xMsg), ZS_INTERNAL_FUNCTION_FILE_LINE);}
#define ZS_INTERNAL_LOG_SUBSYSTEM_DEBUG(xSubsystem, xMsg)               if (ZS_INTERNAL_IS_LOGGING(Debug))  {::zsLib::Log::log((xSubsystem), ::zsLib::Log::Informational, ::zsLib::Log::Debug, ::zsLib::Log::Params(xMsg), ZS_INTERNAL_FUNCTION_FILE_LINE);}
#define ZS_INTERNAL_LOG_SUBSYSTEM_TRACE(xSubsystem, xMsg)               if (ZS_INTERNAL_IS_LOGGING(Trace))  {::zsLib::Log::log((xSubsystem), ::zsLib::Log::Informational, ::zsLib::Log::Trace, ::zsLib::Log::Params(xMsg), ZS_INTERNAL_FUNCTION_FILE_LINE);}
#define ZS_INTERNAL_LOG_SUBSYSTEM_INSANE(xSubsystem, xMsg)              if (ZS_INTERNAL_IS_LOGGING(Insane)) {::zsLib::Log::log((xSubsystem), ::zsLib::Log::Informational, ::zsLib::Log::Insane, ::zsLib::Log::Params(xMsg), ZS_INTERNAL_FUNCTION_FILE_LINE);}

#define ZS_INTERNAL_LOG_FORCED(xSeverity, xLevel, xMsg)                 {::zsLib::Log::log(ZS_GET_SUBSYSTEM(), ::zsLib::Log::xSeverity, ::zsLib::Log::xLevel, ::zsLib::Log::Params(xMsg), ZS_INTERNAL_FUNCTION_FILE_LINE);}

#define ZS_INTERNAL_LOG_BASIC(xMsg)                                     ZS_INTERNAL_LOG_SUBSYSTEM_BASIC(ZS_GET_SUBSYSTEM(), xMsg)
#define ZS_INTERNAL_LOG_DETAIL(xMsg)                                    ZS_INTERNAL_LOG_SUBSYSTEM_DETAIL(ZS_GET_SUBSYSTEM(), xMsg)
#define ZS_INTERNAL_LOG_DEBUG(xMsg)                                     ZS_INTERNAL_LOG_SUBSYSTEM_DEBUG(ZS_GET_SUBSYSTEM(), xMsg)
#define ZS_INTERNAL_LOG_TRACE(xMsg)                                     ZS_INTERNAL_LOG_SUBSYSTEM_TRACE(ZS_GET_SUBSYSTEM(), xMsg)
#define ZS_INTERNAL_LOG_INSANE(xMsg)                                    ZS_INTERNAL_LOG_SUBSYSTEM_INSANE(ZS_GET_SUBSYSTEM(), xMsg)

#define ZS_INTERNAL_LOG_SUBSYSTEM(xSubsystem, xLevel, xMsg)             if (ZS_INTERNAL_IS_LOGGING(xLevel)) {::zsLib::Log::log((xSubsystem), ::zsLib::Log::Informational, ::zsLib::Log::xLevel, ::zsLib::Log::Params(xMsg), ZS_INTERNAL_FUNCTION_FILE_LINE);}
#define ZS_INTERNAL_LOG_SUBSYSTEM_WARNING(xSubsystem, xLevel, xMsg)     if (ZS_INTERNAL_IS_LOGGING(xLevel)) {::zsLib::Log::log((xSubsystem), ::zsLib::Log::Warning, ::zsLib::Log::xLevel, ::zsLib::Log::Params(xMsg), ZS_INTERNAL_FUNCTION_FILE_LINE);}
#define ZS_INTERNAL_LOG_SUBSYSTEM_ERROR(xSubsystem, xLevel, xMsg)       if (ZS_INTERNAL_IS_LOGGING(xLevel)) {::zsLib::Log::log((xSubsystem), ::zsLib::Log::Error, ::zsLib::Log::xLevel, ::zsLib::Log::Params(xMsg), ZS_INTERNAL_FUNCTION_FILE_LINE);}
#define ZS_INTERNAL_LOG_SUBSYSTEM_FATAL(xSubsystem, xLevel, xMsg)       if (ZS_INTERNAL_IS_LOGGING(xLevel)) {::zsLib::Log::log((xSubsystem), ::zsLib::Log::Fatal, ::zsLib::Log::xLevel, ::zsLib::Log::Params(xMsg), ZS_INTERNAL_FUNCTION_FILE_LINE);}

#define ZS_INTERNAL_LOG(xLevel, xMsg)                                   ZS_INTERNAL_LOG_SUBSYSTEM(ZS_GET_SUBSYSTEM(), xLevel, xMsg)
#define ZS_INTERNAL_LOG_WARNING(xLevel, xMsg)                           ZS_INTERNAL_LOG_SUBSYSTEM_WARNING(ZS_GET_SUBSYSTEM(), xLevel, xMsg)
#define ZS_INTERNAL_LOG_ERROR(xLevel, xMsg)                             ZS_INTERNAL_LOG_SUBSYSTEM_ERROR(ZS_GET_SUBSYSTEM(), xLevel, xMsg)
#define ZS_INTERNAL_LOG_FATAL(xLevel, xMsg)                             ZS_INTERNAL_LOG_SUBSYSTEM_FATAL(ZS_GET_SUBSYSTEM(), xLevel, xMsg)

#define ZS_INTERNAL_LOG_IF(xCond, xLevel, xMsg)                         if ((xCond) && (ZS_INTERNAL_IS_LOGGING(xLevel))) {ZS_INTERNAL_LOG(xLevel, xMsg)}
#define ZS_INTERNAL_LOG_WARNING_IF(xCond, xLevel, xMsg)                 if ((xCond) && (ZS_INTERNAL_IS_LOGGING(xLevel))) {ZS_INTERNAL_LOG_WARNING(xLevel, xMsg)}
#define ZS_INTERNAL_LOG_ERROR_IF(xCond, xLevel, xMsg)                   if ((xCond) && (ZS_INTERNAL_IS_LOGGING(xLevel))) {ZS_INTERNAL_LOG_ERROR(xLevel, xMsg)}
#define ZS_INTERNAL_LOG_FATAL_IF(xCond, xLevel, xMsg)                   if ((xCond) && (ZS_INTERNAL_IS_LOGGING(xLevel))) {ZS_INTERNAL_LOG_FATAL(xLevel, xMsg)}

#endif //ZS_INTERNAL_LOG_H_bae05c798f0d8589029db417219553a9

