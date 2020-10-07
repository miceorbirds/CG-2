#pragma pack_matrix( row_major )

cbuffer light_buffer : register(b0)
{
    float3 ambient_color;
    float ambient_strength;
    
    float3 diffuse_color;
    float diffuse_strength;
    
    float3 light_position;
    float specular_strength;
    
    float3 camera_position;
    float padding;
    float3 light_direction;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inWorldPos : WORLD_POSITION;
    float4 inlightViewPosition : SHADOW_TEXCOORD;
};


Texture2D objTexture_diffuse : TEXTURE : register(t0);
Texture2D depthMapTexture : DEPTH_TEXTURE : register(t1);
Texture2D albedo_texture : ALBEDO_TEXTURE : register(t2);
Texture2D normal_texture : NORMAL_TEXTURE : register(t3);
Texture2D position_texture : POSITION_TEXTURE : register(t4);

SamplerState objSamplerStateWrap : SAMPLER : register(s0);
SamplerState objSamplerStateClamp : SAMPLER1 : register(s1);


float4 main(PS_INPUT input) : SV_TARGET
{
    int3 sample_indices = int3(input.inPosition.xy, 0);
    float3 albedo_color = albedo_texture.Load(sample_indices).xyz;
    float3 normal = normalize(normal_texture.Load(sample_indices).xyz);
    float3 position = position_texture.Load(sample_indices).xyz;

    float3 ambient_light = ambient_color * ambient_strength;
    float3 light_dir = -light_direction;
    
    float diffuse_impact = max(dot(normal, light_dir), 0.0);
    float3 diffuse_light = diffuse_impact * diffuse_color;
    
    float3 view_direction = normalize(camera_position - input.inWorldPos);
    float3 reflection_direction = normalize(reflect(light_direction, normal));
    float specular_impact = pow(max(dot(reflection_direction, view_direction), 0.0), 32);
    float3 specular_light = specular_strength * specular_impact * diffuse_color;
    
    
    float3 final_color = (diffuse_light + ambient_light + specular_light) * albedo_color;
    
    
	return float4(final_color, 1.0f);
}