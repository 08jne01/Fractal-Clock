#version 460 core
layout( location = 0 ) out vec4 FragColor;

in vec4 g_vertexColor;

void main()
{
	FragColor = g_vertexColor;
}