struct V_IN
{
	float3 positionIn : POSITION;
	float3 uvIn : UV;
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
	float4 positionOut : SV_POSITION;
	float3 WorldPos : TEXCOORD1;
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

V_OUT main(V_IN input)
{
	V_OUT output = (V_OUT)0;
	float4 localH;
	float4 posWorld = mul(worldMatrix, float4(input.positionIn, 1));
	localH = mul(worldMatrix, float4(input.positionIn, 1));
	localH = mul(viewMatrix, localH);
	localH = mul(projectionMatrix, localH);

	output.positionOut = localH;

	output.uvOut = input.uvIn;

	output.normalOut = mul(input.normalIn, (float3x3)worldMatrix);
	output.normalOut = normalize(output.normalOut);

	//output.normalOut = input.normalIn;
	output.WorldPos = float3(posWorld.x, posWorld.y, posWorld.z);

	return output;
}