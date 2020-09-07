#define VERTEX_SHADER_TEXT "\
#version 460 core\n\
layout( location = 0 ) in vec2 vertexPositions;\n\
layout( location = 1 ) in vec4 vertexColor;\n\
\n\
out float v_ratio;\n\
out float v_width;\n\
out vec4 v_vertexColor;\n\
//\n\
void main()\n\
{\n\
	gl_Position = vec4( vertexPositions, 0.0, 1.0 );\n\
	\n\
	v_width = vertexColor.w;\n\
	v_vertexColor = vec4(vertexColor.xyz, 1.0);\n\
}\n\
"
#define GEOM_SHADER_TEXT "\
#version 460 core\n\
layout( lines ) in;\n\
layout( triangle_strip, max_vertices = 4 ) out;\n\
\n\
in float v_width[];\n\
in float v_ratio[];\n\
in vec4 v_vertexColor[];\n\
out vec4 g_vertexColor;\n\
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
	//ret[1] = vec4( 0.0, 2.0 / tmb, 0.0, 0.0 );\n\
	//ret[2] = vec4( 0.0, 0.0, -1.0, 0.0 );\n\
	//ret[3] = vec4( -ral / rml, -tab / tmb, 1.0, 1.0 );\n\
\n\
	return ret;\n\
}\n\
\n\
void main()\n\
{\n\
	vec2 diff = gl_in[1].gl_Position.xy - gl_in[0].gl_Position.xy;\n\
	float angle = atan(  diff.y, diff.x );\n\
	float x = sin( angle ) / 2.0f;\n\
	float y = cos( angle ) / 2.0f;\n\
\n\
	float xcn = cos( angle + 1.0471975512 );\n\
	float ycn = cos( angle + 1.0471975512 );\n\
\n\
	float xW0 = x * v_width[0];\n\
	float yW0 = y * v_width[0];\n\
	float xW1 = x * v_width[1];\n\
	float yW1 = y * v_width[1];\n\
\n\
	float xcn0 = xcn * v_width[0];\n\
	float ycn0 = ycn * v_width[0];\n\
	float xcn1 = xcn * v_width[1];\n\
	float ycn1 = ycn * v_width[1];\n\
\n\
	//Root Side\n\
	g_vertexColor = v_vertexColor[0];\n\
\n\
	//gl_Position = gl_in[0].gl_Position + vec4( -xcn0, ycn0, 0.0, 0.0 );\n\
	//EmitVertex();\n\
\n\
	//gl_Position = gl_in[0].gl_Position + vec4( xcn0, -ycn0, 0.0, 0.0 );\n\
	//EmitVertex();\n\
\n\
	gl_Position = gl_in[0].gl_Position + vec4( xW0, -yW0, 0.0, 0.0 );\n\
	gl_Position = getProjMatrix() * gl_Position;\n\
	EmitVertex();\n\
\n\
	gl_Position = gl_in[0].gl_Position + vec4( -xW0, yW0, 0.0, 0.0 );\n\
	gl_Position = getProjMatrix() * gl_Position;\n\
	EmitVertex();\n\
\n\
	//Leaf Side\n\
	g_vertexColor = v_vertexColor[1];\n\
\n\
	gl_Position = gl_in[1].gl_Position + vec4( xW1, -yW1, 0.0, 0.0 );\n\
	gl_Position = getProjMatrix() * gl_Position;\n\
	EmitVertex();\n\
\n\
	gl_Position = gl_in[1].gl_Position + vec4( -xW1, yW1, 0.0, 0.0 );\n\
	gl_Position = getProjMatrix() * gl_Position;\n\
	EmitVertex();\n\
\n\
	EndPrimitive();\n\
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
