#pragma once

#include "../error_logger.h"
#include "../Game/game_object.h"

constexpr UINT BUFFER_COUNT = 3;

class GBufferRT
{
public:
	GBufferRT();
	bool Init(ID3D11Device* device, float texture_width, float texture_height);
	int SHADOWMAP_WIDTH = 2048;
	int SHADOWMAP_HEIGHT = 2048;

	ID3D11Texture2D* m_renderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView* m_renderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* m_shaderResourceViewArray[BUFFER_COUNT];

private:

	float m_texture_width = 0;
	float m_texture_height = 0;
};