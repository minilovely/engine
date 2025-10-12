#pragma once
#include "../Core/math.h"
class RenderDevice
{
public:
    static void SetViewport(int x, int y, int w, int h);
    static void Clear(const glm::vec3& colorRGB);
    static void SetDepthTest(bool enable);
};

