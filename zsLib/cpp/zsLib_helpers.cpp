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

#include <boost/uuid/random_generator.hpp>
#include <boost/smart_ptr/detail/atomic_count.hpp>
#include <boost/interprocess/detail/atomic.hpp>

#include <zsLib/helpers.h>
#include <zsLib/Log.h>

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

using namespace boost;
using namespace boost::interprocess;

namespace zsLib
{

  namespace internal
  {
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma (helpers)

    //-------------------------------------------------------------------------
    ULONG &globalPUID()
    {
      static ULONG global = 0;
      return global;
    }
  }

  //---------------------------------------------------------------------------
  PUID createPUID()
  {
    return (PUID)atomicIncrement(zsLib::internal::globalPUID());
  }

  //---------------------------------------------------------------------------
  UUID createUUID()
  {
    boost::uuids::random_generator gen;
    return gen();
  }

  //---------------------------------------------------------------------------
  ULONG atomicIncrement(ULONG &value)
  {
#ifdef __GNUC__
    return __sync_add_and_fetch(&value, 1);
#else
    return ++(*(boost::detail::atomic_count *)((LONG *)(&value)));
#endif //__GNUC__
  }

  //---------------------------------------------------------------------------
  ULONG atomicDecrement(ULONG &value)
  {
#ifdef __GNUC__
    return __sync_sub_and_fetch(&value, 1);
#else
    return --(*(boost::detail::atomic_count *)((LONG *)(&value)));
#endif //__GNUC__
  }

  //---------------------------------------------------------------------------
  ULONG atomicGetValue(ULONG &value)
  {
#ifdef __GNUC__
    return __sync_add_and_fetch(&value, 0);
#else
    return (long)(*(boost::detail::atomic_count *)((LONG *)(&value)));
#endif //__GNUC__
  }

  //---------------------------------------------------------------------------
  DWORD atomicGetValue32(DWORD &value)
  {
#ifdef __GNUC__
    return __sync_add_and_fetch(&value, 0);
#else
    return boost::interprocess::detail::atomic_read32((boost::uint32_t *)&value);
#endif //__GNUC__
  }

  //---------------------------------------------------------------------------
  void atomicSetValue32(DWORD &value, DWORD newValue)
  {
#ifdef __GNUC__
    __sync_lock_test_and_set(&value, newValue);
#else
	  boost::interprocess::detail::atomic_write32((boost::uint32_t *)&value, newValue);
#endif //__GNUC__
  }

  //---------------------------------------------------------------------------
  Time now()
  {
    return boost::posix_time::microsec_clock::universal_time();
  }

  //---------------------------------------------------------------------------
  Duration timeSinceEpoch(Time time)
  {
    if (Time() == time) {
      return Duration();
    }

    static Time epoch(boost::gregorian::date(1970,1,1));
    return (time - epoch);
  }

  //---------------------------------------------------------------------------
  Time timeSinceEpoch(Duration duration)
  {
    if (0 == time) {
      return Time();
    }
    static Time epoch(boost::gregorian::date(1970,1,1));
    return epoch + duration;
  }
}
