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

	delete[] m_indicesArray;
	m_indicesArray = NULL;

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
	if ( m_ready != m_threads.size() && threaded )
	{
		std::unique_lock<std::mutex> lock( m_blocker );
		m_cvMainThread.wait( lock, [&] {return tasksFinished(); } );
	}

	printf( "Generate Clock Level: %d\n", level );
	if ( m_root )
		clearClock();

	
	m_mtx.lock();

	m_level = level;
	m_root = new SubClock( 0, size, ROOT, NULL, m_position, m_clock );
	m_clocks.push_back( m_root );

	m_stack.clear();
	m_stack.push_back( m_root );
	while ( ! m_stack.empty() )
	{
		SubClock* clock = m_stack.back();
		m_stack.pop_back();

		if ( clock->getLevel() < level )
		{
			

			clock->setChildHour( new SubClock( clock->getLevel() + 1, size, HOUR, clock, clock->hourPosition(), m_clock ) );
			clock->setChildMinute( new SubClock( clock->getLevel() + 1, size, MINUTE, clock, clock->minutePosition(), m_clock ) );
			clock->setChildSecond( new SubClock( clock->getLevel() + 1, size, SECOND, clock, clock->secondPosition(), m_clock ) );

			m_stack.push_back( clock->childHour() );
			m_clocks.push_back( m_stack.back() );
			m_stack.push_back( clock->childMinute() );
			m_clocks.push_back( m_stack.back() );
			m_stack.push_back( clock->childSecond() );
			m_clocks.push_back( m_stack.back() );
		}
	}

	delete[] m_indicesArray;
	delete[] m_verticesArray;

	m_indicesArray = (unsigned int*)malloc( sizeof( unsigned int ) * m_clocks.size() * 6 );
	m_verticesArray = (vertex_t*)malloc( sizeof( vertex_t ) * m_clocks.size() * 4 );

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
		m_cvMainThread.wait( lock, [&] {return tasksFinished(); } );
	}
}

void FractalClock::updateBuffers()
{
	m_indices.resize( 0 );
	m_vertices.resize( 0 );
	m_vertices.reserve( m_clocks.size() * 4 );
	m_indices.reserve( m_clocks.size() * 6 );
	int numThreads = m_threads.size();

	int size = m_clocks.size();
	while ( size % numThreads == 0 )
	{
		size--;
	}

	m_threadVertices.resize( 0 );
	m_threadIndices.resize( 0 );
	for ( int i = 0; i < numThreads; i++ )
	{
		m_threadVertices.emplace_back();
		m_threadIndices.emplace_back();
	}


	for ( int i = 0; i < numThreads; i++ )
	{
		int start = (size * i) / numThreads;
		int end = (size * (i + 1)) / numThreads ;
		std::vector<vertex_t>* vert = &m_threadVertices[i];
		std::vector<unsigned int>* ind = &m_threadIndices[i];
		m_bufferDataStack.emplace_back(vert, ind, start, end);
	}

	m_bufferDataStack.back().end = m_clocks.size();
	
	std::unique_lock<std::mutex> lock( m_blocker );
	m_cv.notify_all();
	m_cvMainThread.wait( lock, [&] {return tasksFinished(); } );

	/*for ( int i = 0; i < m_threadVertices.size(); i++ )
	{
		m_vertices.insert( m_vertices.end(), m_threadVertices[i].begin(), m_threadVertices[i].end() );
		m_indices.insert( m_indices.end(), m_threadIndices[i].begin(), m_threadIndices[i].end() );
	}*/

	glBindVertexArray( VAO() );
	glBindBuffer( GL_ARRAY_BUFFER, VBO() );
	glBufferData( GL_ARRAY_BUFFER, 4*m_clocks.size() * sizeof(vertex_t), m_verticesArray, GL_STATIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO() );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6*m_clocks.size() * sizeof( unsigned int ), m_indicesArray, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( vertex_t ), (void*)0 );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( vertex_t ), (void*)offsetof(vertex_t, r) );
	glEnableVertexAttribArray( 1 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
}

void FractalClock::draw( unsigned int shaderProgram )
{
	m_mtx.lock();
	glUseProgram( shaderProgram );
	glBindVertexArray( VAO() );
	glDrawElements( GL_LINES, 6 * m_clocks.size(), GL_UNSIGNED_INT, 0 );
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
			//threadDraw( data.start, data.end, data.vertices, data.indices );
			threadDrawArray( data.start, data.end );
		}
	}
	notifyFinished(id);
}

void FractalClock::threadUpdateCallback( FractalClock* clock, int id )
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
		m_threads.push_back( std::thread( &FractalClock::threadUpdateCallback, this, i ) );
	}
}

