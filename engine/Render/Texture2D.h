#pragma once
#include <string>
#include "../Core/math.h"
class Texture2D
{
public:
	Texture2D(glm::vec3 col);
	Texture2D(const std::string& path);
	~Texture2D();

	void bind(unsigned int slot);

	unsigned int getHandle() const { return id; }
	glm::vec3 getColor() const { return color; }
private:
	unsigned int id = 0;
	unsigned char* data;
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
};

