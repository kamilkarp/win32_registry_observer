#include "win32_registry_observer_plugin.h"

#include <windows.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

#include "reg_notify_registry_observe_service.h"
#include "win32_registry_observer_event_handlers.h"
#include "key_with_path.h"

#pragma warning(disable : 4311)
#pragma warning(disable : 4302)

namespace win32_registry_observer
{
  RegNotifyRegistryObserveService::RegNotifyRegistryObserveService(RegistryChangeEventHandler<> *registry_change_event_handler)
  {
    m_registry_change_event_handler = registry_change_event_handler;

    m_registry_keys_observer_threads_map = std::map<KeyWithPath, std::thread *>();
    m_cancel_threads_events_map = std::map<std::thread *, HANDLE>();
  }

  RegNotifyRegistryObserveService::~RegNotifyRegistryObserveService()
  {
    std::unique_lock<std::mutex> _ul(m_registry_keys_observer_threads_map_mutex);

    for (auto const &pair : m_cancel_threads_events_map)
    {
      HANDLE hCanelEvent = pair.second;
      SetEvent(hCanelEvent);
    }

    for (auto const &pair : m_registry_keys_observer_threads_map)
    {
      std::thread *thread = pair.second;
      thread->detach();
      delete thread;
    }

    m_registry_keys_observer_threads_map.clear();
    m_cancel_threads_events_map.clear();
  }

  bool RegNotifyRegistryObserveService::RegisterKeyForObservation(KeyWithPath keyWithPath)
  {
    std::cout << "registering key for observation: " << keyWithPath.GetStringRepresentation() << std::endl;

    std::unique_lock<std::mutex> _ul(m_registry_keys_observer_threads_map_mutex);
    if (m_registry_keys_observer_threads_map.find(keyWithPath) != m_registry_keys_observer_threads_map.end())
    {
      return false;
    }

    HANDLE hCancelEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    std::thread *thread = new std::thread(&RegNotifyRegistryObserveService::ListenForKeyChanges, this, keyWithPath, hCancelEvent);

    m_registry_keys_observer_threads_map[keyWithPath] = thread;
    m_cancel_threads_events_map[thread] = hCancelEvent;

    return true;
  }

  bool RegNotifyRegistryObserveService::UnregisterKeyFromObservation(KeyWithPath keyWithPath)
  {
    std::unique_lock<std::mutex> _ul(m_registry_keys_observer_threads_map_mutex);
    if (m_registry_keys_observer_threads_map.find(keyWithPath) == m_registry_keys_observer_threads_map.end())
    {
      return false;
    }

    std::cout << "unregistering key from observation: " << keyWithPath.GetStringRepresentation() << std::endl;
    std::thread *thread = m_registry_keys_observer_threads_map[keyWithPath];
    HANDLE hCancelEvent = m_cancel_threads_events_map[thread];

    SetEvent(hCancelEvent);

    thread->detach();
    delete thread;

    m_registry_keys_observer_threads_map.erase(keyWithPath);
    m_cancel_threads_events_map.erase(thread);

    return true;
  }

  void RegNotifyRegistryObserveService::SendRegChangedNotificationViaEventChannel(KeyWithPath keyWithPath, bool fromSubkeysEvent)
  {
    if (m_registry_change_event_handler == NULL)
      return;

    flutter::EncodableMap map;
    map = {
        {flutter::EncodableValue(std::string("type")), flutter::EncodableValue((int64_t)0)},
        {flutter::EncodableValue(std::string("predefinedKey")), flutter::EncodableValue(static_cast<int64_t>((unsigned long long)keyWithPath.GetPredefinedKey()))},
        {flutter::EncodableValue(std::string("path")), flutter::EncodableValue(keyWithPath.GetPath())},
        {flutter::EncodableValue(std::string("subkeysIncluded")), flutter::EncodableValue(fromSubkeysEvent)},
    };

    m_registry_change_event_handler->sendEvent(flutter::EncodableValue(map));
  }

  void RegNotifyRegistryObserveService::SendErrorNotificationViaEventChannel(KeyWithPath keyWithPath, bool fromSubkeysEvent, std::string errorMessage)
  {
    flutter::EncodableMap map;

    map = {
        {flutter::EncodableValue(std::string("type")), flutter::EncodableValue((int64_t)-1)},
        {flutter::EncodableValue(std::string("predefinedKey")), flutter::EncodableValue(static_cast<int64_t>((unsigned long long)keyWithPath.GetPredefinedKey()))},
        {flutter::EncodableValue(std::string("path")), flutter::EncodableValue(keyWithPath.GetPath())},
        {flutter::EncodableValue(std::string("subkeysIncluded")), flutter::EncodableValue(fromSubkeysEvent)},
    };

    m_registry_change_event_handler->sendEvent(flutter::EncodableValue(map));
  }

