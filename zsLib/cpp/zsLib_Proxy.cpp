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

#include <zsLib/Proxy.h>
#include <zsLib/ProxySubscriptions.h>
#include <zsLib/MessageQueueAssociator.h>
#include <zsLib/Log.h>
#include <zsLib/helpers.h>
#include <zsLib/Stringize.h>

#ifdef DEBUG
#include <iostream>
#endif //DEBUG

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
  namespace internal
  {
#ifdef DEBUG
    class ProxyTracking
    {
    public:
      ProxyTracking()
      {
        autoDump();
      }

      ~ProxyTracking()
      {
      }

      static ProxyTracking &singleton()
      {
        static Singleton<ProxyTracking, false> singleton;
        return singleton.singleton();
      }

      void follow(int line, const char *fileName)
      {
        AutoRecursiveLock lock(mLock);

        LocationPair pair(line, fileName);
        LocationCountMap::iterator found = mLocations.find(pair);
        if (found == mLocations.end()) {
          mLocations[pair] = 1;
        } else {
          int &count = (*found).second;
          ++count;
        }
      }

      void unfollow(int line, const char *fileName)
      {
        AutoRecursiveLock lock(mLock);

        LocationPair pair(line, fileName);
        LocationCountMap::iterator found = mLocations.find(pair);
        ZS_THROW_BAD_STATE_IF(found == mLocations.end())

        int &count = (*found).second;
        --count;
        if (0 == count) {
          mLocations.erase(found);
        }
      }

      void output()
      {
        AutoRecursiveLock lock(mLock);
        if (mLocations.size() < 1) return;

        ZS_LOG_BASIC("-------------------------------------------------------------------------------")
        ZS_LOG_BASIC("-------------------------------------------------------------------------------")
        ZS_LOG_BASIC("-------------------------------------------------------------------------------")
        std::cout << "-------------------------------------------------------------------------------\n";
        std::cout << "-------------------------------------------------------------------------------\n";
        std::cout << "-------------------------------------------------------------------------------\n";

        for (LocationCountMap::iterator iter = mLocations.begin(); iter != mLocations.end(); ++iter) {
          String output = String("FILE=") + (*iter).first.second + " LINE=" + string((*iter).first.first) + " COUNT=" + string((*iter).second);
          ZS_LOG_WARNING(Basic, output)
          std::cout << output << "\n";
        }

        ZS_LOG_BASIC("-------------------------------------------------------------------------------")
        ZS_LOG_BASIC("-------------------------------------------------------------------------------")
        ZS_LOG_BASIC("-------------------------------------------------------------------------------")
        std::cout << "-------------------------------------------------------------------------------\n";
        std::cout << "-------------------------------------------------------------------------------\n";
        std::cout << "-------------------------------------------------------------------------------\n";
      }
    protected:

      static void autoDump()
      {
        class AutoDump
        {
        public:
          AutoDump() {}
          ~AutoDump() {ProxyTracking::singleton().output();}
        };

        static AutoDump autodump;
      }

    private:
      RecursiveLock mLock;

      typedef std::pair<int, const char *> LocationPair;
      typedef std::map<LocationPair, int> LocationCountMap;

      LocationCountMap mLocations;
    };

#endif //DEBUG

    static std::atomic_ulong &getProxyCountGlobal()
    {
      static std::atomic_ulong total {};
      return total;
    }

    void proxyCountIncrement(int line, const char *fileName)
    {
      ++getProxyCountGlobal();
#ifdef DEBUG
      (ProxyTracking::singleton()).follow(line, fileName);
#endif //DEBUG
    }

    void proxyCountDecrement(int line, const char *fileName)
    {
      --getProxyCountGlobal();
#ifdef DEBUG
      (ProxyTracking::singleton()).unfollow(line, fileName);
#endif //DEBUG
    }
  }

  ULONG proxyGetTotalConstructed()
  {
    return internal::getProxyCountGlobal();
  }

  void proxyDump()
  {
#ifdef DEBUG
    (internal::ProxyTracking::singleton()).output();
#endif //DEBUG
  }
}
