#include "Clock.h"
#include <iostream>

#define TWO_PI_OVER_60 0.10471975512
#define TWO_PI_OVER_12 0.52359877559

Clock::Clock()
{
	SystemClock::time_point time = SystemClock::now();
	time_t tt = SystemClock::to_time_t( time );
	m_time = *localtime( &tt );
	m_precision = PrecisionClock::now();
}


void Clock::update()
{
	SystemClock::time_point time = SystemClock::now();
	time_t tt = SystemClock::to_time_t( time );

	tm current = *localtime( &tt );
	if ( current.tm_sec != m_time.tm_sec )
	{
		m_precision = PrecisionClock::now();
		m_time = current;
	}

	m_subSecond = (float)(PrecisionClock::now() - m_precision).count() / 1.0e9;
	//std::cout << current.tm_hour << ":" << current.tm_min << ":" << current.tm_sec << "." << m_subSecond << std::endl;

	m_secondHand = TWO_PI_OVER_60*((float)current.tm_sec + m_subSecond);
	m_minuteHand = TWO_PI_OVER_60*(float)current.tm_min + m_secondHand / 60.0f;
	m_hourHand = TWO_PI_OVER_12*(float)(current.tm_hour % 12) + m_minuteHand / 12.0f;

	//printf( "HOUR: %lf, MINUTE: %lf, SECOND: %lf\n", m_hourHand, m_minuteHand, m_secondHand );
}