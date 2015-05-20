
#ifndef nvpm_h__
#define nvpm_h__

#include <bd/log/gl_log.h>

#include <iomanip>
#include <iostream>
#include <vector>

#ifdef BDPROF

//TODO: move PATH_TO_NVPMAPI_CORE define as a compiler command line definition.
#define PATH_TO_NVPMAPI_CORE L"D:\\libs\\perfkit\\4.4.0-windows-desktop\\bin\\x64\\NvPmApi.Core.dll"
//#define PATH_TO_NVPMAPI_CORE L"C:\\libs\\perfkit\\PerfKit-4.4.0\\bin\\x64\\NvPmApi.Core.dll"

#ifndef NVPM_INIGUID
    #define NVPM_INITGUID 1
#endif

#include "NvPmApi.h"
#include "NvPmApi.Manager.h"

struct Mode;

/// \brief Convert NvPmApi result code to string.
const char* nvpm_resultToString(NVPMRESULT result);
/// \brief Get the singleton nvpmapi manager instance.
extern NvPmApiManager *GetNvPmApiManager();
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


/// Simple singleton implementation for grabbing the NvPmApi
extern NvPmApiManager S_NVPMManager;
extern NVPMContext g_nvpmContext;

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

extern struct NvPmGlobals g_nvpmGlobals;

///////////////////////////////////////////////////////////////////////////////
//   defines
///////////////////////////////////////////////////////////////////////////////


