/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * Image class implementation
 */

#include "Image.hpp"
#include <Core/Log.hpp>
#include <vulkan/vulkan.h>

namespace Engine
{

    template <typename T>
    ResultValueType<ImageStatus> Image<T>::Init(VkPhysicalDevice physicalDevice, VkDevice device, ImageSpec imageSpec)
    {
        m_ImageSpec = imageSpec;


        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, imageSpec.format, &formatProperties);

        auto tiling = ChooseImageTiling(formatProperties);
        if (ImageStatus::Success != tiling) { return ResultValueType{tiling.status}; }

        VkImageUsageFlags usageFlags;
        VkImageAspectFlags aspectFlags;

        auto imageFlagsStatus = GetImageFlags(imageSpec.type, usageFlags, aspectFlags);
        if (ImageStatus::Success != imageFlagsStatus) { return imageFlagsStatus; }

        VkImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.pNext = NULL;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = imageSpec.format;
        imageCreateInfo.extent = {imageSpec.extent.width, imageSpec.extent.height, 1};
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling = tiling.value;
        imageCreateInfo.usage = usageFlags;

        auto imageCreateStatus = vkCreateImage(device, &imageCreateInfo, NULL, &m_Image);

        if (VK_SUCCESS != imageCreateStatus)
        {
            LOG_ERROR("Could Not Create Image!\n");
            return ResultValueType{ImageStatus::Fail};
        }

        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device, m_Image, &memoryRequirements);

        auto allocateBufferStatus = m_MemoryBuffer.InitMemoryBuffer(
                physicalDevice, device, memoryProperties, memoryRequirements, (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

        if (BufferStatus::Success != allocateBufferStatus) { return BufferStatusToImageStatus(allocateBufferStatus); }

        auto bindStatus = m_MemoryBuffer.BindImage(device, m_Image);
        if (BufferStatus::Bound != bindStatus) { return BufferStatusToImageStatus(bindStatus); }

        auto imageViewStatus = CreateImageView(device, aspectFlags);
        if (ImageStatus::Success != imageViewStatus) { return {imageViewStatus}; }

        return ResultValueType{ImageStatus::Success};
    }

    template <typename T>
    ResultValue<ImageStatus, ImageSpec> Image<T>::GetImageSpec()
    {
        return ResultValue<ImageStatus, ImageSpec>{{}, m_ImageSpec};
    }

    template <typename T>
    ResultValue<ImageStatus, VkImage> Image<T>::GetImage()
    {
        if (VK_NULL_HANDLE != m_Image) { return ResultValue<ImageStatus, VkImage>{ImageStatus::Fail}; }
        return ResultValue<ImageStatus, VkImage>{ImageStatus::Success, m_Image};
    }

    template <typename T>
    ResultValue<ImageStatus, VkImageView> Image<T>::GetImageView()
    {
        if (VK_NULL_HANDLE != m_ImageView) { return ResultValue<ImageStatus, VkImageView>{ImageStatus::Fail}; }
        return ResultValue<ImageStatus, VkImageView>{ImageStatus::Success, m_ImageView};
    }

    template <typename T>
    ResultValueType<ImageStatus> Image<T>::Destroy(VkDevice device)
    {
        if (VK_NULL_HANDLE != m_Image) { return ResultValueType{ImageStatus::Fail}; }
        vkDestroyImage(device, m_Image, NULL);
        if (VK_NULL_HANDLE != m_ImageView) { return ResultValueType{ImageStatus::Fail}; }
        vkDestroyImageView(device, m_ImageView, NULL);

        auto destroyStatus = m_MemoryBuffer.DestroyMemoryBuffer(device);
        if (BufferStatus::Success != destroyStatus) { return BufferStatusToImageStatus(destroyStatus); }

        return {ImageStatus::Success};
    }

    template <typename T>
    inline T& Image<T>::GetMemoryBuffer()
    {
        return m_MemoryBuffer;
    }

    template <typename T>
    ResultValue<ImageStatus, VkImageTiling> Image<T>::ChooseImageTiling(VkFormatProperties& formatProperties)
    {
        VkImageTiling tiling;

        if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            tiling = VK_IMAGE_TILING_LINEAR;
        }
        else if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            tiling = VK_IMAGE_TILING_OPTIMAL;
        }
        else
        {
            LOG_ERROR("DepthStencilAttachment is not supported for D16Unorm depth format.\n");
            return ResultValue<ImageStatus, VkImageTiling>{ImageStatus::CanNotChooseTiling};
        }
        return ResultValue<ImageStatus, VkImageTiling>{ImageStatus::Success, tiling};
    }

    template <typename T>
    ResultValueType<ImageStatus> Image<T>::GetImageFlags(ImageType type, VkImageUsageFlags& usageFlags,
                                                         VkImageAspectFlags& aspectFlags)
    {
        switch (type)
        {
            case ImageType::Depth:
                usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
                break;
            default:
                LOG_ERROR("Wrong Image Type\n");
                return ResultValueType{ImageStatus::CanNotChooseImageFlags};
                break;
        }
        return ResultValueType{ImageStatus::Success};
    }

    template <typename T>
    ResultValueType<ImageStatus> Image<T>::CreateImageView(VkDevice device, VkImageAspectFlags aspectFlags)
    {

        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = m_Image;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = m_ImageSpec.format;
        imageViewCreateInfo.subresourceRange = {aspectFlags, 0, 1, 0, 1};

        auto imageViewStatus = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_ImageView);
        if (VK_SUCCESS != imageViewStatus)
        {
            LOG_ERROR("Can Not Create Image View!\n");
            return {ImageStatus::CanNotCreateImageView};
        }

        return {ImageStatus::Success};
    }

    template <typename T>
    ImageStatus Image<T>::BufferStatusToImageStatus(BufferStatus status)
    {
        switch (status)
        {
            case BufferStatus::Fail:
                return ImageStatus::Fail;
                break;
            case BufferStatus::CanNotFindMemoryType:
                return ImageStatus::CanNotFindMemoryType;
                break;
            case BufferStatus::CanNotAllocateMemory:
                return ImageStatus::CanNotAllocateMemory;
                break;
            case BufferStatus::CanNotBindMemory:
                return ImageStatus::CanNotBindImageMemory;
                break;
            case BufferStatus::CanNotMapMemory:
                return ImageStatus::CanNotMapImageMemory;
                break;
            case BufferStatus::Bound:
                return ImageStatus::Success;
            case BufferStatus::Success:
                return ImageStatus::Success;
                break;
            default:
                return ImageStatus::Fail;
                break;
        }
    }

}// namespace Engine