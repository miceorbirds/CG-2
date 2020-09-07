#include "direct2d.h"
#include "../error_logger.h"

bool Direct2D::Initialize(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view)
{
	try
	{
		// create d2d factory
		auto hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_d2d_factory.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create direct2D factory");
		// Add D2D to the swap chain
		Microsoft::WRL::ComPtr <ID3D11Resource> res;
		render_target_view->GetResource(res.GetAddressOf());
		Microsoft::WRL::ComPtr <IDXGISurface> surface;
		res->QueryInterface(__uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf()));

		auto props = 
			D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
		
		hr = m_d2d_factory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, m_d2d_render_target.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create direct2D dxgi render surface");

		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(m_dwrite_factory.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "Failed to create dwrite factory");
		
		hr = m_dwrite_factory->CreateTypography(m_typography.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create typography factory");
		
		// Set the stylistic set.
		DWRITE_FONT_FEATURE font_feature = { DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7, 1 };

		hr = m_typography->AddFontFeature(font_feature);
		COM_ERROR_IF_FAILED(hr, "Failed to add font feature to typography");

		hr = m_dwrite_factory->CreateTextFormat(
			L"Gabriola", // Font family name.
			nullptr,  // Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
			20.0f, L"en-us",
			m_text_format.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create dwrite text format");

		hr = m_d2d_render_target->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black, 1.0f), &m_brush_1);
		COM_ERROR_IF_FAILED(hr, "Failed to create solid color brush 1");
		hr = m_d2d_render_target->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF(0x9ACD32, 1.0f)), &m_brush_2
		);
		COM_ERROR_IF_FAILED(hr, "Failed to create solid color brush 2");

		m_text_length = static_cast<UINT32>(wcslen(m_text));

		hr = m_dwrite_factory->CreateTextLayout(m_text, m_text_length, m_text_format.Get(), 200, 100, m_text_layout.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create dwrite text layout");

		DWRITE_TEXT_RANGE text_range = { 0,m_text_length };
		hr = m_text_layout->SetTypography(m_typography.Get(), text_range);
		COM_ERROR_IF_FAILED(hr, "Failed to set typography");

		hr = m_text_layout->SetFontWeight(DWRITE_FONT_WEIGHT_BOLD, text_range);
		COM_ERROR_IF_FAILED(hr, "Failed to set font weight");

		hr = m_text_layout->SetUnderline(TRUE, text_range);
		COM_ERROR_IF_FAILED(hr, "Failed to set underline");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void Direct2D::Draw()
{
	try
	{
		m_d2d_render_target->BeginDraw();
		m_d2d_render_target->DrawTextLayout(m_origin,
			m_text_layout.Get(), m_brush_2.Get());
		auto hr = m_d2d_render_target->EndDraw();
		COM_ERROR_IF_FAILED(hr, "Failed to end draw direct2D");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
	}
}

void Direct2D::RenderText(std::string text)
{
	try
	{
		auto text_length = static_cast<UINT32>(text.length());
		auto hr = m_dwrite_factory->CreateTextLayout(
			StringConverter::StringToWide(text).c_str(),
			text_length, m_text_format.Get(),
			200, 100, m_text_layout.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create text layout");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
	}
}
