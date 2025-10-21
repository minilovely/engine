#pragma once
#include "../Render/Shader.h"

#include <string>
#include <vector>
#include <memory>

//¶ÁÈ¡jsonÎÄ¼þ
class PassAssets
{
public:
	PassAssets() = default;
	void Load(const std::string& jsonPath);
	std::string ReadText(const std::string& path);
	std::shared_ptr<Shader> getShader();

	std::string getVSPath() { return vsPath; }
	std::string getFSPath() { return fsPath; }

	void setDepthWrite();
	bool getDepthWrite() const { return depthWrite; }
	void setColorWrite();
	bool getColorWrite() const { return colorWrite; }
	void setCullMode(std::string mode) { cullMode = mode; }
	std::string getCullMode() const { return cullMode; }
private:
	std::string name;
	std::string vsPath;
	std::string fsPath;
	std::shared_ptr<Shader> shader;
	bool depthWrite = true;
	bool colorWrite = true;
	std::string cullMode = "Back";

};