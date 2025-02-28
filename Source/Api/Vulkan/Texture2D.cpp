#include "Texture2D.hpp"
#include "CommandBuffer.hpp"
#include "Context.hpp"
#include "Memory.hpp"
#include <Std/TGALoader.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <cmath>

#include <Vulkan/DescriptorSet.hpp>

namespace adh {
    namespace vk {
        Texture2D::Texture2D() noexcept : m_Descriptor{},
                                          m_Extent{},
                                          m_MipLevels{} {
        }

        Texture2D::Texture2D(const char* filePath,
                             VkImageUsageFlagBits imageUsage,
                             const Sampler* sampler,
                             bool isEntityComponent,
                             VkBool32 generateMinMap,
                             VkSharingMode sharingMode) {
            Create(filePath, imageUsage, sampler, isEntityComponent, generateMinMap, sharingMode);
        }

        Texture2D::Texture2D(const void* data,
                             std::size_t size,
                             VkExtent2D extent,
                             VkImageUsageFlagBits imageUsage,
                             const Sampler* sampler,
                             VkBool32 generateMinMap,
                             VkSharingMode sharingMode) {
            Create(data, size, extent, imageUsage, sampler, generateMinMap, sharingMode);
        }

        Texture2D::Texture2D(VkExtent2D extent,
                             VkImageUsageFlagBits imageUsage,
                             VkImageLayout imageLayout,
                             const Sampler* sampler,
                             VkSharingMode sharingMode) {
            Create(extent, imageUsage, imageLayout, sampler, sharingMode);
        }

        Texture2D::Texture2D(
            const char* filePath,
            VkImageUsageFlagBits imageUsage,
            VkFilter filter,
            bool isEntityComponent,
            VkBool32 generateMinMap,
            VkSharingMode sharingMode) {
            Create(filePath, imageUsage, filter, isEntityComponent, generateMinMap, sharingMode);
        }

        Texture2D::Texture2D(Texture2D&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        Texture2D& Texture2D::operator=(Texture2D&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));

            return *this;
        }

