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

#include <zsLib/Log.h>
#include <zsLib/Singleton.h>
#include <zsLib/String.h>
#include <zsLib/Exception.h>

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark SingletonManager
  #pragma mark

  namespace internal
  {
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark SingletonManager
    #pragma mark

    //-------------------------------------------------------------------------
    SingletonManager::SingletonManager()
    {
    }

    //-------------------------------------------------------------------------
    SingletonManager::~SingletonManager()
    {
      if (0 == mDelegates.size()) return;

      for (auto iter = mDelegates.begin(); iter != mDelegates.end(); ++iter) {
        auto delegate = (*iter).second;

        delegate->notifySingletonCleanup();
      }

      mDelegates.clear();
    }

    //-------------------------------------------------------------------------
    SingletonManagerPtr SingletonManager::create()
    {
      SingletonManagerPtr pThis(new zsLib::SingletonManager);
      pThis->mThisWeak = pThis;
      return pThis;
    }

    //-------------------------------------------------------------------------
    SingletonManagerPtr SingletonManager::singleton()
    {
      static SingletonLazySharedPtr<zsLib::SingletonManager> singleton(SingletonManager::create());
      SingletonManagerPtr pThis = singleton.singleton();
      if (!pThis) {
        ZS_LOG_WARNING(Debug, zsLib::Log::Params("singleton manager gone", "zslib::SingletonManager"))
      }
      return pThis;
    }

    //-------------------------------------------------------------------------
    void SingletonManager::registerSingleton(
                                             const char *uniqueNamespacedID,
                                             ISingletonManagerDelegatePtr singleton
                                             )
    {
      SingletonManagerPtr pThis = SingletonManager::singleton();
      if (!pThis) return;

      ISingletonManagerDelegatePtr cleanDelegate;

      {
        AutoRecursiveLock lock(pThis->mLock);
        if (pThis->mCleaned) {
          cleanDelegate = singleton;
          return;
        }
        pThis->mDelegates[String(uniqueNamespacedID)] = singleton;
        return;
      }

    clean:
      {
        if (cleanDelegate) {
          cleanDelegate->notifySingletonCleanup();
        }
      }
    }
    //-------------------------------------------------------------------------
    ISingletonManagerDelegatePtr SingletonManager::find(const char *uniqueNamespacedID)
    {
      SingletonManagerPtr pThis = SingletonManager::singleton();
      if (!pThis) return ISingletonManagerDelegatePtr();

      AutoRecursiveLock lock(pThis->mLock);

      auto found = pThis->mDelegates.find(String(uniqueNamespacedID));
      if (found == pThis->mDelegates.end()) return ISingletonManagerDelegatePtr();

      return (*found).second;
    }

    //-------------------------------------------------------------------------
    void SingletonManager::reference()
    {
      AutoRecursiveLock lock(mLock);
      if (mCleaned) return;

      ++(mReferences);
    }

    //-------------------------------------------------------------------------
    void SingletonManager::unreference()
    {
      DelegateMap cleanDelegates;

      {
        AutoRecursiveLock lock(mLock);
        if (mCleaned) return;

        ZS_THROW_BAD_STATE_IF(0 == mReferences)
        --mReferences;
        if (0 != mReferences) return;

        cleanDelegates = mDelegates;

        mDelegates.clear();
      }

      mCleaned = true;

      if (0 == cleanDelegates.size()) return;

      for (auto iter = cleanDelegates.begin(); iter != cleanDelegates.end(); ++iter) {
        auto delegate = (*iter).second;

        delegate->notifySingletonCleanup();
      }

      cleanDelegates.clear();
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark SingletonManager::Initializer
    #pragma mark

    //-------------------------------------------------------------------------
    SingletonManager::Initializer::Initializer()
    {
      mManager = SingletonManager::singleton();
      if (!mManager) return;

      mManager->reference();
    }

    //-------------------------------------------------------------------------
    SingletonManager::Initializer::~Initializer()
    {
      if (!mManager) return;
      mManager->unreference();
      mManager.reset();
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark SingletonManager::Initializer
    #pragma mark

    //-------------------------------------------------------------------------
    SingletonManager::Register::Register()
    {
      mManager = SingletonManager::singleton();
    }

    //-------------------------------------------------------------------------
    SingletonManager::Register::~Register()
    {
      mManager.reset();
    }
  }

  //---------------------------------------------------------------------------
  SingletonManager::Initializer::Initializer() : internal::SingletonManager::Initializer()
  {
  }

  //---------------------------------------------------------------------------
  SingletonManager::Initializer::~Initializer()
  {
  }

  //---------------------------------------------------------------------------
  SingletonManager::Register::Register(
                                       const char *uniqueNamespacedID,
                                       ISingletonManagerDelegatePtr singleton
                                       ) : internal::SingletonManager::Register()
  {
    if (!mManager) return;
    mManager->registerSingleton(uniqueNamespacedID, singleton);
  }

  //---------------------------------------------------------------------------
  SingletonManager::Register::~Register()
  {
  }

  //---------------------------------------------------------------------------
  ISingletonManagerDelegatePtr SingletonManager::Register::find(const char *uniqueNamespacedID)
  {
    auto manager = SingletonManager::singleton();

    if (!manager) return ISingletonManagerDelegatePtr();
    return manager->find(uniqueNamespacedID);
  }


} // namespace zsLib