void FractalClock::notifyStart(int id)
{
	m_mtx.lock();
	m_ready--;
	m_ready = std::max( std::min( m_ready, (int)m_threads.size() ), 0 );
	m_threadFlags[id] = false;
	m_mtx.unlock();
}

void FractalClock::notifyFinished(int id)
{
	m_mtx.lock();

	m_threadFlags[id] = true;
	m_ready++;
	m_ready = std::max( std::min( m_ready, (int)m_threads.size() ), 0 );
	m_cvMainThread.notify_all();
	m_mtx.unlock();
}

void FractalClock::threadDraw
( 
	int start,
	int end,
	std::vector<vertex_t>* vertices,
	std::vector<unsigned int>* indices
)
{
	vertices->reserve( (end - start)*4 );
	indices->reserve( (end - start)*6 );

	for ( int i = start; i < end; i++ )
	{
		float frac = ((float)m_clocks[i]->getLevel() + 1.0f) / ((float)m_level + 1.0f);
		float r = powf( cosf( -m_clock->minute() + PI ), 2 ) * frac;
		float g = frac * powf( sinf( m_clock->minute() + m_clock->hour() - frac * PI ), 2 );
		float b = powf( sinf( m_clock->minute() - PI ), 2 );

		vertices->emplace_back( m_clocks[i]->position(), r, g, b, 1.0f );
		vertices->emplace_back( m_clocks[i]->hourPosition(), r, g, b, 1.0f );
		vertices->emplace_back( m_clocks[i]->minutePosition(), r, g, b, 1.0f );
		vertices->emplace_back( m_clocks[i]->secondPosition(), r, g, b, 1.0f );

		indices->push_back( 4 * i );
		indices->push_back( 4 * i + 1 );
		indices->push_back( 4 * i );
		indices->push_back( 4 * i + 2 );
		indices->push_back( 4 * i );
		indices->push_back( 4 * i + 3 );
	}

}

void FractalClock::threadDrawArray( int start, int end )
{
	//std::cout << "start " << start << std::endl;
	for ( int i = start; i < end; i++ )
	{
		float frac = ((float)m_clocks[i]->getLevel() + 1.0f) / ((float)m_level + 1.0f);
		float r = powf( cosf( -3.0*m_clock->minute() + PI*0.75 ), 2 ) * frac;
		float g = frac * powf( sinf( m_clock->minute() + m_clock->hour() - frac * PI ), 2 );
		float b = powf( sinf( 1.0 * m_clock->minute() - PI ), 2 );

		int vertexIndex = i * 4;
		int indexIndex = i * 6;

		m_verticesArray[vertexIndex] = vertex_t( m_clocks[i]->position(), r, g, b, (m_lineWidth /frac) );
		m_verticesArray[vertexIndex + 1] = vertex_t( m_clocks[i]->hourPosition(), r, g, b, (m_lineWidth / frac) );
		m_verticesArray[vertexIndex + 2] = vertex_t( m_clocks[i]->minutePosition(), r, g, b, (m_lineWidth / frac) );
		m_verticesArray[vertexIndex + 3] = vertex_t( m_clocks[i]->secondPosition(), r, g, b, (m_lineWidth / frac) );

		m_indicesArray[indexIndex] = vertexIndex;
		m_indicesArray[indexIndex + 2] = vertexIndex;
		m_indicesArray[indexIndex + 4] = vertexIndex;

		m_indicesArray[indexIndex + 1] = vertexIndex + 1;
		m_indicesArray[indexIndex + 3] = vertexIndex + 2;
		m_indicesArray[indexIndex + 5] = vertexIndex + 3;

		assert( indexIndex + 5 < m_clocks.size() * 6 );
		assert( vertexIndex + 3 < m_clocks.size() * 4 );
	}
	//std::cout << "end " << start << std::endl;
}

void FractalClock::threadDrawCallback
(
	FractalClock* clock,
	int start,
	int end,
	std::vector<vertex_t>* vertices,
	std::vector<unsigned int>* indices 
)
{
	//clock->threadDraw( start, end, vertices, indices );
	clock->threadDrawArray( start, end );
}

bool FractalClock::tasksFinished()
{
	m_mtx.lock();/*
	bool threadsReady = true;
	for ( int i = 0; i < m_threadFlags.size(); i++ )
	{
		if ( ! m_threadFlags[i] )
		{
			threadsReady = false;
			break;
		}
	}*/

	bool taskFinished = m_subTreesStack.empty() && m_bufferDataStack.empty() && m_open;
	m_mtx.unlock();
	return taskFinished;
}

bool FractalClock::mainThreadReady()
{
	return false;
}