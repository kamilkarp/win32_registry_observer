#include "include/win32_registry_observer/win32_registry_observer_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "win32_registry_observer_plugin.h"

void Win32RegistryObserverPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar)
{
    win32_registry_observer::Win32RegistryObserverPlugin::RegisterWithRegistrar(
        flutter::PluginRegistrarManager::GetInstance()
            ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
