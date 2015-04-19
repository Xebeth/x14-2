/**************************************************************************
	created		:	2009-06-28
	filename	: 	Timer.cpp
	author		:	xebeth`
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include "Timer.h"


Timer::Timer()
{
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_Frequency);
	Reset();
}

void Timer::Start()
{
	if (m_timerStopped == false)
		return;

	QueryPerformanceCounter((LARGE_INTEGER *)&m_lastTime);
	m_timerStopped = false;
}

void Timer::Stop()
{
	if (m_timerStopped)
		return;

	INT64 stopTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&stopTime);
	m_runningTime += (float)(stopTime - m_lastTime) / (float)m_Frequency;
	m_timerStopped = true;
}

void Timer::Update()
{
	if (m_timerStopped)
	{
		return;
	}

	// Get the current time
	QueryPerformanceCounter((LARGE_INTEGER *)&m_currentTime);

	m_timeElapsed = (float)(m_currentTime - m_lastTime) / (float)m_Frequency;
	m_runningTime += m_timeElapsed;
	// Update FPS
	++m_numFrames;

	if (m_currentTime - m_lastFPSUpdate >= m_FPSUpdateInterval)
	{
		m_fps = (float)(m_numFrames * m_Frequency) / (m_currentTime - m_lastFPSUpdate);
		m_lastFPSUpdate = m_currentTime;
		m_numFrames = 0;
	}

	if (m_TickInterval > 0 && m_currentTime - m_LastTick >= m_TickInterval)
	{
		m_LastTick = m_currentTime;
		m_bTicked = true;
	}
	else
		m_bTicked = false;

	m_lastTime = m_currentTime;
}

void Timer::Reset()
{
	m_currentTime = m_lastTime = m_lastFPSUpdate = 0;
	m_LastTick = m_TickInterval = 0;
	m_numFrames = 0;
	m_runningTime = m_timeElapsed = m_fps = 0.0f;
	m_FPSUpdateInterval = m_Frequency >> 1;
	m_timerStopped = true;
	m_bTicked = false;
}

void Timer::SetTickInterval(float Interval_in)
{
	if (Interval_in < 1.f)
		m_TickInterval = m_Frequency / (INT64)(1.f / Interval_in);
	else
		m_TickInterval = m_Frequency * (INT64)Interval_in;
}

const string_t& Timer::RunningTime(string_t &time_out) const
{
	unsigned long time = (unsigned long)m_runningTime;
	unsigned long seconds = time % 60;
	unsigned long minutes = (time / 60) % 60;
	unsigned long hours = (time / 3600);

	format(time_out, _T("%02d:%02d:%02d"), hours, minutes, seconds);

	return time_out;
}