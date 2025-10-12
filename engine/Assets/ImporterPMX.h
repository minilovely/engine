#pragma once
#include <memory>
#include <string>

class Model;

class ImporterPMX
{
public:
	static std::shared_ptr<Model> Load(const std::string& filePath);
};

