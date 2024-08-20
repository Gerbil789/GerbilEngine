#pragma once

#include <string>

namespace Engine 
{
    class Asset
    {
    public:
        virtual ~Asset() = default;
        //virtual void Load(const std::string& filePath) = 0;
        //virtual void Unload() = 0;
        virtual const std::string& GetFilePath() const { return filePath; }
        virtual const std::string& GetName() const { return name; }
   
    protected:
        std::string filePath;
        std::string name;
        //bool isLoaded = false;

        void SetName(const std::string& filePath) {
			size_t lastSlash = filePath.find_last_of("/\\");
			size_t lastDot = filePath.find_last_of(".");
			name = filePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
		}
    };

    class IAssetFactory {
    public:
        virtual ~IAssetFactory() = default;
        virtual Ref<Asset> Create(const std::string& filePath) = 0;
    };
}