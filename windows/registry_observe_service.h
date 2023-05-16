#ifndef FLUTTER_PLUGIN_REGISTRY_OBSERVE_SERVICE_PLUGIN_H_
#define FLUTTER_PLUGIN_REGISTRY_OBSERVE_SERVICE_PLUGIN_H_

#include <windows.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

#include "win32_registry_observer_plugin.h"
#include "key_with_path.h"


namespace win32_registry_observer
{
  class RegistryObserveService
  {
  public:
    RegistryObserveService(){};

    virtual ~RegistryObserveService(){};

    virtual bool RegisterKeyForObservation(KeyWithPath keyWithPath) = 0;

    virtual bool UnregisterKeyFromObservation(KeyWithPath keyWithPath) = 0;
  };
}

#endif // FLUTTER_PLUGIN_REGISTRY_OBSERVE_SERVICE_PLUGIN_H_
