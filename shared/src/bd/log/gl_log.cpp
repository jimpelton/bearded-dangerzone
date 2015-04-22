#include <bd/log/gl_log.h>

#include <GL/glew.h>

#include <cstdarg>
#include <fstream>
#include <ctime>
#include <iostream>

namespace {

///////////////////////////////////////////////////////////////////////////////
const char *gl_debug_type_str(GLenum type)
{
    const char *str = "";

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            str = "ERROR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            str = "UNDEFINED BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            str = "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            str = "PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_OTHER:
            str = "OTHER";
            break;
        default:
            str = "UNKNOWN ERROR TYPE";
            break;
    }

    return str;
}


///////////////////////////////////////////////////////////////////////////////
const char *gl_debug_severity_str(GLenum severity)
{
    const char *str = "";

    switch (severity) {
        case GL_DEBUG_SEVERITY_LOW:
            str = "LOW";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            str = "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_HIGH:
            str = "HIGH";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            str = "NOTIFY";
            break;
        default:
            str = "UNKNOWN SEVERITY";
            break;
    }

    return str;
}

const char* gl_debug_source_str(GLenum source)
{
   const char* src = "";
    switch(source){
        case GL_DEBUG_SOURCE_API:
            src = "GL_DEBUG_SOURCE_API";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            src = "GL_DEBUG_SOURCE_SHADER_COMPILER";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            src = "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            src = "GL_DEBUG_SOURCE_THIRD_PARTY";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            src = "GL_DEBUG_SOURCE_APPLICATION";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            src = "GL_DEBUG_SOURCE_OTHER";
            break;
        default:
            src = "UNKNOWN SOURCE";
            break;
    }

    return src;
}

const char* gl_err_num_str(GLenum err)
{
    const char* str = "";
    switch(err){
        case GL_NO_ERROR:
            str = "GL_NO_ERROR"; break;
        case GL_INVALID_ENUM:
            str = "GL_INVALID_ENUM"; break;
        case GL_INVALID_VALUE:
            str = "GL_INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:
            str = "GL_INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:
            str = "GL_STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:
            str = "GL_STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:
            str = "GL_OUT_OF_MEMORY"; break;
        default:
            str = "UNKNONW ERROR CODE"; break;
    }

    return str;
}

} // namespace


namespace bd {

namespace {

const char *logFileName = "gl.log";
const char *glDebugFileName = "gl_debug.log";
FILE *file = NULL;

FILE *glDebugFile = NULL;

const char *ogl = "OGL";
const char *gcb = "GCB";
const char *err = "ERR";
const char *log = "LOG";

} // namespace


///////////////////////////////////////////////////////////////////////////////
void gl_debug_message_callback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length, const GLchar *message, void *userParam)
{
    const char* sev = gl_debug_severity_str(severity);
    const char* typ = gl_debug_type_str(type);
    const char* src = gl_debug_source_str(source);
    gl_log_err_fcn(gcb, "Source: %s,\n\tType: %s,\n\tId %u,\n\tSeverity %s,\n\t'%s'",
        src, typ, id, sev, message);
}


