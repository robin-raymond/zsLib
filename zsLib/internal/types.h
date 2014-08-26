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

#ifndef ZSLIB_INTERNAL_ZSTYPES_H_b6763c4cc75e565b376883f85c0186de
#define ZSLIB_INTERNAL_ZSTYPES_H_b6763c4cc75e565b376883f85c0186de

#include <limits.h>

#include <zsLib/internal/boostTypes.h>

#ifdef _WIN32
#include <winsock2.h>
#endif //_WIN32

//#ifndef interface
//#define interface struct
//#endif //interface

#ifndef interaction
#define interaction struct
#endif //interaction

#define ZS_INTERNAL_DECLARE_PTR(xExistingType)                        \
  typedef boost::shared_ptr<xExistingType> xExistingType##Ptr;        \
  typedef boost::weak_ptr<xExistingType> xExistingType##WeakPtr;

#define ZS_INTERNAL_DECLARE_USING_PTR(xNamespace, xExistingType)      \
  using xNamespace::xExistingType;                                    \
  using xNamespace::xExistingType##Ptr;                               \
  using xNamespace::xExistingType##WeakPtr;

#define ZS_INTERNAL_DECLARE_CLASS_PTR(xClassName)                     \
  class xClassName;                                                   \
  typedef boost::shared_ptr<xClassName> xClassName##Ptr;              \
  typedef boost::weak_ptr<xClassName> xClassName##WeakPtr;

#define ZS_INTERNAL_DECLARE_STRUCT_PTR(xStructName)                   \
  struct xStructName;                                                 \
  typedef boost::shared_ptr<xStructName> xStructName##Ptr;            \
  typedef boost::weak_ptr<xStructName> xStructName##WeakPtr;

#define ZS_INTERNAL_DECLARE_TYPEDEF_PTR(xOriginalType, xNewTypeName)  \
  typedef xOriginalType xNewTypeName;                                 \
  typedef boost::shared_ptr<xNewTypeName> xNewTypeName##Ptr;          \
  typedef boost::weak_ptr<xNewTypeName> xNewTypeName##WeakPtr;

#define ZS_INTERNAL_DYNAMIC_PTR_CAST(xType, xObject)                  \
  boost::dynamic_pointer_cast<xType>(xObject)


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
  typedef boost::uuids::uuid UUID;

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
}

#endif //ZSLIB_INTERNAL_ZSTYPES_H_b6763c4cc75e565b376883f85c0186de
