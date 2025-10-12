#include "PassForward.h"
#include "../Scene/Camera.h"
#include "../Scene/Mesh.h"
#include "../Scene/Transform.h"
#include "../Scene/Light.h"
#include "../Scene/Actor.h"
#include "../System/LightManager.h"
#include <iostream>

PassForward::PassForward()
{
}

PassForward::~PassForward()
{
}

void PassForward::Init()
{
    const char* vs = 
    R"(
        #version 450

        layout(location=0) in vec3 pos;
        layout(location=1) in vec3 normal;
        layout(location=2) in vec2 uv;
        
        out vertex
        {
            vec3 vNorm;
            vec2 vUV;
            vec3 wVertPos;
        }vs_out;

        uniform mat4 MVP;
        uniform mat4 M;

        void main()
        {
            vs_out.vNorm = mat3(transpose(inverse(M))) * normal;//变换法线至world space
            vs_out.vUV   = uv;
            gl_Position = MVP * vec4(pos,1.0);

            vs_out.wVertPos = mat3(M) * pos;
        }
    )";

    const char* fs = 
    R"(
        #version 450
        
        in vertex
        {
            vec3 vNorm;
            vec2 vUV;
            vec3 wVertPos;
        }fs_in;

        out vec4 fragColor;
        
        struct Light 
        {
            vec3 position;
            vec3 direction;
            vec3 color;
            int type;  // 0 = directional, 1 = point
            float constant;
            float linear;
            float quadratic;
            float range;
        };

        uniform vec3 viewPos;
        uniform sampler2D diffTex;
        uniform Light lights[8];
        uniform int lightCount;

        vec3 CalculateLight(Light light, vec3 normal, vec3 viewDir, vec3 texColor)
        {
            vec3 lightDir;
            float attenuation = 1.0;
    
            if (light.type == 0) 
            {
                lightDir = normalize(-light.direction);// 方向光
            }
            else
            {
                // 点光源
                lightDir = normalize(light.position - fs_in.wVertPos);
                float distance = length(light.position - fs_in.wVertPos);
                attenuation = 1.0 / (light.constant + light.linear * distance + 
                                    light.quadratic * (distance * distance));
            }
            // 环境光
            vec3 ambient = 0.1 * light.color;
    
            // 漫反射
            float diff = max(dot(normal, lightDir), 0.0);
            vec3 diffuse = diff * light.color;
    
            // 镜面反射
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
            vec3 specular = spec * light.color;

            return (ambient + diffuse + specular) * attenuation * texColor;
        }

        void main()
        {
            vec3 texCol = texture(diffTex, fs_in.vUV).rgb;
            vec3 wNormal = normalize(fs_in.vNorm);
            vec3 viewDir = normalize(viewPos - fs_in.wVertPos);
            vec3 result = vec3(0.0);
            for (int i = 0; i < lightCount; i++) 
            {
                float dist = length(lights[i].position - fs_in.wVertPos);
                if(dist > lights[i].range)
                {
                    
                    fragColor = vec4(0.0,0.0,0.0,1.0);
                }
                else
                {
                result += CalculateLight(lights[i], wNormal, viewDir, texCol);
                }
            }
            fragColor = vec4(result, 1.0);
        }
    )";
    shader = std::make_unique<Shader>(vs, fs);

}

void PassForward::Draw(const std::vector<Mesh*>& meshes, const Camera& camera)
{
    shader->use();
    //Camera数据上传
    glm::mat4 V = camera.getViewMatrix();
    glm::mat4 P = camera.getProjectionMatrix();
    glm::vec3 cameraPos = camera.getPosition();
    shader->setVec3("viewPos", cameraPos);
    //光源数据上传
    auto& lightManager = LightManager::Get();
    lightManager.upLoadToShader(shader.get());
    shader->setInt("lightCount", lightManager.getLightCount());
    for (Mesh* mc : meshes)
    {
        if (!mc) continue;
        Actor* owner = mc->GetOwner();
        auto* trans = owner->GetComponent<Transform>();
        if (!trans) continue;

        glm::mat4 M = trans->getModelMatrix();
        glm::mat4 MVP = P * V * M;

        shader->setMat4("MVP", MVP);
        shader->setMat4("M", M);
        shader->setInt("diffTex", 0);

        mc->Render();
        //点光源距离衰减如何计算,可通过
    }
}

void PassForward::ReloadShader(const std::string& vsSrc, const std::string& fsSrc)
{
    shader = std::make_unique<Shader>(vsSrc, fsSrc);
}
