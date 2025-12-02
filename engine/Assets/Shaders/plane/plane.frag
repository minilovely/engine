#version 450

in vec4 fragPosLightSpace;

uniform vec3 color;
uniform sampler2D shadowMap;
out vec4 outColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || 
       projCoords.y < 0.0 || projCoords.y > 1.0)
        return 1.0;
    
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;
    
    // --- 返回无阴影因子（1=无阴影）---
    return (currentDepth - bias) > closestDepth ? 0.0 : 1.0;
}

void main()
{
    float shadow = ShadowCalculation(fragPosLightSpace);
    vec3 baseColor = color * shadow; // shadow=1时完全显示颜色
    outColor = vec4(baseColor, 1.0);
}
