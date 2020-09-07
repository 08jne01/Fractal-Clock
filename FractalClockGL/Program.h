#pragma once
#include "Clock.h"
#include "FractalClock.h"

struct GLFWwindow;

class Program
{
public:
	Program( const short width, const short height, const bool loadShadersFromFile );
	~Program();
	int mainLoop();
	void update();
	void draw( unsigned int shaderProgram );
	void setDimensions( int width, int height );
	//void handleEvent( const sf::Event& e );
	void handleKeyboardEvent( int button, int scancode, int action, int mods );
private:

	static void frameBufferSizeCallback( GLFWwindow* window, int width, int height );
	static void keyCallback( GLFWwindow* window, int button, int scancode, int action, int mods);
	static void cursorPositionCallback( GLFWwindow* window, double xpos, double ypos);

	const bool m_loadShadersFromFile;
	short m_width;
	short m_height;
	GLFWwindow* m_window;
	Clock m_clock;
	FractalClock m_fractal;
	int m_levels = 0;
	float m_size = 1.0;
	float m_lineWidth = 0.01;
};