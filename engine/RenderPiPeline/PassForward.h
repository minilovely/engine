#pragma once
#include <memory>
#include "Pass.h"
#include "../Render/Shader.h"

class PassForward : public Pass
{
public:
	PassForward();
	~PassForward();

	void Init() override;
	void Draw(const std::vector<Mesh*>& meshes, const Camera& camera) override;
	void ReloadShader(const std::string& vsSrc, const std::string& fsSrc);
private:
	std::shared_ptr<Shader> shader;;
};


