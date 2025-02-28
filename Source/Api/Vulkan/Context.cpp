#include "Context.hpp"
#include "Allocator.hpp"
#include <string>

namespace adh {
    namespace vk {
        Context::Context(const Window& window, const char* name, const char* path) {
            Create(window, name, path);
        }

        Context::~Context() {
            Clear();
        }

        void Context::Create(const Window& window, const char* name, const char* path) {
            m_Contexts.EmplaceBack(this);
            m_Instance.Create(name);
            m_PhysicalDevice.Create(m_Instance);
            m_Surface.Create(m_Instance, window);
            m_DeviceQueues.Create(m_PhysicalDevice, m_Surface);
            m_Device.Create(m_PhysicalDevice, &m_DeviceQueues);

#if defined(ADH_APPLE)
                std::string p = path;
                int pos       = p.find("Contents");
                m_Path        = p.substr(0, pos);
#elif defined(ADH_LINUX)
                std::string p = path;
                int pos       = p.find("Exe");
                m_Path        = p.substr(0, pos);
#endif
        }

        void Context::Destroy() noexcept {
            Clear();
        }

        void Context::Clear() noexcept {
            Allocator::Destroy();
            m_Surface.Destroy();
            m_Device.Destroy();
            m_Instance.Destroy();
        }

        Context* Context::Get() noexcept {
            return m_Contexts[0];
        }

        VkInstance Context::GetInstance() noexcept {
            return m_Instance;
        }

        const VkInstance Context::GetInstance() const noexcept {
            return m_Instance;
        }

        VkPhysicalDevice Context::GetPhysicalDevice() noexcept {
            return m_PhysicalDevice;
        }

        const VkPhysicalDevice Context::GetPhysicalDevice() const noexcept {
            return m_PhysicalDevice;
        }

        VkSurfaceKHR Context::GetSurface() noexcept {
            return m_Surface;
        }

        const VkSurfaceKHR Context::GetSurface() const noexcept {
            return m_Surface;
        }

        DeviceQueueData Context::GetQueue(DeviceQueues::Family index) noexcept {
            return m_DeviceQueues[index];
        }

        const DeviceQueueData Context::GetQueue(DeviceQueues::Family index) const noexcept {
            return m_DeviceQueues[index];
        }

        VkDevice Context::GetDevice() noexcept {
            return m_Device;
        }

        const VkDevice Context::GetDevice() const noexcept {
            return m_Device;
        }

        const std::string Context::GetDataDirectory() const noexcept {
#if defined(ADH_WINDOWS)
            return DATA_DIRECTORY;
#else
#    if defined(ADH_INSTALL)
            return DATA_DIRECTORY;
#    else
            return m_Path + DATA_DIRECTORY;
#    endif
#endif
        }
    } // namespace vk
} // namespace adh
