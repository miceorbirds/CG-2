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
    float dynamic_light_attenuation_a = 1.0f;
    float dynamic_light_attenuation_b = 0.1f;
    float dynamic_light_attenuation_c = 0.1f;
    float dynamic_light_strength = 5.f;
    float light_rad = 10.f;
    
    int3 sample_indices = int3(input.Position.xy, 0);
    float3 albedo_color = albedo_texture.Load(sample_indices).xyz;
    float3 normal = normalize(normal_texture.Load(sample_indices).xyz);
    float3 position = position_texture.Load(sample_indices).xyz;
    
    float3 ambient_light = lightColor * 0.5;
    float3 applied_light = ambient_light;
    
    float3 vector_to_light = normalize(lightPosition - position);
    float3 diffuse_light_intensity = max(dot(vector_to_light, normal), 0);
    
    float distance_to_light = distance(lightPosition, position);
    
    float atten = max(1.0f - distance_to_light / light_rad, 0);
    atten *= atten;
    
    // float attenuation_factor = 1 / (dynamic_light_attenuation_a + dynamic_light_attenuation_b * distance_to_light + dynamic_light_attenuation_c * pow(distance_to_light, 2));
    
    
    
    //diffuse_light_intensity *= atten;
    float3 diffuse_light = diffuse_light_intensity * dynamic_light_strength * lightColor;
    applied_light += diffuse_light;
    
    float3 final_color = albedo_color * applied_light * atten;
  
    return float4(final_color, 1.0f);
}