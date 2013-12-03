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

#include <zsLib/Event.h>
#include <zsLib/helpers.h>
#include <zsLib/Exception.h>
#include <zsLib/Log.h>
#include <zsLib/XML.h>

namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib)}

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
    if (expression) {
      mParams << ZS_PARAM("expression", expression);
    }

    // force log of event
    Log::singleton()->log(subsystem, Log::Error, Log::Basic, mParams, function, filePath, lineNumber);
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
    if (expression) {
      mParams << ZS_PARAM("expression", expression);
    }

    // force log of event
    Log::singleton()->log(subsystem, Log::Error, Log::Basic, mParams, function, filePath, lineNumber);
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
    if (expression) {
      mParams << ZS_PARAM("expression", expression);
    }

    // force log of event
    Log::singleton()->log(subsystem, Log::Error, Log::Basic, mParams, function, filePath, lineNumber);
  }

  //---------------------------------------------------------------------------
  Log::Params Exception::params() const
  {
    return mParams;
  }
}
