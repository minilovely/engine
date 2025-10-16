#include "PassAssets.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::shared_ptr<PassAssets> PassAssets::Load(const std::string& jsonpath)
{
	auto asset = std::make_shared<PassAssets>();
	std::ifstream i(jsonpath);
	if (!i)
	{
		std::cout << "Cannot find json file : " + jsonpath << std::endl;
	}
	json j;
	i >> j;// " << "操作符已被弃用

	asset->name = j.value("name", "Unamed");
	asset->vsPath = j.value("vs", "");
	asset->fsPath = j.value("fs", "");
	asset->depthWrite = j.value("depthWrite", true);
	asset->colorWrite = j.value("colorWrite", true);
	asset->cullMode = j.value("cullMode", "Back");

	return asset;
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