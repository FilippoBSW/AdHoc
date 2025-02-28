#include "Instance.hpp"
#include "Initializers.hpp"

#include <Std/Array.hpp>
#include <Utility.hpp>

namespace adh {
    namespace vk {
        Instance::Instance() noexcept : m_Instance{ VK_NULL_HANDLE } {
        }

        Instance::Instance(const char* name) {
            Create(name);
        }

        Instance::Instance(Instance&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        Instance& Instance::operator=(Instance&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));
            return *this;
        }

        Instance::~Instance() {
            Clear();
        }

        void Instance::Create(const char* name) {
            auto applicationInfo{ initializers::ApplicationInfo("AdHoc", VK_API_VERSION_1_2) };
            Array<const char*> validationLayers;
#if defined(ADH_DEBUG)
            validationLayers.EmplaceBack("VK_LAYER_KHRONOS_validation");
#endif
            Array<const char*> instanceExtentions{
		    "VK_KHR_surface",
		    ADH_VK_PLATFORM_SURFACE,
	    	"VK_KHR_portability_enumeration" };
            auto instanceCreateInfo{ initializers::InstanceCreateInfo(applicationInfo, instanceExtentions, validationLayers) };
            ADH_THROW(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) == VK_SUCCESS,
                      "Failed to create VkInstance!");
        }

        void Instance::Destroy() noexcept {
            Clear();
        }

        Instance::operator VkInstance() noexcept {
            return m_Instance;
        }

        Instance::operator const VkInstance() const noexcept {
            return m_Instance;
        }

        void Instance::MoveConstruct(Instance&& rhs) noexcept {
            m_Instance     = rhs.m_Instance;
            rhs.m_Instance = VK_NULL_HANDLE;
        }

        void Instance::Clear() noexcept {
            if (m_Instance != VK_NULL_HANDLE) {
                vkDestroyInstance(m_Instance, nullptr);
                m_Instance = VK_NULL_HANDLE;
            }
        }
    } // namespace vk
} // namespace adh
