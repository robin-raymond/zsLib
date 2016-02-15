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

#ifndef ZSLIB_INTERNAL_TRACING_H_8b06ec5c28c8379b62d6e50c7cb83df793d6414c
#define ZSLIB_INTERNAL_TRACING_H_8b06ec5c28c8379b62d6e50c7cb83df793d6414c

#include <zsLib/types.h>
#include <zsLib/helpers.h>
#include <type_traits>

#ifdef USE_ETW
#include "zsLib_ETWTracing.h"
#else

// Comment the following line to test inline versions of the same macros to test compilation
#define ZSLIB_INTERNAL_USE_NOOP_EVENT_TRACE_MACROS

// NO-OP VERSIONS OF ALL TRACING MACROS
#ifdef ZSLIB_INTERNAL_USE_NOOP_EVENT_TRACE_MACROS

#define EventWriteZsExceptionEventFired(xStr_Method, xStr_Subsystem, xStr_Message, xStr_Function, xStr_FilePath, xULONG_LineNumber, xStr_Expression) {}

#define EventWriteZsMessageQueueCreate(xStr_Method, xPtr_this)
#define EventWriteZsMessageQueueDestroy(xStr_Method, xPtr_this)
#define EventWriteZsMessageQueuePost(xStr_Method, xPtr_this)
#define EventWriteZsMessageQueueProcess(xStr_Method, xPtr_this)
#define EventWriteZsMessageQueueTotalUnprocessedMessages(xStr_Method, xPtr_this, xsize_t_Total)

#define EventWriteZsSocketError(xStr_Method, xSocket, xInt_Error)
#define EventWriteZsSocketWouldBlock(xStr_Method, xSocket, xBool_WouldBlock)
#define EventWriteZsSocketCreate(xStr_Method, xPtr_this, xSocket, xUInt_Family, xUInt_Type, xUInt_Protocol)
#define EventWriteZsSocketOrphan(xStr_Method, xSocket)
#define EventWriteZsSocketAdopt(xStr_Method, xSocket)
#define EventWriteZsSocketNotifyEventFired(xStr_Method, xPtr_this, xStr_EventType)
#define EventWriteZsSocketNotifyEventReset(xStr_Method, xPtr_this, xSocket)
#define EventWriteZsSocketClose(xStr_Method, xSocket, xInt_Result)
#define EventWriteZsSocketGetLocalAddress(xStr_Method, xSocket, xInt_Result, xPtr_Address, xsocklen_t_AddressSize)
#define EventWriteZsSocketGetRemoteAddress(xStr_Method, xSocket, xInt_Result, xPtr_Address, xsocklen_t_AddressSize)
#define EventWriteZsSocketBind(xStr_Method, xSocket, xInt_Result, xPtr_Address, xsocklen_t_AddressSize)
#define EventWriteZsSocketListen(xStr_Method, xSocket, xInt_Result)
#define EventWriteZsSocketAccept(xStr_Method, xSocket_Result, xSocket_Listening, xPtr_Address, xsocklen_t_AddressSize)
#define EventWriteZsSocketConnect(xStr_Method, xSocket, xInt_Result, xPtr_Address, xsocklen_t_AddressSize)
#define EventWriteZsSocketRecv(xStr_Method, xSocket, xsize_t_Result, xULONG_Flags, xPtr_Buffer, xsize_t_BufferLengthInBytes)
#define EventWriteZsSocketRecvFrom(xStr_Method, xSocket, xsize_t_Result, xULONG_Flags, xPtr_Buffer, xsize_t_BufferLengthInBytes, xPtr_Address, xsocklen_t_AddressSize)
#define EventWriteZsSocketSend(xStr_Method, xSocket, xsize_t_Result, xULONG_Flags, xPtr_Buffer, xsize_t_BufferLengthInBytes)
#define EventWriteZsSocketSendTo(xStr_Method, xSocket, xsize_t_Result, xULONG_Flags, xPtr_Buffer, xsize_t_BufferLengthInBytes, xPtr_Address, xsocklen_t_AddressSize)
#define EventWriteZsSocketShutdown(xStr_Method, xSocket, xInt_Result, xUInt_Options)
#define EventWriteZsSocketSetOption(xStr_Method, xSocket, xInt_Result, xInt_Level, xInt_OptionName, xPtr_OptionValue, xsocklen_t_OptionLengthInBytes)
#define EventWriteZsSocketGetOption(xStr_Method, xSocket, xInt_Result, xInt_Level, xInt_OptionName, xPtr_OptionValue, xsocklen_t_OptionLengthInBytes)
#define EventWriteZsSocketSetOptionFlag(xStr_Method, xSocket, xInt_Result, xUInt_Option, xBool_Enabled)
#define EventWriteZsSocketGetOptionFlag(xStr_Method, xSocket, xInt_Result, xUInt_Option, xBool_Enabled)

#define EventWriteZsTimerCreate(xStr_Method, xPtr_this, xPUID, xBool_Repeat, xLONGLONG_TimeoutMicroseconds);
#define EventWriteZsTimerDestroy(xStr_Method, xPtr_this, xPUID);
#define EventWriteZsTimerEventFired(xStr_Method, xPtr_this, xPUID)

#else

