#pragma once
#include "../Render/Material.h"
#include "../Render/Texture2D.h"
#include <memory>
#include <unordered_map>
#include <string>

class MaterialAssets
{
public:
	static std::shared_ptr<Texture2D> getTexture(const std::string& fullPath);

	static void ClearCache();
	static size_t getCacheSize();
	static std::shared_ptr<Material> createMaterialWithTexture(const std::string& texturePath);

private:
	static std::unordered_map<std::string, std::shared_ptr<Texture2D>> textureCache;
};

