#include "SkyBox.h"
#include "../Assets/PassAssets.h"
#include "../Assets/MeshPrimitives.h"
#include "RenderDevice.h"

#include "glad.h"

void SkyBox::Init()
{
    auto passAsset = std::make_shared<PassAssets>();
    passAsset->Load("Assets/Passes_json/skyBox.json");
    shader = passAsset->getShader();

    MeshPrimitives prim;
    MeshCPU cube = prim.makeCube(1.0f);
    auto mat = std::make_shared<Material>();
    mat->setShader(shader);
    meshGPU = std::make_unique<MeshGPU>(cube.vertices, cube.indices, mat);
    std::vector<std::string> faces =
    {
        "D:/Unity/unity project/MoveAndAnimation/Assets/Fantasy Skybox FREE/Cubemaps/FS013/Images/FS013_Night_Cubemap_left.png",
        "D:/Unity/unity project/MoveAndAnimation/Assets/Fantasy Skybox FREE/Cubemaps/FS013/Images/FS013_Night_Cubemap_right.png",
        "D:/Unity/unity project/MoveAndAnimation/Assets/Fantasy Skybox FREE/Cubemaps/FS013/Images/FS013_Night_Cubemap_up.png",
        "D:/Unity/unity project/MoveAndAnimation/Assets/Fantasy Skybox FREE/Cubemaps/FS013/Images/FS013_Night_Cubemap_down.png",
        "D:/Unity/unity project/MoveAndAnimation/Assets/Fantasy Skybox FREE/Cubemaps/FS013/Images/FS013_Night_Cubemap_front.png",
        "D:/Unity/unity project/MoveAndAnimation/Assets/Fantasy Skybox FREE/Cubemaps/FS013/Images/FS013_Night_Cubemap_back.png"
    };
    cubeMap = std::make_shared<TextureCube>(faces);
}

void SkyBox::Render(const glm::mat4& view, const glm::mat4& proj)
{
    if (!cubeMap)
    {
        std::cout << "[SkyBox] cubeMap is null" << std::endl;
        return;
    }

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    RenderDevice::SetCullEnabled(false);

    shader->use();
    shader->setMat4("MVP", proj * glm::mat4(glm::mat3(view)));
    cubeMap->Bind(0);
    shader->setInt("skybox", 0);

    meshGPU->Bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}
