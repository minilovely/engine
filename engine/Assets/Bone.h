#pragma once
#include "../Core/math.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <algorithm>

struct Bone
{
	std::string name;
	glm::mat4 offsetMatrix{ 1.0f };
    glm::mat4 localTransMat{ 1.0f };
	int parent = -1;
};

class Skeleton
{
public:
	std::vector<Bone> bones;

	//这个表是为了创建索引表，记录单个模型的所有出现了的骨骼名，并标上索引
	std::unordered_map<std::string, int> boneMap; 

	int addIfNew(const std::string& boneName,const glm::mat4& offset)
	{
		auto it = boneMap.find(boneName);
		if (it != boneMap.end())
		{
			return it->second;
		}
		else
		{
			int index = static_cast<int>(bones.size());
			bones.push_back({ boneName, offset, -1 });
			boneMap[boneName] = index;
			return index;
		}
	}
};
// SkeletonPose存储每根骨骼的变换矩阵
class SkeletonPose
{
public:
	std::vector<glm::mat4> localMatrices;  // 局部动画矩阵
	std::vector<glm::mat4> globalMatrices; // 全局变换矩阵
	std::vector<glm::mat4> finalMatrices;  // 最终蒙皮矩阵 (global * offset)

	void resize(size_t count)
	{
		//assign函数会替换vector内容并初始化
		localMatrices.assign(count, glm::mat4(1.0f));
		globalMatrices.assign(count, glm::mat4(1.0f));
		finalMatrices.assign(count, glm::mat4(1.0f));
	}
	void setIdentity()
	{
		std::fill(localMatrices.begin(), localMatrices.end(), glm::mat4(1.0f));
		std::fill(globalMatrices.begin(), globalMatrices.end(), glm::mat4(1.0f));
	}
};
//全局单例骨骼缓存，存储静态骨骼和动态姿势
class GlobalSkeletonCache
{
public:
	static GlobalSkeletonCache& get()
	{
		static GlobalSkeletonCache inst;
		return inst;
	}

	const Skeleton* getSkeleton(const std::string& key) const
	{
		auto it = skeletons.find(key);
		return (it == skeletons.end()) ? nullptr : &it->second;
	}

	SkeletonPose* getPose(const std::string& key)  // key = 任意字符串，比如模型路径
	{
		auto it = poses.find(key);
		return (it == poses.end()) ? nullptr : &it->second;
	}

	SkeletonPose& createPose(const std::string& key, std::size_t boneCount)
	{
		poses[key].resize(boneCount);
		return poses[key];
	}

	void registerSkeleton(const std::string& key, const Skeleton& sk)
	{
		skeletons[key] = sk;
	}
private:
	std::unordered_map<std::string, SkeletonPose> poses;//动态姿势，存储场景的每个model实例的骨骼姿势
	std::unordered_map<std::string, Skeleton> skeletons;//静态骨骼,由assimp导入
};

