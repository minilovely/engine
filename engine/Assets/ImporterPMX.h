#pragma once
#include <memory>
#include <string>

class Model;
//ImporterPMX类需要Model类作为一个中间层，中间层主要作用是对多个mesh封装
class ImporterPMX
{
public:
	static std::shared_ptr<Model> Load(const std::string& filePath);
};

