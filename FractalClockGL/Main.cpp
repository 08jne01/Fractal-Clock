#include "Program.h"
#include <stdio.h>
int main(int argc, char** argv)
{
	int width = 1000;
	int height = 1000;
	bool loadShaderFromFile = false;
	
	for ( int i = 1; i < argc; i++ )
	{
		if ( argv[i][0] == '-' && argv[i][1] == '-' )
		{
			if ( strcmp( "--shaders", argv[i] ) == 0 )
			{
				loadShaderFromFile = true;
			}
		}
		else if ( i < argc-1 )
		{
			if ( ! sscanf_s( argv[i], "%d", &width ) ||
				! sscanf_s( argv[i+1], "%d", &height ) )
			{
				fprintf( stderr, "Incorrect argument format\n" );
			}
		}
	}

	Program program( width, height, loadShaderFromFile);
	return program.mainLoop();
}
