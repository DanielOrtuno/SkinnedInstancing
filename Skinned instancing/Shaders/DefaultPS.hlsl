#define MAX_LIGHTS 16


Texture2D diffuseMap : register( t0 );
Texture2D noiseMap   : register( t1 );
SamplerState filter	 : register( s0 );

struct INPUT
{
	float4 vPosition : SV_POSITION;
	float3 vWorldPos : WPOSITION;
	float3 vNormal : NORMAL;
	float2 vUV : TEXTURECOORD;
	float2 vNDC : NDCPOS;
	uint nInstanceId : SV_INSTANCEID;
};


float4 main(INPUT input) : SV_TARGET
{
	return diffuseMap.Sample(filter, input.vUV);
}

