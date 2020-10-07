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

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inWorldPos : WORLD_POSITION;
    float4 inlightViewPosition : SHADOW_TEXCOORD;
};


Texture2D objTexture_diffuse : TEXTURE : register(t0);
Texture2D depthMapTexture : DEPTH_TEXTURE : register(t1);
Texture2D albedo_texture : ALBEDO_TEXTURE : register(t2);
Texture2D normal_texture : NORMAL_TEXTURE : register(t3);
Texture2D position_texture : POSITION_TEXTURE : register(t4);

SamplerState objSamplerStateWrap : SAMPLER : register(s0);
SamplerState objSamplerStateClamp : SAMPLER1 : register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{
    float diffuse_impact;
    
    int3 sample_indices = int3(input.inPosition.xy, 0);
    float3 albedo_color = albedo_texture.Load(sample_indices).xyz;
    float3 normal = normalize(normal_texture.Load(sample_indices).xyz);
    float3 position = position_texture.Load(sample_indices).xyz;

    float3 ambient_light = ambient_color * ambient_strength;
    float3 light_dir = -light_direction;
    
    
    //float3 diffuse_light = diffuse_impact * diffuse_color;
    
    float depth_value;
    float light_depth_value;
    // Set the bias value for fixing the floating point precision issues.
    float bias = 0.001f;
	// Set the default output color to the ambient light value for all pixels.
    float3 shadow_color = ambient_color;

    float2 projectTexCoord;
    // Calculate the projected texture coordinates.
    projectTexCoord.x = input.inlightViewPosition.x / input.inlightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.inlightViewPosition.y / input.inlightViewPosition.w / 2.0f + 0.5f;
    // Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        // Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
        depth_value = depthMapTexture.Sample(objSamplerStateClamp, projectTexCoord).r;
		// Calculate the depth of the light.
        light_depth_value = input.inlightViewPosition.z / input.inlightViewPosition.w;
		// Subtract the bias from the lightDepthValue.
        light_depth_value = light_depth_value - bias;
		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
        if (light_depth_value < depth_value)
        {
            // Calculate the amount of light on this pixel.
            diffuse_impact = saturate(dot(normal, light_dir));
            if(diffuse_impact > 0.0f)
            {
                // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
                shadow_color += diffuse_color * diffuse_impact;
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
            shadow_color += diffuse_color * diffuse_impact;
            shadow_color = saturate(shadow_color);
        }
    }
    
    // calculate specular
    float3 view_direction = normalize(camera_position - input.inWorldPos);
    float3 reflection_direction = normalize(reflect(light_direction, normal));
    float specular_impact = pow(max(dot(reflection_direction, view_direction), 0.0), 32);
    float3 specular_light = specular_strength * specular_impact * diffuse_color;
    
    
    
    float3 final_color = (shadow_color * albedo_color + specular_light);
    
    
	return float4(final_color, 1.0f);
}