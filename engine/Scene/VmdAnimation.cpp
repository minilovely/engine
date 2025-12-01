#include "VmdAnimation.h"
#include "Mesh.h"
#include "../include/EncodingHelper.h"
#include <queue>
#include <functional>

VmdAnimation::VmdAnimation(Actor* owner) : Component(owner)
{
    std::string key = owner->getName();
    pose = GlobalSkeletonCache::get().getPose(key);
    skeleton = GlobalSkeletonCache::get().getSkeleton(key);
	pose->initTPoseMatrices(skeleton);
    AnimationSystem::Get().Register(this);
}

VmdAnimation::~VmdAnimation()
{
    AnimationSystem::Get().UnRegister(this);
}

bool VmdAnimation::Load(const std::string& vmdFilePath)
{
    motion = vmd::VmdMotion::LoadFromFile(vmdFilePath.c_str());
    if (motion && skeleton && pose && !motion->bone_frames.empty())
    {
        std::cout << "[VmdAnimation] Building bone mapping..." << std::endl;
        BuildBoneIndexMapping();
		BuildIKChains();
        
        // 预处理：按骨骼名称对帧进行分组并排序，优化后续查找性能
        std::cout << "[VmdAnimation] Preprocessing animation frames..." << std::endl;
        PreprocessAnimationFrames();
    }
    return motion != nullptr;
}

void VmdAnimation::Update(float dt)
{
    //基础状态检查
    if (!isPlay || !pose || motion->bone_frames.empty())
        return;

    //更新时间（VMD标准30fps）
    currentTime += dt * 30.0f;
    int targetFrame = static_cast<int>(currentTime);

    //循环动画处理
    if (targetFrame > motion->bone_frames.back().frame)
    {
        currentTime = 0.0f;
        targetFrame = 0;
    }
    
    ResetToDefaultPose();
    ApplyFKAnimation(targetFrame);
	CalculateGlobalPoses();

	for (auto& chain : IKChains)
	{
		glm::vec3 targetPos = ExtractIKTarget(chain.ikBoneName);
		//std::cout << "[IK] Frame " << targetFrame << " " << chain.ikBoneName
		//	<< " Target=" << glm::to_string(targetPos) << std::endl;
		SolveIKChain(chain, targetPos);
	}

    CalculateGlobalPoses();
    ApplyOffsetMatrices();
}
void VmdAnimation::BuildBoneIndexMapping()
{
    if (!skeleton || !pose || motion->bone_frames.empty())
    {
        std::cerr << "[VmdAnimation] Invalid state for mapping!" << std::endl;
        return;
    }
    std::unordered_set<std::string> uniqueVmdBoneNames;
    for (const auto& frame : motion->bone_frames)
    {
        uniqueVmdBoneNames.insert(frame.name);
    }
    int successCount = 0;
    int failCount = 0;
    std::vector<std::string> unmatchedBones;
    for (const std::string& vmdBoneName : uniqueVmdBoneNames)
    {
		//std::cout << "[VmdAnimation] Mapping VMD bone: " << vmdBoneName << std::endl;
        if (BoneMapper::IsPhysicsBone(vmdBoneName))
        {
            std::cout << "[VmdAnimation] Skipping physics bone: " << vmdBoneName << std::endl;
            continue;
        }
        int pmxIndex = BoneMapper::FindBoneIndex(skeleton, vmdBoneName);

        if (pmxIndex >= 0)
        {
            vmdToPmxMap[vmdBoneName] = pmxIndex;
            successCount++;
        }
        else
        {
            failCount++;
            unmatchedBones.push_back(vmdBoneName);
        }
    }
    std::cout << "[VmdAnimation] Bone mapping complete: "
        << successCount << " succeeded, " << failCount << " failed" << std::endl;
}

