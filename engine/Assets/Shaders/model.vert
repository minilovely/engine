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
