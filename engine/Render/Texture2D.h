#pragma once
#include <string>
class Texture2D
{
public:
	Texture2D(const std::string& path);
	~Texture2D();
	void bind(unsigned int slot);

	unsigned int getHandle() const { return id; }
private:
	unsigned int id = 0;
};

