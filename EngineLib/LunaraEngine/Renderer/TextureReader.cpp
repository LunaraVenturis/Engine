#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wimplicit-int-conversion"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma GCC diagnostic pop

#include <LunaraEngine/Renderer/TextureReader.hpp>
#include <LunaraEngine/Core/Log.h>
#include <cassert>

namespace LunaraEngine
{
    auto TextureReader::Read(const std::filesystem::path path, std::wstring name)
            -> std::expected<TextureDataView, std::error_code>
    {
        return Read(path, std::wstring_view(name));
    }

    auto TextureReader::Read(const std::filesystem::path& path, std::wstring_view name)
            -> std::expected<TextureDataView, std::error_code>
    {
        std::string fileName((path / name).string());

        if (!std::filesystem::exists(fileName)) { throw std::runtime_error("file does not exist!"); }

        uint32_t channels{}, width{}, height{};
        auto data = stbi_load(fileName.c_str(), (int*) &width, (int*) &height, (int*) &channels, 4);
        if (data == nullptr) { return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory)); }
        assert(data != nullptr);
        if (width == 0 || height == 0)
        {
            LOG_ERROR("%s", stbi_failure_reason());
            return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
        }
        assert(width > 0 && height > 0);
        if (channels != 4)
        {
            LOG_ERROR("%s", stbi_failure_reason());
            return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
        }
        assert(channels == 4);

        return TextureDataView{.data = data, .size = width * height * channels};
    }

    TextureInfo TextureReader::GetInfo(const std::filesystem::path& path, std::wstring_view name)
    {
        int width{}, height{}, channels{};

        std::string fileName((path / name).string());

        if (!std::filesystem::exists(fileName))
        {
            LOG_ERROR("%s does not exist!", fileName.c_str());
            throw std::runtime_error("file does not exist!");
        }

        int result = stbi_info(fileName.c_str(), (int*) &width, (int*) &height, (int*) &channels);
        assert(result != 0);
        if (result == 0) { LOG_ERROR("%s", stbi_failure_reason()); }
        if (width == 0 || height == 0) { LOG_ERROR("%s", stbi_failure_reason()); }
        assert(width > 0 && height > 0);
        if (channels != 4) { LOG_WARNING("%ls is not a 4 channel image(channels: %d)", name.data(), channels); }

        return {.path = path,
                .name = name,
                .width = (uint32_t) width,
                .height = (uint32_t) height,
                .format = static_cast<TextureFormat>(channels),
                .type = TextureDataType::Int,
                .channelDepth = (uint32_t) channels};
    }

}// namespace LunaraEngine
