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

#ifndef ZSLIB_INTERNAL_ZSTYPES_H_b6763c4cc75e565b376883f85c0186de
#define ZSLIB_INTERNAL_ZSTYPES_H_b6763c4cc75e565b376883f85c0186de

#include <limits.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <uuid/uuid.h>


#ifdef _WIN32
#include <winsock2.h>
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
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::weak_ptr<xExistingType> xExistingType##WeakPtr;

#define ZS_INTERNAL_DECLARE_USING_PTR(xNamespace, xExistingType)                                    \
  using xNamespace::xExistingType;                                                                  \
  using xNamespace::xExistingType##Ptr;                                                             \
  using xNamespace::xExistingType##WeakPtr;

#define ZS_INTERNAL_DECLARE_CLASS_PTR(xClassName)                                                   \
  class xClassName;                                                                                 \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::shared_ptr<xClassName> xClassName##Ptr;              \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::weak_ptr<xClassName> xClassName##WeakPtr;

#define ZS_INTERNAL_DECLARE_STRUCT_PTR(xStructName)                                                 \
  struct xStructName;                                                                               \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::shared_ptr<xStructName> xStructName##Ptr;            \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::weak_ptr<xStructName> xStructName##WeakPtr;

#define ZS_INTERNAL_DECLARE_TYPEDEF_PTR(xOriginalType, xNewTypeName)                                \
  typedef xOriginalType xNewTypeName;                                                               \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::shared_ptr<xNewTypeName> xNewTypeName##Ptr;          \
  typedef ZS_INTERNAL_SMART_POINTER_NAMESPACE::weak_ptr<xNewTypeName> xNewTypeName##WeakPtr;

#define ZS_INTERNAL_DYNAMIC_PTR_CAST(xType, xObject)                                                \
  ZS_INTERNAL_SMART_POINTER_NAMESPACE::dynamic_pointer_cast<xType>(xObject)


namespace zsLib
{
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

  typedef std::intmax_t LONGEST;
  typedef std::uintmax_t ULONGEST;

  typedef float FLOAT;
  typedef double DOUBLE;

#if UCHAR_MAX == 0xFF
  typedef UCHAR BYTE;
#endif

#if USHRT_MAX == 0xFFFF
  typedef USHORT WORD;
#endif

#if UINT_MAX == 0xFFFFFFFF
#ifdef _WIN32
  typedef ::DWORD DWORD;
#else
  typedef UINT DWORD;
#endif //_WIN32
#endif //UINT_MAX == 0xFFFFFFFF

#if ULONG_MAX == 0xFFFFFFFFFFFFFFFF
  typedef ULONG QWORD;

#else //ULONG_MAX == 0xFFFFFFFFFFFFFFFF

#if ULLONG_MAX == 0xFFFFFFFFFFFFFFFF
  typedef ULONGLONG QWORD;
#endif

#endif //ULONG_MAX == 0xFFFFFFFFFFFFFFFF

  typedef ULONG PUID;

#if defined(__LP64__) || defined(_WIN64) || defined(_Wp64)
#define ZSLIB_64BIT
#else
#define ZSLIB_32BIT
#endif //defined(__LP64__) || defined(_WIN64) || defined(_Wp64)

#if defined(_LP64) || defined(_WIN64) || defined(_Wp64)
  typedef QWORD PTRNUMBER;
#else
  typedef DWORD PTRNUMBER;
#endif //_LP64

  typedef PTRNUMBER USERPARAM;
  namespace internal {
    struct uuid_wrapper {
      typedef uuid_t raw_uuid_type;
      raw_uuid_type mUUID {};

      typedef UCHAR * iterator;
      typedef UCHAR const* const_iterator;

      iterator begin() { return mUUID; }
      const_iterator begin() const { return mUUID; }
      iterator end() { return mUUID + sizeof(raw_uuid_type); }
      const_iterator end() const { return mUUID + sizeof(raw_uuid_type); }

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
        return !uuid_is_null(mUUID);
      }

      bool operator==(const uuid_wrapper &op2) const {
        return 0 == (*this).compare(op2);
      }

      bool operator!=(const uuid_wrapper &op2) const {
        return 0 != (*this).compare(op2);
      }

      bool operator<(const uuid_wrapper &op2) const {
        return (*this).compare(op2) < 0;
      }

      bool operator>(const uuid_wrapper &op2) const {
        return (*this).compare(op2) > 0;
      }

      bool operator<=(const uuid_wrapper &op2) const {
        return !((*this).compare(op2) > 0);
      }

      bool operator>=(const uuid_wrapper &op2) const {
        return !((*this).compare(op2) < 0);
      }

      uuid_wrapper &operator=(const uuid_wrapper &op2) {
        uuid_copy(mUUID, op2.mUUID);
        return *this;
      }
    };
  }

  typedef internal::uuid_wrapper UUID;

  inline bool operator==(const UUID &op1, const UUID &op2) {
    return 0 == op1.compare(op2);
  }

  inline bool operator!=(const UUID &op1, const UUID &op2) {
    return 0 != op1.compare(op2);
  }

  inline bool operator<(const UUID &op1, const UUID &op2) {
    return op1.compare(op2) < 0;
  }

  inline bool operator>(const UUID &op1, const UUID &op2) {
    return op1.compare(op2) > 0;
  }

  inline bool operator<=(const UUID &op1, const UUID &op2) {
    return !(op1.compare(op2) > 0);
  }

  inline bool operator>=(const UUID &op1, const UUID &op2) {
    return !(op1.compare(op2) < 0);
  }
  
  typedef char CHAR;
  typedef wchar_t WCHAR;

#ifdef _NATIVE_WCHAR_T_DEFINED
  typedef WCHAR TCHAR;
#else
  typedef CHAR TCHAR;
#endif //_NATIVE_WCHAR_T_DEFINED


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

#ifdef _WIN32
#define ZS_TARGET_UTF16_CHAR_IS_LITTLE_ENDIAN
#else
#define ZS_TARGET_UTF16_CHAR_IS_BIG_ENDIAN
#endif //_WIN32

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
    class noncopyable {
    private:
      noncopyable(const noncopyable &) = delete;
      noncopyable &operator=(const noncopyable &) = delete;
    protected:
      noncopyable() {}
    public:
      ~noncopyable() {}
    };
  }

  typedef internal::noncopyable noncopyable;

}

#endif //ZSLIB_INTERNAL_ZSTYPES_H_b6763c4cc75e565b376883f85c0186de
