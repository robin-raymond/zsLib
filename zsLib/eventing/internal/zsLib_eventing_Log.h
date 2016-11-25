/*

 Copyright (c) 2016, Robin Raymond
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

#ifndef ZSLIB_EVENTING_INTERNAL_LOG_H_b7e3d951af94efd0359c51a8776f025e0562dd4e
#define ZSLIB_EVENTING_INTERNAL_LOG_H_b7e3d951af94efd0359c51a8776f025e0562dd4e

#define ZS_EVENTING_INTERNAL_GET_LOG_LEVEL()                                       ((ZS_GET_SUBSYSTEM()).getEventingLevel())
#define ZS_EVENTING_INTERNAL_GET_SUBSYSTEM_LOG_LEVEL(xSubsystem)                   ((xSubsystem).getEventingLevel())
#define ZS_EVENTING_INTERNAL_IS_LOGGING(xLevel)                                    (((ZS_GET_SUBSYSTEM()).getEventingLevel()) >= ::zsLib::Log::xLevel)
#define ZS_EVENTING_INTERNAL_IS_LOGGING_VALUE(xLevelValue)                         (((ZS_GET_SUBSYSTEM()).getEventingLevel()) >= (xLevelValue))
#define ZS_EVENTING_INTERNAL_IS_SUBSYSTEM_LOGGING(xSubsystem, xLevel)              (((xSubsystem).getEventingLevel()) >= ::zsLib::Log::xLevel)

#define ZS_EVENTING_INTERNAL_REGISTER_EVENT_WRITER(xHandleReference, xProviderID, xProviderName, xUniqueProviderHash) 
#define ZS_EVENTING_INTERNAL_UNREGISTER_EVENT_WRITER(xHandleReference)                           


namespace zsLib
{
  namespace eventing
  {
    namespace internal
    {
    }
  }
}

#endif //ZSLIB_EVENTING_INTERNAL_LOG_H_b7e3d951af94efd0359c51a8776f025e0562dd4e
