#pragma once

#include <vector>
#include <string>

class TextureCube
{
public:
	TextureCube(const std::vector<std::string>& cubeFaces);
	~TextureCube();

	void Bind(unsigned int unit = 0);
	unsigned int GetHandle() const { return handle; }
private:
	unsigned int handle = 0;
};

