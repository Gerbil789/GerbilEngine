#pragma once

#include <string>

namespace Engine 
{
    class Asset
    {
    public:
        virtual ~Asset() = default;
        const std::string& GetFilePath() const { return filePath; }
        const std::string& GetName() const { return name; }
        void SetFilePath(const std::string& filePath) { this->filePath = filePath;  SetName(filePath); }
       
    protected:
        Asset(const std::string& filePath) : filePath(filePath) { SetName(filePath); }
        std::string filePath;
        std::string name;

    private:
        void SetName(const std::string& filePath) 
        {
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