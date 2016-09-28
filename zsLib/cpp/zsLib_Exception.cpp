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

#include <zsLib/Event.h>
#include <zsLib/helpers.h>
#include <zsLib/Exception.h>
#include <zsLib/Log.h>
#include <zsLib/XML.h>

#include <zsLib/internal/zsLib_Tracing.h>

//namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib)}

namespace zsLib
{
  namespace internal
  {
  }

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark Exception
  #pragma mark

  //---------------------------------------------------------------------------
  Exception::Exception(
                       const Subsystem &subsystem,
                       CSTR message,
                       CSTR function,
                       CSTR filePath,
                       ULONG lineNumber,
                       const char *expression
                       ) :
    mSubsystem(subsystem),
    mMessage(message),
    mFunction(function),
    mFilePath(filePath),
    mLineNumber(lineNumber)
  {
    EventWriteZsExceptionEventFired(__func__, subsystem.getName(), message, function, filePath, lineNumber, expression);

    if (expression) {
      mParams << ZS_PARAM("expression", expression);
    }

    // force log of event
    Log::log(subsystem, Log::Error, Log::Basic, mParams, function, filePath, lineNumber);
  }

  //---------------------------------------------------------------------------
  Exception::Exception(
                       const Subsystem &subsystem,
                       const String &message,
                       CSTR function,
                       CSTR filePath,
                       ULONG lineNumber,
                       const char *expression
                       ) :
    mSubsystem(subsystem),
    mMessage(message),
    mFunction(function),
    mFilePath(filePath),
    mLineNumber(lineNumber)
  {
    EventWriteZsExceptionEventFired(__func__, subsystem.getName(), message, function, filePath, lineNumber, expression);

    if (expression) {
      mParams << ZS_PARAM("expression", expression);
    }

    // force log of event
    Log::log(subsystem, Log::Error, Log::Basic, mParams, function, filePath, lineNumber);
  }

  //---------------------------------------------------------------------------
  Exception::Exception(
                       const Subsystem &subsystem,
                       const Log::Params &params,
                       CSTR function,
                       CSTR filePath,
                       ULONG lineNumber,
                       const char *expression
                       ) :
    mSubsystem(subsystem),
    mMessage(params.message()),
    mFunction(function),
    mFilePath(filePath),
    mLineNumber(lineNumber),
    mParams(params)
  {
    EventWriteZsExceptionEventFired(__func__, subsystem.getName(), params.message(), function, filePath, lineNumber, expression);

    if (expression) {
      mParams << ZS_PARAM("expression", expression);
    }

    // force log of event
    Log::log(subsystem, Log::Error, Log::Basic, mParams, function, filePath, lineNumber);
  }

  //---------------------------------------------------------------------------
  Log::Params Exception::params() const
  {
    return mParams;
  }
}
