#pragma once

#include <d3d11.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <string>
#include <wrl/client.h>

class Direct2D
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view);
	void Update();
	void Draw() const;
	void RenderText(std::string text);
private:
	Microsoft::WRL::ComPtr<ID2D1Factory> m_d2d_factory;
	Microsoft::WRL::ComPtr<ID2D1RenderTarget> m_d2d_render_target;

	Microsoft::WRL::ComPtr <IDWriteFactory> m_dwrite_factory;
	Microsoft::WRL::ComPtr <IDWriteTypography> m_typography;

	Microsoft::WRL::ComPtr <IDWriteTextFormat> m_text_format;
	Microsoft::WRL::ComPtr <IDWriteTextLayout> m_text_layout;

	std::wstring m_frame_text = L"Hello";
	const wchar_t* m_text = L"Hello";
	UINT32 m_text_length{ 0 };

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush_1;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush_2;
	D2D1_POINT_2F m_origin{ D2D1::Point2F(30, 30) };
};
