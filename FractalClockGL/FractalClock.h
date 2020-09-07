#pragma once
#include "Vertex.h"
#include "Drawable.h"
#include "SubClock.h"
#include <thread>
#include <mutex>
#include <vector>

struct GLFWwindow;

struct BufferData
{
	BufferData
	(
		std::vector<vertex_t>* vertices_,
		std::vector<unsigned int>* indices_,
		unsigned int start_,
		unsigned int end_
	) :
		vertices( vertices_ ),
		indices( indices_ ),
		start( start_ ),
		end( end_ )
	{

	}

	BufferData() {}

	std::vector<vertex_t>* vertices;
	std::vector<unsigned int>* indices;
	unsigned int start;
	unsigned int end;
};

class FractalClock : public Drawable
{
public:
	FractalClock(Clock* clock, const short width, const short height, GLFWwindow*& window);
	~FractalClock();
	void clearClock();
	void generateClock( int level, float size, bool threaded );
	void resizeClock( float size );
	
	void update();
	void updateThreaded();
	void updateBuffers();
	void draw( unsigned int shaderProgram );
	void updateSubTree(SubClock* clock);
	void threadMain(int id);
	void launchThreads();
	void notifyFinished(int id);
	void notifyStart(int id);
	bool tasksFinished();
	bool threadsFinished();
	bool mainThreadReady();
	static void threadUpdateCallback(FractalClock* clock, int id);
	static bool taskFinishedStatic( FractalClock* clock );
	void threadDraw( int start, int end, std::vector<vertex_t>* vertices, std::vector<unsigned int>* indices );
	void threadDrawArray( int start, int end );
	static void threadDrawCallback( FractalClock* clock, int start, int end, std::vector<vertex_t>* vertices, std::vector<unsigned int>* indices );

	void setLineWidth( float lineWidth )
	{
		m_lineWidth = lineWidth;
	}

private:
	SubClock* m_root = NULL;
	std::vector<SubClock*> m_stack;
	std::vector<SubClock*> m_clocks;
	Vec2f m_position;
	Clock* m_clock;

	float m_lineWidth = 0.01;
	int m_level;
	const short m_width;
	const short m_height;
	GLFWwindow*& m_window;

	std::vector<std::thread> m_threads;
	std::vector<SubClock*> m_subTreesStack;
	std::vector<BufferData> m_bufferDataStack;

	std::mutex m_mtx;
	std::mutex m_blocker;
	std::mutex m_mainThreadBlocker;
	std::condition_variable m_cv;
	std::condition_variable m_cvMainThread;

	unsigned int* m_indicesArray = NULL;
	vertex_t* m_verticesArray = NULL;


	std::vector<unsigned int> m_indices;
	std::vector<vertex_t> m_vertices;

	std::vector<std::vector<vertex_t>> m_threadVertices;
	std::vector<std::vector<unsigned int>> m_threadIndices;

	std::vector<bool> m_threadFlags;

	int m_ready = 0;
	bool m_open = true;
};