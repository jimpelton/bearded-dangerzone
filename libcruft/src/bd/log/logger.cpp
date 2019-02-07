
#include <bd/log/logger.h>

namespace bd
{

logger *logger::s_instance{ nullptr };
std::chrono::time_point<std::chrono::high_resolution_clock> 
  logger::programStartTime{ };
} // namespace bd
