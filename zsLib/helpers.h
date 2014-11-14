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

#ifndef ZSLIB_ZSHELPERS_H_f77ff525153521711e8350b22b766d17
#define ZSLIB_ZSHELPERS_H_f77ff525153521711e8350b22b766d17

#include <zsLib/types.h>

namespace zsLib
{

  //---------------------------------------------------------------------------
  PUID createPUID();
  UUID createUUID();

  //---------------------------------------------------------------------------
  ULONG atomicIncrement(ULONG &value);
  ULONG atomicDecrement(ULONG &value);
  ULONG atomicGetValue(ULONG &value);

  //---------------------------------------------------------------------------
  DWORD atomicGetValue32(DWORD &value);
  void atomicSetValue32(DWORD &value, DWORD newValue);

  //---------------------------------------------------------------------------
  Time now();

  Duration timeSinceEpoch(Time time);
  Time timeSinceEpoch(Duration duration);

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  template <typename T, bool allowDestroy = true>
  class BoxedAllocation
  {
  public:
    BoxedAllocation()
    {
      BYTE *buffer = (&(mBuffer[0]));
      BYTE *alignedBuffer = buffer + alignof(T) - reinterpret_cast<uintptr_t>(buffer) % alignof(T);
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

} // namespace zsLib

#endif //ZSLIB_ZSHELPERS_H_f77ff525153521711e8350b22b766d17
