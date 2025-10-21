#pragma once
#include "Shader.h"
#include "Texture2D.h"

#include <memory>
#include <vector>
#include <iostream>
class Material
{
public:

	void bindTextures()
	{
		if (!shader)
		{
			std::cerr << "[Material] no shader, skip bind." << std::endl;
			return;
		}
		shader->use();
		for (size_t i = 0; i < textures.size(); i++)
		{
			textures[i]->bind(i);
			shader->setInt("diffTex[" + std::to_string(i)+"]", i);
		}
	}

	void setShader(std::shared_ptr<Shader> s) { shader = s; }
	const auto& getShader() const { return shader; }
	void addTexture(std::shared_ptr<Texture2D> t) { textures.push_back(t); }
	glm::vec3 getTexColor() const { return textures[0]->getColor(); }
private:
	std::shared_ptr<Shader> shader;
	std::vector<std::shared_ptr<Texture2D>> textures;
};
