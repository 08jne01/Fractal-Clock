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
		unsigned int start_,
		unsigned int end_
	) :
		start( start_ ),
		end( end_ )
	{

	}

	BufferData() {}

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
	static void threadMainCallback(FractalClock* clock, int id);
	void threadDrawArray( int start, int end );

	inline void setLineWidth( float lineWidth )
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

	//Drawing Related Stuff
	const short m_width;
	const short m_height;
	GLFWwindow*& m_window;
	clock_vertex_t* m_verticesArray = NULL;

	//Threaded Data
	std::vector<std::thread> m_threads;
	std::vector<SubClock*> m_subTreesStack; //Stack for Update calculations.
	std::vector<BufferData> m_bufferDataStack; //Stack for Drawing Calculations.

	std::mutex m_mtx;
	std::mutex m_blocker;
	std::condition_variable m_cv;
	std::condition_variable m_cvMainThread;
	std::vector<bool> m_threadFlags;

	bool m_open = true;
};