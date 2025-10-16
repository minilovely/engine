#pragma once
#include "Shader.h"
#include "Texture2D.h"
#include <memory>
#include <vector>
#include <iostream>
class Material
{
public:
	std::shared_ptr<Shader> shader;
	std::vector<std::shared_ptr<Texture2D>> textures;

	void bind() const
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
	void addTexture(std::shared_ptr<Texture2D> t) { textures.push_back(t); }
private:

};
