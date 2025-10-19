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
		result += CalculateLight(lights[i], wNormal, viewDir, texCol);
		}
	}
	fragColor = vec4(result, 1.0);
}
