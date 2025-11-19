#pragma once
#include "model.h"
class MeshPrimitives
{
public:
	MeshCPU makePlane(float width = 1.0f, float height = 1.0f);
	MeshCPU makeCube(float size);

	void setColor(glm::vec3 col) { color = col; }
private:
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
};
