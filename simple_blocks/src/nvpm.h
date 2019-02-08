
#ifndef nvpm_h__
#define nvpm_h__

#include <bd/log/logger.h>

#include <iomanip>
#include <iostream>
#include <vector>

#ifdef SUBVOL_PROF

//TODO: move PATH_TO_NVPMAPI_CORE define as a compiler command line definition.
//#ifndef PATH_TO_NVPMAPI_CORE
#define PATH_TO_NVPMAPI_CORE L"D:\\libs\\perfkit\\4.4.0-windows-desktop\\bin\\x64\\NvPmApi.Core.dll"
//#endif
//#define PATH_TO_NVPMAPI_CORE L"C:\\libs\\perfkit\\PerfKit-4.4.0\\bin\\x64\\NvPmApi.Core.dll"

#ifndef NVPM_INIGUID
#define NVPM_INITGUID 1
#endif

#include "NvPmApi.h"

struct Mode;

/// \brief Convert NvPmApi result code to string.
const char* nvpm_resultToString(NVPMRESULT result);
/// \brief Get the singleton nvpmapi manager instance.
//extern NvPmApiManager *GetNvPmApiManager();
/// \brief Get the singleton nvpmapi instance.
const NvPmApi *GetNvPmApi();
/// \brief initialize an nvpmapi instance.
bool nvpm_init();
/// \brief shut'er down!
bool nvpm_shutdown();
/// \brief Mark the begining of a frame for profiling.
void nvpm_experimentFrameBegin(Mode*);
/// \brief Mark the end of a frame being profiled.
void nvpm_experimentFrameEnd(Mode*);
/// \brief Mark a draw call within a frame for profiling.
void nvpm_experimentWorkBegin(Mode*);
/// \brief Mark the end of the draw call being profilied.
void nvpm_experimentWorkEnd(Mode*);
/// \brief Reset the given mode to some initial values.
void nvpm_resetMode(Mode*);
/// \brief Initialize given mode using the NVPMContext. If \c quitAfterProfiles is true,
///  then GLFW is told to close the render window after profiling is completed.
void nvpm_initMode(Mode*, NVPMContext, bool quitAfterProfiles);
/// \brief Returns true if the frames sampled is equal to required frames sampled.
bool nvpm_isdone(Mode*);
/// \brief Output string representation of the counters for the context in the given \c Mode.
void nvpm_printCounters(std::ostream&, Mode*);


///////////////////////////////////////////////////////////////////////////////
//   NVPM Experiment Mode Declarations
///////////////////////////////////////////////////////////////////////////////




/// State information passed around between nvpm_* methods.
struct Mode
{
    /// The nvpm context under which the experiment is performed.
    NVPMContext perfCtx;
    /// The total number of passes required to complete collection of
    /// every counter in counterNames the entire experiment.
    NVPMUINT  nTotalPasses;
    /// Current pass.
    NVPMUINT nPass;
    /// Frame count since program start.
    NVPMUINT nFrame;
    /// i don't know what this does, but its probably important 
    // TODO: DON'T DELETE THIS!
    int objectId;
    /// true if the experiment is started, false otherwise.
    bool isCollecting;
    /// string names of the counters.
    const char** counterNames;
    /// number of counters in counterNames
    size_t counterCount;
    /// true if the window should close on profile completion.
    bool exitOnDone;
};

struct NvPmGlobals
{
    /// The frame number to start the experiment on (wait until Mode::nFrame==framestart)
    NVPMUINT framestart;
    /// Yup, its the mode for this experiment!
    Mode mode;
};


// Defined in nvpm.cpp
extern std::vector<const char *> g_experimentModeCounters;
//extern NvPmApiManager S_NVPMManager;
extern NVPMContext g_nvpmContext;
extern NvPmGlobals g_nvpmGlobals;

///////////////////////////////////////////////////////////////////////////////
//   defines
///////////////////////////////////////////////////////////////////////////////


#define perf_initNvPm()                               \
    do {                                                    \
        if (! nvpm_init()) {                          \
            bd::Err() << "Could not init nvperf api!";       \
            cleanup();                                      \
            exit(1);                                        \
        }                                                   \
    } while(0)


#define perf_initMode(_bool_perfMode) nvpm_initMode(&g_nvpmGlobals.mode, g_nvpmContext, (_bool_perfMode));
#define perf_frameBegin() nvpm_experimentFrameBegin(&g_nvpmGlobals.mode);
#define perf_frameEnd() nvpm_experimentFrameEnd(&g_nvpmGlobals.mode);
#define perf_workBegin() nvpm_experimentWorkBegin(&g_nvpmGlobals.mode);
#define perf_workEnd() nvpm_experimentWorkEnd(&g_nvpmGlobals.mode);
#define perf_shutdown() nvpm_shutdown();
#define perf_printCounters(_outstream) nvpm_printCounters((_outstream), &g_nvpmGlobals.mode);
#define perf_isDone() nvpm_isdone(&g_nvpmGlobals.mode);




#else // BDPROF

#define perf_initNvPm()
#define perf_initMode(_dummy)
#define perf_frameBegin()
#define perf_frameEnd()
#define perf_workBegin()
#define perf_workEnd()
#define perf_shouldQuit()
#define perf_isDone()
#define perf_printCounters(_dummy)

#endif // BDPROF
#endif // !nvpm_h__

