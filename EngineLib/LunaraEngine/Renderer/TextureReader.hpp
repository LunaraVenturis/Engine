#pragma once
#include <cstdint>
#include <vector>

#include <expected>
#include <LunaraEngine/Renderer/CommonTypes.hpp>

namespace LunaraEngine
{
    struct TextureDataView {
        uint8_t* data;
        size_t size;
    };

    class TextureDataContainer
    {
    public:
        TextureDataContainer() = default;
        ~TextureDataContainer() = default;

    public:
        void AddView(TextureDataView view) { views.push_back(view); }

        void AddView(uint8_t* data, size_t size) { views.emplace_back(data, size); }

        void Clear() { views.clear(); }

        const std::vector<TextureDataView>& GetViews() const { return views; }

    private:
        std::vector<TextureDataView> views;
    };

    class TextureReader

    {
    public:
        TextureReader() = delete;
        ~TextureReader() = delete;

    public:
        static auto Read(const std::filesystem::path& path, std::wstring_view name)
                -> std::expected<TextureDataView, std::error_code>;
        static auto Read(const std::filesystem::path path, std::wstring name)
                -> std::expected<TextureDataView, std::error_code>;

        static TextureInfo GetInfo(const std::filesystem::path& path, std::wstring_view name);
    };
}// namespace LunaraEngine