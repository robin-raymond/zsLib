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

#include <zsLib/helpers.h>
#include <zsLib/Log.h>

#include <pthread.h>

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

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
    std::atomic_ulong &globalPUID()
    {
      static std::atomic_ulong global {};
      return global;
    }

    class EpochHelper
    {
    public:
      EpochHelper()
      {
        std::tm tmepoch {};

        tmepoch.tm_year = 70;
        tmepoch.tm_mon = 0;
        tmepoch.tm_mday = 1;

        time_t tepoch = std::mktime(&tmepoch);

        mEpoch = std::chrono::system_clock::from_time_t(tepoch);
      }

      static Time &epoch()
      {
        static EpochHelper singleton;
        return singleton.mEpoch;
      }

    private:

    private:
      Time mEpoch;
    };
  }

  //---------------------------------------------------------------------------
  PUID createPUID()
  {
    std::atomic_ulong &global = internal::globalPUID();
    return ++global;
  }

  //---------------------------------------------------------------------------
  UUID createUUID()
  {
    UUID gen;
    uuid_generate_random(gen.mUUID);
    return gen;
  }

  //---------------------------------------------------------------------------
  void debugSetCurrentThreadName(const char *name)
  {
    if (!name) name = "";

#ifdef __APPLE__
    pthread_setname_np(name);
#else
    pthread_setname_np(pthread_self(), name);
#endif //__APPLE__
  }

  //---------------------------------------------------------------------------
  Time now()
  {
    return std::chrono::system_clock::now();
  }

  //---------------------------------------------------------------------------
  Time epoch()
  {
    return internal::EpochHelper::epoch();
  }
}
