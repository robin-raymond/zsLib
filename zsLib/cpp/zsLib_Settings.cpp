/*

 Copyright (c) 2016, Robin Raymond
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

#include <zsLib/internal/zsLib_Settings.h>
#include <zsLib/internal/zsLib_SocketMonitor.h>
#include <zsLib/internal/zsLib_TimerMonitor.h>

#include <zsLib/IHelper.h>
#include <zsLib/Numeric.h>
#include <zsLib/Singleton.h>
#include <zsLib/Stringize.h>
#include <zsLib/XML.h>


#ifndef ZSLIB_EVENTING_NOOP
#include <zsLib/internal/zsLib.events.h>
#else
#include <zsLib/eventing/noop.h>
#endif //ndef ZSLIB_EVENTING_NOOP

namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib)}

namespace zsLib
{
  ZS_EVENTING_TASK(Settings);

  namespace internal
  {
    ZS_DECLARE_TYPEDEF_PTR(XML::Element, Element);
    ZS_DECLARE_TYPEDEF_PTR(XML::Node, Node);
    ZS_DECLARE_TYPEDEF_PTR(XML::Text, Text);

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    #pragma mark
    #pragma mark Settings
    #pragma mark

    //-----------------------------------------------------------------------
    void Settings::setup(ISettingsDelegatePtr delegate)
    {
      StoredSettingsMapPtr stored;

      {
        AutoRecursiveLock lock(mLock);
        mDelegate = delegate;

        ZS_LOG_DEBUG(log("setup called") + ZS_PARAM("has delegate", (bool)delegate))

        if (!delegate) return;

        stored = mStored;

        mStored = make_shared<StoredSettingsMap>();
      }

      // apply all settings that occured before delegate was attached
      for (StoredSettingsMap::iterator iter = stored->begin(); iter != stored->end(); ++iter)
      {
        const Key &key = (*iter).first;
        ValuePair &valuePair = (*iter).second;

        switch (valuePair.first)
        {
          case DataType_String:   setString(key, valuePair.second); break;
          case DataType_Int:      {
            try {
              setInt(key, Numeric<LONG>(valuePair.second));
            } catch(const Numeric<LONG>::ValueOutOfRange &) {
            }
            break;
          }
          case DataType_UInt:     {
            try {
              setUInt(key, Numeric<ULONG>(valuePair.second));
            } catch(const Numeric<ULONG>::ValueOutOfRange &) {
            }
            break;
          }
          case DataType_Bool:     {
            try {
              setBool(key, Numeric<bool>(valuePair.second));
            } catch(const Numeric<bool>::ValueOutOfRange &) {
            }
            break;
          }
          case DataType_Float:    {
            try {
              setFloat(key, Numeric<float>(valuePair.second));
            } catch(const Numeric<float>::ValueOutOfRange &) {
            }
            break;
          }
          case DataType_Double:   {
            try {
              setDouble(key, Numeric<double>(valuePair.second));
            } catch(const Numeric<double>::ValueOutOfRange &) {
            }
            break;
          }
        }
      }
    }

    //-------------------------------------------------------------------------
    SettingsPtr Settings::singleton()
    {
      AutoRecursiveLock lock(*IHelper::getGlobalLock());
      static SingletonLazySharedPtr<Settings> singleton(Settings::create());
      SettingsPtr result = singleton.singleton();
      if (!result) {
        ZS_LOG_WARNING(Detail, slog("singleton gone"))
      }
      return result;
    }
      
    //-------------------------------------------------------------------------
    Settings::Settings(const make_private &) :
      mStored(make_shared<StoredSettingsMap>()),
      mDefaultsDelegates(make_shared<DefaultsDelegatesList>())
    {
      ZS_LOG_DETAIL(log("created"))
    }

    //-------------------------------------------------------------------------
    Settings::~Settings()
    {
      mThisWeak.reset();
      ZS_LOG_DETAIL(log("destroyed"))
    }

    //-------------------------------------------------------------------------
    SettingsPtr Settings::convert(ISettingsPtr settings)
    {
      return ZS_DYNAMIC_PTR_CAST(Settings, settings);
    }

    //-------------------------------------------------------------------------
    SettingsPtr Settings::create()
    {
      SettingsPtr pThis(make_shared<Settings>(make_private{}));
      pThis->mThisWeak = pThis;
      return pThis;
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark Settings => ISettings
    #pragma mark

    //-------------------------------------------------------------------------
    String Settings::getString(const char *key) const
    {
      ISettingsDelegatePtr delegate;

      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;

        if (!delegate) {
          StoredSettingsMap::const_iterator found = mStored->find(key);
          if (found == mStored->end()) return String();
          auto result = (*found).second.second;
          ZS_LOG_TRACE(log("get string") + ZS_PARAM("key", key) + ZS_PARAM("value", result));
          ZS_EVENTING_3(
                        x, i, Debug, SettingGetString, zs, Settings, Info,
                        puid, id, mID,
                        string, key, key,
                        string, value, result
                        );
          return result;
        }
      }

      auto result = delegate->getString(key);
      ZS_EVENTING_3(
                    x, i, Debug, SettingGetString, zs, Settings, Info,
                    puid, id, mID,
                    string, key, key,
                    string, value, result
                    );
      return result;
    }

    //-------------------------------------------------------------------------
    LONG Settings::getInt(const char *key) const
    {
      ISettingsDelegatePtr delegate;

      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;

        if (!delegate) {
          StoredSettingsMap::const_iterator found = mStored->find(key);
          if (found == mStored->end()) return 0;
          try {
            auto result = Numeric<LONG>((*found).second.second);
            ZS_LOG_TRACE(log("get string") + ZS_PARAM("key", key) + ZS_PARAM("value", result))
            ZS_EVENTING_3(
                          x, i, Debug, SettingGetInt, zs, Settings, Info,
                          puid, id, mID,
                          string, key, key,
                          long, value, result
                          );

            return result;
          } catch(const Numeric<LONG>::ValueOutOfRange &) {
          }
          ZS_EVENTING_3(
                        x, i, Debug, SettingGetInt, zs, Settings, Info,
                        puid, id, mID,
                        string, key, key,
                        long, value, 0
                        );
          return 0;
        }
      }

      auto result = delegate->getInt(key);
      ZS_EVENTING_3(
                    x, i, Debug, SettingGetInt, zs, Settings, Info,
                    puid, id, mID,
                    string, key, key,
                    long, value, result
                    );
      return result;
    }

    //-----------------------------------------------------------------------
    ULONG Settings::getUInt(const char *key) const
    {
      ISettingsDelegatePtr delegate;

      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;

        if (!delegate) {
          StoredSettingsMap::const_iterator found = mStored->find(key);
          if (found == mStored->end()) return 0;
          try {
            auto result = Numeric<ULONG>((*found).second.second);
            ZS_LOG_TRACE(log("get string") + ZS_PARAM("key", key) + ZS_PARAM("value", result));
            ZS_EVENTING_3(
                          x, i, Debug, SettingGetUInt, zs, Settings, Info,
                          puid, id, mID,
                          string, key, key,
                          ulong, value, result
                          );
            return result;
          } catch(const Numeric<ULONG>::ValueOutOfRange &) {
          }
          ZS_EVENTING_3(
                        x, i, Debug, SettingGetUInt, zs, Settings, Info,
                        puid, id, mID,
                        string, key, key,
                        ulong, value, 0
                        );

          return 0;
        }
      }

      auto result = delegate->getUInt(key);
      ZS_EVENTING_3(
                    x, i, Debug, SettingGetUInt, zs, Settings, Info,
                    puid, id, mID,
                    string, key, key,
                    ulong, value, result
                    );
      return result;
    }

    //-----------------------------------------------------------------------
    bool Settings::getBool(const char *key) const
    {
      ISettingsDelegatePtr delegate;

      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;

        if (!delegate) {
          StoredSettingsMap::const_iterator found = mStored->find(key);
          if (found == mStored->end()) return false;
          try {
            auto result = Numeric<bool>((*found).second.second);
            ZS_LOG_TRACE(log("get string") + ZS_PARAM("key", key) + ZS_PARAM("value", result));
            ZS_EVENTING_3(
                          x, i, Debug, SettingGetBool, zs, Settings, Info,
                          puid, id, mID,
                          string, key, key,
                          bool, value, result
                          );

            return result;
          } catch(const Numeric<bool>::ValueOutOfRange &) {
          }
          ZS_EVENTING_3(
                        x, i, Debug, SettingGetBool, zs, Settings, Info,
                        puid, id, mID,
                        string, key, key,
                        bool, value, false
                        );
          return false;
        }
      }

      auto result = delegate->getBool(key);
      ZS_EVENTING_3(
                    x, i, Debug, SettingGetBool, zs, Settings, Info,
                    puid, id, mID,
                    string, key, key,
                    bool, value, result
                    );
      return result;
    }

    //-----------------------------------------------------------------------
    float Settings::getFloat(const char *key) const
    {
      ISettingsDelegatePtr delegate;

      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;

        if (!delegate) {
          StoredSettingsMap::const_iterator found = mStored->find(key);
          if (found == mStored->end()) return 0;
          try {
            auto result = Numeric<float>((*found).second.second);
            ZS_LOG_TRACE(log("get string") + ZS_PARAM("key", key) + ZS_PARAM("value", result));
            ZS_EVENTING_3(
                          x, i, Debug, SettingGetFloat, zs, Settings, Info,
                          puid, id, mID,
                          string, key, key,
                          float, value, result
                          );
            return result;
          } catch(const Numeric<float>::ValueOutOfRange &) {
          }
          ZS_EVENTING_3(
                        x, i, Debug, SettingGetFloat, zs, Settings, Info,
                        puid, id, mID,
                        string, key, key,
                        float, value, 0
                        );
          return 0;
        }
      }

      auto result = delegate->getFloat(key);
      ZS_EVENTING_3(
                    x, i, Debug, SettingGetFloat, zs, Settings, Info,
                    puid, id, mID,
                    string, key, key,
                    float, value, result
                    );
      return result;
    }

    //-----------------------------------------------------------------------
    double Settings::getDouble(const char *key) const
    {
      ISettingsDelegatePtr delegate;

      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;

        if (!delegate) {
          StoredSettingsMap::const_iterator found = mStored->find(key);
          if (found == mStored->end()) return 0;
          try {
            auto result = Numeric<double>((*found).second.second);
            ZS_LOG_TRACE(log("get string") + ZS_PARAM("key", key) + ZS_PARAM("value", result));
            ZS_EVENTING_3(
                          x, i, Debug, SettingGetDouble, zs, Settings, Info,
                          puid, id, mID,
                          string, key, key,
                          double, value, result
                          );
            return result;
          } catch(const Numeric<double>::ValueOutOfRange &) {
          }
          ZS_EVENTING_3(
                        x, i, Debug, SettingGetDouble, zs, Settings, Info,
                        puid, id, mID,
                        string, key, key,
                        double, value, 0
                        );
          return 0;
        }
      }

      auto result = delegate->getDouble(key);
      ZS_EVENTING_3(
                    x, i, Debug, SettingGetDouble, zs, Settings, Info,
                    puid, id, mID,
                    string, key, key,
                    double, value, result
                    );
      return result;
    }

    //-----------------------------------------------------------------------
    void Settings::setString(
                              const char *key,
                              const char *value
                              )
    {
      ZS_EVENTING_3(
                    x, i, Debug, SettingSetString, zs, Settings, Info,
                    puid, id, mID,
                    string, key, key,
                    string, value, value
                    );

      ISettingsDelegatePtr delegate;

      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;

        if (!delegate) {
          (*mStored)[String(key)] = ValuePair(DataType_String, String(value));
          ZS_LOG_TRACE(log("set string") + ZS_PARAM("key", key) + ZS_PARAM("value", value))
          return;
        }
      }

      return delegate->setString(key, value);
    }

    //-----------------------------------------------------------------------
    void Settings::setInt(
                          const char *key,
                          LONG value
                          )
    {
      ZS_EVENTING_3(
                    x, i, Debug, SettingSetInt, zs, Settings, Info,
                    puid, id, mID,
                    string, key, key,
                    long, value, value
                    );

      ISettingsDelegatePtr delegate;

      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;

        if (!delegate) {
          (*mStored)[String(key)] = ValuePair(DataType_Int, string(value));
          ZS_LOG_TRACE(log("set int") + ZS_PARAM("key", key) + ZS_PARAM("value", value))
          return;
        }
      }

      return delegate->setInt(key, value);
    }

    //-----------------------------------------------------------------------
    void Settings::setUInt(
                            const char *key,
                            ULONG value
                            )
    {
      ZS_EVENTING_3(
                    x, i, Debug, SettingSetUInt, zs, Settings, Info,
                    puid, id, mID,
                    string, key, key,
                    ulong, value, value
                    );

      ISettingsDelegatePtr delegate;

      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;

        if (!delegate) {
          (*mStored)[String(key)] = ValuePair(DataType_UInt, string(value));
          ZS_LOG_TRACE(log("set uint") + ZS_PARAM("key", key) + ZS_PARAM("value", value))
          return;
        }
      }

      return delegate->setUInt(key, value);
    }

    //-----------------------------------------------------------------------
    void Settings::setBool(
                            const char *key,
                            bool value
                            )
    {
      ZS_EVENTING_3(
                    x, i, Debug, SettingSetBool, zs, Settings, Info,
                    puid, id, mID,
                    string, key, key,
                    bool, value, value
                    );

      ISettingsDelegatePtr delegate;

      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;

        if (!delegate) {
          (*mStored)[String(key)] = ValuePair(DataType_Bool, string(value));
          ZS_LOG_TRACE(log("set bool") + ZS_PARAM("key", key) + ZS_PARAM("value", value))
          return;
        }
      }

      return delegate->setBool(key, value);
    }

    //-----------------------------------------------------------------------
    void Settings::setFloat(
                            const char *key,
                            float value
                            )
    {
      ZS_EVENTING_3(
                    x, i, Debug, SettingSetFloat, zs, Settings, Info,
                    puid, id, mID,
                    string, key, key,
                    float, value, value
                    );

      ISettingsDelegatePtr delegate;

      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;

        if (!delegate) {
          (*mStored)[String(key)] = ValuePair(DataType_Float, string(value));
          ZS_LOG_TRACE(log("set float") + ZS_PARAM("key", key) + ZS_PARAM("value", value))
          return;
        }
      }

      return delegate->setFloat(key, value);
    }

    //-----------------------------------------------------------------------
    void Settings::setDouble(
                              const char *key,
                              double value
                              )
    {
      ZS_EVENTING_3(
                    x, i, Debug, SettingSetDouble, zs, Settings, Info,
                    puid, id, mID,
                    string, key, key,
                    double, value, value
                    );

      ISettingsDelegatePtr delegate;

      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;

        if (!delegate) {
          (*mStored)[String(key)] = ValuePair(DataType_Double, string(value));
          ZS_LOG_TRACE(log("set double") + ZS_PARAM("key", key) + ZS_PARAM("value", value))
          return;
        }
      }

      return delegate->setDouble(key, value);
    }

    //-----------------------------------------------------------------------
    void Settings::clear(const char *key)
    {
      ZS_EVENTING_2(
                    x, i, Debug, SettingClear, zs, Settings, Info,
                    puid, id, mID,
                    string, key, key
                    );

      ISettingsDelegatePtr delegate;

      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;

        if (!delegate) {
          StoredSettingsMap::iterator found = mStored->find(key);
          if (found == mStored->end()) return;

          mStored->erase(found);
          ZS_LOG_TRACE(log("clear setting") + ZS_PARAM("key", key))
          return;
        }
      }

      delegate->clear(key);
    }

    //-----------------------------------------------------------------------
    bool Settings::apply(const char *jsonSettings)
    {
      ZS_EVENTING_2(
                    x, i, Debug, SettingApply, zs, Settings, Info,
                    puid, id, mID,
                    string, jsonSettings, jsonSettings
                    );

      typedef std::list<ElementPtr> NestedList;

      if (!jsonSettings) return false;

      ElementPtr rootEl = IHelper::toJSON(jsonSettings);
      if (!rootEl) return false;

      bool nestedValues = false;
      if (ZSLIB_SETTINGS_ROOT_JSON_IS_NESTED_NODE == rootEl->getValue()) {
        nestedValues = true;
      }

      bool found = false;

      NestedList parents;

      ElementPtr childEl = rootEl->getFirstChildElement();
      while (childEl) {

        if (nestedValues) {
          ElementPtr firstChildEl = childEl->getFirstChildElement();
          if (firstChildEl) {
            parents.push_back(childEl);
            childEl = firstChildEl;
            continue;
          }
        }


        String key = childEl->getValue();
        String value = childEl->getTextDecoded();
        String attribute = childEl->getAttributeValue("type");

        if (key.isEmpty()) continue;
        if (value.isEmpty()) continue;

        if (parents.size() > 0) {
          String path;
          for (auto iter = parents.begin(); iter != parents.end(); ++iter) {
            String name = (*iter)->getValue();
            path += name + "/";
          }
          key = path + key;
        }

        bool handled = true;
        bool wasEmpty = attribute.isEmpty();

        if (wasEmpty) {
          bool numberEncoded = false;

          // attempt to guess type based on input
          NodePtr checkEl = childEl->getFirstChild();
          while (checkEl) {

            if (checkEl->isText()) {
              TextPtr textEl = checkEl->toText();
              if (Text::Format_JSONNumberEncoded == textEl->getFormat()) {
                numberEncoded = true;
                break;
              }
            }

            checkEl = checkEl->getNextSibling();
          }

          if (numberEncoded) {
            if (String::npos != value.find_first_of('.')) {
              attribute = "double";
            } else if (isdigit(*value.c_str())) {
              attribute = "uint";
            } else if ('-' == *(value.c_str())) {
              attribute = "int";
            } else {
              attribute = "bool";
            }
          }
        }

        if (attribute.hasData()) {
          if ("string" == attribute) {
            setString(key, value);
          } else if ("int" == attribute) {
            try {
              setInt(key, Numeric<LONG>(value));
            } catch(const Numeric<LONG>::ValueOutOfRange &) {
              handled = false;
            }
          } else if ("uint" == attribute) {
            try {
              setUInt(key, Numeric<ULONG>(value));
            } catch(const Numeric<ULONG>::ValueOutOfRange &) {
              handled = false;
            }
          } else if ("bool" == attribute) {
            try {
              setBool(key, Numeric<bool>(value));
            } catch(const Numeric<bool>::ValueOutOfRange &) {
              handled = false;
            }
          } else if ("float" == attribute) {
            try {
              setFloat(key, Numeric<float>(value));
            } catch(const Numeric<float>::ValueOutOfRange &) {
              handled = false;
            }
          } else if ("double" == attribute) {
            try {
              setDouble(key, Numeric<double>(value));
            } catch(const Numeric<double>::ValueOutOfRange &) {
              handled = false;
            }
          }

          if (!handled) {
            if (wasEmpty) {
              // guess was wrong, set as a string
              setString(key, value);
            }
          }
        } else {
          setString(key, value);
        }

        found = handled;

        childEl = childEl->getNextSiblingElement();

        if (!childEl) {
          while ((parents.size() > 0) &&
                  (!childEl)) {
            childEl = parents.back();
            parents.pop_back();

            childEl = childEl->getNextSiblingElement();
          }
        }
      }

      return found;
    }

    //-------------------------------------------------------------------------
    void Settings::applyDefaults()
    {
      ZS_EVENTING_1(
                    x, i, Debug, SettingApplyDefaults, zs, Settings, Info,
                    puid, id, mID
                    );

      decltype(mDefaultsDelegates) applyList;

      {
        AutoRecursiveLock lock(mLock);
        applyList = mDefaultsDelegates;
      }

      for (auto iter = applyList->begin(); iter != applyList->end(); ++iter)
      {
        auto delegate = (*iter).lock();
        if (!delegate) continue;
        delegate->notifySettingsApplyDefaults();
      }

      {
        AutoRecursiveLock lock(mLock);
        mAppliedDefaults = true;
      }
    }

    //-------------------------------------------------------------------------
    void Settings::installDefaults(ISettingsApplyDefaultsDelegatePtr defaultsDelegate)
    {
      if (!defaultsDelegate) return;

      AutoRecursiveLock lock(mLock);
      auto replacementList = make_shared<DefaultsDelegatesList>();

      bool shouldAdd = true;
      for (auto iter_doNotUse = mDefaultsDelegates->begin(); iter_doNotUse != mDefaultsDelegates->end(); )
      {
        auto current = iter_doNotUse;
        ++iter_doNotUse;

        auto delegate = (*current).lock();
        if (!delegate) continue;

        if (defaultsDelegate.get() != delegate.get()) {
          replacementList->push_back(delegate);
          continue;
        }
        shouldAdd = false;
      }

      if (shouldAdd) {
        replacementList->push_back(defaultsDelegate);
      }

      mDefaultsDelegates = replacementList;
    }

    //-------------------------------------------------------------------------
    void Settings::removeDefaults(ISettingsApplyDefaultsDelegate &defaultsDelegate)
    {
      AutoRecursiveLock lock(mLock);
      auto replacementList = make_shared<DefaultsDelegatesList>();

      for (auto iter_doNotUse = mDefaultsDelegates->begin(); iter_doNotUse != mDefaultsDelegates->end(); )
      {
        auto current = iter_doNotUse;
        ++iter_doNotUse;

        auto delegate = (*current).lock();
        if (!delegate) continue;

        if (&defaultsDelegate == delegate.get()) continue;

        replacementList->push_back(delegate);
      }

      mDefaultsDelegates = replacementList;
    }

    //-------------------------------------------------------------------------
    void Settings::clearAll()
    {
      ZS_EVENTING_1(
                    x, i, Debug, SettingClearAll, zs, Settings, Info,
                    puid, id, mID
                    );

      ISettingsDelegatePtr delegate;

      {
        AutoRecursiveLock lock(mLock);
        delegate = mDelegate;

        mStored->clear();

        ZS_LOG_TRACE(log("clear all"))

        if (!delegate) return;
      }

      delegate->clearAll();
    }

    //-----------------------------------------------------------------------
    void Settings::verifySettingExists(const char *key) throw (InvalidUsage)
    {
      ZS_THROW_INVALID_ARGUMENT_IF(!key)

      ISettingsDelegatePtr delegate;

      {
        {
          AutoRecursiveLock lock(mLock);
          delegate = mDelegate;

          if (!delegate) {
            StoredSettingsMap::iterator found = mStored->find(key);
            if (found == mStored->end()) goto not_found;

            auto value = (*found).second.second;
            if (value.isEmpty()) goto not_found;

            ZS_EVENTING_3(
                          x, i, Debug, SettingVerifyExists, zs, Settings, Info,
                          puid, id, mID,
                          string, key, key,
                          bool, exists, true
                          );
            return;
          }
        }

        String result = delegate->getString(key);
        if (result.isEmpty()) goto not_found;

        ZS_EVENTING_3(
                      x, i, Debug, SettingVerifyExists, zs, Settings, Info,
                      puid, id, mID,
                      string, key, key,
                      bool, exists, true
                      );
        return;
      }

    not_found:
      {
        ZS_EVENTING_3(
                      x, w, Basic, SettingVerifyExistsFailed, zs, Settings, Info,
                      puid, id, mID,
                      string, key, key,
                      bool, exists, false
                      );

        ZS_LOG_WARNING(Basic, log("setting was not set") + ZS_PARAM("setting name", key))

        ZS_THROW_INVALID_USAGE(String("setting is missing a value: ") + key)
      }
    }

    //-----------------------------------------------------------------------
    void Settings::verifyRequiredSettings() throw (InvalidUsage)
    {
      applyDefaultsIfNoDelegatePresent();

      // check any required settings here:
    }

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    #pragma mark
    #pragma mark Settings => (internal)
    #pragma mark

    //-----------------------------------------------------------------------
    Settings::Params Settings::log(const char *message) const
    {
      ElementPtr objectEl = Element::create("zsLib::Settings");
      IHelper::debugAppend(objectEl, "id", mID);
      return Params(message, objectEl);
    }

    //-----------------------------------------------------------------------
    Settings::Params Settings::slog(const char *message)
    {
      return Params(message, "zsLib::Settings");
    }

    //-----------------------------------------------------------------------
    void Settings::applyDefaultsIfNoDelegatePresent()
    {
      {
        AutoRecursiveLock lock(mLock);
        if (mDelegate) return;

        if (mAppliedDefaults) return;
      }

      ZS_LOG_WARNING(Detail, log("To prevent issues with missing settings, the default settings are being applied. Recommend installing a settings delegate to fetch settings required from a externally."))

      applyDefaults();
    }

  } // namespace internal

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  #pragma mark
  #pragma mark ISettings
  #pragma mark

  //---------------------------------------------------------------------------
  void ISettings::setup(ISettingsDelegatePtr delegate)
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return;
    singleton->setup(delegate);
  }

  //---------------------------------------------------------------------------
  String ISettings::getString(const char *key)
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return String();
    return singleton->getString(key);
  }

  //---------------------------------------------------------------------------
  LONG ISettings::getInt(const char *key)
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return 0;
    return singleton->getInt(key);
  }

  //---------------------------------------------------------------------------
  ULONG ISettings::getUInt(const char *key)
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return 0;
    return singleton->getUInt(key);
  }

  //---------------------------------------------------------------------------
  bool ISettings::getBool(const char *key)
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return false;
    return singleton->getBool(key);
  }

  //---------------------------------------------------------------------------
  float ISettings::getFloat(const char *key)
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return 0.0f;
    return singleton->getFloat(key);
  }

  //---------------------------------------------------------------------------
  double ISettings::getDouble(const char *key)
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return 0.0;
    return singleton->getDouble(key);
  }

  //---------------------------------------------------------------------------
  void ISettings::setString(
                            const char *key,
                            const char *value
                            )
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return;
    singleton->setString(key, value);
  }

  //---------------------------------------------------------------------------
  void ISettings::setInt(
                          const char *key,
                          LONG value
                          )
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return;
    singleton->setInt(key, value);
  }

  //---------------------------------------------------------------------------
  void ISettings::setUInt(
                          const char *key,
                          ULONG value
                          )
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return;
    singleton->setUInt(key, value);
  }

  //---------------------------------------------------------------------------
  void ISettings::setBool(
                          const char *key,
                          bool value
                          )
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return;
    singleton->setBool(key, value);
  }

  //---------------------------------------------------------------------------
  void ISettings::setFloat(
                            const char *key,
                            float value
                            )
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return;
    singleton->setFloat(key, value);
  }

  //---------------------------------------------------------------------------
  void ISettings::setDouble(
                            const char *key,
                            double value
                            )
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return;
    singleton->setDouble(key, value);
  }

  //---------------------------------------------------------------------------
  void ISettings::clear(const char *key)
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return;
    singleton->clear(key);
  }

  //---------------------------------------------------------------------------
  bool ISettings::apply(const char *jsonSettings)
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return false;
    return singleton->apply(jsonSettings);
  }

  //---------------------------------------------------------------------------
  void ISettings::applyDefaults()
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return;
    singleton->applyDefaults();
  }

  //---------------------------------------------------------------------------
  void ISettings::installDefaults(ISettingsApplyDefaultsDelegatePtr defaultsDelegate)
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return;
    singleton->installDefaults(defaultsDelegate);
  }

  //---------------------------------------------------------------------------
  void ISettings::removeDefaults(ISettingsApplyDefaultsDelegate &defaultsDelegate)
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return;
    singleton->removeDefaults(defaultsDelegate);
  }

  //---------------------------------------------------------------------------
  void ISettings::clearAll()
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return;
    singleton->clearAll();
  }

  //---------------------------------------------------------------------------
  void ISettings::verifySettingExists(const char *key) throw (InvalidUsage)
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return;
    singleton->verifySettingExists(key);
  }

  //---------------------------------------------------------------------------
  void ISettings::verifyRequiredSettings() throw (InvalidUsage)
  {
    internal::SettingsPtr singleton = internal::Settings::singleton();
    if (!singleton) return;
    singleton->verifyRequiredSettings();
  }

} // namespace zsLib
