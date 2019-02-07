#ifndef gl_log_h__
#define gl_log_h__


#ifdef _WIN32
#define __func__ __FUNCTION__
#endif

//#define gl_log(fmt_, ...) bd::gl_log_fcn("%s[%d]:%s()::\n\t"  fmt_, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

//#define gl_log_err(fmt_, ...) bd::gl_log_err_fcn(nullptr, "%s[%d]:%s()::\n\t" fmt_, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#ifdef BD_DEBUG
#define gl_check(fn) (fn); bd::checkForAndLogGlError(__FILE__, __func__, __LINE__);
#else
#define gl_check(fn) (fn);
#endif


namespace bd
{
void subscribe_debug_callbacks();

///////////////////////////////////////////////////////////////////////////////
// \brief Check for any and all opengl errors at any given time.
// Dumps errors into gl log io and stderr.
///////////////////////////////////////////////////////////////////////////////
void checkForAndLogGlError(char const * file, char const * func, int line);

} /* namespace bd */

#endif // gl_log_h__


