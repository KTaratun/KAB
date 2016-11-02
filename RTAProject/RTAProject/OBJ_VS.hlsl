struct V_IN
{
	float3 positionIn : POSITION;
	float2 uvIn : UV;
	float3 normalIn : NORMALS;
};

struct V_OUT
{
	float2 uvOut : TEXCOORD;
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

V_OUT main( V_IN input )
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

	//output.normalOut = input.normalIn;6
	output.WorldPos = posWorld;

	return output;
}