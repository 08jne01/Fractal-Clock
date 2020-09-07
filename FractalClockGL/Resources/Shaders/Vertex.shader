#version 460 core
layout( location = 0 ) in vec2 vertexPositions;
layout( location = 1 ) in vec4 vertexColor;

out float v_ratio;
out float v_width;
out vec4 v_vertexColor;
//
void main()
{
	gl_Position = vec4( vertexPositions, 0.0, 1.0 );
	
	v_width = vertexColor.w;
	v_vertexColor = vec4(vertexColor.xyz, 1.0);
}