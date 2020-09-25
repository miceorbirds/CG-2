#include "engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	m_timer.Start();
	if (!this->m_render_window.Initialize(this, hInstance, window_title, window_class, width, height))
		return false;
	if (!m_gfx.Initialize(this->m_render_window.GetHWND(), width, height))
		return false;
	return true;
}

bool Engine::ProcessMessages()
{
	return this->m_render_window.ProcessMessages();
}

void Engine::Update()
{
	const float delta_time = m_timer.GetMilisecondsElapsed();
	m_timer.Restart();
	while (!m_keyboard.CharBufferIsEmpty())
	{
		auto ch = m_keyboard.ReadChar();
	}
	while (!m_keyboard.KeyBufferIsEmpty())
	{
		auto kbe = m_keyboard.ReadKey();
		auto keycode = kbe.GetKeyCode();
	}
	while (!m_mouse.EventBufferIsEmpty())
	{
		auto me = m_mouse.ReadEvent();
		if (m_mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				this->m_gfx.m_camera.AdjustRotation(static_cast<float>(me.GetPosY()) * 0.01f, static_cast<float>(me.GetPosX()) * 0.01f, 0);
			}
		}
	}
	//this->m_gfx.m_katamary.AdjustRotation(.001 * delta_time, 0.f, .0f);
	float camera_speed = 0.006f;
	if (m_keyboard.KeyIsPressed(VK_SHIFT))
	{
		camera_speed = 0.01f;
	}

	if (m_keyboard.KeyIsPressed(VK_CONTROL))
	{
		if (m_keyboard.KeyIsPressed('W'))
		{
			this->m_gfx.m_katamary.AdjustPosition(this->m_gfx.m_katamary.GetForwardVector(true) * camera_speed * delta_time);
			this->m_gfx.m_katamary.AdjustRotation(m_gfx.m_katamary.GetRightVector(true) * camera_speed * delta_time);
			if (!this->m_gfx.m_katamary.childs.empty())
			{
				for (int i = 0; i < m_gfx.m_katamary.childs.size(); i++)
				{
					this->m_gfx.m_katamary.childs[i]->UpdateMatrix();
				}
			}
		}
		if (m_keyboard.KeyIsPressed('S'))
		{
			this->m_gfx.m_katamary.AdjustPosition(this->m_gfx.m_katamary.GetBackwardVector(true) * camera_speed * delta_time);
			this->m_gfx.m_katamary.AdjustRotation(m_gfx.m_katamary.GetLeftVector(true) * camera_speed * delta_time);
			if (!this->m_gfx.m_katamary.childs.empty())
			{
				for (int i = 0; i < m_gfx.m_katamary.childs.size(); i++)
				{
					this->m_gfx.m_katamary.childs[i]->UpdateMatrix();
				}
			}
		}
		if (m_keyboard.KeyIsPressed('A'))
		{
			this->m_gfx.m_katamary.AdjustPosition(this->m_gfx.m_katamary.GetLeftVector(true) * camera_speed * delta_time);
			this->m_gfx.m_katamary.AdjustRotation(m_gfx.m_katamary.GetBackwardVector(true) * camera_speed * delta_time);
			if (!this->m_gfx.m_katamary.childs.empty())
			{
				for (int i = 0; i < m_gfx.m_katamary.childs.size(); i++)
				{
					this->m_gfx.m_katamary.childs[i]->UpdateMatrix();
				}
			}
		}
		if (m_keyboard.KeyIsPressed('D'))
		{
			this->m_gfx.m_katamary.AdjustPosition(this->m_gfx.m_katamary.GetRightVector(true) * camera_speed * delta_time);
			this->m_gfx.m_katamary.AdjustRotation(m_gfx.m_katamary.GetForwardVector(true) * camera_speed * delta_time);
			if (!this->m_gfx.m_katamary.childs.empty())
			{
				for (int i = 0; i < m_gfx.m_katamary.childs.size(); i++)
				{
					this->m_gfx.m_katamary.childs[i]->UpdateMatrix();
				}
			}
		}
	}
	else
	{
		if (m_keyboard.KeyIsPressed('W'))
		{
			this->m_gfx.m_camera.AdjustPosition(this->m_gfx.m_camera.GetForwardVector() * camera_speed * delta_time);
		}
		if (m_keyboard.KeyIsPressed('S'))
		{
			this->m_gfx.m_camera.AdjustPosition(this->m_gfx.m_camera.GetBackwardVector() * camera_speed * delta_time);
		}
		if (m_keyboard.KeyIsPressed('A'))
		{
			this->m_gfx.m_camera.AdjustPosition(this->m_gfx.m_camera.GetLeftVector() * camera_speed * delta_time);
		}
		if (m_keyboard.KeyIsPressed('D'))
		{
			this->m_gfx.m_camera.AdjustPosition(this->m_gfx.m_camera.GetRightVector() * camera_speed * delta_time);
		}
		if (m_keyboard.KeyIsPressed(VK_SPACE))
		{
			this->m_gfx.m_camera.AdjustPosition(0.0f, camera_speed * delta_time, 0.0f);
		}
		if (m_keyboard.KeyIsPressed('Z'))
		{
			this->m_gfx.m_camera.AdjustPosition(0.0f, -camera_speed * delta_time, 0.0f);
		}
		if (m_keyboard.KeyIsPressed('C'))
		{
			XMVECTOR lightPosition = this->m_gfx.m_camera.GetPositionVector();
			lightPosition += this->m_gfx.m_camera.GetForwardVector();
		}
	}
}

void Engine::RenderFrame()
{
	m_gfx.RenderFrame();
}