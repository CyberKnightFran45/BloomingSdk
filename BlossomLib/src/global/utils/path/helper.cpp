#include "helper.h"

void Path::addExt(char* sourcePath, const char* targetExtension)
    {
        if (sourcePath == nullptr || targetExtension == nullptr)
            return;

        std::string path(sourcePath);
        std::string extension(targetExtension);

        if (path.empty() || path.find(extension) != std::string::npos)
            return;

        path += extension;
        checkDuplicated(path);

        std::strcpy(sourcePath, path.c_str());
    }

    void Path::alignWithAppDir(char* targetPath)
    {
        if (targetPath == nullptr)
            return nullptr;

        std::string path(targetPath);

        if (fs::path(path).is_absolute())
            return strdup(targetPath);

        std::string appDir = fs::current_path().string();
        appDir += "/" + path;

        return strcpy(targetPath, appDir.c_str() );
    }

    char* Path::buildFromDir(const char* parentPath, const char* filePath, const char* fileExtension, const char* pathSuffix)
    {
        if (parentPath == nullptr || filePath == nullptr || fileExtension == nullptr)
            return nullptr;

        std::string parent(parentPath);
        std::string file(filePath);
        std::string extension(fileExtension);
        std::string suffix = pathSuffix ? pathSuffix : "";

        fs::path basePath(parent);
        basePath /= file;

        if (!suffix.empty())
            basePath += "_" + suffix;

        basePath += extension;

        return strdup(basePath.c_str());
    }

    void Path::changeExt(char* sourcePath, const char* targetExtension)
    {
        if (sourcePath == nullptr || targetExtension == nullptr)
            return;

        std::string path(sourcePath);
        std::string newExtension(targetExtension);

        size_t pos = path.rfind('.');
        if (pos != std::string::npos && path.substr(pos) != newExtension)
        {
            path = path.substr(0, pos) + newExtension;
        }

        CheckDuplicated(path);
        std::strcpy(sourcePath, path.c_str());
    }

    void Path::checkDuplicated(char* targetPath)
    {
        if (targetPath == nullptr)
            return;

        std::string path(targetPath);
        fs::path fsPath(path);

        if (!fs::exists(fsPath))
            return;

        std::string parent = fsPath.parent_path().string();
        std::string filename = fsPath.filename().string();
        std::string extension = fsPath.extension().string();
        std::string name = filename.substr(0, filename.length() - extension.length());

        int copyIndex = 1;
        std::string newPath = parent + "/" + filename;

        while (fs::exists(newPath))
        {
            newPath = parent + "/" + name + " (" + std::to_string(copyIndex) + ")" + extension;
            copyIndex++;
        }

        std::strcpy(targetPath, newPath.c_str());
    }

    void Path::checkExisting(const char* sourcePath, bool createFileSystem)
    {
        if (sourcePath == nullptr)
            return;

        std::string path(sourcePath);
        fs::path fsPath(path);

        if (!fs::exists(fsPath))
        {
            if (createFileSystem)
            {
                if (fs::path(path).has_extension())
                    fs::create_directory(path);
                else
                    fs::create_directories(path);
            }
        }
        else
        {
            checkDuplicated(path);
        }
    }

    int Path::getType(const char* sourcePath)
    {
        if (sourcePath == nullptr)
            return -1;

        std::string path(sourcePath);
        fs::path fsPath(path);

        if (!fs::exists(fsPath))
            return -1;

        if (fs::is_regular_file(fsPath))
            return 1;
        else if (fs::is_directory(fsPath))
            return 2;

        return 0;
    }

    void Path::createFS(const char* targetPath)
    {
        if (targetPath == nullptr)
            return;

        std::string path(targetPath);
        fs::path fsPath(path);

        if (fsPath.has_extension())
            fs::create_directory(path); 
        else
            fs::create_directories(path); 
    }

    void Path::filterFiles(std::vector<std::string>& sourceList, const std::vector<std::string>& names, 
        const std::vector<std::string>& extensions, 
        const std::vector<std::string>& namesToExclude, const std::vector<std::string>& extensionsToExclude)
    {
        if (sourceList.empty() || (names.empty() && extensions.empty()))
            return;

        std::vector<std::string> filteredList;

        for (auto& file : sourceList)
        {
            bool includeFile = false;

            std::string fileName = fs::path(file).filename().string();
            std::string fileExt = fs::path(file).extension().string();

            if (std::find(names.begin(), names.end(), "*") != names.end() || std::find(names.begin(), names.end(), fileName) != names.end())
            {
                if (std::find(extensionsToExclude.begin(), extensionsToExclude.end(), fileExt) == extensionsToExclude.end())
                    includeFile = true;
            }

            if (std::find(extensions.begin(), extensions.end(), fileExt) != extensions.end() && includeFile)
            {
                filteredList.push_back(file);
            }
        }

        sourceList = filteredList;
    }

    void Path::filterDirs(std::vector<std::string>& sourceList, const std::vector<std::string>& names, 
                          int maxLength, const std::vector<std::string>& namesToExclude)
    {
        if (sourceList.empty() || (names.empty() && maxLength < 0))
            return;

        std::vector<std::string> filteredList;

        for (auto& dir : sourceList)
        {
            bool includeDir = false;

            std::string dirName = fs::path(dir).filename().string();

            if (std::find(names.begin(), names.end(), "*") != names.end() || std::find(names.begin(), names.end(), dirName) != names.end())
            {
                if (std::find(namesToExclude.begin(), namesToExclude.end(), dirName) == namesToExclude.end())
                    includeDir = true;
            }

            if (maxLength >= 0 && includeDir && fs::directory_iterator(dir).distance() <= maxLength)
            {
                filteredList.push_back(dir);
            }
        }

        sourceList = filteredList;
    }

    void Path::formatWindows(char* filePath)
    {
        if (filePath == nullptr)
            return nullptr;

        std::string path(filePath);
        std::replace(path.begin(), path.end(), '/', '\\');
		
       strcpy(filePath, path.c_str() );
    }

    void Path::formatLinux(char* filePath)
    {
        if (filePath == nullptr)
            return nullptr;

        std::string path(filePath);
        std::replace(path.begin(), path.end(), '\\', '/');
		
        strcpy(filePath, path.c_str() );
    }

    char* Path::getDownloadsFolder()
    {
        const char* userProfile = std::getenv("USERPROFILE");
        if (userProfile == nullptr)
            return nullptr;

        std::string downloadsFolder = std::string(userProfile) + "\\Downloads";
        return strdup(downloadsFolder.c_str());
    }

    void Path::removeExt(char* sourcePath, const char* targetExtension)
    {
        if (sourcePath == nullptr)
            return;

        std::string path(sourcePath);
        std::string extension = targetExtension ? targetExtension : "";

        if (extension.empty())
            extension = fs::path(path).extension().string();

        size_t pos = path.rfind(extension);
        if (pos != std::string::npos)
        {
            path = path.substr(0, pos);
        }

        checkDuplicated(path);
        std::strcpy(sourcePath, path.c_str());
    }

