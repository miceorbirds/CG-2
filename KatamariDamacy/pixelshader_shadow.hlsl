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
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
    float4 inlightViewPosition : SHADOW_TEXCOORD;
};

Texture2D objTexture_diffuse : TEXTURE : register(t0);
Texture2D depthMapTexture : DEPTH_TEXTURE : register(t1);

SamplerState objSamplerStateWrap : SAMPLER : register(s0);
SamplerState objSamplerStateClamp : SAMPLER1 : register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{
    float bias;
    float4 color;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    float lightIntensity;
    float4 textureColor;
    float3 lightDir;


	// Invert the light direction.
    lightDir = -light_direction;

	// Set the bias value for fixing the floating point precision issues.
    bias = 0.001f;

	// Set the default output color to the ambient light value for all pixels.
    color = float4(ambient_color, 1.0f);

	// Calculate the projected texture coordinates.
    projectTexCoord.x = input.inlightViewPosition.x / input.inlightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.inlightViewPosition.y / input.inlightViewPosition.w / 2.0f + 0.5f;

	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
        depthValue = depthMapTexture.Sample(objSamplerStateClamp, projectTexCoord).r;

		// Calculate the depth of the light.
        lightDepthValue = input.inlightViewPosition.z / input.inlightViewPosition.w;

		// Subtract the bias from the lightDepthValue.
        lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
        if (lightDepthValue < depthValue)
        {
		    // Calculate the amount of light on this pixel.
            lightIntensity = saturate(dot(input.inNormal, lightDir));

            if (lightIntensity > 0.0f)
            {
				// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
                color += (float4(diffuse_color,1.0f) * lightIntensity);

				// Saturate the final light color.
                color = saturate(color);
            }
        }
    }
    else
    {
        // If this is outside the area of shadow map range then draw things normally with regular lighting.
        lightIntensity = saturate(dot(input.inNormal, lightDir));
        if (lightIntensity > 0.0f)
        {
            color += (float4(diffuse_color, 1.0f) * lightIntensity);
            color = saturate(color);
        }
    }

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = objTexture_diffuse.Sample(objSamplerStateWrap, input.inTexCoord);

	// Combine the light and texture color.
    color = color * textureColor;

    float3 view_direction = normalize(camera_position - input.inWorldPos);
    float3 reflection_direction = normalize(reflect(light_direction, input.inNormal));
    float specular_impact = pow(max(dot(reflection_direction, view_direction), 0.0), 32);
    float3 specular_light = specular_strength * specular_impact * diffuse_color;
    color = saturate(color + float4(specular_light, 1.0f));
    
    return color;
}