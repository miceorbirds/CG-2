cbuffer lightBuffer : register(b0)
{
    float3 ambient_light_color;
    float ambient_light_strength;

    float3 dynamic_light_color;
    float dynamic_light_strength;
    float3 dynamic_light_position;
    
    float dynamic_light_attenuation_a;
    float dynamic_light_attenuation_b;
    float dynamic_light_attenuation_c;
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
    //float3 sampleColor = input.inNormal;

    float3 ambient_light = ambient_light_color * ambient_light_strength;
    
    float3 applied_light = ambient_light;
    float3 vector_to_light = normalize(dynamic_light_position - input.inWorldPos);
    float3 diffuse_light_intensity = max(dot(vector_to_light, input.inNormal), 0);
    
    float distance_to_light = distance(dynamic_light_position, input.inWorldPos);
    float attenuation_factor = 1 / (dynamic_light_attenuation_a + dynamic_light_attenuation_b * distance_to_light + dynamic_light_attenuation_c * pow(distance_to_light, 2));
    diffuse_light_intensity *= attenuation_factor;
    
    float3 diffuse_light = diffuse_light_intensity * dynamic_light_strength * dynamic_light_color;
    applied_light += diffuse_light;
    float3 final_color = sample_color * applied_light;
    return float4(final_color, 1.0f);
}