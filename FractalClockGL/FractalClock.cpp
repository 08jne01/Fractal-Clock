#include "FractalClock.h"
#include <algorithm>
#include <assert.h>
#include "glad/glad.h"
#include "glfw3.h"

#define PI 3.14159265359

FractalClock::FractalClock
(
	Clock* clock,
	const short width,
	const short height,
	GLFWwindow*& window
):
	m_clock(clock),
	m_position(0.0,0.0),
	m_width(width),
	m_height(height),
	m_window(window)
{
	
}

FractalClock::~FractalClock()
{
	delete[] m_verticesArray;
	m_verticesArray = NULL;

	m_open = false;
	m_cv.notify_all();
	for ( int i = 0; i < m_threads.size(); i++ )
	{
		m_threads[i].join();
	}
	clearClock();
}

void FractalClock::clearClock()
{
	m_mtx.lock();
	if ( m_root )
	{
		for ( int i = 0; i < m_clocks.size(); i++ )
		{
			delete m_clocks[i];
		}
	}
	m_clocks.clear();
	m_mtx.unlock();
}

void FractalClock::generateClock( int level, float size, bool threaded )
{
	if ( ! threadsFinished() || ! tasksFinished() )
	{
		std::unique_lock<std::mutex> lock( m_blocker );
		m_cvMainThread.wait( lock, [&] {return tasksFinished() && threadsFinished(); } );
	}

	printf( "Generate Clock Level: %d\n", level );
	if ( m_root )
		clearClock();

	
	m_mtx.lock();

	m_level = level;
	m_root = new SubClock( 0, size, NULL, m_position, m_clock );
	m_clocks.push_back( m_root );

	m_stack.clear();
	m_stack.push_back( m_root );
	while ( ! m_stack.empty() )
	{
		SubClock* clock = m_stack.back();
		m_stack.pop_back();

		if ( clock->getLevel() < level )
		{
			

			clock->setChildHour( new SubClock( clock->getLevel() + 1, size, clock, clock->hourPosition(), m_clock ) );
			clock->setChildMinute( new SubClock( clock->getLevel() + 1, size, clock, clock->minutePosition(), m_clock ) );
			clock->setChildSecond( new SubClock( clock->getLevel() + 1, size, clock, clock->secondPosition(), m_clock ) );

			m_stack.push_back( clock->childHour() );
			m_clocks.push_back( m_stack.back() );
			m_stack.push_back( clock->childMinute() );
			m_clocks.push_back( m_stack.back() );
			m_stack.push_back( clock->childSecond() );
			m_clocks.push_back( m_stack.back() );
		}
	}

	delete[] m_verticesArray;
	m_verticesArray = (clock_vertex_t*)malloc( sizeof( clock_vertex_t ) * m_clocks.size());

	m_stack.clear();
	m_mtx.unlock();
}

void FractalClock::resizeClock(float size)
{
	for ( int i = 0; i < m_clocks.size(); i++ )
	{
		m_clocks[i]->setSize( size );
	}
}

void FractalClock::update()
{
	if ( m_root )
	{
		m_stack.clear();
		m_stack.push_back( m_root );
		while ( ! m_stack.empty() )
		{
			SubClock* clock = m_stack.back();
			m_stack.pop_back();

			clock->update();

			if ( clock->childHour() )
			{
				m_stack.push_back( clock->childHour() );
				m_stack.push_back( clock->childMinute() );
				m_stack.push_back( clock->childSecond() );
			}
		}

		m_stack.clear();
	}
}

void FractalClock::updateThreaded()
{
	if ( m_root )
	{
		m_stack.clear();
		m_stack.push_back( m_root );
		while ( ! m_stack.empty() )
		{
			SubClock* clock = m_stack.back();
			m_stack.pop_back();

			if ( clock->getLevel() < 2 )
			{
				clock->update();

				if ( clock->childHour() )
				{
					m_stack.push_back( clock->childHour() );
					m_stack.push_back( clock->childMinute() );
					m_stack.push_back( clock->childSecond() );
				}
			}
			else
			{
				m_mtx.lock();
				m_subTreesStack.push_back( clock );
				m_mtx.unlock();
			}
		}

		m_stack.clear();
	}

	if ( m_subTreesStack.size() )
	{
		for ( int i = 0; i < m_subTreesStack.size(); i++ )
		{
			m_cv.notify_one();
		}
		
		std::unique_lock<std::mutex> lock( m_blocker );
		m_cvMainThread.wait( lock, [&] {return threadsFinished() && tasksFinished(); } );
	}
}

