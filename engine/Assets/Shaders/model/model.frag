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
uniform sampler2D diffTex[2];
uniform Light lights[8];
uniform int lightCount;
// --- 新增阴影采样参数 ---
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

vec3 CalculateLight(Light light, vec3 normal, vec3 viewDir, vec3 texColor)
{
	vec3 lightDir;
	float attenuation = 1.0;

	if (light.type == 0) 
	{
	lightDir = normalize(-light.direction);// �����
	}
	else
	{
	// ���Դ
	lightDir = normalize(light.position - fs_in.wVertPos);
	float distance = length(light.position - fs_in.wVertPos);
	attenuation = 1.0 / (light.constant + light.linear * distance + 
	light.quadratic * (distance * distance));
	}
	// ������
	vec3 ambient = 0.1 * light.color;

	// ������
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * light.color;

	// ���淴��
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	vec3 specular = spec * light.color;

	return (ambient + diffuse + specular) * attenuation * texColor;
}

// --- 新增：基础阴影采样，仅硬阴影，可拓展软阴影PCF ---
float ShadowCalc(vec3 wPos)
{
	vec4 lightSpacePos = lightSpaceMatrix * vec4(wPos, 1.0);
	vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
	projCoords = projCoords * 0.5 + 0.5;
	float currentDepth = projCoords.z;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float bias = 0.005;
	float shadow = currentDepth - bias > closestDepth ? 0.4 : 1.0;
	return shadow;
}

void main()
{
	vec3 texCol = texture(diffTex[0], fs_in.vUV).rgb;
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
			float shadow = ShadowCalc(fs_in.wVertPos);
			result += CalculateLight(lights[i], wNormal, viewDir, texCol) * shadow;
		}
	}
	fragColor = vec4(result, 1.0);
}
