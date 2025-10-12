#pragma once
#include <vector>
#include "../Core/math.h"

class Camera;
class Mesh;

class Pass
{
public:
	Pass() {};
	virtual ~Pass() = default;
	virtual void Init() = 0;

	virtual void Draw(const std::vector<Mesh*>& meshes, const Camera& camera) = 0;
private:

};
