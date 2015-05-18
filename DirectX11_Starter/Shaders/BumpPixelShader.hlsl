// texture to get samples from and the sampler
Texture2D myTexture : register(t0);
Texture2D normalMap : register(t1);
SamplerState mySampler : register(s0);

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	float3 lightDirection = float3(-0.2f, 0.5f, -0.3f);
	float4 lightAmbience = float4(0.5f, 0.5f, 0.5f, 1.0f);
	float4 lightDiffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 diffuse = myTexture.Sample(mySampler, input.uv)+input.color;
	
	//Load normal from normal map
	float4 nMap = normalMap.Sample( mySampler, input.uv );

	//Change normal map range from [0, 1] to [-1, 1]
	nMap = (2.0f*nMap) - 1.0f;

	//Make sure tangent is completely orthogonal to normal
	input.tangent = normalize(input.tangent - dot(input.tangent, input.normal)*input.normal);

	//Create the biTangent
	float3 biTangent = cross(input.normal, input.tangent);

	//Create the "Texture Space"
	float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);

	//Convert normal from normal map to texture space and store in input.normal
	input.normal = normalize(mul(nMap, texSpace));
	
	float3 finalColor = diffuse * lightAmbience;
	finalColor += saturate(dot(lightDirection, input.normal)) * lightDiffuse * diffuse;

	return float4(finalColor, diffuse.a);
}