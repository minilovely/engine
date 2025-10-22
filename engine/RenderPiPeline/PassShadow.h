#pragma once
#include "Pass.h"
#include "../Render/Shader.h"

#include<memory>

class PassShadow : public Pass
{
public:
	void Init() override;
	void Collect(const Camera& cam, Mesh* mesh, RenderQueue& outQueue) override;

	std::vector<ShadowCommand> getShadowCmds() const { return cmds; }
private:
	std::shared_ptr<Shader> shader;
	std::vector<ShadowCommand> cmds;
};

