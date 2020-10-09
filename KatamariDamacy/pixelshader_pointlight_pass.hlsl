#pragma pack_matrix( row_major )

cbuffer LightData : register(b0)
{
    float3 cameraPosition;
    float3 lightColor;
    float3 lightPosition;
    float lightRadius;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
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

    int3 sample_indices = int3(input.Position.xy, 0);
    float3 albedo_color = albedo_texture.Load(sample_indices).xyz;
    float3 normal = normalize(normal_texture.Load(sample_indices).xyz);
    float3 position = position_texture.Load(sample_indices).xyz;
    
    
    
    float3 final_color = albedo_color * 0.1;
    float3 view_direction = normalize(cameraPosition - position);
    float3 light_vector = lightPosition - position;
    float3 light_dir = normalize(light_vector);
    float attenuation = saturate(1.0f - length(light_vector) / lightRadius);
    
    float3 diffuse = max(dot(normal, light_dir), 0.0) * albedo_color * lightColor;
    final_color += diffuse;
    
   
    
    return float4(final_color, 1.0f);
}