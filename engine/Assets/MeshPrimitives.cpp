#include "MeshPrimitives.h"
#include "../Render/Texture2D.h"

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
	mesh.indices = { 0, 2, 1, 2, 0, 3 };

	mesh.material = std::make_shared<Material>();
	mesh.material->addTexture(std::make_shared<Texture2D>(color));
	return mesh;
}

MeshCPU MeshPrimitives::makeCube(float size)
{
	MeshCPU mesh;
	float s = size * 0.5f;

	mesh.vertices =
	{
		// 前 (+Z)
		{{-s, -s,  s}, {0,0,1}, {0,0}}, {{ s, -s,  s}, {0,0,1}, {1,0}},
		{{ s,  s,  s}, {0,0,1}, {1,1}}, {{-s,  s,  s}, {0,0,1}, {0,1}},
		// 后 (-Z)
		{{ s, -s, -s}, {0,0,-1}, {0,0}}, {{-s, -s, -s}, {0,0,-1}, {1,0}},
		{{-s,  s, -s}, {0,0,-1}, {1,1}}, {{ s,  s, -s}, {0,0,-1}, {0,1}},
		// 左 (-X)
		{{-s, -s, -s}, {-1,0,0}, {0,0}}, {{-s, -s,  s}, {-1,0,0}, {1,0}},
		{{-s,  s,  s}, {-1,0,0}, {1,1}}, {{-s,  s, -s}, {-1,0,0}, {0,1}},
		// 右 (+X)
		{{ s, -s,  s}, {1,0,0}, {0,0}}, {{ s, -s, -s}, {1,0,0}, {1,0}},
		{{ s,  s, -s}, {1,0,0}, {1,1}}, {{ s,  s,  s}, {1,0,0}, {0,1}},
		// 上 (+Y)
		{{-s,  s,  s}, {0,1,0}, {0,0}}, {{ s,  s,  s}, {0,1,0}, {1,0}},
		{{ s,  s, -s}, {0,1,0}, {1,1}}, {{-s,  s, -s}, {0,1,0}, {0,1}},
		// 下 (-Y)
		{{-s, -s, -s}, {0,-1,0}, {0,0}}, {{ s, -s, -s}, {0,-1,0}, {1,0}},
		{{ s, -s,  s}, {0,-1,0}, {1,1}}, {{-s, -s,  s}, {0,-1,0}, {0,1}}
	};

	mesh.indices =
	{
		 0, 1, 2,  2, 3, 0,   // 前
		 4, 5, 6,  6, 7, 4,   // 后
		 8, 9,10, 10,11, 8,   // 左
		12,13,14, 14,15,12,   // 右
		16,17,18, 18,19,16,   // 上
		20,21,22, 22,23,20    // 下
	};

	mesh.material = std::make_shared<Material>();
	mesh.material->addTexture(std::make_shared<Texture2D>(color));
	return mesh;
}
