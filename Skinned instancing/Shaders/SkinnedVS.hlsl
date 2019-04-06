#pragma pack_matrix(row_major)
#define MAX_JOINTS 32

Texture2D AnimPositions : register( t0 );
Texture2D AnimRotations : register( t1 );
SamplerState Sampler : register( s0 );

cbuffer MVP : register ( b0 )
{
	float4x4 modeling;
	float4x4 view;
	float4x4 projection;
};

cbuffer AnimationBuffer : register ( b1 )
{
	float4x4 mBindpose[MAX_JOINTS];
	float	 fCurrentTime;
	float	 fTotalTime;

	int		 nJointCount;
	int		 nCurrentAnimation;
	int		 nFrameCount;

}

struct VERTEX
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXTURECOORD;
	float4 jointIndex : JOINT_INDEX;
	float4 weights : WEIGHTS;
};

struct OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXTURECOORD;
};

float4x4 QuaternionToMatrix(float4 quat)
{
	float4x4 m = float4x4( float4( 0, 0, 0, 0 ), float4( 0, 0, 0, 0 ), float4( 0, 0, 0, 0 ), float4( 0, 0, 0, 0 ) );

	float x = quat.x, y = quat.y, z = quat.z, w = quat.w;
	float x2 = x + x, y2 = y + y, z2 = z + z;
	float xx = x * x2, xy = x * y2, xz = x * z2;
	float yy = y * y2, yz = y * z2, zz = z * z2;
	float wx = w * x2, wy = w * y2, wz = w * z2;

	m[0][0] = 1.0 - ( yy + zz );
	m[0][1] = xy - wz;
	m[0][2] = xz + wy;

	m[1][0] = xy + wz;
	m[1][1] = 1.0 - ( xx + zz );
	m[1][2] = yz - wx;

	m[2][0] = xz - wy;
	m[2][1] = yz + wx;
	m[2][2] = 1.0 - ( xx + yy );

	m[3][3] = 1.0;
	
	return transpose(m);
}

float4x4 GetMatrixFromTexture(float2 vUV)
{
	float4 jointRotation = AnimRotations.SampleLevel(Sampler, vUV, 0);
	float4 jointPosition = AnimPositions.SampleLevel(Sampler, vUV, 0);

	float4x4 jointMatrix = QuaternionToMatrix(jointRotation);
	
	jointMatrix[3] = jointPosition;

	return jointMatrix;
}

OUTPUT main(VERTEX input)
{
	OUTPUT output = (OUTPUT)0;

	float4 skinnedPos = float4( 0, 0, 0, 0 );
	float4 skinnedNorm = float4( 0, 0, 0, 0 );
	input.pos.w = 1;

	for(int i = 0; i < 4; i++)
	{
		int nTextureWidth;
		int nTextureHeight;
		AnimPositions.GetDimensions(nTextureWidth, nTextureHeight);

		float fLerpAdjust = nFrameCount / (nTextureWidth + 0.0f);
		float fRatio = fCurrentTime / fTotalTime;

		float2 vUV = float2(fRatio * fLerpAdjust, (input.jointIndex[i] + ( nJointCount * nCurrentAnimation) )/ (nTextureHeight - 1));

		float4x4 joint = mul(mBindpose[input.jointIndex[i]], GetMatrixFromTexture(vUV));
		skinnedPos += mul(input.pos, joint) * input.weights[i];
		skinnedNorm += mul( float4( input.normal.xyz, 0.0f ), joint * input.weights[i]);
	}

	output.pos = mul(skinnedPos, modeling);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);
	output.normal = (float3)mul(skinnedNorm, modeling);

	output.uv = input.uv;

	return output;
}