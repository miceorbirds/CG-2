cbuffer light_buffer : register(b0)
{
	float3 ambient_light_color;
	float ambient_light_strength;
	float3 light_direction;
	float3 diffuse_light_color;
	float diffuse_light_strength;
}

struct PS_INPUT
{
	float4 inPosition : SV_POSITION;
	float2 inTexCoord : TEXCOORD;
	float3 inNormal : NORMAL;
	float3 inWorldPos : WORLD_POSITION;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);


float4 main(PS_INPUT input) : SV_TARGET
{
	float3 sample_color = objTexture.Sample(objSamplerState, input.inTexCoord);
	float3 ambient_light = ambient_light_color * ambient_light_strength;
	float3 applied_light = sample_color * ambient_light;
	
	float3 inv_light_direction = -light_direction;
	float diffuse_impact = max(dot(input.inNormal, -light_direction), 0);
	float3 diffuse_light = diffuse_light_color * diffuse_impact;
	
	applied_light += diffuse_light;
	float3 final_color = applied_light * sample_color;
	return float4(final_color, 1.0f);
}