#version 460 core
layout( location = 0 ) in vec4 vertexPositions;
layout( location = 1 ) in vec4 vertexColor;

out float v_size;
out float v_width;
out float v_rotation;
out vec4 v_vertexColor;

void main()
{
	gl_Position = vec4( vertexPositions.xy, 0.0, 1.0 );
	
	v_size = vertexPositions.z;
	v_rotation = vertexPositions.w;

	v_width = vertexColor.w;
	v_vertexColor = vec4(vertexColor.xyz, 1.0);
}