#pragma once
#include "../Core/math.h"

#include<glad.h>

class ShadowMap
{
public:
	ShadowMap(unsigned int = 2048);
	~ShadowMap();

	void BindForWriting();
	void BindForReading(unsigned int texUnit = 3);

	unsigned int getDepthMap() const { return depthMap; }
	unsigned getSize() const { return size; }
private:
	unsigned int depthMap = 0;
	unsigned int size;
	unsigned int fbo = 0;
};

