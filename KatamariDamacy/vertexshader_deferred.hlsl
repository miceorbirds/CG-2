#pragma pack_matrix( row_major )

cbuffer per_object_buffer : register(b0)
{
    float4x4 WVP_matrix;
    float4x4 world_matrix;
};

cbuffer camlight_buffer : register(b1)
{
    float4x4 camLightViewMatrix;
    float4x4 camLightProjMatrix;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 WorldPos : WORLD_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = mul(float4(input.Position, 1.f), WVP_matrix);
    output.UV = input.TextureCoord;
    output.Normal = normalize(mul(float4(input.Normal, 0.0f), world_matrix).xyz);
    output.WorldPos = mul(float4(input.Position, 1.0f), world_matrix).xyz;
    
    return output;
}