#pragma pack_matrix( row_major )

struct PS_INPUT
{
    float4 PositionClipSpace : SV_POSITION;
    float2 UV : TEXCOORD;
    float3 NormalWS : NORMAL;
    float3 PositionWS : WORLD_POSITION;
};


struct PS_OUTPUT
{
    float4 TextureColor : SV_Target0;
    float4 NormalWS : SV_Target1;
    float4 PositionWS : SV_Target2;
};

Texture2D objTexture : register(t0);

SamplerState wrapSamplerState : register(s0);

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    // Sample the texture of object
    output.TextureColor = objTexture.Sample(wrapSamplerState, input.UV);
    
    float3 normalWS = normalize(input.NormalWS);
    // Outputs of g-buff
    output.NormalWS = float4(normalWS, 1.0f);
    output.PositionWS = float4(input.PositionWS, 1.0f);
    
	return output;
}