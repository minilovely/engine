#version 450
layout(location = 0) in vec3 pos;
layout(location = 3) in ivec4 boneIndices;
layout(location = 4) in vec4  boneWeights;

out vertex
{
	vec3 wVertPos;//世界顶点坐标
}vs_out;

uniform mat4 lightSpaceMatrix;
uniform mat4 M;

#define MAX_BONES 128
uniform mat4 uBoneMats[MAX_BONES];
uniform bool uHasBones;

mat4 GetSkinMat()
{
    mat4 m = mat4(0.0);
    m += uBoneMats[boneIndices[0]] * boneWeights[0];
    m += uBoneMats[boneIndices[1]] * boneWeights[1];
    m += uBoneMats[boneIndices[2]] * boneWeights[2];
    m += uBoneMats[boneIndices[3]] * boneWeights[3];
	if (m == mat4(0.0)) return mat4(1.0);
    return m;
}

void main()
{
	vec4 finalPos;
	if(uHasBones)
	{
		mat4 skinMat = GetSkinMat();
		vec4 skinnedPos = skinMat * vec4(pos, 1.0);
		vs_out.wVertPos = vec3(M * skinnedPos);
		finalPos = skinnedPos;
	}
	else
	{
		vs_out.wVertPos = vec3(M * vec4(pos, 1.0));
		finalPos = vec4(pos, 1.0);
	}
	
    gl_Position = lightSpaceMatrix * M * finalPos;
}
