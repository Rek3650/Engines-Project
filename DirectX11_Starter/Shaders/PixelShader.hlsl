// texture to get samples from and the sampler
Texture2D myTexture : register(t0);
SamplerState mySampler : register(s0);

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
	float3 normal		: NORMAL;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float3 lightDirection = float3(-1.0f, 0.5f, -0.25f);
	float4 lightAmbience = float4(0.2f, 0.2f, 0.2f, 1.0f);
	float4 lightDiffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 diffuse = myTexture.Sample(mySampler, input.uv)+input.color;

	float3 finalColor = diffuse * lightAmbience;
	finalColor += saturate(dot(lightDirection, input.normal)) * lightDiffuse * diffuse;

	return float4(finalColor, diffuse.a);
}