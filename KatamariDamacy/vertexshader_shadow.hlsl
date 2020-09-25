#pragma pack_matrix( row_major )

cbuffer per_object_buffer : register(b0)
{
    float4x4 WVP_matrix;
    float4x4 world_matrix;
};

cbuffer light_eye_buffer : register(b1)
{
    float4x4 WVP_light_matrix;
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
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WORLD_POSITION;
    float4 outlightViewPosition : SHADOW_TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outlightViewPosition = mul(float4(input.inPos, 1.f), WVP_light_matrix);

    output.outPosition = mul(float4(input.inPos, 1.f), WVP_matrix);

    output.outTexCoord = input.inTexCoord;
    output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), world_matrix));
    output.outWorldPos = mul(float4(input.inPos, 1.0f), world_matrix);

    return output;
}