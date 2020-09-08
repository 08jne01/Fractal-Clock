#define VERTEX_SHADER_TEXT "\
#version 460 core\n\
//Input Buffer\n\
layout( location = 0 ) in vec3 vertexPositions;\n\
layout( location = 1 ) in vec3 clockSettings;\n\
//clockSettings\n\
//x: size (length of the hands second hand )\n\
//y: frac (level of the clock + 1) / (maximum number of levels + 1) this is 0 -> 1\n\
// Levels increase as clock gets smaller.\n\
//z: line width\n\
\n\
//sin(x)\n\
//cos(x)\n\
//pow(x,y) -> x^y\n\
\n\
\n\
#define PI 3.14159265358979\n\
\n\
//Output\n\
out float v_size;\n\
out float v_width;\n\
out float v_rotation;\n\
out vec4 v_vertexColor;\n\
\n\
//Input time\n\
uniform vec3 timeAngle; //hour: x, minute: y, seconds: z\n\
\n\
vec4 getVertexColor()\n\
{\n\
	vec4 color; //r, g, b, alpha\n\
\n\
	//Example 1\n\
	color.x = clockSettings.y * pow( cos( -3.0 * timeAngle.y + PI * 0.75 ), 2 );\n\
	color.y = clockSettings.y * pow( sin( timeAngle.y + timeAngle.x - clockSettings.y * PI ), 2 );\n\
	color.z = pow( sin( 1.0 * timeAngle.y - PI ), 2 );\n\
	color.w = pow( 1.0 - clockSettings.y, 0.5 );\n\
\n\
	//make sure all components of color\n\
	//stay between 0 -> 1\n\
	//0 is no color\n\
	//1 is max color\n\
\n\
	//Example 2\n\
	//color.x = timeAngle.z;\n\
	//color.y = 0.0;\n\
	//color.z = 0.0;\n\
	//color.w = 1.0;\n\
\n\
	return color;\n\
}\n\
\n\
void main()\n\
{\n\
	gl_Position = vec4( vertexPositions.xy, 0.0, 1.0 );\n\
	\n\
	v_size = clockSettings.x;\n\
	v_rotation = vertexPositions.z;\n\
\n\
	v_width = clockSettings.z;\n\
	v_vertexColor = getVertexColor();\n\
}\n\
"
#define GEOM_SHADER_TEXT "\
#version 460 core\n\
layout( points ) in;\n\
layout( triangle_strip, max_vertices = 12 ) out;\n\
//triangle_strip 12\n\
in float v_size[];\n\
in float v_width[];\n\
in float v_rotation[];\n\
in vec4 v_vertexColor[];\n\
out vec4 g_vertexColor;\n\
\n\
uniform vec3 timeAngle; //x: hour y: minute z: second\n\
\n\
\n\
uniform float width;\n\
uniform float height;\n\
\n\
mat4 getProjMatrix()\n\
{\n\
	float left = -0.5 * width;\n\
	float right = 0.5 * width;\n\
\n\
	float top = -0.5 * height;\n\
	float bottom = 0.5 * height;\n\
\n\
	float rml = right - left;\n\
	float ral = right + left;\n\
	float tmb = top - bottom;\n\
	float tab = top + bottom;\n\
\n\
	mat4 ret;\n\
\n\
\n\
	ret[0] = vec4( 500.0/rml, 0.0, 0.0, 0.0 );\n\
	ret[1] = vec4( 0.0, 500.0/tmb, 0.0, 0.0 );\n\
	ret[2] = vec4( 0.0, 0.0, 1.0, 0.0 );\n\
	ret[3] = vec4( 0.0, 0.0, 0.0, 1.0 );\n\
\n\
	return ret;\n\
}\n\
\n\
vec2 rotate( vec2 v, float a )\n\
{\n\
	float s = sin( a );\n\
	float c = cos( a );\n\
	mat2 m = mat2( c, s, -s, c );\n\
	return m * v;\n\
}\n\
\n\
void emitRectangleFromLine( vec2 v )\n\
{\n\
	vec2 diff = v - gl_in[0].gl_Position.xy;\n\
	float angle = atan(  diff.y, diff.x );\n\
	float x = sin( angle ) / 2.0f;\n\
	float y = cos( angle ) / 2.0f;\n\
\n\
	float xW = x * v_width[0];\n\
	float yW = y * v_width[0];\n\
\n\
	g_vertexColor = v_vertexColor[0];\n\
\n\
	//Root Side\n\
\n\
	gl_Position = gl_in[0].gl_Position + vec4( xW, -yW, 0.0, 0.0 );\n\
	gl_Position = getProjMatrix() * gl_Position;\n\
	EmitVertex();\n\
\n\
	gl_Position = gl_in[0].gl_Position + vec4( -xW, yW, 0.0, 0.0 );\n\
	gl_Position = getProjMatrix() * gl_Position;\n\
	EmitVertex();\n\
\n\
	//Leaf Side\n\
\n\
	gl_Position = vec4( v, gl_in[0].gl_Position.zw ) + vec4( xW, -yW, 0.0, 0.0 );\n\
	gl_Position = getProjMatrix() * gl_Position;\n\
	EmitVertex();\n\
\n\
	gl_Position = vec4( v, gl_in[0].gl_Position.zw ) + vec4( -xW, yW, 0.0, 0.0 );\n\
	gl_Position = getProjMatrix() * gl_Position;\n\
	EmitVertex();\n\
\n\
	EndPrimitive();\n\
}\n\
\n\
void main()\n\
{\n\
	vec3 finalRotations = timeAngle + v_rotation[0];\n\
\n\
	vec2 hour = vec2( 0, -v_size[0]*0.4 );\n\
	vec2 minute = vec2( 0, -v_size[0]*0.8 );\n\
	vec2 second = vec2( 0, -v_size[0] );\n\
\n\
	hour = rotate( hour, finalRotations.x );\n\
	minute = rotate( minute, finalRotations.y );\n\
	second = rotate( second, finalRotations.z );\n\
\n\
	hour += gl_in[0].gl_Position.xy;\n\
	minute += gl_in[0].gl_Position.xy;\n\
	second += gl_in[0].gl_Position.xy;\n\
\n\
	emitRectangleFromLine( hour );\n\
	emitRectangleFromLine( minute );\n\
	emitRectangleFromLine( second );\n\
}\n\
"
#define FRAG_SHADER_TEXT "\
#version 460 core\n\
layout( location = 0 ) out vec4 FragColor;\n\
\n\
in vec4 g_vertexColor;\n\
\n\
void main()\n\
{\n\
	FragColor = g_vertexColor;\n\
}\n\
"