///////////////////////////////////////////////////////////////////////////////
void subscribe_debug_callbacks()
{
    gl_log("Registering for GL debug message callbacks.");
    GLuint unusedIds = 0;
    glDebugMessageControl(GL_DONT_CARE,
        GL_DONT_CARE,
        GL_DONT_CARE,
        0,
        &unusedIds,
        GL_TRUE);
    glDebugMessageCallback(( GLDEBUGPROC ) gl_debug_message_callback, NULL);
    //glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

///////////////////////////////////////////////////////////////////////////////
void checkForAndLogGlError(const char *file, const char *func, int line)
{
    const char* fmt = "%s[%d]:%s():: \n\t%s (0x%04X)";
//    const char* fmt = "(OGL): %s[%d]:%s():: \n\t0x%04X";
    GLint error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        const char* estr = gl_err_num_str(error);

        gl_log_err_fcn(ogl, fmt, file, line, func, estr, error);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool gl_log_close()
{
    if (!file) {
        fprintf(stderr, "Error: not closing gl log file %s because it was not"
                " opened.\n", logFileName);
    }
    if (!glDebugFile) {
        fprintf(stderr, "Error: not closing gl debug log file %s because it "
                "was not opened.\n", logFileName);
    }

    if (!file || !glDebugFile)
        return false;

    time_t now = time(NULL);
    char *date = ctime(&now);
    fprintf(file, "------------------------\nClosing gl_log file. local time %s\n", date);

    fclose(file);
    fclose(glDebugFile);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool gl_debug_log_restart()
{
    if (!glDebugFile) {
        glDebugFile = fopen(glDebugFileName, "a");
    } else {
        fprintf(stderr, "GL debug log file was already opened.\n");
        return true;
    }

    if (!glDebugFile) {
        fprintf(stderr, "ERROR: could not open GL debug output log file %s for writing\n", glDebugFileName);
        return false;
    }

    std::time_t now = std::time(NULL);
    char *date = ctime(&now);
    fprintf(file, "\n------------------------\n"
            "GL debug output log. local time %s\n", date);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool gl_log_restart()
{
    if (!file) {
        file = fopen(logFileName, "a");
    } else {
        fprintf(stderr, "Log file %s was already opened.\n", logFileName);
        return true;
    }

    if (!file) {
        fprintf(stderr, "Could not open %s for writing, will use stdout/stderr instead.\n", logFileName);
        return false;
    }

    std::time_t now = std::time(NULL);
    char *date = ctime(&now);
    fprintf(file, "\n-------------BEGIN LOG---------\n"
                  "logFileName log. local time %s\n", date);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool gl_log_fcn(const char *message, ...)
{
    va_list argptr;

    // print to stdout if no file open.
    if (!file) {
        fprintf(stderr, "Log file %s is not open for appending\n", logFileName);

        fprintf(stdout, "(LOG): ");

        va_start(argptr, message);
        vfprintf(stdout, message, argptr);
        va_end(argptr);
        fprintf(stdout, "\n");
        fflush(stdout);

        return true;
    }

    fprintf(file, "(LOG): ");

    va_start(argptr, message);
    vfprintf(file, message, argptr);
    va_end(argptr);
    fprintf(file, "\n");
    fflush(file);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool gl_log_err_fcn(const char *prefix, const char *message, ...)
{
    if (!prefix) {
        prefix = err;
    }

    va_list argptr;

    if (file) {
        fprintf(file, "(%s): ", prefix);
        va_start(argptr, message);
        vfprintf(file, message, argptr);
        va_end(argptr);
        fprintf(file, "\n");
        fflush(file);
    }

    fprintf(stderr, "(%s): ", prefix);

    va_start(argptr, message);
    vfprintf(stderr, message, argptr);
    va_end(argptr);

    fprintf(stderr, "\n");

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void log_gl_params()
{
    GLenum params [] = {
        GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
        GL_MAX_CUBE_MAP_TEXTURE_SIZE,
        GL_MAX_DRAW_BUFFERS,
        GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
        GL_MAX_TEXTURE_IMAGE_UNITS,
        GL_MAX_TEXTURE_SIZE,
        GL_MAX_VARYING_FLOATS,
        GL_MAX_VERTEX_ATTRIBS,
        GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
        GL_MAX_VERTEX_UNIFORM_COMPONENTS,
        GL_MAX_VIEWPORT_DIMS,
        GL_STEREO,
    };
    const char *names [] = {
        "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
        "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
        "GL_MAX_DRAW_BUFFERS",
        "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
        "GL_MAX_TEXTURE_IMAGE_UNITS",
        "GL_MAX_TEXTURE_SIZE",
        "GL_MAX_VARYING_FLOATS",
        "GL_MAX_VERTEX_ATTRIBS",
        "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
        "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
        "GL_MAX_VIEWPORT_DIMS",
        "GL_STEREO",
    };

    gl_log("GL Context Params:\n");
    //  char msg[256];
    // integers - only works if the order is 0-10 integer return types
    for (int i = 0; i < 10; i++) {
        int v = 0;
        glGetIntegerv(params[i], &v);
        gl_log("%s %i", names[i], v);
    }
    // others
    int v[2];
    v[0] = v[1] = 0;
    glGetIntegerv(params[10], v);
    gl_log("%s %i %i", names[10], v[0], v[1]);
    unsigned char s = 0;
    glGetBooleanv(params[11], &s);
    gl_log("%s %u", names[11], (unsigned int)s);
    //gl_log ("-----------------------------\n");
}

} // namespace

