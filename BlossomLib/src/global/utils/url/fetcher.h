#include "../../lib_export.h"
#include "../../../modules/file_manager.hpp"
#include <string>
#include <curl/curl.h>

struct Common
{
protected:

// Callback function to write received data to a string

static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total = size * nmemb;
    std::string* buf = static_cast<std::string*>(userp);
    buf->append(static_cast<char*>(contents), total);
    return total;
}

};

struct UrlFetcher : protected Common
{
static char* getResponse(const char* url);
static std::unique_ptr<ReadOnlyStream> getResponseStream(const char* url); // C++ exclusive

static bool downloadFile(const char* url, const char* filePath);
};

extern "C"
{
DLL_EXPORT char* getResponse(const char* url);

DLL_EXPORT bool downloadFile(const char* url, const char* filePath);
}