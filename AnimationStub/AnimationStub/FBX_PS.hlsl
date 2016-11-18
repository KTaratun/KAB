texture2D baseTexture : register(t0);
texture2D normalTexture : register(t1);
texture2D specTexture : register(t2);

SamplerState filters[2] : register(s0);

struct P_IN
{
	float3 uv : TEXCOORD;
	float3 normal : NORMALS;
	float3 binormal : BINORMALS;
	float3 tangent : TANGENT;
	float4 position : SV_POSITION;
	float3 WorldPos : TEXCOORD1;
	//float3 cameraPos : CAMERAPOS;
};

cbuffer LightBuffer1 : register(b0)
{
	float4 ambientDiffuse;

	//float4 directionDiffuse;
	//float3 lightDirection;
	//float pointRadius;

	//float4 pointDiffuse;
	//float4 pointPosition;

	float4 spotDiffuse;
	float4 spotPosition;
	float4 spotDirection;
	float spotRatio;
	float3 padding;
};

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

	float4 textureColor = baseTexture.Sample(filters[0], (float2)input.uv);
	float4 bumpMap = normalTexture.Sample(filters[0], (float2)input.uv);
	float4 specMap = specTexture.Sample(filters[0], (float2)input.uv);

	//Ambient Light
	float4 ambientResult = ambientDiffuse*textureColor;

	//Normalize


	////Directional Light
	//float directionalRatio = saturate(dot(-lightDirection, input.normal));
	//float4 directionResult = directionalRatio*directionDiffuse*textureColor;

	//	//Point Light
	//	float3 pointDir = normalize(pointPosition.xyz - input.WorldPos.xyz);
	//	float4 pointAttenuation = 1.0 - saturate(length(pointPosition - input.WorldPos) / pointRadius);
	//	float pointRatio = saturate(dot(pointDir, input.normal));
	//float4 pointResult = pointAttenuation*pointRatio*pointDiffuse*textureColor;

	bumpMap = (bumpMap * 2.0f) - 1.0f;
	float3 bumpNormal = bumpMap.x*input.tangent + bumpMap.y*input.binormal + bumpMap.z*input.normal;
	bumpNormal = normalize(bumpNormal);

	//Spotlight
	float3 spotDir = normalize(spotPosition.xyz - input.WorldPos);
	float surfaceRatio = saturate(dot(-(float3)spotDir, (float3)spotDirection));
	float3 cameraDir = normalize((float3)input.position - input.WorldPos);
	//float3 reflectionVec = normalize(reflect(spotDirection, bumpNormal));
	//float specular = pow(saturate(dot(reflectionVec, cameraDir)), 1000)*30.0f;
	//float surfaceRatio = saturate(dot((float3)bumpNormal, -(float3)spotDir));
	float4 spotAttenuation = 1.0 - saturate(length((float3)spotPosition - (float3)input.WorldPos) / 30.0f);
	float spotFactor = (surfaceRatio > spotRatio) ? 1 : 0;
	float finalRatio = saturate(dot(spotDir, bumpNormal));
	float4 spotResult = spotAttenuation*finalRatio*textureColor*spotFactor;

	if (finalRatio > 0.0f)
	{
		float3 reflectionVec = reflect(spotDir, (float3)bumpMap);//spotDir - ((spotDir + spotDir) + (input.normal * 2 * (dot(input.normal, -spotDir))));// normalize(2 * finalRatio * bumpNormal + spotDir);
		//Reflection Direction = Light Direction - ((Light Direction + Light Direction) + (Normal * 2 * (Normal dot - Light Direction)))
		//float3 cameraDir = normalize(input.position);

		float specular = 10.0f*pow(saturate(dot(reflectionVec, cameraDir)), 1000);

		specular = specular * (float)specMap;


		spotResult = saturate(spotResult + specular);
	}

	//return directionResult + pointResult + spotResult;
	return/* textureColor**/ ambientResult + spotResult;
	//return textureColor * saturate(ambientResult + directionResult + spotResult + pointResult)
	//return textureColor;
}