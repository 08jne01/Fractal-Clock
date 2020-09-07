#include "Drawable.h"

#include "glad/glad.h"
#include "glfw3.h"
Drawable::Drawable()
{

}

Drawable::~Drawable()
{
	destroyBuffers();
}

void Drawable::createBuffers()
{
	glGenVertexArrays( 1, &m_VAO );
	glGenBuffers( 1, &m_VBO );
	glGenBuffers( 1, &m_EBO );
}

void Drawable::destroyBuffers()
{
	glDeleteBuffers( 1, &m_VAO );
	glDeleteBuffers( 1, &m_VBO );
	glDeleteBuffers( 1, &m_EBO );
}