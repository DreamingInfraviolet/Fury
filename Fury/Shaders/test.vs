cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
}

struct VertexInputType
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
};

struct PixelInputType
{
	float4 positionProjectionspace : SV_POSITION;
	float4 positionCameraspace : POSITION;
	float3 normalCameraspace : NORMAL;
	float3 lightDirection : POSITION1;
	float2 uv : TEXCOORD0;
};

static const float3 LightVector = float3(0,1,1);

PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	
	output.positionCameraspace = float4(input.position,1.f);
	output.positionCameraspace = mul(worldMatrix,output.positionCameraspace);
	output.positionCameraspace = mul(viewMatrix,output.positionCameraspace);
	
	output.positionProjectionspace = mul(projectionMatrix,output.positionCameraspace);
	
	float4 normalOut = float4(input.normal, 0.f);
	normalOut = mul(worldMatrix, normalOut);
	normalOut = mul(viewMatrix, normalOut);
	output.normalCameraspace = normalOut.xyz;
	
	output.lightDirection = mul(viewMatrix, LightVector);
	
	output.uv = float2(input.uv.x, -input.uv.y);
	
	return output;
}