extern "C"
{
void Path_AddExt(char* sourcePath, const char* targetExtension) { Path::addExt(sourcePath, targetExtension); }
void Path_ChangeExt(char* sourcePath, const char* targetExtension) { Path::changeExt(sourcePath, targetExtension); }
void Path_RemoveExt(char* sourcePath, const char* targetExtension) { Path::removeExt(sourcePath, targetExtension); }

void Path_AlignWithAppDir(char* targetPath) { Path::alignWithAppDir(targetPath); }

char* Path_BuildFromDir(const char* parentPath, const char* filePath, const char* fileExtension, const char* pathSuffix) 
{ 
return Path::buildFromDir(parentPath, filePath, fileExtension, pathSuffix); 
}

void Path_CheckDuplicated(char* targetPath) { Path::checkDuplicated(targetPath); }
void Path_CheckExisting(const char* sourcePath, bool createFileSystem) { Path::checkexisting(sourcePath, createFileSystem); }

int Path_GetType(const char* sourcePath) { return Path::getType(sourcePath); }

void Path_CreateFS(const char* targetPath) { Path::createFS(targetPath); }

void Path_FormatWindows(char* filePath) { Path::formatWindows(filePath); }
void Path_FormatLinux(char* filePath) { Path::formatLinux(filePath); }

char* GetDownloadsFolder() { return Path::getDownloadsFolder(); }
}