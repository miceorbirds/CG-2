
struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
    float4 inlightViewPosition : SHADOW_TEXCOORD;
};


struct PS_OUTPUT
{
    float4 albedocolor : SV_Target0;
    float4 normalcolor : SV_Target1;
    float4 worldposition : SV_Target2;
};

Texture2D objTexture : register(t0);

SamplerState wrapSamplerState : register(s0);

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    
    output.albedocolor = objTexture.Sample(wrapSamplerState, input.inTexCoord);
    output.normalcolor = float4(input.inNormal, 1.0f);
    output.worldposition = input.inPosition;
    
	return output;
}