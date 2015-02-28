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

#include <zsLib/Stringize.h>
#include <zsLib/Exception.h>
#include <zsLib/IPAddress.h>
#include <zsLib/helpers.h>

#include <cmath>

#ifdef _WIN32
#define snprintf sprintf_s
#endif //_WIN32

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
  namespace internal
  {
    static char gDigits[10+26] =
    {
      '0','1','2','3','4','5','6','7','8','9',
      'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'
    };

    //-----------------------------------------------------------------------
    String convert(ULONGLONG value, size_t base)
    {
      ZS_THROW_INVALID_USAGE_IF((base < 2) || (base > (10+26)))

      std::unique_ptr<char[]> buffer(new char[sizeof(ULONGLONG)*8+2]);
      memset(buffer.get(), 0, sizeof(char)*((sizeof(ULONGLONG)*8)+2));

      STR end = (buffer.get() + (sizeof(ULONGLONG)*8));
      STR dest = end;
      *dest = 0;
      while (value > 0)
      {
        --dest;
        *dest = gDigits[value % base];
        value /= base;
      }

      if (dest == end)
        *dest = '0';
      return String(dest);
    }

    //-----------------------------------------------------------------------
    String timeToString(const Time &value)
    {
      if (Time() == value) return String();

      Microseconds sinceEpoch = zsLib::timeSinceEpoch<Microseconds>(value);
      Seconds asSeconds = toSeconds(sinceEpoch);

      if (asSeconds < sinceEpoch) {
        return durationToString<Microseconds>(sinceEpoch);
      }

      return std::to_string(asSeconds.count());
    }

    //-----------------------------------------------------------------------
    String durationToString(
                            const Seconds &secPart,
                            std::intmax_t fractionalPart,
                            std::intmax_t den
                            )
    {

      if (den < 1) {
        return string(secPart.count());
      }

      char buffer[100] {};
      char format[50] {};

      std::intmax_t secValue = static_cast<std::intmax_t>(secPart.count());

      int digits = static_cast<int>(log10(den));

      snprintf(format, sizeof(format)-1, "%%lli.%%0%illi", digits);
      snprintf(buffer, sizeof(buffer)-1, format, (long long)secValue, (long long)fractionalPart);

      return buffer;
    }

    //-----------------------------------------------------------------------
    void trimTrailingZeros(std::string &value)
    {
      if (std::string::npos == value.find('.')) return;

      // strip 0
      {
        std::size_t found = value.find_last_not_of('0');
        if (std::string::npos == found) return;
        value.erase(found+1);
      }

      // strip .
      {
        std::size_t found = value.find_last_not_of('.');
        if (std::string::npos == found) return;
        value.erase(found+1);
      }

    }
  }

  //-----------------------------------------------------------------------
  String string(const IPAddress & x, bool includePort)
  {
    return x.string(includePort);
  }

}
