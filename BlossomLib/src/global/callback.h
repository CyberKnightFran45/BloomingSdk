extern "C"
{
typedef void (*ProgressCallback)(int processed, int total, const char* message, bool hasError);

typedef void (*ErrorCallback)(int errorCode, const char* errorMessage);
}