cbuffer light_buffer : register(b0)
{
    float3 ambient_light_color;
    float ambient_light_strength;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);


float4 main(PS_INPUT input) : SV_TARGET
{
    //float3 sample_color = objTexture.Sample(objSamplerState, input.inTexCoord);
    float3 sample_color = input.inNormal;
    float3 ambient_light = ambient_light_color * ambient_light_strength;
    float3 final_color = sample_color * ambient_light;
    return float4(final_color, 1.0f);
}