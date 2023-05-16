#ifndef FLUTTER_PLUGIN_WIN32_REGISTRY_OBSERVER_PLUGIN_H_
#define FLUTTER_PLUGIN_WIN32_REGISTRY_OBSERVER_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

#include "win32_registry_observer_event_handlers.h"
#include "registry_observe_service.h"

namespace win32_registry_observer
{

    class Win32RegistryObserverPlugin : public flutter::Plugin
    {
    public:
        static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

        Win32RegistryObserverPlugin();

        virtual ~Win32RegistryObserverPlugin();

        // Disallow copy and assign.
        Win32RegistryObserverPlugin(const Win32RegistryObserverPlugin &) = delete;
        Win32RegistryObserverPlugin &operator=(const Win32RegistryObserverPlugin &) = delete;

    private:
        // Called when a method is called on this plugin's channel from Dart.
        void HandleMethodCall(
            const flutter::MethodCall<flutter::EncodableValue> &method_call,
            std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

        std::unique_ptr<flutter::EventChannel<flutter::EncodableValue>> m_registry_change_event_channel;
        RegistryChangeEventHandler<> *m_registry_change_event_handler;

        std::unique_ptr<RegistryObserveService> m_registry_observe_service;

    };

} // namespace win32_registry_observer

#endif // FLUTTER_PLUGIN_WIN32_REGISTRY_OBSERVER_PLUGIN_H_
