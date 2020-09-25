cbuffer light_buffer : register(b0)
{
    float3 ambient_color;
    float ambient_strength;
    
    float3 diffuse_color;
    float diffuse_strength;
    
    float3 light_position;
    float specular_strength;
    
    float3 camera_position;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
    float4 outShadowMap : SHADOW_TEXCOORD;
};

Texture2D objTexture_diffuse : TEXTURE : register(t0);
Texture2D depthTexture : DEPTH_TEXTURE : register(t1);

SamplerState objSamplerStateWrap : SAMPLER : register(s0);
SamplerState objSamplerStateClamp : SAMPLER1 : register(s1);

float GetShadow(float4 shadowCoord)
{
    float2 projectTexCoord;
    projectTexCoord.x = shadowCoord.x / shadowCoord.w / 2.0f + 0.5f;
    projectTexCoord.y = -shadowCoord.y / shadowCoord.w / 2.0f + 0.5f;
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        const float shadowMapTexelSize = 1.0f / 1000.f;
        const float dx = shadowMapTexelSize;
        const float shadowCoordV = shadowCoord.z / shadowCoord.w;
        float bias = 0.001f;
    
        if (shadowCoordV - bias < depthTexture.Sample(objSamplerStateClamp, projectTexCoord).r)
        {
            return 1.0f;
        }
        
        bool d0 = shadowCoordV - bias < depthTexture.Sample(objSamplerStateClamp, projectTexCoord).r;
        float d1 = shadowCoordV - bias < depthTexture.Sample(objSamplerStateClamp, projectTexCoord + float2(dx, 0)).r;
        float d2 = shadowCoordV - bias < depthTexture.Sample(objSamplerStateClamp, projectTexCoord + float2(0, dx)).r;
        float d3 = shadowCoordV - bias < depthTexture.Sample(objSamplerStateClamp, projectTexCoord + float2(-dx, 0)).r;
        float d4 = shadowCoordV - bias < depthTexture.Sample(objSamplerStateClamp, projectTexCoord + float2(0, -dx)).r;
        
        float d5 = shadowCoordV - bias < depthTexture.Sample(objSamplerStateClamp, projectTexCoord + float2(dx, dx)).r;
        float d6 = shadowCoordV - bias < depthTexture.Sample(objSamplerStateClamp, projectTexCoord + float2(dx, -dx)).r;
        float d7 = shadowCoordV - bias < depthTexture.Sample(objSamplerStateClamp, projectTexCoord + float2(-dx, dx)).r;
        float d8 = shadowCoordV - bias < depthTexture.Sample(objSamplerStateClamp, projectTexCoord + float2(-dx, -dx)).r;
        
        float res = (d0 * 2 + d1 + d2 + d3 + d4 + d5 + d6 + d7 + d8) / 10.0f;
        res *= 1 - dot(min(pow(abs(projectTexCoord - 0.5f) * 2, 2), 1.0f), float2(1, 1));
        return res;
        
        return d0;
    }
    
    return 1;
}



float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sample_color = (objTexture_diffuse.Sample(objSamplerStateWrap, input.inTexCoord));
	
    float3 ambient_light = ambient_color * ambient_strength;
	
    float3 light_direction = normalize(light_position - input.inWorldPos);
    float diffuse_impact = max(dot(input.inNormal, light_direction), 0.0);
    float3 diffuse_light = diffuse_impact * diffuse_color;
	
    float3 view_direction = normalize(camera_position - input.inWorldPos);
    float3 reflection_direction = normalize(reflect(-light_direction, input.inNormal));
    float specular_impact = pow(max(dot(reflection_direction, view_direction), 0.0), 32);
    float3 specular_light = specular_strength * specular_impact * diffuse_color;
	
    float3 final_color = (diffuse_light + ambient_light + specular_light) * sample_color;
    return float4(final_color, 1.0f);
}