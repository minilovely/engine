#version 450

in vec4 fragPosLightSpace;

uniform vec3 color;
uniform sampler2D shadowMap;
out vec4 outColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0) return 0.0;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;
    return (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
}

void main()
{
    float shadow = ShadowCalculation(fragPosLightSpace);
    vec3 baseColor = color * (1.0 - shadow);
    outColor = vec4(baseColor, 1.0);

//        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//    projCoords = projCoords * 0.5 + 0.5;
//    float closestDepth = texture(shadowMap, projCoords.xy).r;
//        outColor = vec4(closestDepth,closestDepth,closestDepth, 1.0);

}
