#include "win32_registry_observer_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

#include "reg_notify_registry_observe_service.h"

namespace win32_registry_observer
{
  void Win32RegistryObserverPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarWindows *registrar)
  {
    auto channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "win32_registry_observer",
            &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<Win32RegistryObserverPlugin>();

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result)
        {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });

    plugin->m_registry_change_event_channel = std::make_unique<flutter::EventChannel<flutter::EncodableValue>>(
        registrar->messenger(), std::string("win32_registry_observer/registry_change_channel"),
        &flutter::StandardMethodCodec::GetInstance());

    RegistryChangeEventHandler<> *registry_change_handler = new RegistryChangeEventHandler<>();
    plugin->m_registry_change_event_handler = registry_change_handler;

    RegistryObserveService *service = new RegNotifyRegistryObserveService(plugin->m_registry_change_event_handler);
    plugin->m_registry_observe_service = std::unique_ptr<RegistryObserveService>(service);

    auto obj_handle = static_cast<flutter::StreamHandler<flutter::EncodableValue> *>(plugin->m_registry_change_event_handler);

    std::unique_ptr<flutter::StreamHandler<flutter::EncodableValue>> ptr_handle{obj_handle};

    plugin->m_registry_change_event_channel->SetStreamHandler(std::move(ptr_handle));

    registrar->AddPlugin(std::move(plugin));
  }

  Win32RegistryObserverPlugin::Win32RegistryObserverPlugin() {}

  Win32RegistryObserverPlugin::~Win32RegistryObserverPlugin()
  {
    m_registry_observe_service->~RegistryObserveService();
    m_registry_observe_service = nullptr;
  }

  void Win32RegistryObserverPlugin::HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    const auto *args = std::get_if<flutter::EncodableMap>(method_call.arguments());

    if (method_call.method_name().compare("startObservingRegistryKey") == 0)
    {
      if (args == NULL)
      {
        return result->Error(false);
      }

      try
      {
        auto *predefinedKey = std::get_if<int64_t>(&args->at(flutter::EncodableValue("predefinedKey")));
        if (predefinedKey == NULL)
          return result->Error(false);

        const auto *path = std::get_if<std::string>(&args->at(flutter::EncodableValue("keyPath")));
        if (path == NULL)
          return result->Error(false);

        const auto *includeSubkeys = std::get_if<bool>(&args->at(flutter::EncodableValue("includeSubkeys")));
        if (includeSubkeys == NULL)
          return result->Error(false);

        m_registry_observe_service->RegisterKeyForObservation(KeyWithPath((HKEY) static_cast<uint64_t>(*predefinedKey), *path));
      }
      catch (const std::exception &e)
      {
        result->Error(e.what());
      }

      result->Success(true);
    }
    else if (method_call.method_name().compare("stopObservingRegistryKey") == 0)
    {
      if (args == NULL)
      {
        return result->Error(false);
      }

      try
      {
        const auto *predefinedKey = std::get_if<int64_t>(&args->at(flutter::EncodableValue("predefinedKey")));
        if (predefinedKey == NULL)
          return result->Error(false);

        const auto *path = std::get_if<std::string>(&args->at(flutter::EncodableValue("keyPath")));
        if (path == NULL)
          return result->Error(false);

        const auto *includeSubkeys = std::get_if<bool>(&args->at(flutter::EncodableValue("includeSubkeys")));
        if (includeSubkeys == NULL)
          return result->Error(false);

        m_registry_observe_service->UnregisterKeyFromObservation(KeyWithPath((HKEY) static_cast<uint64_t>(*predefinedKey), *path));
      }
      catch (const std::exception &e)
      {
        result->Error(e.what());
      }

      result->Success(true);
    }
    else
    {
      result->NotImplemented();
    }
  }

}
