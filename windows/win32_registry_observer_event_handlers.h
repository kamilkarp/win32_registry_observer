#ifndef FLUTTER_PLUGIN_WIN32_REGISTRY_OBERVER_EVENT_HANDLERS_H_
#define FLUTTER_PLUGIN_WIN32_REGISTRY_OBERVER_EVENT_HANDLERS_H_

#include <flutter/method_channel.h>
#include <flutter/event_channel.h>

#include <iostream>
#include <mutex>
#include <atomic>

namespace win32_registry_observer
{
    template <typename T = flutter::EncodableValue>
    class RegistryChangeEventHandler : public flutter::StreamHandler<T>
    {
    public:
        RegistryChangeEventHandler() = default;
        virtual ~RegistryChangeEventHandler() = default;

        void sendEvent(flutter::EncodableValue _data)
        {
            std::unique_lock<std::mutex> _ul(m_mtx);
            if (m_sink.get())
                m_sink.get()->Success(_data);
        }

    protected:
        std::unique_ptr<flutter::StreamHandlerError<T>> OnListenInternal(const T *arguments, std::unique_ptr<flutter::EventSink<T>> &&events) override
        {
            std::unique_lock<std::mutex> _ul(m_mtx);
            m_sink = std::move(events);
            return nullptr;
        }
        std::unique_ptr<flutter::StreamHandlerError<T>> OnCancelInternal(const T *arguments) override
        {
            std::unique_lock<std::mutex> _ul(m_mtx);
            m_sink.release();
            return nullptr;
        }

    private:
        flutter::EncodableValue m_value;
        std::mutex m_mtx;
        std::unique_ptr<flutter::EventSink<T>> m_sink;
    };
}

#endif // FLUTTER_PLUGIN_WIN32_REGISTRY_OBERVER_EVENT_HANDLERS_H_