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
    float4 Position : SV_POSITION;
    float2 Texture : TEXCOORD;
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
    int3 sampleIndices = int3(input.Position.xy, 0);

    float3 normal = normal_texture.Load(sampleIndices).xyz;

    float3 position = position_texture.Load(sampleIndices).xyz;

    float3 diffuse = albedo_texture.Load(sampleIndices).xyz;

    float3 L = -light_direction;

    float lightAmountDL = saturate(dot(normal, L));
    float3 color = diffuse_color * lightAmountDL * diffuse;

    return float4(color, 1.0f);
}