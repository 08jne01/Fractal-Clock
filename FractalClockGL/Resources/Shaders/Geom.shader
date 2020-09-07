#version 460 core
layout( points ) in;
layout( triangle_strip, max_vertices = 12 ) out;
//triangle_strip 12
in float v_size[];
in float v_width[];
in float v_rotation[];
in vec4 v_vertexColor[];
out vec4 g_vertexColor;

uniform vec3 timeAngle; //x: hour y: minute z: second


uniform float width;
uniform float height;

mat4 getProjMatrix()
{
	float left = -0.5 * width;
	float right = 0.5 * width;

	float top = -0.5 * height;
	float bottom = 0.5 * height;

	float rml = right - left;
	float ral = right + left;
	float tmb = top - bottom;
	float tab = top + bottom;

	mat4 ret;


	ret[0] = vec4( 500.0/rml, 0.0, 0.0, 0.0 );
	ret[1] = vec4( 0.0, 500.0/tmb, 0.0, 0.0 );
	ret[2] = vec4( 0.0, 0.0, 1.0, 0.0 );
	ret[3] = vec4( 0.0, 0.0, 0.0, 1.0 );

	return ret;
}

vec2 rotate( vec2 v, float a )
{
	float s = sin( a );
	float c = cos( a );
	mat2 m = mat2( c, s, -s, c );
	return m * v;
}

void emitRectangleFromLine( vec2 v )
{
	vec2 diff = v - gl_in[0].gl_Position.xy;
	float angle = atan(  diff.y, diff.x );
	float x = sin( angle ) / 2.0f;
	float y = cos( angle ) / 2.0f;

	float xW = x * v_width[0];
	float yW = y * v_width[0];

	g_vertexColor = v_vertexColor[0];

	//Root Side

	gl_Position = gl_in[0].gl_Position + vec4( xW, -yW, 0.0, 0.0 );
	gl_Position = getProjMatrix() * gl_Position;
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4( -xW, yW, 0.0, 0.0 );
	gl_Position = getProjMatrix() * gl_Position;
	EmitVertex();

	//Leaf Side

	gl_Position = vec4( v, gl_in[0].gl_Position.zw ) + vec4( xW, -yW, 0.0, 0.0 );
	gl_Position = getProjMatrix() * gl_Position;
	EmitVertex();

	gl_Position = vec4( v, gl_in[0].gl_Position.zw ) + vec4( -xW, yW, 0.0, 0.0 );
	gl_Position = getProjMatrix() * gl_Position;
	EmitVertex();

	EndPrimitive();
}

void main()
{
	vec3 finalRotations = timeAngle + v_rotation[0];

	vec2 hour = vec2( 0, -v_size[0]*0.4 );
	vec2 minute = vec2( 0, -v_size[0]*0.8 );
	vec2 second = vec2( 0, -v_size[0] );

	hour = rotate( hour, finalRotations.x );
	minute = rotate( minute, finalRotations.y );
	second = rotate( second, finalRotations.z );

	hour += gl_in[0].gl_Position.xy;
	minute += gl_in[0].gl_Position.xy;
	second += gl_in[0].gl_Position.xy;

	emitRectangleFromLine( hour );
	emitRectangleFromLine( minute );
	emitRectangleFromLine( second );
}