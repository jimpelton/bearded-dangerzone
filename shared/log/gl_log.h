
#ifndef gl_log_h__
#define gl_log_h__

#include <GL/glew.h>

#ifdef _WIN32
#define __func__ __FUNCTION__
#endif

#define gl_log(str_, ...) gl_log_fcn("%s[%d]:%s()::" # str_, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define gl_log_err(str_, ...) gl_log_err_fcn("%s[%d]:%s()::" # str_, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

void checkForAndLogGlError(const char* func, int line);
bool restart_gl_log();
bool restart_gl_debug_log();
bool gl_log_close();
bool gl_log_fcn(const char* message, ...);

void gl_debug_message_callback(GLenum source,
       GLenum type, GLuint id, GLenum severity,
       GLsizei length, const GLchar *message, void *userParam);

bool gl_log_err_fcn(const char* message, ...);
void log_gl_params ();

#endif // gl_log_h__

