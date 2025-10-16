#pragma once
#include "Pass.h"
#include "../Render/Shader.h"
#include "../Assets/PassAssets.h"
#include <memory>

class PassForward : public Pass
{
public:
	PassForward() = default;

	void Init() override;
	void Draw(const std::vector<Mesh*>& meshes, const Camera& camera) override;
	void ReloadShader(const std::string& vsSrc, const std::string& fsSrc);
private:
	std::shared_ptr<PassAssets> asset;
	std::shared_ptr<Shader> shader;
};


