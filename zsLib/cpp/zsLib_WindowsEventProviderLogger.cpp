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

#ifdef _WIN32
#include <Evntprov.h>

namespace zsLib { ZS_DECLARE_SUBSYSTEM(zsLib) }

namespace zsLib
{
  namespace internal
  {
    ZS_DECLARE_CLASS_PTR(WindowsEventProviderLogger);

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark WindowsEventProviderLogger
    #pragma mark

    class WindowsEventProviderLogger : public RecursiveLock,
                                       public ILogEventingDelegate,
                                       public ILogEventingProviderDelegate,
                                       public ISingletonManagerDelegate
    {
    protected:
      struct make_private {};
      struct ProviderInfo;

      typedef zsLib::Log::EventingAtomDataArray EventingAtomDataArray;
      typedef std::set<ProviderInfo *> ProviderSet;

      typedef ::zsLib::Log::ProviderHandle ProviderHandle;
      typedef ::zsLib::Log::EventingAtomIndex EventingAtomIndex;
      typedef ::zsLib::Log::EventingAtomData EventingAtomData;

      struct ProviderInfo
      {
        ProviderHandle mEventingHandle {};
        GUID mID {};
        REGHANDLE mRegistrationHandle {};
      };

    protected:
      //-----------------------------------------------------------------------
      static WindowsEventProviderLoggerPtr create(bool &outCreated)
      {
        auto pThis(make_shared<WindowsEventProviderLogger>(make_private{}));
        pThis->mThisWeak = pThis;
        pThis->init();
        outCreated = true;
        return pThis;
      }

      //-----------------------------------------------------------------------
      void init()
      {
      }

      void installEventListener()
      {
        zsLib::Log::addEventingProviderListener(mThisWeak.lock());
      }

    public:
      //-----------------------------------------------------------------------
      WindowsEventProviderLogger(const make_private &)
      {
      }

      //-----------------------------------------------------------------------
      ~WindowsEventProviderLogger()
      {
        mThisWeak.reset();

        for (auto iter = mCleanupList.begin(); iter != mCleanupList.end(); ++iter)
        {
          auto provider = (*iter);
          delete provider;
        }

        mCleanupList.clear();
      }

      //-----------------------------------------------------------------------
      static WindowsEventProviderLoggerPtr singleton()
      {
        bool created {};
        static SingletonLazySharedPtr<WindowsEventProviderLogger> singleton(create(created));
        static zsLib::SingletonManager::Register registerSingleton("org.zsLib.WindowsEventProviderLogger", singleton.singleton());
        auto result = singleton.singleton();
        if (created) {
          result->installEventListener();
        }
        return result;
      }

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark WindowsEventProviderLogger => ILogEventingDelegate
      #pragma mark

      //-----------------------------------------------------------------------
      virtual void notifyWriteEvent(
                                    ProviderHandle handle,
                                    EventingAtomDataArray eventingAtomDataArray,
                                    Severity severity,
                                    Level level,
                                    EVENT_DESCRIPTOR_HANDLE descriptor,
                                    EVENT_PARAMETER_DESCRIPTOR_HANDLE paramDescriptor,
                                    EVENT_DATA_DESCRIPTOR_HANDLE dataDescriptor,
                                    size_t dataDescriptorCount
                                    ) override
      {
        EventingAtomIndex index = mAtomIndex;
        if (0 == index) return;

        ProviderInfo *provider = reinterpret_cast<ProviderInfo *>(eventingAtomDataArray[index]);
        if (!provider) return;

        auto result = EventWrite(provider->mRegistrationHandle, descriptor, dataDescriptorCount, const_cast<PEVENT_DATA_DESCRIPTOR>(dataDescriptor));
        if (!result) result = 1;
        (void)result;
      }

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark WindowsEventProviderLogger => ILogEventingDelegate
      #pragma mark

      //-----------------------------------------------------------------------
      virtual void notifyEventingProviderRegistered(
                                                    ProviderHandle handle,
                                                    EventingAtomDataArray eventingAtomDataArray
                                                    ) override
      {
        if (0 == mAtomIndex) {
          mAtomIndex = zsLib::Log::registerEventingAtom("org.zsLib.WindowsEventProviderLogger");
          if (0 == mAtomIndex) return;
        }

        EventingAtomIndex index = mAtomIndex;

        UUID providerID {};
        String providerName;
        String providerHash;
        if (!zsLib::Log::getEventingWriterInfo(handle, providerID, providerName, providerHash)) return;

        GUID registerID {};
        memcpy(&registerID, providerID.begin(), sizeof(GUID));

        AutoRecursiveLock lock(*this);

        ProviderInfo *provider = new ProviderInfo();
        provider->mID = registerID;
        provider->mEventingHandle = handle;
        
        auto result = EventRegister(&(provider->mID), eventWriterEnableCallback, reinterpret_cast<PVOID>(provider), &(provider->mRegistrationHandle));
        if (ERROR_SUCCESS != result) {
          delete provider;
          provider = NULL;
          return;
        }

        eventingAtomDataArray[index] = reinterpret_cast<EventingAtomData>(provider);
      }

      //-----------------------------------------------------------------------
      virtual void notifyEventingProviderUnregistered(
                                                      ProviderHandle handle,
                                                      EventingAtomDataArray eventingAtomDataArray
                                                      ) override
      {
        //mCleanupList
        EventingAtomIndex index = mAtomIndex;
        if (0 == index) return;

        ProviderInfo *provider = reinterpret_cast<ProviderInfo *>(eventingAtomDataArray[index]);
        if (!provider) return;

        AutoRecursiveLock lock(*this);

        auto result = EventUnregister(provider->mRegistrationHandle);
        (void)result;
        provider->mRegistrationHandle = NULL;
        mCleanupList.insert(provider);
      }

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark WindowsEventProviderLogger => ISingletonManagerDelegate
      #pragma mark

      //-----------------------------------------------------------------------
      virtual void notifySingletonCleanup() override
      {
        zsLib::Log::removeEventingProviderListener(mThisWeak.lock());
      }

    protected:
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark WindowsEventProviderLogger => (internal)
      #pragma mark

      //-----------------------------------------------------------------------
      static void NTAPI eventWriterEnableCallback(
        _In_     LPCGUID                  SourceId,
        _In_     ULONG                    IsEnabled,
        _In_     UCHAR                    Level,
        _In_     ULONGLONG                MatchAnyKeyword,
        _In_     ULONGLONG                MatchAllKeywords,
        _In_opt_ PEVENT_FILTER_DESCRIPTOR FilterData,
        _In_opt_ PVOID                    CallbackContext
      )
      {
        // see https://msdn.microsoft.com/en-us/library/windows/desktop/aa363707(v=vs.85).aspx
        // for some reason these constants are not defined in the header file
#ifndef EVENT_CONTROL_CODE_DISABLE_PROVIDER
#define EVENT_CONTROL_CODE_DISABLE_PROVIDER (0)
#define EVENT_CONTROL_CODE_ENABLE_PROVIDER (1)
#define EVENT_CONTROL_CODE_CAPTURE_STATE (2)
#endif //EVENT_CONTROL_CODE_DISABLE_PROVIDER

        ProviderInfo *provider = reinterpret_cast<ProviderInfo *>(CallbackContext);
        if (!provider) return;

        auto pThis = singleton();
        if (!pThis) return;
        
        {
          AutoRecursiveLock lock(*pThis);

          if (EVENT_CONTROL_CODE_ENABLE_PROVIDER == IsEnabled) {
            ++(pThis->mTotalEnabled);
            if (1 == pThis->mTotalEnabled) {
              zsLib::Log::addEventingListener(pThis);
            }
          } else if (EVENT_CONTROL_CODE_DISABLE_PROVIDER == IsEnabled) {
            if (pThis->mTotalEnabled > 0) {
              --(pThis->mTotalEnabled);
              if (0 == pThis->mTotalEnabled) {
                zsLib::Log::removeEventingListener(pThis);
              }
            }
          }
        }
        
        if (EVENT_CONTROL_CODE_ENABLE_PROVIDER == IsEnabled) {
          zsLib::Log::setEventingLogging(provider->mEventingHandle, pThis->mID, true, static_cast<zsLib::Log::KeywordBitmaskType>(MatchAnyKeyword));
        } else if (EVENT_CONTROL_CODE_DISABLE_PROVIDER == IsEnabled) {
          zsLib::Log::setEventingLogging(provider->mEventingHandle, pThis->mID, false);
        }
      }

    protected:
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark WindowsEventProviderLogger => (date)
      #pragma mark

      AutoPUID mID;
      WindowsEventProviderLoggerWeakPtr mThisWeak;
      std::atomic<EventingAtomIndex> mAtomIndex {};

      size_t mTotalEnabled {};

      ProviderSet mCleanupList;
    };

    void initWindowsEventProviderLogger()
    {
      WindowsEventProviderLogger::singleton();
    }

  } // namespace internal

} // namespace zsLib

#endif //_WIN32
