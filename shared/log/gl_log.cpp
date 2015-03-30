#include "gl_log.h"

#include <GL/glew.h>

#include <cstdarg>
#include <fstream>
#include <ctime>
namespace bd {
    namespace log {
        const char *logFileName = "gl.log";
        const char *glDebugFileName = "gl_debug.log";
        FILE *file = NULL;
        FILE *glDebugFile = NULL;

        void checkForAndLogGlError(const char * func, int line)
        {
            GLint error;
            while ((error = glGetError()) != GL_NO_ERROR) {
                gl_log_err("(OGL): Func: %s: Line: %d: Error: 0x%04X", func, line, error);
            }
        }

        bool gl_log_close()
        {
            if (!file) {
                fprintf(stderr, "Error: not closing gl log file %s because it was not opened.", logFileName);
            }
            if (!glDebugFile) {
                fprintf(stderr, "Error: not closing gl debug log file %s because it was not opened.", logFileName);
            }

            if (!file || !glDebugFile)
                return false;

            time_t now = time(NULL);
            char* date = ctime(&now);
            fprintf(file, "------------------------\nClosing gl_log file. local time %s\n", date);

            fclose(file);
            fclose(glDebugFile);

            return true;
        }

        bool gl_debug_log_restart()
        {
            if (!glDebugFile) {
                glDebugFile = fopen(glDebugFileName, "a");
            } else {
                fprintf(stderr, "GL debug log file was already opened.\n");
                return true;
            }

            if (!glDebugFile) {
                fprintf(stderr, "ERROR: could not open GL debug output log file %s for writing\n", logFileName);
                return false;
            }

            time_t now = time(NULL);
            char* date = ctime(&now);
            fprintf(file, "\n------------------------\n"
                "GL debug output log. local time %s\n", date);

            return true;
        }

        bool gl_log_restart()
        {
            if (!file) {
                file = fopen(logFileName, "a");
            } else {
                fprintf(stderr, "Log file was already opened.\n");
                return true;
            }

            if (!file) {
                fprintf(stderr, "Could not open logFileName log file %s for writing\n", logFileName);
                return false;
            }

            time_t now = time(NULL);
            char* date = ctime(&now);
            fprintf(file, "\n------------------------\n"
                "logFileName log. local time %s\n", date);

            return true;
        }

        bool gl_log_fcn(const char* message, ...)
        {
            va_list argptr;

            if (!file) {
                fprintf(stderr, "log file %s is not open for appending", logFileName);
                return false;
            }

            fprintf(file, "(LOG): ");

            va_start(argptr, message);
            vfprintf(file, message, argptr);
            va_end(argptr);
            fprintf(file, "\n");
            fflush(file);

            return true;
        }

        bool gl_log_err_fcn(const char* message, ...)
        {
            va_list argptr;

            if (!file) {
                fprintf(stderr, "ERROR: Log file %s was not open for appending\n", logFileName);
                return false;
            }

            fprintf(file, "(ERR): ");
            fprintf(stderr, "(ERR): ");

            va_start(argptr, message);
            vfprintf(file, message, argptr);
            va_end(argptr);

            va_start(argptr, message);
            vfprintf(stderr, message, argptr);
            va_end(argptr);

            fprintf(file, "\n");
            fprintf(stderr, "\n");
            fflush(file);

            return true;
        }

        void
            log_gl_params() {
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
            const char* names [] = {
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

        void gl_debug_message_callback(GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            void *userParam)
        {
            const char *msg = "OGL_DEBUG: source: 0x%04X, type 0x%04X, id %u, severity 0x%0X, '%s'\n";
            if (glDebugFile)
                fprintf(glDebugFile, msg,
                source, type, id, severity, message);
        }
    }
} // namespace