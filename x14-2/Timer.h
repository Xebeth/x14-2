/**************************************************************************
	created		:	2009-06-28
	filename	: 	Timer.h
	author		:	xebeth`
	purpose		:	Timer class
**************************************************************************/
#ifndef __SYS_TIMER_H__
#define __SYS_TIMER_H__

class Timer
{
public:
	Timer();
	void Start();
	void Stop();
	void Update();

	bool IsStopped() { return m_timerStopped; }
	float GetFPS() { return m_fps; }
	float GetRunningTime() { return m_runningTime; }
	float GetElapsedTime() { return m_timerStopped ? 0.0f : m_timeElapsed; }
	void SetTickInterval(float Interval_in);
	void SetRawTickInterval(INT64 Interval_in) { m_TickInterval = Interval_in; }
	bool HasTicked() { return m_bTicked; }

private:
	INT64 m_ticksPerSecond;
	INT64 m_currentTime;
	INT64 m_lastTime;
	INT64 m_lastFPSUpdate;
	INT64 m_FPSUpdateInterval;
	INT64 m_LastTick;
	INT64 m_TickInterval;
	UINT  m_numFrames;
	float m_runningTime;
	float m_timeElapsed;
	float m_fps;
	bool  m_timerStopped;
	bool  m_bTicked;
};

#endif//__SYS_TIMER_H__