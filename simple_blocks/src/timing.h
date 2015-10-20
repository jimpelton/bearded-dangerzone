#ifndef perf_timing_h___
#define perf_timing_h___

#include <GL/glew.h>

#include <chrono>

void genQueries();
void swapQueryBuffers();
void startGpuTimerQuery();
void endGpuTimerQuery();

void startCpuTime(); 
void endCpuTime();

unsigned long long getTotalGPUTime_NonEmptyBlocks();
unsigned long long getTotalFramesRendered();
double getTotalElapsedCPUFrameTime();

#endif // !timing_h__
