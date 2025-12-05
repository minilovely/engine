#include "TextureCube.h"
#include "glad.h"

#include <stb_image.h>
#include <iostream>
TextureCube::TextureCube(const std::vector<std::string>& cubeFaces)
{
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
    int w, h, c;
    for (size_t i = 0; i < 6; ++i)
    {
        unsigned char* data = stbi_load(cubeFaces[i].c_str(), &w, &h, &c, 0);
        if (!data)
        {
            std::cerr << "Cube map load fail: " << cubeFaces[i] << std::endl; continue;
        }
        GLenum fmt = (c == 3 ? GL_RGB : GL_RGBA);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

TextureCube::~TextureCube()
{
    glDeleteTextures(1, &handle);
}

void TextureCube::Bind(unsigned int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
}

