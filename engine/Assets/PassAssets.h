#pragma once
#include <string>
#include <vector>
#include <memory>

//¶ÁÈ¡jsonÎÄ¼þ
class PassAssets
{
public:
	PassAssets() = default;
	static std::shared_ptr<PassAssets> Load(const std::string& jsonPath);
	static std::string ReadText(const std::string& path);

	std::string getVSPath() { return vsPath; }
	std::string getFSPath() { return fsPath; }

private:

	std::string name;
	std::string vsPath;
	std::string fsPath;
	bool depthWrite = true;
	bool colorWrite = true;
	std::string cullMode = "Back";
};