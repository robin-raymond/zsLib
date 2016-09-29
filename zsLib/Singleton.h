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

#ifndef ZSLIB_SINGLETON_H_22884db148f0ffb0d830ba431102b0b5
#define ZSLIB_SINGLETON_H_22884db148f0ffb0d830ba431102b0b5

#include <zsLib/internal/zsLib_Singleton.h>


namespace zsLib
{
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark BoxedAllocation<T>
  #pragma mark

  template <typename T, bool allowDestroy = true>
  class BoxedAllocation
  {
  public:
    BoxedAllocation()
    {
      BYTE *buffer = (&(mBuffer[0]));
      auto alignment = alignof(T);
      auto misalignment = reinterpret_cast<uintptr_t>(buffer) % alignment;
      // alignment = 8, misalignment = 1, add alignment - misalignmnet (8-1 = +7)
      // alignment = 8, misalignment = 7, add alignment - misalignmnet (8-7 = +1)
      // alignment = 8, misalignment = 0, add 0 to alignment
      BYTE *alignedBuffer = buffer + (misalignment == 0 ? 0 : (alignment - misalignment));
      mObject = new (alignedBuffer) T;
    }

    ~BoxedAllocation()
    {
      if (!allowDestroy) return;

      mObject->~T();
    }

    T &ref()
    {
      return *mObject;
    }

    const T &ref() const
    {
      return *mObject;
    }

  private:
    BYTE mBuffer[sizeof(T) + alignof(T)];
    T *mObject;
  };

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark Singleton<T>
  #pragma mark

  template <typename T, bool allowDestroy = true>
  class Singleton : BoxedAllocation<T, allowDestroy>
  {
  public:
    T &singleton()
    {
      return BoxedAllocation<T, allowDestroy>::ref();
    }

  private:
  };

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark SingletonLazySharedPtr<T, bool allowDestroy>
  #pragma mark

  template <typename T, bool allowDestroy = true>
  class SingletonLazySharedPtr : BoxedAllocation< std::weak_ptr<T>, false >
  {
  public:
    ZS_DECLARE_PTR(T)

  public:
    SingletonLazySharedPtr(TPtr pThis)
    {
      mThis = pThis;
      weakRef() = pThis;

      if (!allowDestroy) {
        // throw away an extra reference to "pThis" intentionally
        BoxedAllocation<TPtr, false> bogusReference;
        (bogusReference.ref()) = pThis;
      }
    }

    TPtr singleton()
    {
      return weakRef().lock();
    }

  private:
    TWeakPtr &weakRef()
    {
      return BoxedAllocation< TWeakPtr, false >::ref();
    }
    
  private:
    TPtr mThis;
  };
  
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark SingletonManager
  #pragma mark

  class SingletonManager : public internal::SingletonManager
  {
  public:

    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark Initializer
    #pragma mark

    class Initializer : public internal::SingletonManager::Initializer
    {
    public:
      //-----------------------------------------------------------------------
      // PURPOSE: Initialize the singleton manager
      // NOTES:   Declare this an instance of this type whereever the
      //          SingletonManager is used. At the end of all instance's
      //          lifetime all singletons will be notified to clean-up.
      //          Typically an instance of this class is declared inside
      //          "main(...)"
      Initializer();
      ~Initializer();
    };

    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark Register
    #pragma mark

    class Register : public internal::SingletonManager::Register
    {
    public:
      //-----------------------------------------------------------------------
      // PURPOSE: Register a singleton for cleanup at the end of the
      //          application lifecycle.
      // NOTES:   This ensures the singleton is cleaned up without relying on
      //          the global space cleanup. Declare this class statically after
      //          creating the singleton. This will ensure at least one
      //          reference to the object exists outside the singleton to
      //          ensure a reference exists during the cleanup phase.
      Register(
               const char *uniqueNamespacedID,
               ISingletonManagerDelegatePtr singleton
               );
      ~Register();

      //-----------------------------------------------------------------------
      // PURPOSE: Find a previously registered singleton
      static ISingletonManagerDelegatePtr find(const char *uniqueNamespacedID);
    };
  };

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark ISingletonManagerDelegate
  #pragma mark

  interaction ISingletonManagerDelegate
  {
    virtual void notifySingletonCleanup() = 0;
  };

} // namespace zsLib

#endif //ZSLIB_SINGLETON_H_22884db148f0ffb0d830ba431102b0b5