// duplicate testing compilation methods used to verify compilation when macros get defined
namespace zsLib {

inline void EventWriteZsExceptionEventFired(const char *xStr_Method, const char *xStr_Subsystem, const char *xStr_Message, const char *xStr_Function, const char *xStr_FilePath, ULONG xULONG_LineNumber, const char *xStr_Expression) {}

inline void EventWriteZsMessageQueueCreate(const char *xStr_Method, void *xPtr_this) {}
inline void EventWriteZsMessageQueueDestroy(const char *xStr_Method, void *xPtr_this) {}
inline void EventWriteZsMessageQueuePost(const char *xStr_Method, void *xPtr_this) {}
inline void EventWriteZsMessageQueueProcess(const char *xStr_Method, void *xPtr_this) {}
inline void EventWriteZsMessageQueueTotalUnprocessedMessages(const char *xStr_Method, const void *xPtr_this, size_t xsize_t_Total) {}

inline void EventWriteZsSocketError(const char *xStr_Method, PTRNUMBER xSocket, int xInt_Error) {}
inline void EventWriteZsSocketWouldBlock(const char *xStr_Method, PTRNUMBER xSocket, bool xBool_WouldBlock) {}
inline void EventWriteZsSocketCreate(const char *xStr_Method, void *pThis, PTRNUMBER xSocket, unsigned xUInt_Family, unsigned xUInt_Type, unsigned xUInt_Protocol) {}
inline void EventWriteZsSocketOrphan(const char *xStr_Method, PTRNUMBER xSocket) {}
inline void EventWriteZsSocketAdopt(const char *xStr_Method, PTRNUMBER xSocket) {}
inline void EventWriteZsSocketNotifyEventFired(const char *xStr_Method, void *pThis, const char *xStr_EventType) {}
inline void EventWriteZsSocketNotifyEventReset(const char *xStr_Method, const void *pThis, PTRNUMBER xSocket) {}
inline void EventWriteZsSocketClose(const char *xStr_Method, PTRNUMBER xSocket, int xInt_Result) {}
inline void EventWriteZsSocketGetLocalAddress(const char *xStr_Method, PTRNUMBER xSocket, int xInt_Result, void *xPtr_Address, size_t xsocklen_t_AddressSize) {}
inline void EventWriteZsSocketGetRemoteAddress(const char *xStr_Method, PTRNUMBER xSocket, int xInt_Result, void *xPtr_Address, size_t xsocklen_t_AddressSize) {}
inline void EventWriteZsSocketBind(const char *xStr_Method, PTRNUMBER xSocket, int xInt_Result, void *xPtr_Address, size_t xsocklen_t_AddressSize) {}
inline void EventWriteZsSocketListen(const char *xStr_Method, PTRNUMBER xSocket, int xInt_Result) {}
inline void EventWriteZsSocketAccept(const char *xStr_Method, PTRNUMBER xSocket_Result, PTRNUMBER xSocket_Listening, void *xPtr_Address, size_t xsocklen_t_AddressSize) {}
inline void EventWriteZsSocketConnect(const char *xStr_Method, PTRNUMBER xSocket, int xInt_Result, void *xPtr_Address, size_t xsocklen_t_AddressSize) {}
inline void EventWriteZsSocketRecv(const char *xStr_Method, PTRNUMBER xSocket, size_t xsize_t_Result, ULONG xULONG_Flags, void *xPtr_Buffer, size_t xsize_t_BufferLengthInBytes) {}
inline void EventWriteZsSocketRecvFrom(const char *xStr_Method, PTRNUMBER xSocket, size_t xsize_t_Result, ULONG xULONG_Flags, void *xPtr_Buffer, size_t xsize_t_BufferLengthInBytes, void *xPtr_Address, size_t xsocklen_t_AddressSize) {}
inline void EventWriteZsSocketSend(const char *xStr_Method, PTRNUMBER xSocket, size_t xsize_t_Result, ULONG xULONG_Flags, const void *xPtr_Buffer, size_t xsize_t_BufferLengthInBytes) {}
inline void EventWriteZsSocketSendTo(const char *xStr_Method, PTRNUMBER xSocket, size_t xsize_t_Result, ULONG xULONG_Flags, const void *xPtr_Buffer, size_t xsize_t_BufferLengthInBytes, void *xPtr_Address, size_t xsocklen_t_AddressSize) {}
inline void EventWriteZsSocketShutdown(const char *xStr_Method, PTRNUMBER xSocket, int xInt_Result, unsigned xUInt_Options) {}
inline void EventWriteZsSocketSetOption(const char *xStr_Method, PTRNUMBER xSocket, int xInt_Result, int xInt_Level, int xInt_OptionName, void *xPtr_OptionValue, size_t xsocklen_t_OptionLengthInBytes) {}
inline void EventWriteZsSocketGetOption(const char *xStr_Method, PTRNUMBER xSocket, int xInt_Result, int xInt_Level, int xInt_OptionName, void *xPtr_OptionValue, size_t xsocklen_t_OptionLengthInBytes) {}
inline void EventWriteZsSocketSetOptionFlag(const char *xStr_Method, PTRNUMBER xSocket, int xInt_Result, unsigned int xUInt_Option, bool xBool_Enabled) {}
inline void EventWriteZsSocketGetOptionFlag(const char *xStr_Method, PTRNUMBER xSocket, int xInt_Result, unsigned int xUInt_Option, bool xBool_Enabled) {}

inline void EventWriteZsTimerCreate(const char *xStr_Method, void *xPtr_this, PUID xPUID, bool xBool_Repeat, long long xLONGLONG_TimeoutMicroseconds) {}
inline void EventWriteZsTimerDestroy(const char *xStr_Method, void *xPtr_this, PUID xPUID) {}
inline void EventWriteZsTimerEventFired(const char *xStr_Method, void *xPtr_this, PUID xPUID) {}
}
#endif //ndef ZSLIB_INTERNAL_USE_NOOP_EVENT_TRACE_MACROS

#endif //USE_ETW


#endif //ZSLIB_INTERNAL_TRACING_H_8b06ec5c28c8379b62d6e50c7cb83df793d6414c
