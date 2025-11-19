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
		// 核心躯干（添加更多变体）
				{"センター", {"center", "Center", "root", "Root", "全ての親", "すべての親", "センター"}},
				{"グルーブ", {"groove", "Groove", "グルーブ"}},
				{"腰", {"waist", "Waist", "腰"}},
				{"下半身", {"lower_body", "下半身", "LowerBody", "Lower Body", "下半身2"}},
				{"上半身", {"upper_body", "上半身", "UpperBody", "Upper Body", "上半身2", "上半身3"}},
				{"上半身2", {"upper_body2", "上半身2", "UpperBody2"}},
				{"首", {"neck", "Neck", "首"}},
				{"頭", {"head", "Head", "頭"}},

				// 手臂（左）
				{"左肩", {"shoulder_L", "左肩", "LeftShoulder", "肩.L", "shoulder.l", "lshoulder", "LShoulder"}},
				{"左腕", {"arm_L", "左腕", "LeftArm", "腕.L", "arm.l", "larm", "LArm"}},
				{"左ひじ", {"elbow_L", "左肘", "LeftElbow", "ひじ.L", "elbow.l", "lelbow", "LElbow"}},
				{"左手首", {"wrist_L", "左手首", "LeftWrist", "手首.L", "wrist.l", "lwrist", "LWrist"}},

				// 手臂（右）
				{"右肩", {"shoulder_R", "右肩", "RightShoulder", "肩.R", "shoulder.r", "rshoulder", "RShoulder"}},
				{"右腕", {"arm_R", "右腕", "RightArm", "腕.R", "arm.r", "rarm", "RArm"}},
				{"右ひじ", {"elbow_R", "右肘", "RightElbow", "ひじ.R", "elbow.r", "relbow", "RElbow"}},
				{"右手首", {"wrist_R", "右手首", "RightWrist", "手首.R", "wrist.r", "rwrist", "RWrist"}},

				// 腿部（左）
				{"左足", {"leg_L", "左足", "LeftLeg", "足.L", "leg.l", "lleg", "LLeg"}},
				{"左ひざ", {"knee_L", "左ひざ", "LeftKnee", "ひじ.L", "knee.l", "lknee", "LKnee"}},
				{"左足首", {"ankle_L", "左足首", "LeftAnkle", "足首.L", "ankle.l", "lankle", "LAnkle"}},

				// 腿部（右）
				{"右足", {"leg_R", "右足", "RightLeg", "足.R", "leg.r", "rleg", "RLeg"}},
				{"右ひざ", {"knee_R", "右ひざ", "RightKnee", "ひじ.R", "knee.r", "rknee", "RKnee"}},
				{"右足首", {"ankle_R", "右足首", "RightAnkle", "足首.R", "ankle.r", "rankle", "RAnkle"}},

				// IK骨骼
				{"左足ＩＫ", {"leg_IK_L", "左足IK", "LegIK_L", "足IK.L", "左足ＩＫ"}},
				{"右足ＩＫ", {"leg_IK_R", "右足IK", "LegIK_R", "足IK.R", "右足ＩＫ"}},
				{"左つま先ＩＫ", {"toe_IK_L", "左つま先IK", "ToeIK_L", "左つま先ＩＫ"}},
				{"右つま先ＩＫ", {"toe_IK_R", "右つま先IK", "ToeIK_R", "右つま先ＩＫ"}},

				// 扭转骨骼（映射到主骨骼）
				{"左腕捩", {"arm_twist_L", "左腕捩", "ArmTwist_L", "左腕捩", "左腕捩1", "左腕捩2", "左腕捩3"}},
				{"右腕捩", {"arm_twist_R", "右腕捩", "ArmTwist_R", "右腕捩", "右腕捩1", "右腕捩2", "右腕捩3"}},
				{"左手捩", {"hand_twist_L", "左手捩", "HandTwist_L", "左手捩", "左手捩1", "左手捩2", "左手捩3"}},
				{"右手捩", {"hand_twist_R", "右手捩", "HandTwist_R", "右手捩", "右手捩1", "右手捩2", "右手捩3"}},
				// 头发（添加通用映射）
				{"FaShi", {"FaShi", "FaShi_01", "FaShi_02", "FaShi_03", "FaShi_04"}},
				{"QianFa", {"QianFa", "前髪"}},
				{"HouFa", {"HouFa", "後髪"}},
				{"TouFa_L", {"TouFa_L", "左髪"}},
				{"TouFa_R", {"TouFa_R", "右髪"}},
    };

    static int FindBoneIndex(const Skeleton* skeleton, const std::string& vmdBoneName)
    {
        if (!skeleton || vmdBoneName.empty()) return -1;

		// ===== 第1优先级：精确匹配（原始名称）=====
		auto exact = skeleton->boneMap.find(vmdBoneName);
		if (exact != skeleton->boneMap.end())
		{
			std::cout << "[BoneMapper] Exact match: '" << vmdBoneName << "' -> index " << exact->second << std::endl;
			return exact->second;
		}
		// ===== 第2优先级：直接映射（VMD标准名 → PMX变体）=====
		auto mapIt = BONE_NAME_MAP.find(vmdBoneName);
		if (mapIt != BONE_NAME_MAP.end())
		{
			for (const std::string& possibleName : mapIt->second)
			{
				auto boneIt = skeleton->boneMap.find(possibleName);
				if (boneIt != skeleton->boneMap.end())
				{
					std::cout << "[BoneMapper] Direct map: '" << vmdBoneName << "' -> '" << possibleName << "' -> index " << boneIt->second << std::endl;
					return boneIt->second;
				}
			}
		}
		// ===== 第3优先级：反向映射（VMD名可能是PMX变体）=====
				// 遍历映射表，检查vmdBoneName是否在某个变体列表中
		for (const auto& [standardName, variants] : BONE_NAME_MAP) {
			for (const std::string& variant : variants) {
				// 如果VMD名匹配某个变体，则返回标准名对应的索引
				if (AreNamesEquivalent(variant, vmdBoneName)) {
					auto boneIt = skeleton->boneMap.find(standardName);
					if (boneIt != skeleton->boneMap.end()) {
						std::cout << "[BoneMapper] ✓ Reverse map: '" << vmdBoneName << "' -> standard '" << standardName << "' -> index " << boneIt->second << std::endl;
						return boneIt->second;
					}
				}
			}
		}
		// ===== 第4优先级：归一化匹配（处理全角/半角、大小写）=====
		std::string normVMD = Normalize(vmdBoneName);
		for (const auto& pair : skeleton->boneMap) {
			if (Normalize(pair.first) == normVMD) {
				std::cout << "[BoneMapper] Normalized match: '" << vmdBoneName << "' -> '" << pair.first << "' -> index " << pair.second << std::endl;
				return pair.second;
			}
		}
		// ===== 第5优先级：智能子串匹配（最后手段）=====
		for (const auto& pair : skeleton->boneMap) {
			if (IsNameCompatible(pair.first, vmdBoneName)) {
				std::cout << "[BoneMapper] Substring match: '" << vmdBoneName << "' ~ '" << pair.first << "' -> index " << pair.second << std::endl;
				return pair.second;
			}
		}
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
		// 移除分隔符
		result.erase(std::remove_if(result.begin(), result.end(),
			[](char c) { return c == ' ' || c == '_' || c == '-' || c == '.'; }), result.end());

		return result;
	}
	// 检查两个名称是否等价（归一化后比较）
	static bool AreNamesEquivalent(const std::string& a, const std::string& b)
	{
		return Normalize(a) == Normalize(b);
	}
	// 智能兼容性检查（子串+关键词匹配）
	static bool IsNameCompatible(const std::string& pmxName, const std::string& vmdName)
	{
		std::string normPMX = Normalize(pmxName);
		std::string normVMD = Normalize(vmdName);

		// 子串匹配
		if (normPMX.find(normVMD) != std::string::npos ||
			normVMD.find(normPMX) != std::string::npos) {
			return true;
		}

		// 关键词匹配（左右+部位）
		return MatchKeyComponents(normPMX, normVMD);
	}
	static bool MatchKeyComponents(const std::string& a, const std::string& b)
	{
		// 定义关键词组件映射
		const std::unordered_map<std::string, std::vector<std::string>> components = {
			{"left", {"左", "l", "left", "L"}},
			{"right", {"右", "r", "right", "R"}},
			{"shoulder", {"肩", "shoulder"}},
			{"arm", {"腕", "arm"}},
			{"elbow", {"肘", "ひじ", "elbow"}},
			{"wrist", {"手首", "wrist"}},
			{"leg", {"足", "leg"}},
			{"knee", {"ひざ", "knee"}},
			{"ankle", {"足首", "ankle"}},
			{"ik", {"ＩＫ", "IK", "ik"}}
		};

		// 检查是否共享至少一个关键词组件
		for (const auto& [key, variants] : components) {
			bool aHas = std::any_of(variants.begin(), variants.end(),
				[&](const std::string& v) { return a.find(v) != std::string::npos; });
			bool bHas = std::any_of(variants.begin(), variants.end(),
				[&](const std::string& v) { return b.find(v) != std::string::npos; });
			if (aHas && bHas) return true;
		}
		return false;
	}
	// 检查是否为物理控制骨（需要跳过）
    static bool IsPhysicsBone(const std::string& name)
    {
		if (name.empty()) return false;

		return name.find("qq_") == 0 ||
			name.find("qh_") == 0 ||
			name.find("g_") == 0 ||    // 碰撞体
			name.find("j_") == 0 ||    // 关节
			name.find("M-") == 0 ||
			name.find("補助") != std::string::npos ||
			name.find("裏") != std::string::npos ||
			name.find("取消") != std::string::npos ||    // 腰キャンセル等
			name.find("dummy") != std::string::npos ||   // ダミー
			name.find("ダミー") != std::string::npos;
	}
};
