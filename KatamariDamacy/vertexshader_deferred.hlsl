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
    float4 PositionClipSpace : SV_POSITION;
    float2 UV : TEXCOORD;
    float3 NormalWS : NORMAL;
    float3 PositionWS : WORLD_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    // Calculate the clip-space position
    output.PositionClipSpace = mul(float4(input.Position, 1.f), WVP_matrix);
    // Pass along the texture coordinate
    output.UV = input.TextureCoord;
    // Convert position and normals to world space
    output.NormalWS = normalize(mul(float4(input.Normal, 0.0f), world_matrix).xyz);
    output.PositionWS = mul(float4(input.Position, 1.0f), world_matrix).xyz;
    
    return output;
}