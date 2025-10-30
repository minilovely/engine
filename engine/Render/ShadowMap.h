#pragma once
#include "../Core/math.h"
#include <memory>
#include <glad.h>

class ShadowMap {
public:
    ShadowMap();
    ~ShadowMap();
    void Init(int size = 2048);
    void Bind();
    void BindTexture(unsigned int unit);
    void Unbind();
    GLuint GetTexture() const;
    int GetSize() const { return shadowSize; }

private:
    GLuint fbo = 0;
    GLuint depthTexture = 0;
    int shadowSize = 2048;
};

