#pragma once

#include <string>

namespace Engine 
{
    class Asset
    {
    public:
        virtual ~Asset() = default;
        virtual void Load(const std::string& filePath) = 0;
        virtual void Unload() = 0;
        virtual const std::string& GetFilePath() const { return filePath; }
   
    protected:
        std::string filePath;
        bool isLoaded = false;
    };
}