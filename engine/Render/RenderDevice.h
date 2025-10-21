#pragma once
#include "../Core/math.h"

#include <string>
class RenderDevice
{
public:
    static void SetViewport(int x, int y, int w, int h);
    static void Clear(const glm::vec3& colorRGB);
    static void SetDepthTest(bool enable);
    static void SetDepthWrite(bool enable);
    static void SetColorWrite(bool enable);
    static void SetCullMode(std::string mode);
    static void SetCullEnabled(bool on);
};

