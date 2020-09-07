#version 460 core
layout( lines ) in;
layout( triangle_strip, max_vertices = 4 ) out;

in float v_width[];
in float v_ratio[];
in vec4 v_vertexColor[];
out vec4 g_vertexColor;

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
	//ret[1] = vec4( 0.0, 2.0 / tmb, 0.0, 0.0 );
	//ret[2] = vec4( 0.0, 0.0, -1.0, 0.0 );
	//ret[3] = vec4( -ral / rml, -tab / tmb, 1.0, 1.0 );

	return ret;
}

void main()
{
	vec2 diff = gl_in[1].gl_Position.xy - gl_in[0].gl_Position.xy;
	float angle = atan(  diff.y, diff.x );
	float x = sin( angle ) / 2.0f;
	float y = cos( angle ) / 2.0f;

	float xcn = cos( angle + 1.0471975512 );
	float ycn = cos( angle + 1.0471975512 );

	float xW0 = x * v_width[0];
	float yW0 = y * v_width[0];
	float xW1 = x * v_width[1];
	float yW1 = y * v_width[1];

	float xcn0 = xcn * v_width[0];
	float ycn0 = ycn * v_width[0];
	float xcn1 = xcn * v_width[1];
	float ycn1 = ycn * v_width[1];

	//Root Side
	g_vertexColor = v_vertexColor[0];

	//gl_Position = gl_in[0].gl_Position + vec4( -xcn0, ycn0, 0.0, 0.0 );
	//EmitVertex();

	//gl_Position = gl_in[0].gl_Position + vec4( xcn0, -ycn0, 0.0, 0.0 );
	//EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4( xW0, -yW0, 0.0, 0.0 );
	gl_Position = getProjMatrix() * gl_Position;
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4( -xW0, yW0, 0.0, 0.0 );
	gl_Position = getProjMatrix() * gl_Position;
	EmitVertex();

	//Leaf Side
	g_vertexColor = v_vertexColor[1];

	gl_Position = gl_in[1].gl_Position + vec4( xW1, -yW1, 0.0, 0.0 );
	gl_Position = getProjMatrix() * gl_Position;
	EmitVertex();

	gl_Position = gl_in[1].gl_Position + vec4( -xW1, yW1, 0.0, 0.0 );
	gl_Position = getProjMatrix() * gl_Position;
	EmitVertex();

	EndPrimitive();
}