  void RegNotifyRegistryObserveService::ListenForKeyChanges(KeyWithPath keyWithPath, HANDLE hCancelEvent)
  {
    HKEY hKey;
  checkForPossibleFallback:
    LONG lResult = RegOpenKeyExA(keyWithPath.GetPredefinedKey(), keyWithPath.GetPath().length() == 0 ? NULL : keyWithPath.GetPath().c_str(), 0, KEY_NOTIFY, &hKey);
    if (lResult != ERROR_SUCCESS)
    {
      if (keyWithPath.GetPath().length() == 0)
      {
        std::cout << "Key does not exist: " << keyWithPath.GetStringRepresentation() << " " << lResult << std::endl;
        return;
      }

      std::cout << "Key does not exist. Trying to fallback to predefined key and wait until it gets created..." << keyWithPath.GetStringRepresentation() << " " << lResult << std::endl;

      hKey = WaitUntilKeyGetsCreated(keyWithPath, hCancelEvent);
      if (hKey == NULL)
      {
        std::cout << "Waiting for key aborted or failed: " << keyWithPath.GetStringRepresentation() << " " << lResult << std::endl;
        return;
      }

      SendRegChangedNotificationViaEventChannel(keyWithPath, false);
    }

    HANDLE hMultipleEvents[2];
    HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hEvent == NULL)
    {
      std::cout << "Error creating event key: " << keyWithPath.GetStringRepresentation() << " " << GetLastError() << std::endl;
      return;
    }

    hMultipleEvents[0] = hEvent;
    hMultipleEvents[1] = hCancelEvent;

    while (true)
    {
      // Make sure the key still exists with the same path. We could not do that, but then we will be notified about changes even when something renames the key.
      // Users of the lib want to be notified only about changes in the key with the specified path.
      HKEY foundKey = NULL;
      lResult = RegOpenKeyExA(keyWithPath.GetPredefinedKey(), keyWithPath.GetPath().length() == 0 ? NULL : keyWithPath.GetPath().c_str(), 0, KEY_NOTIFY, &hKey);
      if (lResult != ERROR_SUCCESS)
      {
        goto checkForPossibleFallback;
      }
      else
      {
        RegCloseKey(foundKey);
      }

      lResult = RegNotifyChangeKeyValue(hKey, TRUE, REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_ATTRIBUTES | REG_NOTIFY_CHANGE_SECURITY | REG_NOTIFY_CHANGE_NAME, hEvent, TRUE);
      if (lResult != ERROR_SUCCESS)
      {
        if (lResult == ERROR_KEY_DELETED)
        {
          std::cout << "Key deleted. Checking for possible fallback. " << keyWithPath.GetStringRepresentation() << " " << lResult << std::endl;
          goto checkForPossibleFallback;
        }

        std::cout << "Error notifying key: " << keyWithPath.GetStringRepresentation() << " " << lResult << std::endl;
        return;
      }

      DWORD dwWaitResult = WaitForMultipleObjects(2, hMultipleEvents, FALSE, INFINITE);

      if (dwWaitResult == WAIT_OBJECT_0 + 1)
      {
        std::cout << "fallback cancelled for: " << keyWithPath.GetStringRepresentation() << std::endl;
        RegCloseKey(hKey);
        return;
      }

      SendRegChangedNotificationViaEventChannel(keyWithPath, false);
    }
  }

  HKEY RegNotifyRegistryObserveService::WaitUntilKeyGetsCreated(KeyWithPath keyWithPath, HANDLE hCancelEvent)
  {
    HKEY hKey;
    LONG lResult;

    lResult = RegOpenKeyExA(keyWithPath.GetPredefinedKey(), NULL, 0, KEY_NOTIFY, &hKey);
    if (lResult != ERROR_SUCCESS)
    {
      std::cout << "Unable to fallback for: " << keyWithPath.GetStringRepresentation() << " " << lResult << std::endl;
      return NULL;
    }

    HANDLE hMultipleEvents[2];
    HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hEvent == NULL)
    {
      std::cout << "Error creating event key: " << keyWithPath.GetStringRepresentation() << " " << GetLastError() << std::endl;
      RegCloseKey(hKey);
      return NULL;
    }

    hMultipleEvents[0] = hEvent;
    hMultipleEvents[1] = hCancelEvent;

    std::cout << "fallback: observing until key gets created " << keyWithPath.GetStringRepresentation() << std::endl;

    while (true)
    {
      lResult = RegNotifyChangeKeyValue(hKey, TRUE, REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_ATTRIBUTES | REG_NOTIFY_CHANGE_SECURITY | REG_NOTIFY_CHANGE_NAME, hEvent, TRUE);
      if (lResult != ERROR_SUCCESS)
      {
        std::cout << "Error notifying key: " << keyWithPath.GetStringRepresentation() << lResult << std::endl;
        return NULL;
      }

      DWORD dwWaitResult = WaitForMultipleObjects(2, hMultipleEvents, FALSE, INFINITE);
      if (dwWaitResult == WAIT_OBJECT_0 + 1)
      {
        std::cout << "fallback cancelled for: " << keyWithPath.GetStringRepresentation() << std::endl;
        RegCloseKey(hKey);
        return NULL;
      }

      HKEY foundKey = NULL;
      lResult = RegOpenKeyExA(keyWithPath.GetPredefinedKey(), keyWithPath.GetPath().length() == 0 ? NULL : keyWithPath.GetPath().c_str(), 0, KEY_NOTIFY, &foundKey);
      if (lResult == ERROR_SUCCESS)
      {
        std::cout << "Key created: " << keyWithPath.GetStringRepresentation() << std::endl;

        // close predefined key and return observed key
        RegCloseKey(hKey);
        return foundKey;
      }
    }
  }

}
