#pragma once
#include <filesystem>
#include <any>

#define PROPERTY(type, name)                                 \
private:                                                     \
    type name##_;                                            \
public:                                                      \
    type& Get##name() { return name##_; }                    \
    void Set##name(const type& value) {                      \
        if (name##_ != value) {                              \
            name##_ = value;                                 \
            modified = true;  /* Mark asset as modified */   \
        }                                                    \
    }

namespace Engine 
{
    class Asset
    {
    public:
        virtual ~Asset() = default;
		const std::filesystem::path& GetFilePath() const { return path; }
		void SetFilePath(const std::filesystem::path& path) { this->path = path; }
		void SetModified(bool modified) { this->modified = modified; }
		bool IsModified() const { return modified; }
    protected:
		Asset(const std::filesystem::path& path) { SetFilePath(path); }
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