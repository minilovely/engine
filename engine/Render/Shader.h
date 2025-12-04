#pragma once
#include "../Core/math.h"

#include <vector>
#include <string>
class Shader
{
public:
	Shader(const std::string& vs_src, const std::string& fs_src);
	~Shader();
	
	void use();

	//基本类型按值传递，复杂只读类型选择const+引用
	void setBool(const std::string& name, bool b);
	void setMat4(const std::string& name, const glm::mat4& m);
	void setVec3(const std::string& name, const glm::vec3& v);
	void setInt(const std::string& name, int i);
	void setFloat(const std::string& name, float f);
	void setMat4Array(const std::string& name, std::vector<glm::mat4> m);
	void setIntArray(const std::string& name, std::vector<int> i);

	void SetLightSpaceMatrix(const glm::mat4& m)  { setMat4("lightSpaceMatrix", m); }
	void SetModel(const glm::mat4& m)             { setMat4("M", m); }
	void SetMVP(const glm::mat4& m)               { setMat4("MVP", m); }
	void SetLightPos(const glm::vec3& v)          { setVec3("wlightPos", v); }
	void SetViewPos(const glm::vec3& v)           { setVec3("wviewPos", v); }
private:
	//编译后创建的program的id
	unsigned int id = 0;

	bool CheckCompile(unsigned int shader);
	bool CheckLink(unsigned int prog);
};

