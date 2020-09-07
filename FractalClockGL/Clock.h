#pragma once
#include <chrono>
typedef std::chrono::high_resolution_clock PrecisionClock;
typedef std::chrono::system_clock SystemClock;



class Clock
{
public:
	Clock();
	void update();

	inline float hour()
	{
		return m_hourHand;
	}
	inline float minute()
	{
		return m_minuteHand;
	}
	inline float second()
	{
		return m_secondHand;
	}
private:
	tm m_time;
	float m_subSecond;
	PrecisionClock::time_point m_precision;

	float m_hourHand;
	float m_minuteHand;
	float m_secondHand;

};

