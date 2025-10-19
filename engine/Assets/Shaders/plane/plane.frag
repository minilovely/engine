#version 450
in vec4 fragPosLightSpace;

uniform vec3 color;
out vec4 outColor;

//uniform sampler2D shadowMap;
//
//float ShadowCalculation(vec4 fragPosLightSpace)
//{
//	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//	projCoords = projCoords * 0.5 + 0.5;
//
//	if (projCoords.z > 1.0) return 0.0;
//
//	float closestDepth = texture(shadowMap, projCoords.xy).r;
//	float currentDepth = projCoords.z;
//	float bias = 0.005;                          // ¿Éµ÷
//
//	return (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
//}
//
void main()
{
	//float shadow = ShadowCalculation(fragPosLightSpace);
	//vec3 color = vec3(0.7) * (1.0 - shadow);
	outColor = vec4(color, 1.0);
}