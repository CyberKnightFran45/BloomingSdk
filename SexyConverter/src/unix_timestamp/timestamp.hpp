#pragma once

#include "BlossomLib/src/global/lib_export.h"
#include <cmath>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <string>

namespace SexyConverter::UnixTimestamp
{
using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;

TimePoint toDate(const double target);

double fromTime(const TimePoint& source);
}

namespace unix_ts = SexyConverter::UnixTimestamp;

extern "C"
{
DLL_EXPORT char* TimeStampToDate(double target);

DLL_EXPORT double DateToTimeStamp(char* source);
}
