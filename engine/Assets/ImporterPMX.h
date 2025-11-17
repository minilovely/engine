#pragma once
#include "Bone.h"
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <memory>
#include <string>

//ImporterPMX类需要Model类作为一个中间层，中间层主要作用是对多个mesh封装
class Model;

struct TempVertex
{
	glm::vec3 pos, normal;
	glm::vec2 uv;
	std::vector<std::pair<int, float>> boneInfluences;
};

class ImporterPMX
{
public:
	static std::shared_ptr<Model> Load(const std::string& filePath);
	static void BuildBoneHierarchy(const aiNode* node, Skeleton& skeleton, int parentIndex);

private:

};

