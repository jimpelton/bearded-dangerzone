#ifndef perf_timing_h___
#define perf_timing_h___

#include <chrono>
#include <iostream>

namespace subvol
{
namespace timing
{

void
genQueries();


void
swapQueryBuffers();


void
startGpuTimerQuery();


void
endGpuTimerQuery();


void
startCpuTime();


void
endCpuTime();


unsigned long long
getTotalGPUTime_NonEmptyBlocks();


unsigned long long
getTotalFramesRendered();


double
getTotalElapsedCPUFrameTime();


void
printTimes(std::ostream &);
} // namespace timing
} // namespace subvol
#endif // !timing_h__
