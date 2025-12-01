#pragma once
#include "Component.h"
#include "Actor.h"
#include "../include/Vmd.h"
#include "../Assets/Bone.h"
#include "../System/AnimationSystem.h"

#include <string>
#include <memory>

class VmdAnimation : public Component
{
public:
	explicit VmdAnimation(Actor* owner);
	~VmdAnimation();

	bool Load(const std::string& vmdFilePath);
	void Update(float dt) override;

	void Play() { isPlay = true; }
	void Pause() { isPlay = false; }
	void Stop() { isPlay = false; currentTime = 0.0f; if (pose) pose->setIdentity(); }
	
	bool isPlaying() { return isPlay; }

private:
	bool isPlay = false;
	float currentTime = 0.0f;
	
	std::unique_ptr<vmd::VmdMotion> motion;
	const Skeleton* skeleton = nullptr;
	SkeletonPose* pose = nullptr;

	std::unordered_map<std::string, int> vmdToPmxMap; //记录vmd文件的骨骼名到pmx骨骼索引的映射
	std::unordered_map<std::string, vmd::VmdBoneFrame*> vmdIKFrameMap;//记录某一帧IK骨骼的临时快照
	
	// 优化数据结构：按骨骼名称分组并排序的帧数据，用于快速二分查找
	std::unordered_map<std::string, std::vector<const vmd::VmdBoneFrame*>> sortedBoneFramesMap;

	struct IKChain
	{
		std::string ikBoneName;
		int ikBoneIndex = -1;
		int endEffectorIndex = -1;
		std::vector<int> jointIndices;
		bool isValid() const { return ikBoneIndex >= 0 && endEffectorIndex >= 0 && !jointIndices.empty(); }
	};

	std::vector<IKChain> IKChains;

	void BuildBoneIndexMapping();
	void BuildIKChains();
	void BuildChainFromEndEffector(int endIndex, IKChain& chain, std::string stopBoneName);
	glm::vec3 ExtractIKTarget(const std::string& ikBoneName);
	void SolveIKChain(IKChain& chain, const glm::vec3& targetPos);
	glm::vec3 CalculateBoneWorldPosition(int jointIndex);

	void ResetToDefaultPose();
	void ApplyFKAnimation(int targetFrame);
	void ApplyBoneFrame(const vmd::VmdBoneFrame& frame, int pmxIndex);
	void CalculateGlobalPoses();
	void ApplyOffsetMatrices();
	
	// 优化函数
	void PreprocessAnimationFrames();
	const vmd::VmdBoneFrame* FindLatestKeyFrame(const std::vector<const vmd::VmdBoneFrame*>& frames, int targetFrame);
};