void VmdAnimation::BuildIKChains()
{
	const std::vector<std::tuple<std::string,std::string,std::string>> ikMappings =
	{
		{"左足ＩＫ",      "左足首",		"左足"},
		{"右足ＩＫ",      "右足首",		"右足"},
		{"左つま先ＩＫ",  "左つま先",	"左足首"},
		{"右つま先ＩＫ",  "右つま先",	"右足首"}
	};
	for (const auto& [ikBoneName, endEffectorName, stopBoneName] : ikMappings)
	{
		auto it = vmdToPmxMap.find(ikBoneName);
		if (it == vmdToPmxMap.end())
		{
			std::cout << "[VmdAnimation]VMD IK bone not found in mapping : " << ikBoneName << std::endl;
			continue;
		}
		IKChain chain;
		chain.ikBoneName = ikBoneName;
		chain.ikBoneIndex = it->second;
		auto endIt = skeleton->boneMap.find(endEffectorName);
		if (endIt != skeleton->boneMap.end())
		{
			chain.endEffectorIndex = endIt->second;	
		}
		else
		{
			std::cout << "[VmdAnimation] IK end effector bone not found in PMX: " << endEffectorName << std::endl;
		}
		BuildChainFromEndEffector(chain.endEffectorIndex, chain,stopBoneName);
		if (chain.isValid())
		{
			IKChains.push_back(chain);
			std::cout << "[IK] Built chain: " << ikBoneName << ": ";
			for (int idx : chain.jointIndices)
				std::cout << skeleton->bones[idx].name << " ";
			std::cout << " " << std::endl;
		}
	}
}

void VmdAnimation::BuildChainFromEndEffector(int endIndex, IKChain& chain, std::string stopBoneName)
{
	chain.jointIndices.clear();
	int current = endIndex;
	while (current != -1)
	{
		chain.jointIndices.insert(chain.jointIndices.begin(), current);
		if (skeleton->bones[current].name == stopBoneName)
		{
			break;
		}
		int parent = skeleton->bones[current].parent;
		current = parent;
	}
}
//处理vmd文件的IK数据
glm::vec3 VmdAnimation::ExtractIKTarget(const std::string& ikBoneName)
{
	auto it = vmdIKFrameMap.find(ikBoneName);
	if (it != vmdIKFrameMap.end())
	{
		vmd::VmdBoneFrame* ikFrame = it->second;
		glm::vec3 localPos = glm::vec3(ikFrame->position[0], ikFrame->position[1], ikFrame->position[2]);
		glm::quat localRot = glm::quat(ikFrame->orientation[3], ikFrame->orientation[0], ikFrame->orientation[1], ikFrame->orientation[2]);

		int ikBoneIndex = vmdToPmxMap[ikBoneName];
		int parentIndex = skeleton->bones[ikBoneIndex].parent;
		if (parentIndex != -1)
		{
			glm::vec3 parentPos = glm::vec3(pose->globalMatrices[parentIndex][3]);
			glm::mat3 parentRot = glm::mat3(pose->globalMatrices[parentIndex]);
			return parentPos + parentRot * localPos;
		}
		else
		{
			return localPos;
		}
	}
	std::cout << "[IK] Warning: No IK frame found for " << ikBoneName << std::endl;
	return glm::vec3(0.0f);
}
//这一部分未完成,使用CCD算法
void VmdAnimation::SolveIKChain(IKChain& chain, const glm::vec3& targetPos)
{
	const int iterations = 15;
	const float tolerance = 0.05f;
	int endEffectorIndex = chain.endEffectorIndex;

	for (int i = 0; i < iterations; i++)
	{
		CalculateGlobalPoses();
		glm::vec3 endPos = CalculateBoneWorldPosition(endEffectorIndex);
		if (glm::distance(endPos, targetPos) < tolerance)
			break;
		for (int j = chain.jointIndices.size() - 2; j >= 0; j--)
		{
			int jointIndex = chain.jointIndices[j];
			glm::vec3 jointPos = CalculateBoneWorldPosition(jointIndex);
			glm::vec3 currentEndPos = CalculateBoneWorldPosition(endEffectorIndex);

			glm::vec3 toEnd = glm::normalize(currentEndPos - jointPos);
			glm::vec3 toTarget = glm::normalize(targetPos - jointPos);

			float cosAngle = glm::dot(toEnd, toTarget);

			if (cosAngle > 0.9999f)
				continue;
			float angle = glm::acos(glm::clamp(cosAngle, -1.0f, 1.0f));
			glm::vec3 axis = glm::normalize(glm::cross(toEnd, toTarget));
			glm::quat rotChange = glm::angleAxis(angle, axis);
			glm::quat currentRot = glm::quat_cast(glm::mat3(pose->localMatrices[jointIndex]));
			glm::quat newRot = rotChange * currentRot;

			glm::vec3 pos = glm::vec3(pose->localMatrices[jointIndex][3]);
			glm::vec3 scale = glm::vec3(
				glm::length(glm::vec3(pose->localMatrices[jointIndex][0])),
				glm::length(glm::vec3(pose->localMatrices[jointIndex][1])),
				glm::length(glm::vec3(pose->localMatrices[jointIndex][2]))
			);
			pose->localMatrices[jointIndex] =
				glm::translate(glm::mat4(1.0f), pos) *
				glm::mat4_cast(newRot) *
				glm::scale(glm::mat4(1.0f), scale);
		}
	}
}

