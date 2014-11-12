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
