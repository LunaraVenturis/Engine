#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * Image class definition
 */


#include <Core/Result.hpp>
#include <Renderer/MemoryBuffer.hpp>

namespace Engine
{
    enum class ImageType
    {
        None,
        Depth,
        Color
    };
    enum class ImageStatus
    {
        Fail,
        CanNotChooseTiling,
        CanNotChooseImageFlags,
        CanNotFindMemoryType,
        CanNotAllocateMemory,
        CanNotBindImageMemory,
        CanNotMapImageMemory,
        CanNotCreateImageView,
        Success
    };
}// namespace Engine

namespace Engine
{
    struct ImageSpec {
        ImageType type;
        VkFormat format;
        VkExtent2D extent;
    };
}// namespace Engine

namespace Engine
{

    template <typename T = MemoryBuffer>
    class Image
    {
    public:
        Image() = default;
        ~Image() = default;

    public:
        ResultValueType<ImageStatus> Init(VkPhysicalDevice physicalDevice, VkDevice device, ImageSpec imageSpec);
        ResultValue<ImageStatus, VkImage> GetImage();
        ResultValue<ImageStatus, VkImageView> GetImageView();
        ResultValue<ImageStatus, ImageSpec> GetImageSpec();
        ResultValueType<ImageStatus> Destroy(VkDevice device);
        T& GetMemoryBuffer();

    private:
        ResultValueType<ImageStatus> CreateImageView(VkDevice device, VkImageAspectFlags aspectFlags);

    private:
        virtual ResultValue<ImageStatus, VkImageTiling> ChooseImageTiling(VkFormatProperties& formatProperties);
        virtual ResultValueType<ImageStatus> GetImageFlags(ImageType type, VkImageUsageFlags& usageFlags,
                                                           VkImageAspectFlags& aspectFlags);

    private:
        static ImageStatus BufferStatusToImageStatus(BufferStatus status);

    private:
        VkImage m_Image;
        VkImageView m_ImageView;
        ImageSpec m_ImageSpec;
        T m_MemoryBuffer;
    };
}// namespace Engine

#include "Image_impl.hpp"