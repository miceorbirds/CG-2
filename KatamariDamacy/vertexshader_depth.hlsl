#pragma pack_matrix( row_major )

cbuffer per_object_buffer : register(b0)
{
    float4x4 WVP_matrix;
    float4x4 world_matrix;
};

cbuffer camlight_buffer : register(b1)
{
    row_major float4x4 camLightViewMatrix;
    row_major float4x4 camLightProjMatrix;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexureCoord : TEXCOORD;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
};


VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4x4 camLightVPmatrix = mul(camLightViewMatrix, camLightProjMatrix);
    float4x4 wvplight_matrix = mul(world_matrix, camLightVPmatrix);
    output.Position = mul(float4(input.Position, 1.f), wvplight_matrix);
    return output;
}
