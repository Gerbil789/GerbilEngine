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
		void SetModified(bool modified) { this->modified = modified; } //TODO: i dont like this
		bool IsModified() const { return modified; }
	protected:
		Asset(const std::filesystem::path& path) : path(path) {}
		std::filesystem::path path;
		bool modified = false;
	};

	class IAssetFactory {
	public:
		virtual ~IAssetFactory() = default;
		virtual Ref<Asset> Load(const std::filesystem::path& path, const std::any& data = std::any()) = 0;
		virtual Ref<Asset> Create(const std::filesystem::path& path, const std::any& data = std::any()) = 0;
	};
}