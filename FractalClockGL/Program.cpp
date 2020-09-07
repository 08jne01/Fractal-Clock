#include "Program.h"
#include "Shader.h"
#include "glad/glad.h"
#include "glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "InternalShader.h"
Program::Program
(
	const short width,
	const short height,
	const bool loadShadersFromFile
) :
	m_width(width),
	m_height(height),
	m_fractal(&m_clock, width, height, m_window),
	m_loadShadersFromFile(loadShadersFromFile)
{

}

Program::~Program()
{
	glfwTerminate();
}

int Program::mainLoop()
{
	//sf::ContextSettings settings;
	//settings.antialiasingLevel = 8;
	//m_window.create( sf::VideoMode( m_width, m_height ), "Fractal Clock", sf::Style::Default, settings);
	if ( ! glfwInit() )
	{
		fprintf( stderr, "GLFW did not init.\n" );
		return EXIT_FAILURE;
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint( GLFW_SAMPLES, 8 );

	m_window = glfwCreateWindow( m_width, m_height, "Fractal Clock", NULL, NULL );

	if ( ! m_window )
	{
		fprintf( stderr, "Failed to create window.\n" );
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent( m_window );

	if ( ! gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
	{
		fprintf( stderr, "Failed to init GLAD.\n" );
		return EXIT_FAILURE;
	}


	m_fractal.generateClock( m_levels , m_size, false );
	m_fractal.launchThreads();

	glViewport( 0, 0, m_width, m_height );

	glfwSetFramebufferSizeCallback( m_window, frameBufferSizeCallback );
	glfwSetKeyCallback( m_window, keyCallback );
	//glfwSetCursorPosCallback( m_window, cursorPositionCallback );
	//glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetWindowUserPointer( m_window, this );
	glfwSwapInterval( 1 );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable( GL_MULTISAMPLE );

	//Shader Code here
	unsigned int vertexShader = 0;
	unsigned int fragmentShader = 0;
	unsigned int geometryShader = 0;
	unsigned int shaderProgram;

	int shaderLoaded = 1;

	if ( m_loadShadersFromFile )
	{

		shaderLoaded &= loadShader( "Resources/Shaders/Vertex.shader", GL_VERTEX_SHADER, &vertexShader );
		shaderLoaded &= loadShader( "Resources/Shaders/Geom.shader", GL_GEOMETRY_SHADER, &geometryShader );
		shaderLoaded &= loadShader( "Resources/Shaders/Frag.shader", GL_FRAGMENT_SHADER, &fragmentShader );
		
	}
	else
	{
		shaderLoaded &= loadShaderFromConstChar( VERTEX_SHADER_TEXT, GL_VERTEX_SHADER, &vertexShader );
		shaderLoaded &= loadShaderFromConstChar( GEOM_SHADER_TEXT, GL_GEOMETRY_SHADER, &geometryShader );
		shaderLoaded &= loadShaderFromConstChar( FRAG_SHADER_TEXT, GL_FRAGMENT_SHADER, &fragmentShader );
	}

	if ( shaderLoaded )
	{
		shaderProgram = glCreateProgram();
		glAttachShader( shaderProgram, vertexShader );
		glAttachShader( shaderProgram, fragmentShader );
		glAttachShader( shaderProgram, geometryShader );
		glLinkProgram( shaderProgram );

		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );
		glDeleteShader( geometryShader );
	}
	else
	{
		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );
		glDeleteShader( geometryShader );
		fprintf( stderr, "Failed to load shaders.\n" );
		return EXIT_FAILURE;
	}
	
	m_fractal.createBuffers();

	glm::mat4 projectionMat = glm::ortho( -m_width / 2, m_width / 2, -m_height / 2, m_height / 2, -1, 1 );

	//glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(0,0,0));
	//glm::mat4 view = glm::translate(glm::mat4(1.0), glm::vec3(0,0,-1));

	//projectionMat = projectionMat * view * model;
	//projectionMat = glm::mat4( 1.0 );
	//Mainloop
	while ( ! glfwWindowShouldClose( m_window ) )
	{
		//int proj = glGetUniformLocation( shaderProgram, "proj" );
		//glUniformMatrix4fv( proj, 1, GL_FALSE, &projectionMat[0][0] );

		int width = glGetUniformLocation( shaderProgram, "width" );
		glUniform1f( width, m_width );

		int height = glGetUniformLocation( shaderProgram, "height" );
		glUniform1f( height, m_height );


		update();
		draw( shaderProgram );

		glfwPollEvents();
	}



	return 0;
}

//void Program::handleEvent( const sf::Event& e )
//{
//	switch ( e.type )
//	{
//	case sf::Event::EventType::Closed:
//		m_window.close();
//		break;
//	case sf::Event::EventType::KeyPressed:
//		handleKeyboardEvent( e );
//		break;
//	}
//}

void Program::handleKeyboardEvent( int button, int scancode, int action, int mods )
{
	if ( action == GLFW_PRESS )
	{
		switch ( button )
		{
		case GLFW_KEY_UP:
			m_fractal.generateClock( ++m_levels, m_size, true );
			break;

		case GLFW_KEY_DOWN:
			m_fractal.generateClock( --m_levels, m_size, true );
			break;

		case GLFW_KEY_LEFT:
			m_size -= 0.1;
			m_fractal.resizeClock( m_size );
			break;

		case GLFW_KEY_RIGHT:
			m_size += 0.1;
			m_fractal.resizeClock( m_size );
			break;
		case GLFW_KEY_PAGE_UP:
			m_lineWidth += 0.001;
			m_fractal.setLineWidth( m_lineWidth );
			break;
		case GLFW_KEY_PAGE_DOWN:
			m_lineWidth -= 0.001;
			m_fractal.setLineWidth( m_lineWidth );
			break;
		}
		
	}
}

void Program::update()
{
	m_clock.update();
	//m_fractal.update();
	m_fractal.updateThreaded();
}

void Program::draw( unsigned int shaderProgram )
{
	glClear( GL_COLOR_BUFFER_BIT );

	//Draw here
	m_fractal.updateBuffers();
	m_fractal.draw( shaderProgram );
	//

	glfwSwapBuffers( m_window );
}

void Program::setDimensions( int width, int height )
{
	m_width = width;
	m_height = height;
}

void Program::frameBufferSizeCallback( GLFWwindow* window, int width, int height )
{
	Program* thisPtr = (Program*)glfwGetWindowUserPointer( window );
	glViewport( 0, 0, width, height );
	thisPtr->setDimensions( width, height );
}

void Program::keyCallback( GLFWwindow* window, int button, int scancode, int action, int mods )
{
	Program* thisPtr = (Program*)glfwGetWindowUserPointer( window );
	thisPtr->handleKeyboardEvent( button, scancode, action, mods );
}

void Program::cursorPositionCallback( GLFWwindow* window, double xpos, double ypos )
{
	Program* thisPtr = (Program*)glfwGetWindowUserPointer( window );
}