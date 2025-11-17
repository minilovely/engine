#version 450
layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;
layout(location = 3) in ivec4 boneIndices;
layout(location = 4) in vec4  boneWeights;

out vertex
{
	vec3 vNorm;//世界法线
	vec2 vUV;
	vec3 wVertPos;//世界顶点坐标
}vs_out;

uniform mat4 MVP;
uniform mat4 M;

#define MAX_BONES 128
uniform mat4 uBoneMats[MAX_BONES];
uniform bool uHasBones;

//这里要做的事情主要目的是改变渲染策略，每个mesh不能再存整个骨架的最终变换矩阵，OpenGL存在寄存器上限
//要做的有2件事：每个mesh只存储和自己有关的骨骼矩阵，这里每个顶点对应的全局骨骼索引也需要改变为局部索引
//1.骨骼矩阵在哪里获取？->先在ImporterPMX处记录每个mesh所持有的骨骼索引，在vmdAnimation完成所有骨骼的全局变换计算后再根据变换为局部后的骨骼索引所对应的变换矩阵写入shader
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
		mat3 normalMat = mat3(transpose(inverse(M * skinMat)));
		vs_out.vNorm = normalMat * normal;
		vs_out.wVertPos = vec3(M * skinnedPos);
		finalPos = skinnedPos;
	}
	else
	{
		mat3 normalMat = mat3(transpose(inverse(M)));
		vs_out.vNorm = normalMat * normal;
		vs_out.wVertPos = vec3(M * vec4(pos, 1.0));
		finalPos = M * vec4(pos, 1.0);
	}
	vs_out.vUV   = uv;

	gl_Position = MVP * finalPos;

}
