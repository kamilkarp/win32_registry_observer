#ifndef FLUTTER_PLUGIN_KEY_WITH_PATH_PLUGIN_H_
#define FLUTTER_PLUGIN_KEY_WITH_PATH_PLUGIN_H_

#include <windows.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

#include "win32_registry_observer_plugin.h"

#pragma warning(disable : 4302)
#pragma warning(disable : 4311)

namespace win32_registry_observer
{
  class KeyWithPath
  {
  private:
    HKEY m_predefinedKey;
    std::string m_path;

  public:
    KeyWithPath(HKEY predefinedKey, std::string path)
    {
      m_predefinedKey = predefinedKey;
      m_path = path;
    };

    KeyWithPath(const KeyWithPath &keyWithPath)
    {
      m_predefinedKey = keyWithPath.m_predefinedKey;
      m_path = keyWithPath.m_path;
    }

    bool operator==(const KeyWithPath &keyWithPath) const
    {
      return m_predefinedKey == keyWithPath.m_predefinedKey && m_path == keyWithPath.m_path;
    }

    bool operator<(const KeyWithPath &keyWithPath) const
    {
      return m_predefinedKey < keyWithPath.m_predefinedKey || m_path < keyWithPath.m_path;
    }

    HKEY GetPredefinedKey()
    {
      return m_predefinedKey;
    }

    std::string GetPath()
    {
      return m_path;
    }

    std::string GetStringRepresentation()
    {
      std::stringstream ss;

      if (static_cast<int64_t>((unsigned long long)m_predefinedKey) == 0x80000000)
      {
        ss << "HKEY_CLASSES_ROOT\\";
      }
      else if (static_cast<int64_t>((unsigned long long)m_predefinedKey) == 0x80000001)
      {
        ss << "HKEY_CURRENT_USER\\";
      }
      else if (static_cast<int64_t>((unsigned long long)m_predefinedKey) == 0x80000002)
      {
        ss << "HKEY_LOCAL_MACHINE\\";
      }
      else if (static_cast<int64_t>((unsigned long long)m_predefinedKey) == 0x80000003)
      {
        ss << "HKEY_USERS\\";
      }
      else if (static_cast<int64_t>((unsigned long long)m_predefinedKey) == 0x80000005)
      {
        ss << "HKEY_CURRENT_CONFIG\\";
      }
      else if (static_cast<int64_t>((unsigned long long)m_predefinedKey) == 0x80000004)
      {
        ss << "HKEY_PERFORMANCE_DATA\\";
      }

      if (m_path.length() > 0)
      {
        ss << m_path;
      }

      return ss.str();
    }

    ~KeyWithPath(){};
  };
}

#endif // FLUTTER_PLUGIN_KEY_WITH_PATH_PLUGIN_H_
