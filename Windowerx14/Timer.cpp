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
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_ticksPerSecond);

	m_currentTime = m_lastTime = m_lastFPSUpdate = 0;
	m_numFrames = 0;
	m_runningTime = m_timeElapsed = m_fps = 0.0f;
	m_FPSUpdateInterval = m_ticksPerSecond >> 1;
	m_timerStopped = TRUE;
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
	m_runningTime += (float)(stopTime - m_lastTime) / (float)m_ticksPerSecond;
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

	m_timeElapsed = (float)(m_currentTime - m_lastTime) / (float)m_ticksPerSecond;
	m_runningTime += m_timeElapsed;

	// Update FPS
	++m_numFrames;
	if (m_currentTime - m_lastFPSUpdate >= m_FPSUpdateInterval)
	{
		m_fps = (float)(m_numFrames * m_ticksPerSecond) / (m_currentTime - m_lastFPSUpdate);
		m_lastFPSUpdate = m_currentTime;
		m_numFrames = 0;
	}

	m_lastTime = m_currentTime;
}