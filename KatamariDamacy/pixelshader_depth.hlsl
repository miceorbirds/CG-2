struct PS_INPUT
{
    float4 Position : SV_POSITION;
};
Texture2D objTexture : register(t0);
SamplerState objSamplerState : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	// Получаем значение глубины пикселя деля Z пикселя глубины на гомогенную координату W
    float depthValue = input.Position.z / input.Position.w;

    return float4(depthValue, depthValue, depthValue, 1.0f);
}