class BoneMapper
{
public:
	// 基础骨骼名称映射表（VMD标准名称 -> PMX可能的变体）
    inline static const std::unordered_map<std::string, std::vector<std::string>> BONE_NAME_MAP =
    {
        // 核心躯干
        {"センター", {"center", "Center", "root", "Root", "全ての親", "すべての親"}},
        {"グルーブ", {"groove", "Groove"}},
        {"腰", {"waist", "Waist"}},
        {"下半身", {"lower_body", "下半身", "LowerBody", "Lower Body", "下半身2"}},
        {"上半身", {"upper_body", "上半身", "UpperBody", "Upper Body", "上半身2", "上半身3"}},
        {"上半身2", {"upper_body2", "上半身2", "UpperBody2"}},
        {"首", {"neck", "Neck"}},
        {"頭", {"head", "Head"}},

        // 手臂
        {"左肩", {"shoulder_L", "左肩", "LeftShoulder", "肩.L", "shoulder.l"}},
        {"左腕", {"arm_L", "左腕", "LeftArm", "腕.L", "arm.l"}},
        {"左ひじ", {"elbow_L", "左肘", "LeftElbow", "ひじ.L", "elbow.l"}},
        {"左手首", {"wrist_L", "左手首", "LeftWrist", "手首.L", "wrist.l"}},
        {"右肩", {"shoulder_R", "右肩", "RightShoulder", "肩.R", "shoulder.r"}},
        {"右腕", {"arm_R", "右腕", "RightArm", "腕.R", "arm.r"}},
        {"右ひじ", {"elbow_R", "右肘", "RightElbow", "ひじ.R", "elbow.r"}},
        {"右手首", {"wrist_R", "右手首", "RightWrist", "手首.R", "wrist.r"}},

        // 手指（简化版）
        {"左親指０", {"thumb0_L", "左親指0", "Thumb0_L"}},
        {"左親指１", {"thumb1_L", "左親指1", "Thumb1_L"}},
        // ... 其他手指

        // 腿部
        {"左足", {"leg_L", "左足", "LeftLeg", "足.L", "leg.l"}},
        {"左ひざ", {"knee_L", "左ひざ", "LeftKnee", "ひざ.L", "knee.l"}},
        {"左足首", {"ankle_L", "左足首", "LeftAnkle", "足首.L", "ankle.l"}},
        {"右足", {"leg_R", "右足", "RightLeg", "足.R", "leg.r"}},
        {"右ひざ", {"knee_R", "右ひざ", "RightKnee", "ひざ.R", "knee.r"}},
        {"右足首", {"ankle_R", "右足首", "RightAnkle", "足首.R", "ankle.r"}},

        // IK 骨
        {"左足ＩＫ", {"leg_IK_L", "左足IK", "LegIK_L", "足IK.L"}},
        {"右足ＩＫ", {"leg_IK_R", "右足IK", "LegIK_R", "足IK.R"}},
        {"左つま先ＩＫ", {"toe_IK_L", "左つま先IK", "ToeIK_L"}},
        {"右つま先ＩＫ", {"toe_IK_R", "右つま先IK", "ToeIK_R"}},

        // 扭转骨
        {"左腕捩", {"arm_twist_L", "左腕捩", "ArmTwist_L"}},
        {"右腕捩", {"arm_twist_R", "右腕捩", "ArmTwist_R"}},
        {"左手捩", {"hand_twist_L", "左手捩", "HandTwist_L"}},
        {"右手捩", {"hand_twist_R", "右手捩", "HandTwist_R"}},

    };

	// 主查找函数（模仿 Blender MMD Tools）
    static int FindBoneIndex(const Skeleton* skeleton, const std::string& vmdBoneName)
    {
        if (!skeleton || vmdBoneName.empty()) return -1;

        // ===== 第1优先级：精确匹配（原始名称）=====
        auto exact = skeleton->boneMap.find(vmdBoneName);
        if (exact != skeleton->boneMap.end()) return exact->second;

        // ===== 第2优先级：映射表匹配（最可靠）=====
        auto mapIt = BONE_NAME_MAP.find(vmdBoneName);
        if (mapIt != BONE_NAME_MAP.end()) {
            for (const std::string& possibleName : mapIt->second) {
                auto boneIt = skeleton->boneMap.find(possibleName);
                if (boneIt != skeleton->boneMap.end()) {
                    return boneIt->second;
                }
            }
        }

        // ===== 第3优先级：归一化匹配（后备方案）=====
        std::string normVMD = Normalize(vmdBoneName);
        for (const auto& pair : skeleton->boneMap) {
            if (Normalize(pair.first) == normVMD) {
                return pair.second;
            }
        }

        //// ===== 第4优先级：子串匹配（最后手段）=====
        //for (const auto& pair : skeleton->boneMap) {
        //    const std::string& pmxName = pair.first;
        //    // VMD 名包含 PMX 名 或反之
        //    if (pmxName.find(vmdBoneName) != std::string::npos ||
        //        vmdBoneName.find(pmxName) != std::string::npos) {
        //        return pair.second;
        //    }
        //}

        // 完全失败
        std::cerr << "[BoneMapper] Failed to match VMD bone: '" << vmdBoneName << "'" << std::endl;
        return -1;

    }

	// 简化版归一化（仅移除变量后缀）
    static std::string Normalize(const std::string& name)
    {
        std::string result = name;
        // 转为小写
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::tolower(c); });
        // 移除空格、下划线、连字符、点
        result.erase(std::remove_if(result.begin(), result.end(),
            [](char c) { return c == ' ' || c == '_' || c == '-' || c == '.'; }), result.end());
        return result;

    }

	// 检查是否为物理控制骨（需要跳过）
    static bool IsPhysicsBone(const std::string& name)
    {
        // 更精确的物理骨判断
        return name.find("qq_") == 0 ||
            name.find("qh_") == 0 ||
            name.find("g_") == 0 ||  // 碰撞体
            name.find("j_") == 0 ||  // 关节
            name.find("M-") == 0 ||
            name.find("補助") != std::string::npos ||
            name.find("裏") != std::string::npos;
    }
};
