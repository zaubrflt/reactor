#ifndef REACTOR_BASE_ERRORCODE_H
#define REACTOR_BASE_ERRORCODE_H

#include <string>
#include <string.h>

namespace reactor {

inline const std::string errorStr(int errnum)
{
    std::string result = "";
    char buf[1024], *res = buf;
    buf[0] = 0;
    res = strerror_r(errnum, buf, sizeof(buf));
    result += (std::string(res) + " (code " + std::to_string(errnum) + ")");
    return result;
}

}  // namespace reactor

#endif  // REACTOR_BASE_ERRORCODE_H