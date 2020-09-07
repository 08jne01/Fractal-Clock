#pragma once
#include "Maths/Maths.h"
#include <stdlib.h>
#include "Clock.h"

class SubClock
{
public:


	SubClock( int level, float size, SubClock* parent, const Vec2f& position, Clock* realClock );
	void update();

	inline void setRotation(float rotation)
	{
		m_rotation = rotation;
	}

	inline float rotation() const
	{
		return m_rotation;
	}

	inline const Vec2f& position() const
	{
		return m_position;
	}

	inline const Vec2f& hourPosition() const
	{
		return m_hourPosition;
	}

	inline const Vec2f& minutePosition() const
	{
		return m_minutePosition;
	}

	inline const Vec2f& secondPosition() const
	{
		return m_secondPosition;
	}

	inline SubClock* childHour() const
	{
		return m_hour;
	}

	inline SubClock* childMinute() const
	{
		return m_minute;
	}

	inline SubClock* childSecond() const
	{
		return m_second;
	}

	inline void setChildHour(SubClock* hour)
	{
		m_hour = hour;
	}

	inline void setChildMinute(SubClock* minute)
	{
		m_minute = minute;
	}

	inline void setChildSecond(SubClock* second)
	{
		m_second = second;
	}

	inline int getLevel() const
	{
		return m_level;
	}

	inline float getSize() const
	{
		return m_size;
	}

	inline void setSize( float size )
	{
		m_size = size* powf( 0.6, m_level );
	}

private:
	//Geometry about SubClock.
	const Vec2f& m_position;
	Vec2f m_hourPosition;
	Vec2f m_minutePosition;
	Vec2f m_secondPosition;

	//Config Values.
	float m_rotation = 0.0;
	float m_size;
	int m_level;

	//References to others in the tree.
	SubClock* m_parent = NULL;
	SubClock* m_hour = NULL;
	SubClock* m_minute = NULL;
	SubClock* m_second = NULL;

	//Actual time angles.
	Clock* m_realClock;
};