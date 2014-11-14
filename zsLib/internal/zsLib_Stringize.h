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

#ifndef ZSLIB_INTERNAL_STRINGIZE_H_0c235f6defcccb275d602da44da60e58
#define ZSLIB_INTERNAL_STRINGIZE_H_0c235f6defcccb275d602da44da60e58

#include <zsLib/types.h>
#include <zsLib/String.h>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace zsLib
{
  namespace internal
  {
    String convert(ULONGLONG value, size_t base);

    String timeToString(const Time &value);
  }
} // namespace zsLib

#endif //ZSLIB_INTERNAL_STRINGIZE_H_0c235f6defcccb275d602da44da60e58

#ifdef ZSLIB_INTERNAL_STRINGIZE_H_0c235f6defcccb275d602da44da60e58_SECOND_INCLUDE
#undef ZSLIB_INTERNAL_STRINGIZE_H_0c235f6defcccb275d602da44da60e58_SECOND_INCLUDE
namespace zsLib
{
  template<typename t_type>
  inline Stringize<t_type>::operator String() const
  {
    if (10 == mBase)
      return boost::lexical_cast<String>(mValue);

    return internal::convert((ULONGLONG)mValue, mBase);
  }

  template<>
  inline Stringize<bool>::operator String() const
  {
    return mValue ? String("true") : String("false");
  }

  template<>
  inline Stringize<const char *>::operator String() const
  {
    return mValue ? String(mValue) : String();
  }

  template<>
  inline Stringize<CHAR>::operator String() const
  {
    if (10 == mBase)
      return boost::lexical_cast<String>((int)mValue);
    return internal::convert((ULONGLONG)((UCHAR)mValue), mBase);
  }

  template<>
  inline Stringize<UCHAR>::operator String() const
  {
    if (10 == mBase)
      return boost::lexical_cast<String>((UINT)mValue);
    return internal::convert((ULONGLONG)((UINT)mValue), mBase);
  }

  template<>
  inline Stringize<SHORT>::operator String() const
  {
    if (10 == mBase)
      return boost::lexical_cast<String>(mValue);
    return internal::convert((ULONGLONG)((USHORT)mValue), mBase);
  }

  template<>
  inline Stringize<USHORT>::operator String() const
  {
    if (10 == mBase)
      return boost::lexical_cast<String>(mValue);
    return internal::convert((ULONGLONG)mValue, mBase);
  }

  template<>
  inline Stringize<INT>::operator String() const
  {
    if (10 == mBase)
      return boost::lexical_cast<String>(mValue);
    return internal::convert((ULONGLONG)((UINT)mValue), mBase);
  }

  template<>
  inline Stringize<UINT>::operator String() const
  {
    if (10 == mBase)
      return boost::lexical_cast<String>(mValue);
    return internal::convert((ULONGLONG)mValue, mBase);
  }

  template<>
  inline Stringize<LONG>::operator String() const
  {
    if (10 == mBase)
      return boost::lexical_cast<String>(mValue);
    return internal::convert((ULONGLONG)((ULONG)mValue), mBase);
  }

  template<>
  inline Stringize<ULONG>::operator String() const
  {
    if (10 == mBase)
      return boost::lexical_cast<String>(mValue);
    return internal::convert((ULONGLONG)mValue, mBase);
  }

  template<>
  inline Stringize<LONGLONG>::operator String() const
  {
    if (10 == mBase)
      return boost::lexical_cast<String>(mValue);
    return internal::convert((ULONGLONG)mValue, mBase);
  }

  template<>
  inline Stringize<ULONGLONG>::operator String() const
  {
    if (10 == mBase)
      return boost::lexical_cast<String>(mValue);
    return internal::convert(mValue, mBase);
  }

  template<>
  inline Stringize<float>::operator String() const
  {
    return boost::lexical_cast<String>(mValue);
  }

  template<>
  inline Stringize<double>::operator String() const
  {
    return boost::lexical_cast<String>(mValue);
  }

  template<>
  inline Stringize<UUID>::operator String() const
  {
    return String(boost::lexical_cast<std::string>(mValue));
  }

  template<>
  inline Stringize<Time>::operator String() const
  {
    return internal::timeToString(mValue);
  }

}
#else
#define ZSLIB_INTERNAL_STRINGIZE_H_0c235f6defcccb275d602da44da60e58_SECOND_INCLUDE
#endif //ZSLIB_INTERNAL_STRINGIZE_H_0c235f6defcccb275d602da44da60e58_SECOND_INCLUDE
