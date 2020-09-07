#include "Maths/Maths.h"
typedef struct vertex_s
{
	vertex_s(){}
	vertex_s
	(
		const Vec2f& position_,
		float r_,
		float g_,
		float b_,
		float a_
	):
		position(position_),
		r(r_),
		g(g_),
		b(b_),
		a(a_)
	{}

	Vec2f position;
	float r;
	float g;
	float b;
	float a;
} vertex_t;

typedef struct clock_vertex_s
{
	clock_vertex_s() {}
	clock_vertex_s
	(
		float x,
		float y,
		float rotation,
		float size,
		float r,
		float g,
		float b,
		float width
	) :
		m_x( x ),
		m_y( y ),
		m_rotation( rotation ),
		m_size( size ),
		m_r( r ),
		m_g( g ),
		m_b( b ),
		m_lineWidth( width )
	{

	}

	float m_x;
	float m_y;
	float m_size;
	float m_rotation;
	float m_r;
	float m_g;
	float m_b;
	float m_lineWidth;
} clock_vertex_t;