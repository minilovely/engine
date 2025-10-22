#pragma once
#include "../Core/math.h"
#include "RenderQueue.h"

#include <vector>

class Camera;
class Mesh;

class Pass
{
public:
	Pass() {};
	~Pass() = default;
	virtual void Init() = 0;
	virtual void Collect(const Camera& cam,Mesh* mesh,RenderQueue& outQueue) = 0;
private:

};