void FractalClock::updateBuffers()
{
	int numThreads = m_threads.size();

	int size = m_clocks.size();
	while ( size % numThreads == 0 )
	{
		size--;
	}


	for ( int i = 0; i < numThreads; i++ )
	{
		int start = (size * i) / numThreads;
		int end = (size * (i + 1)) / numThreads ;
		m_bufferDataStack.emplace_back(start, end);
	}

	m_bufferDataStack.back().end = m_clocks.size();
	
	std::unique_lock<std::mutex> lock( m_blocker );
	m_cv.notify_all();
	m_cvMainThread.wait( lock, [&] {return threadsFinished() && tasksFinished(); } );

	glBindVertexArray( VAO() );
	glBindBuffer( GL_ARRAY_BUFFER, VBO() );
	glBufferData( GL_ARRAY_BUFFER, m_clocks.size() * sizeof(clock_vertex_t), m_verticesArray, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, sizeof( clock_vertex_t ), (void*)0 );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( clock_vertex_t ), (void*)offsetof( clock_vertex_t, m_r) );
	glEnableVertexAttribArray( 1 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
}

void FractalClock::draw( unsigned int shaderProgram )
{
	m_mtx.lock();
	glUseProgram( shaderProgram );
	glBindVertexArray( VAO() );
	glDrawArrays( GL_POINTS, 0, m_clocks.size() );
	glBindVertexArray( 0 );
	m_mtx.unlock();
}

void FractalClock::updateSubTree(SubClock* root)
{
	if ( ! root )
		return;

	std::vector<SubClock*> stack;
	stack.push_back( root );

	while ( ! stack.empty() )
	{
		SubClock* clock = stack.back();
		stack.pop_back();

		clock->update();

		if ( clock->childHour() )
		{
			stack.push_back( clock->childHour() );
			stack.push_back( clock->childMinute() );
			stack.push_back( clock->childSecond() );
		}
	}
}

void FractalClock::threadMain(int id)
{
	
	while ( m_open )
	{
		std::unique_lock<std::mutex> lock( m_blocker );
		//if ( tasksFinished() )
		//{
		notifyFinished(id);
		m_cv.wait( lock, [&] {return ! tasksFinished(); } );
			//std::cout << "Thread Start" << std::endl;
		notifyStart(id);
		//}
		


		SubClock* clock = NULL;
		bool bufferData = false;
		BufferData data;
		if ( ! m_subTreesStack.empty() )
		{
			clock = m_subTreesStack.back();
			m_subTreesStack.pop_back();
		}
		else if ( ! m_bufferDataStack.empty() )
		{
			bufferData = true;
			data = m_bufferDataStack.back();
			m_bufferDataStack.pop_back();
		}
		lock.unlock();
		lock.release();

		updateSubTree( clock ); 

		if ( bufferData )
		{
			threadDrawArray( data.start, data.end );
		}
	}
	notifyFinished(id);
}

void FractalClock::threadMainCallback( FractalClock* clock, int id )
{
	clock->threadMain(id);
}

void FractalClock::launchThreads()
{
	int numThreads = std::thread::hardware_concurrency();
	std::cout << "Number of threads " << numThreads << std::endl;
	for ( int i = 0; i < numThreads; i++ )
	{
		m_threadFlags.push_back( false );
		m_threads.push_back( std::thread( &FractalClock::threadMainCallback, this, i ) );
	}
}

void FractalClock::notifyStart(int id)
{
	m_mtx.lock();
	m_threadFlags[id] = false;
	m_mtx.unlock();
}

void FractalClock::notifyFinished(int id)
{
	m_mtx.lock();

	m_threadFlags[id] = true;
	m_cvMainThread.notify_all();
	m_mtx.unlock();
}

void FractalClock::threadDrawArray( int start, int end )
{
	for ( int i = start; i < end; i++ )
	{
		float frac = ((float)m_clocks[i]->getLevel() + 1.0f) / ((float)m_level + 1.0f);
		float r = powf( cosf( -3.0*m_clock->minute() + PI*0.75 ), 2 ) * frac;
		float g = frac * powf( sinf( m_clock->minute() + m_clock->hour() - frac * PI ), 2 );
		float b = powf( sinf( 1.0 * m_clock->minute() - PI ), 2 );

		float width = m_lineWidth / frac;
		m_verticesArray[i] = clock_vertex_t( m_clocks[i]->position().x, m_clocks[i]->position().y, m_clocks[i]->rotation(), m_clocks[i]->getSize(), r, g, b, width );
	}
}

bool FractalClock::tasksFinished()
{
	m_mtx.lock();
	bool taskFinished = m_subTreesStack.empty() && m_bufferDataStack.empty() && m_open;
	m_mtx.unlock();
	return taskFinished;
}

bool FractalClock::threadsFinished()
{
	bool threadsReady = true;
	for ( int i = 0; i < m_threadFlags.size(); i++ )
	{
		if ( ! m_threadFlags[i] )
		{
			threadsReady = false;
			break;
		}
	}

	return threadsReady;
}