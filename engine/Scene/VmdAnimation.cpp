#include "VmdAnimation.h"
#include "Mesh.h"
#include "../include/EncodingHelper.h"
#include <queue>

VmdAnimation::VmdAnimation(Actor* owner) : Component(owner)
{
    std::string key = owner->getName();
    pose = GlobalSkeletonCache::get().getPose(key);
    skeleton = GlobalSkeletonCache::get().getSkeleton(key);
    
    transform = owner->GetComponent<Transform>();
    if (!transform)
    {
        std::cerr << "[VmdAnimation] Warning: No Transform component found!" << std::endl;
    }
}

bool VmdAnimation::Load(const std::string& vmdFilePath)
{
    motion = vmd::VmdMotion::LoadFromFile(vmdFilePath.c_str());
    if (motion && skeleton && pose && !motion->bone_frames.empty())
    {
        std::cout << "[VmdAnimation] Building bone mapping..." << std::endl;
        BuildBoneIndexMapping();
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
    ApplyVMDAnimation(targetFrame);

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

void VmdAnimation::ResetToDefaultPose()
{
    for (size_t i = 0; i < pose->localMatrices.size(); i++)
    {
        pose->localMatrices[i] = skeleton->bones[i].localTransMat;
    }
}

void VmdAnimation::ApplyVMDAnimation(int targetFrame)
{
    for (const auto& [vmdName, pmxIndex] : vmdToPmxMap)
    {
        const vmd::VmdBoneFrame* targetKeyFrame = nullptr;
        for (const auto& frame : motion->bone_frames)
        {
            if (frame.name == vmdName && frame.frame <= targetFrame)
            {
                if (!targetKeyFrame || frame.frame > targetKeyFrame->frame)
                {
                    targetKeyFrame = &frame;
                }
            }
        }
        // 应用找到的关键帧
        if (targetKeyFrame)
        {
            ApplyBoneFrame(*targetKeyFrame, pmxIndex);
        }
    }
}

void VmdAnimation::ApplyBoneFrame(const vmd::VmdBoneFrame& frame, int pmxIndex)
{
    if (pmxIndex < 0 || pmxIndex >= static_cast<int>(pose->localMatrices.size()))
    {
        return;
    }
    glm::vec3 pos(frame.position[0], frame.position[1], -frame.position[2]);
    glm::quat rot(frame.orientation[3], frame.orientation[0],frame.orientation[1], -frame.orientation[2]);

    glm::mat4 T = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 R = glm::mat4_cast(rot);

	//glm::vec3 zeroPos(0.0f);
	//pose->localMatrices[1] = R * glm::translate(glm::mat4(1.0f), zeroPos);
    pose->localMatrices[pmxIndex] = T * R * skeleton->bones[pmxIndex].localTransMat;
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


//TODO:1.有vmd读取乱码问题 A:这是vmd文件的问题，部分vmd是没有转码后还有乱码的问题
//     2.顶点位置扭曲问题，可能是现在骨骼没有匹配成功的问题
//     3.pmx和vmd骨骼匹配存在问题，正在修复解决
//     4.vmd和pmx好像存在坐标系差异，而且感觉不仅仅是坐标系的问题,PMX采用左手坐标系，vmd是左手坐标系
//     5.代码框架好像有问题，怎么我旋转模型，我的平行光方向就变了----这是shader写错了
//     6.这个版本的vmd组件只vmd存在的骨骼动画，如果PMX包含没有动画数据的骨骼，这些骨骼就不会动，解释为什么头发之类的部分和躯干是分开的