glm::vec3 VmdAnimation::CalculateBoneWorldPosition(int jointIndex)
{
	std::vector<int> path;
	int current = jointIndex;
	glm::mat4 globalMat(1.0f);

	while(current != -1)
	{
		path.push_back(current);
		current = skeleton->bones[current].parent;
	}
	for (auto it = path.rbegin(); it < path.rend(); it++)
	{
		globalMat *= pose->localMatrices[*it];
	}
	return glm::vec3(globalMat[3]);
}

void VmdAnimation::ResetToDefaultPose()
{
    for (size_t i = 0; i < pose->localMatrices.size(); i++)
    {
        pose->localMatrices[i] = skeleton->bones[i].localTransMat;
    }
}
// 优化后的帧查找和应用函数
void VmdAnimation::ApplyFKAnimation(int targetFrame)
{
	vmdIKFrameMap.clear();
    
    // 利用预处理的排序帧数据进行快速查找
    for (auto& [vmdName, pmxIndex] : vmdToPmxMap)
    {
        // 在预处理的映射中查找该骨骼的帧数据
        auto it = sortedBoneFramesMap.find(vmdName);
        if (it == sortedBoneFramesMap.end() || it->second.empty())
            continue;
        
        // 使用二分查找找到最近的关键帧
        const vmd::VmdBoneFrame* targetKeyFrame = FindLatestKeyFrame(it->second, targetFrame);
        if (!targetKeyFrame) continue;
        
        // 分离IK骨骼和普通骨骼
        if (vmdName.find("ＩＫ") != std::string::npos)
        {
            vmdIKFrameMap[vmdName] = const_cast<vmd::VmdBoneFrame*>(targetKeyFrame);
            continue;
        }
        
        ApplyBoneFrame(*targetKeyFrame, pmxIndex);
    }
}

// 预处理动画帧，按骨骼名称分组并按帧号排序
void VmdAnimation::PreprocessAnimationFrames()
{
    // 按骨骼名称分组
    for (const auto& frame : motion->bone_frames)
    {
        sortedBoneFramesMap[frame.name].push_back(&frame);
    }
    
    // 对每组帧按帧号排序，以便后续使用二分查找
    for (auto& [boneName, frames] : sortedBoneFramesMap)
    {
        std::sort(frames.begin(), frames.end(), 
            [](const vmd::VmdBoneFrame* a, const vmd::VmdBoneFrame* b) {
                return a->frame < b->frame;
            });
    }
    
    std::cout << "[VmdAnimation] Animation frames preprocessed. Grouped " 
              << sortedBoneFramesMap.size() << " bone animations." << std::endl;
}

// 使用二分查找找到不大于目标帧号的最新关键帧
const vmd::VmdBoneFrame* VmdAnimation::FindLatestKeyFrame(const std::vector<const vmd::VmdBoneFrame*>& frames, int targetFrame)
{
    if (frames.empty())
        return nullptr;
        
    // 特殊处理：当目标帧为0时，返回第一个关键帧
    // 这样可以确保所有骨骼在初始帧都应用动画数据，避免混合姿态
    if (targetFrame == 0)
    {
        return frames[0];
    }
    
    // 如果目标帧小于第一个关键帧，则没有合适的帧
    if (targetFrame < frames[0]->frame)
        return nullptr;
    
    // 二分查找
    int left = 0;
    int right = static_cast<int>(frames.size()) - 1;
    int result = left;
    
    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        
        if (frames[mid]->frame == targetFrame)
            return frames[mid];
        else if (frames[mid]->frame < targetFrame)
        {
            result = mid;
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }
    
    // 返回找到的最大的不大于目标帧的帧
    return frames[result];
}

