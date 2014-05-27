#include "misc.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

namespace ubase
{
    std::string now_to_string()
    {
        std::string out;
        struct tm *ptm = localtime((const time_t *)time(0));
        if (ptm) {
            out.resize(10+1+8+1);
            memset((char *)out.data(), 0, out.size());
            snprintf((char *)out.data(), out.size()-1, "%4d-%2d-%2d %2d:%2d:%2d", 
                    ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
                    ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
        }
        return out;
    }
}
