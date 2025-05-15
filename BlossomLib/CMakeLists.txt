# CMakeList.txt: CMake project for BlossomLib

cmake_minimum_required (VERSION 3.8)

# Enable Active Reload if MSVC compilers allow it

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("BlossomLib")

# Recursive search for C++ sources
file(GLOB_RECURSIVE LIB_SOURCES "src/*.cpp")

# Build Lib by using the sources found
add_library (BlossomLib ${LIB_SOURCES})

# Declare headers folder

target_include_directories(BlossomLib 
  PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Restart standar to C++20

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET BlossomLib PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests or installer destinations if desired