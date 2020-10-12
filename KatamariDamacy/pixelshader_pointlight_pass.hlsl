#pragma pack_matrix( row_major )

cbuffer LightData : register(b0)
{   float3 cameraPosition;
    float lightRadius;
    float3 lightColor;
    float padding;
    float3 lightPosition;
}

struct PS_INPUT
{
    float4 PositionClipSpace : SV_POSITION;
    float3 LightPositionWS : WORLD_POSITION;

};

Texture2D objTexture_diffuse : TEXTURE : register(t0);
Texture2D depthMapTexture : DEPTH_TEXTURE : register(t1);
Texture2D TextureColor : ALBEDO_TEXTURE : register(t2);
Texture2D NormalWS : NORMAL_TEXTURE : register(t3);
Texture2D PositionWS : POSITION_TEXTURE : register(t4);

SamplerState objSamplerStateWrap : SAMPLER : register(s0);
SamplerState objSamplerStateClamp : SAMPLER1 : register(s1);


float4 main(PS_INPUT input) : SV_TARGET
{
    int3 sample_indices = int3(input.PositionClipSpace.xy, 0);
    float3 albedo_color = TextureColor.Load(sample_indices).xyz;
    float3 normal = normalize(NormalWS.Load(sample_indices).xyz);
    float3 position = PositionWS.Load(sample_indices).xyz;
    
    float3 final_color;
    
    float3 L = lightPosition - position;
    float dist = length(L);
    
    L /= dist;
   
    
    float atten = max(1.0f - dist / lightRadius, 0);
    atten *= atten;

    float lightAmount = saturate(dot(normal, L));
    float3 color = lightAmount * lightColor * atten;

	//Specular calc
   // float3 V = cameraPosition - position;
   // float3 H = normalize(L + V);
   // float specular = pow(saturate(dot(normal, H)), 10) * atten;

    float3 finalDiffuse = color * albedo_color;
   // float3 finalSpecular = specular * albedo_color * atten;

    float4 totalColor = float4(finalDiffuse, 1.0f);
    return totalColor;
}