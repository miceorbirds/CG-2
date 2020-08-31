#include "timer.h"

Timer::Timer()
{
	start = std::chrono::high_resolution_clock::now();
	stop = std::chrono::high_resolution_clock::now();
}

double Timer::GetMilisecondsElapsed()
{
	if (m_is_running)
	{
		auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start);
		return elapsed.count();
	}
	else
	{
		auto elapsed = std::chrono::duration<double, std::milli>(stop - start);
		return elapsed.count();
	}
}

void Timer::Restart()
{
	m_is_running = true;
	start = std::chrono::high_resolution_clock::now();
}

bool Timer::Stop()
{
	if (!m_is_running)
		return false;
	else
	{
		stop = std::chrono::high_resolution_clock::now();
		m_is_running = false;
		return true;
	}
}

bool Timer::Start()
{
	if (m_is_running)
	{
		return false;
	}
	else
	{
		start = std::chrono::high_resolution_clock::now();
		m_is_running = true;
		return true;
	}
}