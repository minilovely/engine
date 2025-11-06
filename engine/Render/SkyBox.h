#pragma once
#include "../Render/MeshGPU.h"
#include "../Render/Shader.h"
#include "../Render/TextureCube.h"

#include <memory>
class SkyBox
{
public:
	SkyBox() = default;
	~SkyBox() = default;

	void Init();

	void SetCubeMap(std::shared_ptr<TextureCube> newCube) { cubeMap = std::move(newCube); }

	void Render(const glm::mat4& view, const glm::mat4& proj);
private:
	std::unique_ptr<MeshGPU> meshGPU;
	std::shared_ptr<TextureCube> cubeMap; 
	std::shared_ptr<Shader> shader;
};

