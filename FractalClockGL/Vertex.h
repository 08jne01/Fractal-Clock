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