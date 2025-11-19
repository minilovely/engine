#include "MaterialAssets.h"

std::unordered_map<std::string, std::shared_ptr<Texture2D>> MaterialAssets::textureCache;

std::shared_ptr<Texture2D> MaterialAssets::getTexture(const std::string& fullPath)
{
    //找到纹理
    auto it = textureCache.find(fullPath);
    if (it != textureCache.end())
    {
        std::cout << "[MaterialAssets] Using cached texture: " << fullPath << std::endl;
        return it->second;
    }
    //未找到纹理
    try
    {
        auto texture = std::make_shared<Texture2D>(fullPath);
        textureCache[fullPath] = texture;
        std::cout << "[MaterialAssets] Loaded new texture: " << fullPath << std::endl;
        return texture;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[MaterialAssets] Failed to load texture: " << fullPath
            << " Error: " << e.what() << std::endl;
        return nullptr;
    }
}

    void MaterialAssets::ClearCache()
    {
        textureCache.clear();
    }

    size_t MaterialAssets::getCacheSize()
    {
        return textureCache.size();
    }

    std::shared_ptr<Material> MaterialAssets::createMaterialWithTexture(const std::string& texturePath)
    {
        auto tex = getTexture(texturePath);
        auto mat = std::shared_ptr<Material>();
        mat->addTexture(tex);
        return mat;
    }