void VmdAnimation::ApplyBoneFrame(const vmd::VmdBoneFrame& frame, int pmxIndex)
{
    if (pmxIndex < 0 || pmxIndex >= static_cast<int>(pose->localMatrices.size()))
    {
        return;
    }
    glm::vec3 pos(frame.position[0], frame.position[1], frame.position[2]);
    glm::quat rot(frame.orientation[3], frame.orientation[0],frame.orientation[1], frame.orientation[2]);

    glm::mat4 T = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 R = glm::mat4_cast(rot);

	glm::mat4 initialTransform = skeleton->bones[pmxIndex].localTransMat;

	glm::vec3 initialPos = glm::vec3(initialTransform[3]);
	glm::mat3 initialRotMat = glm::mat3(initialTransform);
	glm::quat initialRot = glm::quat_cast(initialRotMat);

	glm::vec3 initialScale = glm::vec3(
		glm::length(glm::vec3(initialTransform[0])),
		glm::length(glm::vec3(initialTransform[1])),
		glm::length(glm::vec3(initialTransform[2]))
	);

	glm::vec3 finalPos = initialPos + pos;
	glm::quat finalRot = rot * initialRot;

    pose->localMatrices[pmxIndex] = glm::translate(glm::mat4(1.0f), finalPos) *
		glm::mat4_cast(finalRot) * glm::scale(glm::mat4(1.0f), initialScale);
}

void VmdAnimation::CalculateGlobalPoses()
{
    const Skeleton& sk = *skeleton;
    for (size_t i = 0; i < pose->localMatrices.size(); ++i)
    {
        if (sk.bones[i].parent == -1)
        {
            pose->globalMatrices[i] = pose->localMatrices[i];
        }
        else
        {
            int p = sk.bones[i].parent;
            pose->globalMatrices[i] = pose->globalMatrices[p] * pose->localMatrices[i];
        }
    }
}

void VmdAnimation::ApplyOffsetMatrices()
{
    const Skeleton& sk = *skeleton;
    for (size_t i = 0; i < pose->globalMatrices.size(); ++i)
    {
        pose->finalMatrices[i] = pose->globalMatrices[i] * sk.bones[i].offsetMatrix;
    }
}


//TODO:1.有vmd读取乱码问题
//			A:这是vmd文件的问题，部分vmd是没有转码后还有乱码的问题（管不了，有些vmd文件的骨骼命名可能不是utf-8编码的）
//     2.顶点位置扭曲问题，可能是现在骨骼没有匹配成功的问题（√ 是矩阵运算步骤有问题）
//     3.pmx和vmd骨骼匹配存在问题，正在修复解决
//     4.vmd和pmx好像存在坐标系差异，而且感觉不仅仅是坐标系的问题,PMX采用左手坐标系，vmd是左手坐标系(√）
//     5.代码框架好像有问题，怎么我旋转模型，我的平行光方向就变了----这是shader写错了（√）
//     6.这个版本的vmd组件只vmd存在的骨骼动画，如果PMX包含没有动画数据的骨骼，
//		 这些骨骼就不会动，解释为什么头发之类的部分和躯干是分开的
//     7.骨骼映射表有些问题，其中一个问题是骨骼表单向映射的问题，应该能解释目前出现的所有问题
//	   8.腿部的动需要进行IK解算才能有动画，现在还没有，正在解决
//	   9.IK运算思路：
//			pmx获取vmd关于IK骨骼的数据，经过计算后拿到该IK的世界位置，把该坐标交给CCD算法进行IK解算，拿到IK骨骼全局变换矩阵，结束
//     10.腿部的IK分成两大块：一个是腿部的IK，左右各一个，另外的是脚部IK,左右各一个，现在IK链问题解决了
//	   11.vmd文件读取这里需要把IK的读取分开来，好像有污染
