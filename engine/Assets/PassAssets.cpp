#include "PassAssets.h"
#include "../Render/RenderDevice.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void PassAssets::Load(const std::string& jsonpath)
{
	std::ifstream i(jsonpath);
	if (!i)
	{
		std::cout << "Cannot find json file : " + jsonpath << std::endl;
	}
	json j;
	i >> j;// " << "操作符已被弃用

	name = j.value("name", "Unamed");
	vsPath = j.value("vs", "");
	fsPath = j.value("fs", "");
	shader = std::make_shared<Shader>(ReadText(vsPath),ReadText(fsPath));
	depthWrite = j.value("depthWrite", true);
	colorWrite = j.value("colorWrite", true);
	cullMode = j.value("cullMode", "Back");

}
std::string PassAssets::ReadText(const std::string& path)
{
	std::ifstream i(path);
	if (!i)
	{
		std::cout << "Cannot read shader file: " + path << std::endl;
	}
	std::ostringstream o;
	o << i.rdbuf();
	return o.str();
}

std::shared_ptr<Shader> PassAssets::getShader()
{
	return shader;
}

void PassAssets::setDepthWrite()
{
	RenderDevice::SetDepthWrite(depthWrite);
}

void PassAssets::setColorWrite()
{
	RenderDevice::SetColorWrite(colorWrite);
}
