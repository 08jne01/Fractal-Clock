#include "SubClock.h"

SubClock::SubClock
(
	int level,
	float size,
	SubClock* parent,
	const Vec2f& position,
	Clock* realClock
):
	m_level(level),
	m_parent(parent),
	m_position(position),
	m_realClock(realClock),
	m_hourPosition(-1.0),
	m_minutePosition(-1.0),
	m_secondPosition(-1.0)
{
	m_size = size*powf( 0.6, level );
}

void SubClock::update()
{
	if ( m_hour )
	{
		m_hour->setRotation( m_rotation + m_realClock->hour() );
		m_minute->setRotation( m_rotation + m_realClock->minute() );
		m_second->setRotation( m_rotation + m_realClock->second() );
	}

	m_hourPosition.x = 0.0f;
	m_hourPosition.y = -m_size * 0.4;

	m_minutePosition.x = 0.0;
	m_minutePosition.y = -m_size * 0.8;

	m_secondPosition.x = 0.0;
	m_secondPosition.y = -m_size;

	rotateInPlacef( m_hourPosition, m_rotation + m_realClock->hour() );
	m_hourPosition += m_position;

	
	rotateInPlacef( m_minutePosition, m_rotation + m_realClock->minute() );
	m_minutePosition += m_position;

	
	rotateInPlacef( m_secondPosition, m_rotation + m_realClock->second() );
	m_secondPosition += m_position;
}