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

cbuffer pointlight_buffer : register(b1)
{   
    float dynamic_light_attenuation_a;
    float dynamic_light_attenuation_b;
    float dynamic_light_attenuation_c;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
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
    float3 final_color = ambient_color;
    
    
    int3 sample_indices = int3(input.inPosition.xy, 0);
    float3 albedo_color = albedo_texture.Load(sample_indices).xyz;
    float3 normal = normalize(normal_texture.Load(sample_indices).xyz);
    float3 position = position_texture.Load(sample_indices).xyz;

    if (length(normal) > 0.0f)
    {
        float3 ambient_light = ambient_color * ambient_strength;

        float3 applied_light = ambient_light;
        
        float3 vector_to_light = normalize(light_position - position);
        float3 diffuse_light_intensity = max(dot(vector_to_light, normal), 0);
        
        float distance_to_light = length(vector_to_light);
        if (distance_to_light > 2.0f)
        {
            return float4(0.0f, 0.0f, 0.0f, 0.0f);
        }
        vector_to_light /= distance_to_light;
        
        float attenuation_factor = 1 / (dynamic_light_attenuation_a + dynamic_light_attenuation_b * distance_to_light + dynamic_light_attenuation_c * pow(distance_to_light, 2));
        diffuse_light_intensity *= attenuation_factor;
    
        float3 diffuse_light = diffuse_light_intensity * diffuse_strength * diffuse_color;
        applied_light += diffuse_light;
        final_color += applied_light;
    }
    
    return float4(final_color, 1.0f);
}