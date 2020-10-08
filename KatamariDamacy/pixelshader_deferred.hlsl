#pragma pack_matrix( row_major )

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 WorldPos : WORLD_POSITION;
    float4 LightViewPosition : SHADOW_TEXCOORD;
};


struct PS_OUTPUT
{
    float4 texturecolor : SV_Target0;
    float4 normalcolor : SV_Target1;
    float4 position : SV_Target2;
};

Texture2D objTexture : register(t0);

SamplerState wrapSamplerState : register(s0);

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    
    output.texturecolor = objTexture.Sample(wrapSamplerState, input.TextureCoord);
    output.normalcolor = float4(input.Normal, 1.0f);
    output.position = input.Position;
    
	return output;
}