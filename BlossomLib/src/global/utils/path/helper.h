#pragma once

#include "../../../lib_export.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <cstring>
#include <stdexcept>

namespace fs = std::filesystem;

struct Path
{
static void addExt(char* sourcePath, const char* targetExtension);
static void changeExt(char* sourcePath, const char* targetExtension);
static void removeExt(char* sourcePath, const char* targetExtension);

static void alignWithAppDir(char* targetPath);
static char* buildFromDir(const char* parentPath, const char* filePath, const char* fileExtension, const char* pathSuffix);

static void checkDuplicated(char* targetPath);
static void checkExisting(const char* sourcePath, bool createFileSystem);

// Types: Error = -1, Unknown = 0, File = 1, Dir = 2
static int getType(const char* sourcePath);

static void createFS(const char* targetPath);

static void filterFiles(std::vector<std::string>& sourceList, const std::vector<std::string>& names, const std::vector<std::string>& extensions,
                 const std::vector<std::string>& namesToExclude, const std::vector<std::string>& extensionsToExclude);

static void filterDirs(std::vector<std::string>& sourceList, const std::vector<std::string>& names, 
                int maxLength, const std::vector<std::string>& namesToExclude);

static void formatWindows(char* filePath);
static void formatLinux(char* filePath);

static char* getDownloadsFolder();
};

extern "C" 
{
DLL_EXPORT void Path_AddExt(char* sourcePath, const char* targetExtension);
DLL_EXPORT void Path_ChangeExt(char* sourcePath, const char* targetExtension);
DLL_EXPORT void Path_RemoveExt(char* sourcePath, const char* targetExtension);

DLL_EXPORT void Path_AlignWithAppDir(char* targetPath);
DLL_EXPORT char* Path_BuildFromDir(const char* parentPath, const char* filePath, const char* fileExtension, const char* pathSuffix);

DLL_EXPORT void Path_CheckDuplicated(char* targetPath);
DLL_EXPORT void Path_CheckExisting(const char* sourcePath, bool createFileSystem);

DLL_EXPORT int Path_GetType(const char* sourcePath);

DLL_EXPORT void Path_CreateFS(const char* targetPath);

DLL_EXPORT void Path_FormatWindows(char* filePath);
DLL_EXPORT void Path_FormatLinux(char* filePath);

DLL_EXPORT char* Path_GetDownloadsFolder();
}