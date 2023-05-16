#ifndef FLUTTER_PLUGIN_REG_NOTIFY_REGISTRY_OBSERVE_SERVICE_PLUGIN_H_
#define FLUTTER_PLUGIN_REG_NOTIFY_REGISTRY_OBSERVE_SERVICE_PLUGIN_H_

#include <windows.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

#include "win32_registry_observer_plugin.h"
#include "registry_observe_service.h"
#include "key_with_path.h"

namespace win32_registry_observer
{
  class RegNotifyRegistryObserveService : public RegistryObserveService
  {
  private:
    std::map<KeyWithPath, std::thread *> m_registry_keys_observer_threads_map;
    std::map<std::thread *, HANDLE> m_cancel_threads_events_map;
    std::mutex m_registry_keys_observer_threads_map_mutex;
    void ListenForKeyChanges(KeyWithPath keyWithPath, HANDLE hCancelEvent);
    HKEY RegNotifyRegistryObserveService::WaitUntilKeyGetsCreated(KeyWithPath keyWithPath, HANDLE hCancelEvent);
    void RegNotifyRegistryObserveService::SendRegChangedNotificationViaEventChannel(KeyWithPath keyWithPath, bool fromSubkeysEvent);
    void SendErrorNotificationViaEventChannel(KeyWithPath keyWithPath, bool fromSubkeysEvent, std::string errorMessage);

    RegistryChangeEventHandler<> *m_registry_change_event_handler;

  public:
    RegNotifyRegistryObserveService(RegistryChangeEventHandler<> *registry_change_event_handler);

    ~RegNotifyRegistryObserveService();

    bool RegisterKeyForObservation(KeyWithPath keyWithPath);

    bool UnregisterKeyFromObservation(KeyWithPath keyWithPath);
  };
}

#endif // FLUTTER_PLUGIN_REG_NOTIFY_REGISTRY_OBSERVE_SERVICE_PLUGIN_H_
