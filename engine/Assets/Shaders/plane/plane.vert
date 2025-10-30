#version 450
layout(location = 0) in vec3 pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;

out vec4 fragPosLightSpace;

uniform mat4 M;
uniform mat4 MVP;
uniform mat4 lightSpaceMatrix;

void main()
{
	fragPosLightSpace = lightSpaceMatrix * M * vec4(pos, 1.0);
	gl_Position = MVP * vec4(pos, 1.0);
}
