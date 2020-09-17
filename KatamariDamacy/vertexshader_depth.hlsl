#pragma pack_matrix( row_major )

cbuffer per_object_buffer : register(b0)
{
    float4x4 WVP_matrix;
    float4x4 world_matrix;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
};


VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos, 1.f), WVP_matrix);
    return output;
}