#define perf_initNvPm()                               \
    do {                                                    \
        if (! nvpm_init()) {                          \
            gl_log_err("Could not init nvperf api!");       \
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


///////////////////////////////////////////////////////////////////////////////
//   Implementation
///////////////////////////////////////////////////////////////////////////////


NvPmApiManager S_NVPMManager;
NVPMContext g_nvpmContext{ 0 };
NvPmGlobals g_nvpmGlobals;

/// Counter names as provided by NvPmApiQuery.exe (FOR KEPLER ARCH :) )
std::vector<const char *> g_experimentModeCounters
{
    "IA Bottleneck",
    "IA SOL",
    
    "Primitive Setup Bottleneck",
    "Primitive Setup SOL",
    
    "ROP Bottleneck",
    "ROP SOL",
    
    "Rasterization Bottleneck",
    "Rasterization SOL",
    
    "SHD Bottleneck",
    "SHD SOL",
    
    "TEX Bottleneck",
    "TEX SOL",

    "FB Bottleneck",
    "FB SOL",

    "L2 Bottleneck",

    "tex_cache_hitrate",

    "l2_read_bytes_mem",
    "l2_read_bytes_tex",
    
    "shd_tex_read_bytes",
    "shd_tex_requests",

    "inst_executed_ps",
    "inst_executed_ps_ratio",
    "inst_executed_vs",
    "inst_executed_vs_ratio",
    
    "setup_primitive_count",
    "shaded_pixel_count",

    "OGL frame time"
};



///////////////////////////////////////////////////////////////////////////
const char* nvpm_resultToString(NVPMRESULT result)
{
    switch (result)
    {
        case NVPM_FAILURE_DISABLED:                 return "NVPM_FAILURE_DISABLED";             break;
        case NVPM_FAILURE_32BIT_ON_64BIT:           return "NVPM_FAILURE_32BIT_ON_64BIT";       break;
        case NVPM_NO_IMPLEMENTATION:                return "NVPM_NO_IMPLEMENTATION";            break;
        case NVPM_LIBRARY_NOT_FOUND:                return "NVPM_LIBRARY_NOT_FOUND";            break;
        case NVPM_FAILURE:                          return "NVPM_FAILURE";                      break;
        case NVPM_OK:                               return "NVPM_OK";                           break;
        case NVPM_ERROR_INVALID_PARAMETER:          return "NVPM_ERROR_INVALID_PARAMETER";      break;
        case NVPM_ERROR_DRIVER_MISMATCH:            return "NVPM_ERROR_DRIVER_MISMATCH";        break;
        case NVPM_ERROR_NOT_INITIALIZED:            return "NVPM_ERROR_NOT_INITIALIZED";        break;
        case NVPM_ERROR_ALREADY_INITIALIZED:        return "NVPM_ERROR_ALREADY_INITIALIZED";    break;
        case NVPM_ERROR_BAD_ENUMERATOR:             return "NVPM_ERROR_BAD_ENUMERATOR";         break;
        case NVPM_ERROR_STRING_TOO_SMALL:           return "NVPM_ERROR_STRING_TOO_SMALL";       break;
        case NVPM_ERROR_INVALID_COUNTER:            return "NVPM_ERROR_INVALID_COUNTER";        break;
        case NVPM_ERROR_OUT_OF_MEMORY:              return "NVPM_ERROR_OUT_OF_MEMORY";          break;
        case NVPM_ERROR_EXPERIMENT_INCOMPLETE:      return "NVPM_ERROR_EXPERIMENT_INCOMPLETE";  break;
        case NVPM_ERROR_INVALID_PASS:               return "NVPM_ERROR_INVALID_PASS";           break;
        case NVPM_ERROR_INVALID_OBJECT:             return "NVPM_ERROR_INVALID_OBJECT";         break;
        case NVPM_ERROR_COUNTER_NOT_ENABLED:        return "NVPM_ERROR_COUNTER_NOT_ENABLED";    break;
        case NVPM_ERROR_COUNTER_NOT_FOUND:          return "NVPM_ERROR_COUNTER_NOT_FOUND";      break;
        case NVPM_ERROR_EXPERIMENT_NOT_RUN:         return "NVPM_ERROR_EXPERIMENT_NOT_RUN";     break;
        case NVPM_ERROR_32BIT_ON_64BIT:             return "NVPM_ERROR_32BIT_ON_64BIT";         break;
        case NVPM_ERROR_STATE_MACHINE:              return "NVPM_ERROR_STATE_MACHINE";          break;
        case NVPM_ERROR_INTERNAL:                   return "NVPM_ERROR_INTERNAL";               break;
        case NVPM_WARNING_ENDED_EARLY:              return "NVPM_WARNING_ENDED_EARLY";          break;
        case NVPM_ERROR_TIME_OUT:                   return "NVPM_ERROR_TIME_OUT";               break;
        case NVPM_WARNING_DUPLICATE:                return "NVPM_WARNING_DUPLICATE";            break;
        case NVPM_ERROR_COUNTERS_ENABLED:           return "NVPM_ERROR_COUNTERS_ENABLED";       break;
        case NVPM_ERROR_CONTEXT_NOT_SUPPORTED:      return "NVPM_ERROR_CONTEXT_NOT_SUPPORTED";  break;
        case NVPM_ERROR_INVALID_CONTEXT:            return "NVPM_ERROR_INVALID_CONTEXT";        break;
        case NVPM_ERROR_GPU_UNSUPPORTED:            return "NVPM_ERROR_GPU_UNSUPPORTED";        break;
        case NVPM_INCORRECT_VALUE_TYPE:             return "NVPM_INCORRECT_VALUE_TYPE";         break;
        default:                                    return "NVPM_ERROR_UNKNOWN";                break;
    }
}


///////////////////////////////////////////////////////////////////////////
extern NvPmApiManager *GetNvPmApiManager()
{
    return &S_NVPMManager;
}


//////////////////////////////////////////////////////////////////////////
const NvPmApi *GetNvPmApi()
{
    return S_NVPMManager.Api();
}


//////////////////////////////////////////////////////////////////////////
bool nvpm_init()
{
    NVPMRESULT nvResult;

    if ((nvResult = GetNvPmApiManager()->Construct(PATH_TO_NVPMAPI_CORE)) != S_OK) {
        gl_log_err("NvPmApi Manager could not construct. result: %d", nvResult);
        return false; 
    }

    if ((nvResult = GetNvPmApi()->Init()) != NVPM_OK) {
        gl_log_err("NvPmApi could not initialize. result: %d", nvResult);
        return false; 
    }

    nvResult = 
        GetNvPmApi()->CreateContextFromOGLContext(uint64_t(::wglGetCurrentContext()), 
            &g_nvpmContext);
    
    if (nvResult != NVPM_OK) {
        gl_log_err("NvPmApi could not create context from ogl context! result: %d", nvResult);
        return false; 
    }

    gl_log("NvPmApi initialized.");

    return true;
}


//////////////////////////////////////////////////////////////////////////
bool nvpm_shutdown()
{
    if (!S_NVPMManager.Api()) {
        return false;
    }

    S_NVPMManager.Api()->Shutdown();
    
    return true;
}


///////////////////////////////////////////////////////////////////////////////
//   NVPM Experiment/Work/Frame Begin & End
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void nvpm_experimentFrameBegin(Mode *m)
{
    if (m->nFrame == g_nvpmGlobals.framestart){
        glFinish();
        GetNvPmApi()->BeginExperiment(m->perfCtx, &m->nTotalPasses);
        m->isCollecting = true;
        m->nPass = 0;
    }

    if (m->isCollecting) {
        GetNvPmApi()->BeginPass(m->perfCtx, m->nPass);
    }
}


//////////////////////////////////////////////////////////////////////////
void nvpm_experimentFrameEnd(Mode *mode)
{
    if (mode->isCollecting) {
        GetNvPmApi()->EndPass(mode->perfCtx, mode->nPass);
        ++mode->nPass;
        mode->objectId = 0;
    }

    if (mode->isCollecting && mode->nPass >= mode->nTotalPasses) {
        std::cout << "Profiling completed after " << mode->nTotalPasses << " passes." << std::endl;
        mode->isCollecting = false;
        mode->nPass = 0;
        mode->nTotalPasses = 0;
        GetNvPmApi()->EndExperiment(mode->perfCtx);
        if (mode->exitOnDone)
            glfwSetWindowShouldClose(glfwGetCurrentContext(), GL_TRUE);
    }

    ++mode->nFrame;
}


//////////////////////////////////////////////////////////////////////////
/// \brief Call before each draw
//////////////////////////////////////////////////////////////////////////
void nvpm_experimentWorkBegin(Mode *mode)
{
    if (mode->isCollecting) {
        glFinish();
        GetNvPmApi()->BeginObject(mode->perfCtx, mode->objectId);
    }
}


//////////////////////////////////////////////////////////////////////////
/// \brief Call after each draw
//////////////////////////////////////////////////////////////////////////
void nvpm_experimentWorkEnd(Mode *mode)
{
    if (mode->isCollecting) {
        glFinish();
        GetNvPmApi()->EndObject(mode->perfCtx, mode->objectId);
        ++mode->objectId;
    }
}


///////////////////////////////////////////////////////////////////////////////
//   struct Mode Initialization
///////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
void nvpm_resetMode(Mode *mode)
{
    mode->nFrame = 0;
    mode->nPass = 0;
    mode->nTotalPasses = 0;
    mode->objectId = 0;
    mode->counterNames = nullptr;
    mode->counterCount = 0;
    mode->isCollecting = false;
    mode->exitOnDone = false;
}


//////////////////////////////////////////////////////////////////////////
void nvpm_initMode(Mode *mode, NVPMContext perftext, bool perfMode=false)
{
    nvpm_resetMode(mode);
    mode->perfCtx = perftext;
    mode->counterNames = g_experimentModeCounters.data();
    mode->counterCount = g_experimentModeCounters.size();
    mode->exitOnDone = perfMode;
    
    for (size_t i = 0; i < g_experimentModeCounters.size(); ++i) {
        NVPMRESULT nvpmResult{ NVPM_OK };
        const char* counterName = mode->counterNames[i];
        nvpmResult = GetNvPmApi()->AddCounterByName(mode->perfCtx, counterName);

        if (nvpmResult != NVPM_OK) {
            const char *resultStr = nvpm_resultToString(nvpmResult);
            gl_log_err("AddCounterByName(%s) returned %08x => %s",
                counterName, nvpmResult, resultStr);
        }
    }
}

bool nvpm_isdone(Mode *mode)
{
    return mode->isCollecting && mode->nPass >= mode->nTotalPasses;
}

//////////////////////////////////////////////////////////////////////////
void nvpm_printCounters(std::ostream &outStream, Mode *mode) // NVPMContext perfCtx, const char ** const counterNames, int counterCount)
{
    outStream <<
        std::left << std::setw(40) << "Total passes: " <<
        "value: " << std::setw(10) << mode->nTotalPasses << std::endl;

    outStream <<
        std::left << std::setw(40) << "n frame: " <<
        "value: " << std::setw(10) << mode->nFrame << std::endl;

    outStream <<
        std::left << std::setw(40) << "Total objects: " <<
        "value: " << std::setw(10) << mode->objectId << std::endl;

    for (int i = 0; i < mode->counterCount; ++i) {

        NVPMCounterID id = 0;
        NVPMUINT64 type = 0;

        /*NVPMCHECKCONTINUE(*/GetNvPmApi()->GetCounterIDByContext(mode->perfCtx, mode->counterNames[i], &id); //);
        /*NVPMCHECKCONTINUE(*/GetNvPmApi()->GetCounterAttribute(id, NVPMA_COUNTER_VALUE_TYPE, &type); //);

        NVPMUINT64 cycles = 0;
        NVPMUINT8  overflow = 0;
        if (type == NVPM_VALUE_TYPE_UINT64) {

            NVPMUINT64 value = 0;
            GetNvPmApi()->GetCounterValueUint64(mode->perfCtx, id, 0, &value, &cycles, &overflow);

            outStream << 
                std::left    << std::setw(40) << mode->counterNames[i] <<
                "value: "    << std::setw(15) << static_cast<unsigned long long>(value) <<
                "cycles: "   << std::setw(15) << static_cast<unsigned long long>(cycles) <<
                "overflow: " << (overflow ? "true" : "false") << std::endl;
        }
        else if (type == NVPM_VALUE_TYPE_FLOAT64) {

            NVPMFLOAT64 value = 0;
            GetNvPmApi()->GetCounterValueFloat64(mode->perfCtx, id, 0, &value, &cycles, &overflow);

            outStream << 
                std::left    <<  std::setw(40) << mode->counterNames[i] <<
                "value: "    <<  std::setw(15) << value <<
                "cycles: "   <<  std::setw(15) << static_cast<unsigned long long>(cycles) <<
                "overflow: " << (overflow ? "true" : "false") << std::endl;
        }
    }
}

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

