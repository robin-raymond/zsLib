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

#ifdef DEBUG
#ifndef _DEBUG
#define _DEBUG 1
#endif //_DEBUG
#endif //DEBUG

#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG 1
#endif //DEBUG
#endif //_DEBUG

#ifdef _WIN32

#ifdef __cplusplus_winrt
#undef WINRT
#define WINRT
#endif //__cplusplus_winrt

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <rpc.h>
#include <stdint.h>
#endif //_WIN32

#include <atomic>
#include <limits.h>
#include <chrono>
#include <thread>
#include <mutex>
#ifndef _WIN32
#include <cstring>
#include <uuid/uuid.h>
#endif //_WIN32

//#ifndef interface
//#define interface struct
//#endif //interface

#ifndef interaction
#define interaction struct
#endif //interaction

#include <memory>

#define ZS_INTERNAL_SMART_POINTER_NAMESPACE std

#define ZS_INTERNAL_DECLARE_PTR(xExistingType)                                                      \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::shared_ptr<xExistingType> xExistingType##Ptr;        \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::weak_ptr<xExistingType> xExistingType##WeakPtr;      \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::unique_ptr<xExistingType> xExistingType##UniPtr;

#define ZS_INTERNAL_DECLARE_USING_PTR(xNamespace, xExistingType)                                    \
  using xNamespace::xExistingType;                                                                  \
  using xNamespace::xExistingType##Ptr;                                                             \
  using xNamespace::xExistingType##WeakPtr;                                                         \
  using xNamespace::xExistingType##UniPtr;

#define ZS_INTERNAL_DECLARE_CLASS_PTR(xClassName)                                                   \
  class xClassName;                                                                                 \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::shared_ptr<xClassName> xClassName##Ptr;              \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::weak_ptr<xClassName> xClassName##WeakPtr;            \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::unique_ptr<xClassName> xClassName##UniPtr;

#define ZS_INTERNAL_DECLARE_STRUCT_PTR(xStructName)                                                 \
  struct xStructName;                                                                               \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::shared_ptr<xStructName> xStructName##Ptr;            \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::weak_ptr<xStructName> xStructName##WeakPtr;          \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::unique_ptr<xStructName> xStructName##UniPtr;

#define ZS_INTERNAL_DECLARE_TYPEDEF_PTR(xOriginalType, xNewTypeName)                                \
  typedef xOriginalType xNewTypeName;                                                               \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::shared_ptr<xNewTypeName> xNewTypeName##Ptr;          \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::weak_ptr<xNewTypeName> xNewTypeName##WeakPtr;        \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::unique_ptr<xNewTypeName> xNewTypeName##UniPtr;

#define ZS_INTERNAL_DYNAMIC_PTR_CAST(xType, xObject)                                                \
  ZS_INTERNAL_SMART_POINTER_NAMESPACE::dynamic_pointer_cast<xType>(xObject)

#ifdef _WIN32
namespace std
{
	typedef intmax_t intmax_t;
	typedef uintmax_t uintmax_t;
}
#define alignof(xValue) __alignof(xValue)

#if ULLONG_MAX == 0xFFFFFFFFFFFFFFFF
typedef ULONGLONG QWORD;

#else //ULLONG_MAX == 0xFFFFFFFFFFFFFFFF

#if ULONG_MAX == 0xFFFFFFFFFFFFFFFF
typedef ULONG QWORD;
#endif //ULONG_MAX == 0xFFFFFFFFFFFFFFFF

#endif //ULONG_MAX == 0xFFFFFFFFFFFFFFFF

#endif //_WIN32

namespace zsLib
{
#ifdef _WIN32
  using ::CHAR;
  using ::UCHAR;

  using ::SHORT;
  using ::USHORT;

  using ::INT;
  using ::UINT;

  using ::LONG;
  using ::ULONG;

  using ::LONGLONG;
  using ::ULONGLONG;

  using ::BYTE;
  using ::WORD;
  using ::DWORD;
  using ::QWORD;
  
  typedef SSIZE_T ssize_t;

#else
  typedef char CHAR;
  typedef unsigned char UCHAR;

  typedef short SHORT;
  typedef unsigned short USHORT;

  typedef int INT;
  typedef unsigned int UINT;

  typedef long LONG;
  typedef unsigned long ULONG;

  typedef long long LONGLONG;
  typedef unsigned long long ULONGLONG;

#if UCHAR_MAX == 0xFF
  typedef UCHAR BYTE;
#endif //UCHAR_MAX == 0xFF

#if USHRT_MAX == 0xFFFF
  typedef USHORT WORD;
#endif

#if UINT_MAX == 0xFFFFFFFF
  typedef UINT DWORD;
#endif //UINT_MAX == 0xFFFFFFFF

#if ULONG_MAX == 0xFFFFFFFFFFFFFFFF
  typedef ULONG QWORD;

#else //ULONG_MAX == 0xFFFFFFFFFFFFFFFF

#if ULLONG_MAX == 0xFFFFFFFFFFFFFFFF
  typedef ULONGLONG QWORD;
#endif

#endif //ULONG_MAX == 0xFFFFFFFFFFFFFFFF

#endif //_WIN32

  typedef std::intmax_t LONGEST;
  typedef std::uintmax_t ULONGEST;

  typedef float FLOAT;
  typedef double DOUBLE;

  typedef ULONG PUID;

#if defined(__LP64__) || defined(_WIN64) || defined(_Wp64)
#define ZSLIB_64BIT
#else
#define ZSLIB_32BIT
#endif //defined(__LP64__) || defined(_WIN64) || defined(_Wp64)

  typedef uintptr_t PTRNUMBER;
  typedef PTRNUMBER USERPARAM;

