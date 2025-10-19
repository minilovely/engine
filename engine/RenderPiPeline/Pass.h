#pragma once
#include <vector>
#include "../Core/math.h"
#include "RenderQueue.h"
class Camera;
class Mesh;

class Pass
{
public:
	Pass() {};
	virtual ~Pass() = default;
	virtual void Init() = 0;
	virtual void Collect(const Camera& cam,Mesh* mesh,RenderQueue& outQueue) = 0;
private:

};
