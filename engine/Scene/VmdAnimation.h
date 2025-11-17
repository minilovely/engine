#pragma once
#include "Component.h"
#include "Actor.h"
#include "../include/Vmd.h"
#include "../Assets/Bone.h"

#include <string>
#include <memory>
#include "Transform.h"

class VmdAnimation : public Component
{
public:
	explicit VmdAnimation(Actor* owner);
	~VmdAnimation() = default;

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

	Transform* transform = nullptr;

	void BuildBoneIndexMapping();
	void ResetToDefaultPose();
	void ApplyVMDAnimation(int targetFrame);
	void ApplyBoneFrame(const vmd::VmdBoneFrame& frame, int pmxIndex);
	void CalculateGlobalPoses();
	void ApplyOffsetMatrices();
};

