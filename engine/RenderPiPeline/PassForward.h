#pragma once
#include "Pass.h"
#include "../RenderPiPeline/RenderQueue.h"
#include "../Assets/PassAssets.h"
#include <memory>

class PassForward : public Pass
{
public:
	PassForward() = default;

	void Init() override;
	void Collect(const Camera& camera, Mesh* mesh,RenderQueue& outQueue) override;
private:

};


