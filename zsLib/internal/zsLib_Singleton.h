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

#ifndef ZSLIB_INTERNAL_SINGLETON_H_bad6aed17b17d998189d5a5d1ba2ced7
#define ZSLIB_INTERNAL_SINGLETON_H_bad6aed17b17d998189d5a5d1ba2ced7

#include <zsLib/types.h>
#include <zsLib/String.h>

#include <map>

namespace zsLib
{
  namespace internal
  {
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark SingletonManager::Register
    #pragma mark

    class SingletonManager
    {
    public:
      class Initializer;
      class Register;

      friend class Initializer;
      friend class Register;

      typedef std::map<String, ISingletonManagerDelegatePtr> DelegateMap;

    protected:
      static SingletonManagerPtr create();

    public:
      SingletonManager();
      ~SingletonManager();

    protected:
      static SingletonManagerPtr singleton();

      void registerSingleton(
                             const char *uniqueNamespacedID,
                             ISingletonManagerDelegatePtr singleton
                             );

      ISingletonManagerDelegatePtr find(const char *uniqueNamespacedID);

      void reference();
      void unreference();

    public:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark SingletonManager::Initializer
      #pragma mark

      class Initializer
      {
      public:
        Initializer();
        ~Initializer();

      protected:
        SingletonManagerPtr mManager;
      };

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark SingletonManager::Register
      #pragma mark

      class Register
      {
      public:
        Register();
        ~Register();

      protected:
        SingletonManagerPtr mManager;
      };

    private:

    protected:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark SingletonManager => (data)
      #pragma mark

      SingletonManagerWeakPtr mThisWeak;

      size_t mReferences {};

      RecursiveLock mLock;

      bool mCleaned {};
      DelegateMap mDelegates;
    };
  }
}


#endif //ndef ZSLIB_INTERNAL_SINGLETON_H_bad6aed17b17d998189d5a5d1ba2ced7
