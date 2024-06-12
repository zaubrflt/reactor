#include "Thread.h"

namespace reactor {

std::thread::id tid()
{
  return std::this_thread::get_id();
}

}  // namespace reactor
