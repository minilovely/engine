#pragma once
#include "../Core/math.h"

#include <string>
class Shader
{
public:
	Shader(const std::string& vs_src, const std::string& fs_src);
	~Shader();
	
	void use() const;

	//基本类型按值传递，复杂只读类型选择const+引用
	void setMat4(const std::string& name, const glm::mat4& m) const;
	void setVec3(const std::string& name, const glm::vec3& v) const;
	void setInt(const std::string& name, int i) const;
	void setFloat(const std::string& name, float f) const;

	void SetLightSpaceMatrix(const glm::mat4& m) const { setMat4("lightSpaceMatrix", m); }
	void SetModel(const glm::mat4& m)            const { setMat4("M", m); }
	void SetMVP(const glm::mat4& m)              const { setMat4("MVP", m); }
	void SetLightPos(const glm::vec3& v)         const { setVec3("wlightPos", v); }
	void SetViewPos(const glm::vec3& v)          const { setVec3("wviewPos", v); }
private:
	//编译后创建的program的id
	unsigned int id = 0;
};

