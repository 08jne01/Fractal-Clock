#include "glad/glad.h"
#include "glfw3.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

static char* readSource( const char* shaderFile );
static int compile( const char* code, unsigned id );

int loadShaderFromConstChar( const char* text, const unsigned int shaderType, unsigned int* id )
{
	*id = glCreateShader( shaderType );

	int success = 0;
	if ( text )
	{
		success = compile( text, *id );
	}

	return success;
}

int loadShader( const char* shaderFile, const unsigned int shaderType, unsigned int* id )
{
	*id = glCreateShader( shaderType );

	char* code = readSource( shaderFile );

	int success = 0;

	if ( code )
	{
		success = compile( code, *id );
		free( code );
	}
		
	return success;
}

char* readSource( const char* shaderFile )
{
	FILE* file;
	fopen_s(&file, shaderFile, "rb" );

	char* str = NULL;

	if ( file )
	{
		fseek( file, 0, SEEK_END );
		size_t size = ftell( file );
		fseek( file, 0, SEEK_SET );

		str = (char*)malloc( size + 1 );

		fread_s( str, size, 1, size, file );
		str[size] = 0;
		fclose( file );
	}
	else
	{
		fprintf( stderr, "Failed to open shader file at \"%s\".\n", shaderFile );
	}

	return str;
}

int compile( const char* code, unsigned int id )
{
	glShaderSource( id, 1, &code, NULL );
	glCompileShader( id );

	int success;
	glGetShaderiv( id, GL_COMPILE_STATUS, &success );
	if ( ! success )
	{
		char infoLog[512];
		glGetShaderInfoLog( id, 512, NULL, infoLog );
		fprintf( stderr, "ERROR SHADER %d COMPILATION FAILED\n%s", id, infoLog );
	}
	return success;
}