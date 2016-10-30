texture2D baseTexture : register(t0);

SamplerState filters[2] : register(s0);

struct P_IN
{
	float2 uv : TEXCOORD;
	float3 normal : NORMALS;
	float4 position : SV_POSITION;
	float3 WorldPos : TEXCOORD1;
};

//cbuffer LightBuffer1 : register(b0)
//{
//	float4 ambientDiffuse;
//
//	float4 directionDiffuse;
//	float3 lightDirection;
//	float pointRadius;
//
//	float4 pointDiffuse;
//	float4 pointPosition;
//
//	float4 spotDiffuse;
//	float4 spotPosition;
//	float4 spotDirection;
//	float spotRatio;
//	float3 padding;
//};

//cbuffer LightBuffer2 : register(b1)
//{
//	float4 ambientDiffuse2;
//
//	float4 directionDiffuse2;
//	float3 lightDirection2;
//	float pointRadius2;
//
//
//	float4 pointDiffuse2;
//	float4 pointPosition2;
//
//	float4 spotDiffuse2;
//	float4 spotPosition2;
//	float4 spotDirection2;
//	float spotRatio2;
//	float3 padding2;
//};

float4 main(P_IN input) : SV_TARGET
{

	float4 textureColor = baseTexture.Sample(filters[0], input.uv);

	//Ambient Light
	//float4 ambientResult = ambientDiffuse2*textureColor;

	////Directional Light
	//float directionalRatio = saturate(dot(-lightDirection, input.normal));
	//float4 directionResult = directionalRatio*directionDiffuse*textureColor;

	//	//Point Light
	//	float3 pointDir = normalize(pointPosition.xyz - input.WorldPos.xyz);
	//	float4 pointAttenuation = 1.0 - saturate(length(pointPosition - input.WorldPos) / pointRadius);
	//	float pointRatio = saturate(dot(pointDir, input.normal));
	//float4 pointResult = pointAttenuation*pointRatio*pointDiffuse*textureColor;

	//	//Spotlight
	//	float3 spotDir = normalize(spotPosition.xyz - input.WorldPos.xyz);
	//	float surfaceRatio = saturate(dot(-spotDir, spotDirection));
	//float4 spotAttenuation = 1.0 - saturate(length(spotPosition - input.WorldPos) / 30.0f);
	//	float spotFactor = (surfaceRatio > spotRatio) ? 1 : 0;
	//float finalRatio = saturate(dot(spotDir, input.normal));
	//float4 spotResult = spotAttenuation*spotFactor*finalRatio*spotDiffuse*textureColor;

		//return directionResult + pointResult + spotResult;
	return textureColor;
			//* saturate(ambientResult + directionResult + spotResult + pointResult)
	//return textureColor;
}