struct V_IN
{
	float3 positionIn : POSITION;
	float3 uvIn : UVW;
	float3 normalIn : NORMALS;
	float3 binormalIn : BINORMALS;
	float3 tangentIn : TANGENTS;
	int4 boneIn : BONE;
	float4 weightIn : WEIGHT;
};

struct V_OUT
{
	float3 uvOut : TEXCOORD;
	float3 normalOut : NORMALS;
	float3 binormalOut : BINORMALS;
	float3 tangentOut : TANGENT;
	float4 positionOut : SV_POSITION;
	float3 WorldPos : TEXCOORD1;
	//float3 cameraPos : CAMERAPOS;
};

cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
};

cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

cbuffer BOFFSET : register(b2)
{
	float4x4 BoneOffset[28];
}

V_OUT main(V_IN input)
{
	V_OUT output = (V_OUT)0;
	float4 localH;
	float4 posWorld = mul(worldMatrix, float4(input.positionIn, 1));

	//float4 tempVert;

	localH = mul(BoneOffset[input.boneIn.x], float4(input.positionIn.x, input.positionIn.y, input.positionIn.z, 1)*input.weightIn.x);
	localH += mul(BoneOffset[input.boneIn.y], float4(input.positionIn.x, input.positionIn.y, input.positionIn.z, 1)*input.weightIn.y);
	localH += mul(BoneOffset[input.boneIn.z], float4(input.positionIn.x, input.positionIn.y, input.positionIn.z, 1)*input.weightIn.z);
	localH += mul(BoneOffset[input.boneIn.w], float4(input.positionIn.x, input.positionIn.y, input.positionIn.z, 1)*input.weightIn.w);

	//localH = mul(tempVert,localH);
	localH = mul(worldMatrix, localH);

	localH = mul(viewMatrix, localH);
	localH = mul(projectionMatrix, localH);
	output.positionOut = localH;

	float4 tempBi;
	tempBi = mul(BoneOffset[input.boneIn.x], float4(input.binormalIn.x, input.binormalIn.y, input.binormalIn.z, 0)*input.weightIn.x);
	tempBi += mul(BoneOffset[input.boneIn.y], float4(input.binormalIn.x, input.binormalIn.y, input.binormalIn.z, 0)*input.weightIn.y);
	tempBi += mul(BoneOffset[input.boneIn.z], float4(input.binormalIn.x, input.binormalIn.y, input.binormalIn.z, 0)*input.weightIn.z);
	tempBi += mul(BoneOffset[input.boneIn.w], float4(input.binormalIn.x, input.binormalIn.y, input.binormalIn.z, 0)*input.weightIn.w);
	output.binormalOut = (float3)tempBi;

	float4 tempNorm;
	tempNorm = mul(BoneOffset[input.boneIn.x], float4(input.normalIn.x, input.normalIn.y, input.normalIn.z, 0)*input.weightIn.x);
	tempNorm += mul(BoneOffset[input.boneIn.y], float4(input.normalIn.x, input.normalIn.y, input.normalIn.z, 0)*input.weightIn.y);
	tempNorm += mul(BoneOffset[input.boneIn.z], float4(input.normalIn.x, input.normalIn.y, input.normalIn.z, 0)*input.weightIn.z);
	tempNorm += mul(BoneOffset[input.boneIn.w], float4(input.normalIn.x, input.normalIn.y, input.normalIn.z, 0)*input.weightIn.w);
	output.normalOut = (float3)tempNorm;

	output.tangentOut = cross(tempNorm, tempBi);

	output.uvOut = input.uvIn;

	output.normalOut = mul(input.normalIn, (float3x3)worldMatrix);
	output.normalOut = normalize(output.normalOut);
	output.binormalOut = normalize(output.binormalOut);
	output.tangentOut = normalize(output.tangentOut);


	//	tempVert = XMVector4Transform(vertPos, blender.GetSkinningMatrix(vertOut[i].bone.x) * meshes[0].verts[i].weights.x);
	//	tempVert += XMVector4Transform(vertPos, blender.GetSkinningMatrix(vertOut[i].bone.y) * meshes[0].verts[i].weights.y);
	//	tempVert += XMVector4Transform(vertPos, blender.GetSkinningMatrix(vertOut[i].bone.z) * meshes[0].verts[i].weights.z);
	//	tempVert += XMVector4Transform(vertPos, blender.GetSkinningMatrix(vertOut[i].bone.w) * meshes[0].verts[i].weights.w);


	//output.normalOut = input.normalIn;
	output.WorldPos = float3(posWorld.x, posWorld.y, posWorld.z);

	//float4 tempVert;


	//output.positionOut = tempVert;

	return output;
}