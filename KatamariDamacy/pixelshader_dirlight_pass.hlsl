#pragma pack_matrix( row_major )

cbuffer light_buffer : register(b0)
{
    float3 ambient_color;
    float ambient_strength;
    
    float3 diffuse_color;
    float diffuse_strength;
    
    float3 light_position;
    float specular_strength;
    
    float3 camera_position;
    float padding;
    float3 light_direction;
}

cbuffer camlight_buffer : register(b1)
{
    float4x4 camLightViewMatrix;
    float4x4 camLightProjMatrix;
};


struct PS_INPUT
{
    float4 PositionClipSpace : SV_POSITION;
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
    // Determine our indices for sampling the texture based on the current screen position
    int3 sampleIndices = int3(input.PositionClipSpace.xy, 0);
    float3 normal = NormalWS.Load(sampleIndices).xyz;
    float3 position = PositionWS.Load(sampleIndices).xyz;
    float3 albedo_color = TextureColor.Load(sampleIndices).xyz;

    float4x4 camShadowVPMatrix = mul(camLightViewMatrix, camLightProjMatrix);
    float4 LightViewPosition = mul(float4(position, 1.f), camShadowVPMatrix);
    
    float3 ambient_light = ambient_color * ambient_strength;
    float3 light_dir = -light_direction;

    float depth_value;
    float light_depth_value;
    float diffuse_impact;
    float3 diffuse = diffuse_color * diffuse_strength;
    // Set the bias value for fixing the floating point precision issues.
    float bias = 0.001f;
	// Set the default output color to the ambient light value for all pixels.
    float3 shadow_color = ambient_light;

    float2 projectTexCoord;
    // Calculate the projected texture coordinates.
    projectTexCoord.x = LightViewPosition.x / LightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -LightViewPosition.y / LightViewPosition.w / 2.0f + 0.5f;
    // Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        // Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
        depth_value = depthMapTexture.Sample(objSamplerStateClamp, projectTexCoord).r;
		// Calculate the depth of the light.
        light_depth_value = LightViewPosition.z / LightViewPosition.w;
		// Subtract the bias from the lightDepthValue.
        light_depth_value = light_depth_value - bias;
		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
        if (light_depth_value < depth_value)
        {
            // Calculate the amount of light on this pixel.
            diffuse_impact = saturate(dot(normal, light_dir));
            if (diffuse_impact > 0.0f)
            {
                // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
                shadow_color += diffuse * diffuse_impact;
                // Saturate the final light color.
                shadow_color = saturate(shadow_color);
            }

        }
    }
    else
    {
        // If this is outside the area of shadow map range then draw things normally with regular lighting.
        diffuse_impact = saturate(dot(normal, light_dir));
        if (diffuse_impact > 0.0f)
        {
            shadow_color += diffuse * diffuse_impact;
            shadow_color = saturate(shadow_color);
        }
    }
    
    // calculate specular
    float3 view_direction = normalize(camera_position - position);
    float3 reflection_direction = normalize(reflect(light_direction, normal));
    float specular_impact = pow(max(dot(reflection_direction, view_direction), 0.0), 32);
    float3 specular_light = specular_strength * specular_impact * diffuse;
    
    
    
    float3 final_color = (shadow_color * albedo_color + specular_light);
    
    
    return float4(final_color, 1.0f);
}