        void Texture2D::Create(const char* filePath,
                               VkImageUsageFlagBits imageUsage,
                               const Sampler* sampler,
                               bool isEntityComponent,
                               VkBool32 generateMinMap,
                               VkSharingMode sharingMode) {
            int texWidth, texHeight, texChannels;
            stbi_set_flip_vertically_on_load(true);
            stbi_uc* pixels        = stbi_load(filePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            VkDeviceSize imageSize = texWidth * texHeight * 4;

            m_Extent = { static_cast<std::uint32_t>(texWidth), static_cast<std::uint32_t>(texHeight) };
            UniformBuffer staging{ pixels, imageSize, 1u, VK_BUFFER_USAGE_TRANSFER_SRC_BIT };
            auto usageFlag{ SelectImageUsage(imageUsage, generateMinMap) };
            SelectImageLayout(imageUsage);

            CreateImage(
                staging,
                generateMinMap,
                usageFlag,
                sharingMode);

            mFilePath = filePath;
            auto pos  = mFilePath.find_last_of('/');
            mFileName = mFilePath.substr(pos + 1, mFilePath.size());

            InitializeDescriptor(sampler, isEntityComponent);
            stbi_image_free(pixels);
        }

        void Texture2D::Create(const void* data,
                               std::size_t size,
                               VkExtent2D extent,
                               VkImageUsageFlagBits imageUsage,
                               const Sampler* sampler,
                               VkBool32 generateMinMap,
                               VkSharingMode sharingMode) {
            m_Extent = extent;
            UniformBuffer staging{ data, size, 1u, VK_BUFFER_USAGE_TRANSFER_SRC_BIT };
            auto usageFlag{ SelectImageUsage(imageUsage, generateMinMap) };
            SelectImageLayout(imageUsage);

            CreateImage(
                staging,
                generateMinMap,
                usageFlag,
                sharingMode);

            InitializeDescriptor(sampler);
        }

        void Texture2D::Create(VkExtent2D extent,
                               VkImageUsageFlagBits imageUsage,
                               VkImageLayout imageLayout,
                               const Sampler* sampler,
                               VkSharingMode sharingMode) {
            m_Extent    = extent;
            m_MipLevels = 1u;

            SelectImageLayout(imageUsage);
            CreateImage(imageUsage, imageLayout, sharingMode);
            InitializeDescriptor(sampler);
        }

        void Texture2D::Create(
            const char* filePath,
            VkImageUsageFlagBits imageUsage,
            VkFilter filter,
            bool isEntityComponent,
            VkBool32 generateMinMap,
            VkSharingMode sharingMode) {
            auto samplerId = filter == VK_FILTER_LINEAR ? 0 : 1;
            Create(filePath, imageUsage, &m_DefaultSamplers[samplerId], isEntityComponent, generateMinMap, sharingMode);
        }

        void Texture2D::InitializeDefaultSamplers() {
            if (m_DefaultSamplers.IsEmpty()) {
                m_DefaultSamplers.EmplaceBack(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_COMPARE_OP_NEVER, VK_FALSE, VK_TRUE);
                m_DefaultSamplers.EmplaceBack(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_COMPARE_OP_NEVER, VK_FALSE, VK_TRUE);
            }
        }

        void Texture2D::CleanUpDefaultSamplers() {
            m_DefaultSamplers.Clear();
        }

        VkImage Texture2D::GetImage() noexcept {
            return m_Image.GetImage();
        }

        const VkImage Texture2D::GetImage() const noexcept {
            return m_Image.GetImage();
        }

        VkImageView Texture2D::GetImageView() noexcept {
            return m_Image.GetImageView();
        }

        const VkImageView Texture2D::GetImageView() const noexcept {
            return m_Image.GetImageView();
        }

        std::size_t Texture2D::GetWidth() const noexcept {
            return m_Extent.width;
        }

        std::size_t Texture2D::GetHeight() const noexcept {
            return m_Extent.height;
        }

        VkDescriptorImageInfo Texture2D::GetDescriptor() noexcept {
            return m_Descriptor;
        }

        const VkDescriptorImageInfo Texture2D::GetDescriptor() const noexcept {
            return m_Descriptor;
        }

        void Texture2D::SetLayout(VkImageLayout imageLayout) noexcept {
            TransferImageLayout(
                m_Image,
                m_Descriptor.imageLayout,
                imageLayout,
                VK_IMAGE_ASPECT_COLOR_BIT);
        }

        std::uint32_t Texture2D::GetMipLevels() const noexcept {
            return m_MipLevels;
        }

        void Texture2D::Destroy() noexcept {
            Clear();
        }

        Texture2D::operator VkImageView() noexcept {
            return m_Image.GetImageView();
        }

        Texture2D::operator const VkImageView() const noexcept {
            return m_Image.GetImageView();
        }

        VkImageUsageFlagBits Texture2D::SelectImageUsage(VkImageUsageFlagBits imageUsage, VkBool32 generateMinMap) noexcept {
            if (!generateMinMap) {
                m_MipLevels = 1u;
                return VkImageUsageFlagBits(imageUsage | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
            } else {
                m_MipLevels = static_cast<std::uint32_t>(std::floor(std::log2(std::max(m_Extent.width, m_Extent.height)))) + 1u;
                return VkImageUsageFlagBits(imageUsage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
            }
        }

        void Texture2D::SelectImageLayout(VkImageUsageFlagBits imageUsage) noexcept {
            if (static_cast<bool>((imageUsage & VK_IMAGE_USAGE_STORAGE_BIT))) {
                m_Descriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            } else {
                m_Descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }
        }

        void Texture2D::CreateImage(const UniformBuffer& staging,
                                    VkBool32 generateMinMap,
                                    VkImageUsageFlagBits usageFlag,
                                    VkSharingMode sharingMode) {
            m_Image.Create(
                { m_Extent.width, m_Extent.height, 1u },
                VK_FORMAT_R8G8B8A8_UNORM,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_TYPE_2D,
                (VkImageCreateFlagBits)0,
                m_MipLevels,
                1u,
                VK_SAMPLE_COUNT_1_BIT,
                usageFlag,
                VK_IMAGE_ASPECT_COLOR_BIT,
                VK_IMAGE_VIEW_TYPE_2D,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                sharingMode);

            TransferImageLayout(
                m_Image,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_ASPECT_COLOR_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                m_MipLevels,
                1u);

            CopyBufferToImage(
                staging,
                m_Image,
                VK_IMAGE_ASPECT_COLOR_BIT,
                { m_Extent.width, m_Extent.height, 1u },
                0u,
                1u);

            if (!generateMinMap) {
                TransferImageLayout(
                    m_Image,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    m_Descriptor.imageLayout,
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    m_MipLevels,
                    1u);
            }

            else {
                GenerateMipMaps();
            }
        }

        void Texture2D::CreateImage(VkImageUsageFlagBits usageFlag, VkImageLayout imageLayout, VkSharingMode sharingMode) {
            m_Image.Create(
                { m_Extent.width, m_Extent.height, 1u },
                VK_FORMAT_R8G8B8A8_UNORM,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_TYPE_2D,
                (VkImageCreateFlagBits)0,
                m_MipLevels,
                1u,
                VK_SAMPLE_COUNT_1_BIT,
                usageFlag,
                VK_IMAGE_ASPECT_COLOR_BIT,
                VK_IMAGE_VIEW_TYPE_2D,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                sharingMode);

            if (imageLayout != VK_IMAGE_LAYOUT_UNDEFINED) {
                TransferImageLayout(
                    m_Image,
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    imageLayout,
                    VK_IMAGE_ASPECT_COLOR_BIT);
            }
        }

        void Texture2D::GenerateMipMaps() ADH_NOEXCEPT {
            std::int32_t mipWidth{ static_cast<std::int32_t>(m_Extent.width) };
            std::int32_t mipHeight{ static_cast<std::int32_t>(m_Extent.height) };

            CommandBuffer commandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, DeviceQueues::Family::eTransfer);
            commandBuffer.Begin();

            GenerateImageBarriers(commandBuffer[0], mipWidth, mipHeight);

            commandBuffer.End();

            auto submitInfo{ initializers::SubmitInfo(
                1u, &commandBuffer[0],
                0, nullptr, nullptr,
                0, nullptr) };
            auto queue{ Context::Get()->GetQueue(DeviceQueues::Family::eTransfer).queue };
            ADH_THROW(vkQueueSubmit(queue, 1u, &submitInfo, VK_NULL_HANDLE) == VK_SUCCESS,
                      "Failed to submit to queue!");
            vkQueueWaitIdle(queue);

            commandBuffer.Free();
        }

        void Texture2D::GenerateImageBarriers(VkCommandBuffer commandBuffer, std::int32_t mipWidth, std::int32_t mipHeight) noexcept {
            for (std::int32_t i{ 1 }; i < m_MipLevels; ++i) {
                ImageBarrier(
                    commandBuffer,
                    VK_ACCESS_TRANSFER_WRITE_BIT,
                    VK_ACCESS_TRANSFER_READ_BIT,
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    VK_QUEUE_FAMILY_IGNORED,
                    VK_QUEUE_FAMILY_IGNORED,
                    m_Image,
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    i - 1);

                BlitImage(commandBuffer, i, mipWidth, mipHeight);

                ImageBarrier(
                    commandBuffer,
                    VK_ACCESS_TRANSFER_READ_BIT,
                    VK_ACCESS_SHADER_READ_BIT,
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    m_Descriptor.imageLayout,
                    VK_QUEUE_FAMILY_IGNORED,
                    VK_QUEUE_FAMILY_IGNORED,
                    m_Image,
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    i - 1);

                if (mipWidth > 1) {
                    mipWidth /= 2;
                }

                if (mipHeight > 1) {
                    mipHeight /= 2;
                }
            }

            ImageBarrier(
                commandBuffer,
                VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_ACCESS_SHADER_READ_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                m_Descriptor.imageLayout,
                VK_QUEUE_FAMILY_IGNORED,
                VK_QUEUE_FAMILY_IGNORED,
                m_Image,
                VK_IMAGE_ASPECT_COLOR_BIT,
                m_MipLevels - 1);
        }

        void Texture2D::BlitImage(VkCommandBuffer commandBuffer, std::int32_t index, std::int32_t mipWidth, std::int32_t mipHeight) noexcept {
            VkImageBlit blit{};
            blit.srcOffsets[0]                 = { 0u, 0u, 0u };
            blit.srcOffsets[1]                 = { mipWidth, mipHeight, 1u };
            blit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel       = index - 1;
            blit.srcSubresource.baseArrayLayer = 0u;
            blit.srcSubresource.layerCount     = 1u;
            blit.dstOffsets[0]                 = { 0u, 0u, 0u };
            blit.dstOffsets[1]                 = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel       = index;
            blit.dstSubresource.baseArrayLayer = 0u;
            blit.dstSubresource.layerCount     = 1u;

            vkCmdBlitImage(commandBuffer,
                           m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1u, &blit,
                           VK_FILTER_LINEAR);
        }

        void Texture2D::InitializeDescriptor(const Sampler* sampler, bool isEntityComponent) noexcept {
            if (sampler) {
                m_Descriptor.sampler = sampler->Get();
            } else {
                m_Descriptor.sampler = nullptr;
            }
            m_Descriptor.imageView = m_Image.GetImageView();

            if (isEntityComponent) {
                mDescriptorSetID = TextureDescriptors::GetDescriptorID(m_Descriptor);
            }
        }

        void Texture2D::MoveConstruct(Texture2D&& rhs) noexcept {
            m_Image = Move(rhs.m_Image);

            m_Descriptor = rhs.m_Descriptor;
            m_Extent     = rhs.m_Extent;
            m_MipLevels  = rhs.m_MipLevels;

            mDescriptorSetID = rhs.mDescriptorSetID;
            mFileName        = rhs.mFileName;
            mFilePath        = rhs.mFilePath;
            mIsLinearFilter  = rhs.mIsLinearFilter;

            rhs.m_Descriptor = {};
            rhs.m_Extent     = {};
            rhs.m_MipLevels  = 0u;
        }

        void Texture2D::Clear() noexcept {
            m_Image.Destroy();
            m_Descriptor    = {};
            m_Extent        = {};
            m_MipLevels     = 0u;
            mIsLinearFilter = true;
            // TextureDescriptors::FreeDescriptor(mDescriptorSetID);
        }
    } // namespace vk
} // namespace adh