  namespace internal
  {
    struct uuid_wrapper
    {
		  typedef UCHAR * iterator;
		  typedef UCHAR const* const_iterator;

#ifndef _WIN32
      typedef uuid_t raw_uuid_type;
	    raw_uuid_type mUUID{};

	    iterator begin() { return mUUID; }
	    const_iterator begin() const { return mUUID; }
	    iterator end() { return mUUID + sizeof(raw_uuid_type); }
	    const_iterator end() const { return mUUID + sizeof(raw_uuid_type); }
      size_t size() const { return sizeof(raw_uuid_type); }
#else
	    typedef GUID raw_uuid_type;
	    raw_uuid_type mUUID {};

	    iterator begin() { return (iterator) (&mUUID); }
	    const_iterator begin() const { return (iterator)(&mUUID); }
	    iterator end() { return begin() + sizeof(raw_uuid_type); }
	    const_iterator end() const { return begin() + sizeof(raw_uuid_type); }
      size_t size() const { return sizeof(raw_uuid_type); }

	    static void uuid_clear(raw_uuid_type &uuid)
      {
		    memset(&uuid, 0, sizeof(uuid));
	    }

	    static void uuid_copy(raw_uuid_type &dest, const raw_uuid_type &source)
      {
		    memcpy(&dest, &source, sizeof(dest));
	    }

	    static int uuid_compare(const raw_uuid_type &op1, const raw_uuid_type &op2)
      {
		    return memcmp(&op1, &op2, sizeof(op1));
	    }

	    static bool uuid_is_null(const raw_uuid_type &op)
      {
        struct ClearUUID
        {
          ClearUUID() {uuid_clear(mEmpty);}
        
          const raw_uuid_type &value() const {return mEmpty;}
          raw_uuid_type mEmpty;
        };
        static ClearUUID emptyUUID;
		    return 0 == memcmp(&op, &(emptyUUID.value()), sizeof(op));
	    }
#endif //ndef _WIN32

      uuid_wrapper() {
        uuid_clear(mUUID);
      }

      uuid_wrapper(const uuid_wrapper &op2) {
        uuid_copy(mUUID, op2.mUUID);
      }

      int compare(const uuid_wrapper &op2) const {
        return uuid_compare(mUUID, op2.mUUID);
      }

      bool operator!() const {
        return uuid_is_null(mUUID);
      }

      uuid_wrapper &operator=(const uuid_wrapper &op2) {
        uuid_copy(mUUID, op2.mUUID);
        return *this;
      }

      bool operator==(const uuid_wrapper &op2) const {
        return 0 == uuid_compare(mUUID, op2.mUUID);
      }
      bool operator!=(const uuid_wrapper &op2) const {
        return 0 != uuid_compare(mUUID, op2.mUUID);
      }
      bool operator<(const uuid_wrapper &op2) const {
        return uuid_compare(mUUID, op2.mUUID) < 0;
      }
      bool operator>(const uuid_wrapper &op2) const {
        return uuid_compare(mUUID, op2.mUUID) > 0;
      }
      bool operator<=(const uuid_wrapper &op2) const {
        return uuid_compare(mUUID, op2.mUUID) <= 0;
      }
      bool operator>=(const uuid_wrapper &op2) const {
        return uuid_compare(mUUID, op2.mUUID) >= 0;
      }
    };
  }

  typedef internal::uuid_wrapper UUID;
  
  typedef char CHAR;
  typedef wchar_t WCHAR;

#ifdef UNICODE
  typedef WCHAR TCHAR;
#else
  typedef CHAR TCHAR;
#endif //UNICODE


#if (__WCHAR_MAX__ == 0xFFFFFFFF) || (__WCHAR_MAX__ == 0x7FFFFFFF)

#define ZS_TARGET_WCHAR_IS_UTF32

#else // (__WCHAR_MAX__ == 0xFFFFFFFF) || (__WCHAR_MAX__ == 0x7FFFFFFF)

#if (__WCHAR_MAX__ == 0xFFFF) || (__WCHAR_MAX__ == 0x7FFF)

#define ZS_TARGET_WCHAR_IS_UTF16

#else //(__WCHAR_MAX__ == 0xFFFF) || (__WCHAR_MAX__ == 0x7FFF)

#ifdef _WIN32
#define ZS_TARGET_WCHAR_IS_UTF16
#else
#error Unknown WCHAR size. Is this a UTF16 or UTF32 platform?
#endif //_WIN32

#endif // (__WCHAR_MAX__ == 0xFFFF) || (__WCHAR_MAX__ == 0x7FFF)

#endif // (__WCHAR_MAX__ == 0xFFFFFFFF) || (__WCHAR_MAX__ == 0x7FFFFFFF)

  typedef char * STR;
  typedef const char * CSTR;

  typedef TCHAR * TSTR;
  typedef const TCHAR * CTSTR;

  typedef wchar_t * WSTR;
  typedef const wchar_t * CWSTR;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

#define ZS_TARGET_LITTLE_ENDIAN

#else

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define ZS_TARGET_BIG_ENDIAN
#endif //__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

#endif //__ORDER_BIG_ENDIAN__

  namespace internal
  {
    ZS_INTERNAL_DECLARE_CLASS_PTR(Settings);
    ZS_INTERNAL_DECLARE_CLASS_PTR(Timer);
    ZS_INTERNAL_DECLARE_CLASS_PTR(MessageQueue);
    ZS_INTERNAL_DECLARE_CLASS_PTR(MessageQueueThread);
    ZS_INTERNAL_DECLARE_CLASS_PTR(MessageQueueThreadPool);
    ZS_INTERNAL_DECLARE_CLASS_PTR(MessageQueueManager);
  }

}
