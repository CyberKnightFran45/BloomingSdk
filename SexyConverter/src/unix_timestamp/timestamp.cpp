#include "timestamp.hpp"

namespace timestamp
{

inline TimePoint toDate(const double target) {
    return Clock::from_time_t(0) + std::chrono::seconds(static_cast<int64_t>(target) );
}

inline double fromTime(const TimePoint& source) 
{
    auto duration = source.time_since_epoch();
    return std::trunc(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
}

}

extern "C" 
{
	
DLL_EXPORT char* TimeStampToDate(double target) {
    timestamp::TimePoint tp = timestamp::toDate(target);
    std::time_t timeT = timestamp::Clock::to_time_t(tp);

    std::tm* tmStruct = std::gmtime(&timeT); // Use UTC
    if (!tmStruct) return nullptr;

    static char buffer[20]; // "YYYY-MM-DD HH:MM:SS" + \0
    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tmStruct)) {
        return buffer;
    } else {
        return nullptr;
    }
}

DLL_EXPORT double DateToTimeStamp(char* source) {
    if (!source) return 0;

    std::tm tmStruct = {};
    if (strptime(source, "%Y-%m-%d %H:%M:%S", &tmStruct) == nullptr) {
        return 0;
    }

    std::time_t timeT = timegm(&tmStruct); // UTC
    if (timeT == -1) return 0;

    return static_cast<double>(timeT);
}

}