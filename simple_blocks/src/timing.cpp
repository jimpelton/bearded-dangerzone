#include "timing.h"

#include <bd/log/gl_log.h>

#include <cassert>

#define QUERY_BUFFERS 2
#define QUERY_COUNT 1

static GLuint queryID[QUERY_BUFFERS][QUERY_COUNT];

static unsigned int queryBackBuffer{ 0 };
static unsigned int queryFrontBuffer{ 1 };

static unsigned long long g_totalGPUTime_nonEmptyBlocks{ 0 };
static unsigned long long g_totalFramesRendered{ 0 };
static double g_totalElapsedCPUFrameTime{ 0 };

static std::chrono::high_resolution_clock::time_point g_cpuFrameStartTime{ };

void genQueries() {

  gl_check(glGenQueries(QUERY_COUNT, queryID[queryBackBuffer]));
  gl_check(glGenQueries(QUERY_COUNT, queryID[queryFrontBuffer]));
  // dummy query to prevent OpenGL errors from popping out
  //gl_check(glQueryCounter(queryID[queryFrontBuffer][0], GL_TIMESTAMP));

}


///////////////////////////////////////////////////////////////////////////////
void swapQueryBuffers() {
  if (queryBackBuffer) {
    queryBackBuffer = 0;
    queryFrontBuffer = 1;
  } else {
    queryBackBuffer = 1;
    queryFrontBuffer = 0;
  }
}


///////////////////////////////////////////////////////////////////////////////
void startGpuTimerQuery() {
  gl_check(glBeginQuery(GL_TIME_ELAPSED, queryID[queryBackBuffer][0]));
}


///////////////////////////////////////////////////////////////////////////////
void endGpuTimerQuery() {
  GLuint64 frameTime;

  gl_check(glEndQuery(GL_TIME_ELAPSED));
  gl_check(glGetQueryObjectui64v(queryID[queryFrontBuffer][0], GL_QUERY_RESULT,
                                 &frameTime));
  
  g_totalGPUTime_nonEmptyBlocks += frameTime;

  swapQueryBuffers();
}


///////////////////////////////////////////////////////////////////////////////
void startCpuTime() {
  g_cpuFrameStartTime = std::chrono::high_resolution_clock::now();
}


///////////////////////////////////////////////////////////////////////////////
void endCpuTime() {
  using namespace std::chrono;
  auto now = high_resolution_clock::now();

  g_totalElapsedCPUFrameTime +=
      duration_cast<microseconds>(now - g_cpuFrameStartTime).count();

  g_totalFramesRendered++;
}


///////////////////////////////////////////////////////////////////////////////
unsigned long long getTotalGPUTime_NonEmptyBlocks() {
  return g_totalGPUTime_nonEmptyBlocks;
}


///////////////////////////////////////////////////////////////////////////////
unsigned long long getTotalFramesRendered() {
  return g_totalFramesRendered;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Returns total CPU frametime in microseconds
///////////////////////////////////////////////////////////////////////////////
double getTotalElapsedCPUFrameTime() {
  return g_totalElapsedCPUFrameTime;
}
