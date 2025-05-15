#include "fetcher.h"

 // Get response from server as a string
 
char* UrlFetcher::getResponse(const char* url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return nullptr;
    }

    std::string buffer;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: "
                  << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return nullptr;
    }

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    if (httpCode >= 400) {
        std::cerr << "HTTP Error: " << httpCode << " for URL " << url << std::endl;
        return nullptr;
    }

    char* result = static_cast<char*>(std::malloc(buffer.size() + 1));
    if (!result) {
        std::cerr << "malloc failed\n";
        return nullptr;
    }
    std::memcpy(result, buffer.c_str(), buffer.size() + 1);
    return result;
}

 // Get ReadOnlyStrea
 
 std::unique_ptr<ReadOnlyStream> UrlFetcher::getResponseStream(const char* url)
 {
	 char* response = getResponse(url);
	 
	 if(!response)
		return nullptr; // On error
	
	   auto dataLen = strlen(response);
	   auto streambuf = std::make_unique<ReadOnlyStream>(dataLen);
	   
	   streambuf.WriteString(response, dataLen);
	   free(response);
	   
	   return streambuf;
 }
 
 
    // Download file (true if success, false otherwise)
	
    bool UrlFetcher::downloadFile(const char* url, const char* filePath) 
	{
       
	   if(!url || !filePath)
		return false;
	
	     char* response = getResponse(url);
	     auto streambuf = fileMgr::openWrite(filePath);
		 
		 streambuf.WriteString(response, strlen(response) );
	   
        return true;
    }

extern "C"
{
char* getResponse(const char* url) { return UrlFetcher::getResponse(url); }

bool downloadFile(const char* url, const char* filePath) { return UrlFetcher::downloadFile(url, filePath); }
}