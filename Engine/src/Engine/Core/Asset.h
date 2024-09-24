#pragma once
#include <filesystem>
#include <any>

namespace Engine 
{
    class Asset
    {
    public:
        virtual ~Asset() = default;
		const std::filesystem::path& GetFilePath() const { return path; }
		void SetFilePath(const std::filesystem::path& path) { this->path = path; }
    protected:
		Asset(const std::filesystem::path& path) { SetFilePath(path); }
		std::filesystem::path path;
    };

    class IAssetFactory {
    public:
        virtual ~IAssetFactory() = default;
        virtual Ref<Asset> Load(const std::filesystem::path& path, const std::any& data = std::any()) = 0;
        virtual Ref<Asset> Create(const std::filesystem::path& path, const std::any& data = std::any()) = 0;
    };
}