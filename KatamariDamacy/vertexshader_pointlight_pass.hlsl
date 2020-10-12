#pragma pack_matrix( row_major )

cbuffer per_object_buffer : register(b0)
{
    float4x4 WVP_matrix;
    float4x4 world_matrix;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexureCoord : TEXCOORD;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 PositionClipSpace : SV_POSITION;
    float3 PositionWS : WORLD_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.PositionClipSpace = mul(float4(input.Position, 1.f), WVP_matrix);
    output.PositionWS = mul(float4(input.Position, 1.f), world_matrix);
    return output;
}