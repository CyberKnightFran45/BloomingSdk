#pragma once

#if defined(_WIN32) || defined(_WIN64)  // Windows (MSVC, MinGW, Clang-cl)
    #ifdef BUILDING_DLL
        #define DLL_EXPORT __declspec(dllexport)
    #else
        #define DLL_EXPORT __declspec(dllimport)
    #endif
	
#elif defined(__linux__) || defined(__APPLE__)  // Linux/macOS (GCC, Clang)
    #ifdef BUILDING_DLL
        #define DLL_EXPORT __attribute__((visibility("default")))
    #else
        #define DLL_EXPORT
    #endif
	
#else
    #define DLL_EXPORT
    #pragma warning "Cannot export Dll symbols on this platform"
#endif