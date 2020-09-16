cbuffer light_buffer : register(b0)
{
	float3 ambient_color;
	float ambient_strength;
    
    float3 diffuse_color;
    float diffuse_strength;
    
    float3 light_position;
    float specular_strength;
    
    float3 camera_position;
}

struct PS_INPUT
{
	float4 inPosition : SV_POSITION;
	float2 inTexCoord : TEXCOORD;
	float3 inNormal : NORMAL;
	float3 inWorldPos : WORLD_POSITION;
};

Texture2D objTexture_diffuse: TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 sample_color = (objTexture_diffuse.Sample(objSamplerState, input.inTexCoord));
	
	float3 ambient_light = ambient_color * ambient_strength;
	
    float3 light_direction = normalize(light_position - input.inWorldPos);
    float diffuse_impact = max(dot(input.inNormal, light_direction), 0.0);
    float3 diffuse_light = diffuse_impact * diffuse_color;
	
    float3 view_direction = normalize(camera_position - input.inWorldPos);
    float3 reflection_direction = normalize(reflect(-light_direction, input.inNormal));
    float specular_impact = pow(max(dot(reflection_direction, view_direction), 0.0), 32);
    float3 specular_light = specular_strength * specular_impact * diffuse_color;
	
    float3 final_color = (diffuse_light + ambient_light + specular_light)  * sample_color;
	return float4(final_color, 1.0f);
}