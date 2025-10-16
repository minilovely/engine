#include "MeshPrimitives.h"

MeshCPU MeshPrimitives::makePlane(float width, float height)
{
	MeshCPU mesh;
	float w = width * 0.5f;
	float h = height * 0.5f;
	mesh.vertices =
	{
		//position     normal     uv
		{{-w, 0, -h}, {0, 1, 0}, {0, 0}},
		{{ w, 0, -h}, {0, 1, 0}, {1, 0}},
		{{ w, 0,  h}, {0, 1, 0}, {1, 1}},
		{{-w, 0,  h}, {0, 1, 0}, {0, 1}}
	};
	mesh.indices = { 0, 1, 2, 2, 3, 0 };
	return MeshCPU();
}
