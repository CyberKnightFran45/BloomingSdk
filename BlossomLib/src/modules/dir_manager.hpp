#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <functional>
#include <system_error>

namespace fs = std::filesystem;

namespace BlossomLib::Modules::DirectoryManager
{
/**
 * @brief Changes a path to a new one, handling duplicates
 * @param oldPath Original path
 * @param newPath Desired new path
 * @return Actual path that will be used (may be modified if duplicate exists)
 */
inline std::string ChangePath(const std::string& oldPath, const std::string& newPath) {
    if (newPath == oldPath) {
        auto parent_path = fs::path(newPath).parent_path();
        auto folder_name = fs::path(newPath).filename();
        std::string unique_path;
        int counter = 1;
        
        do {
            unique_path = (parent_path / (std::to_string(counter++) + "_" + folder_name.string())).string();
        } while (fs::exists(unique_path));
        
        return unique_path;
    }
    return newPath;
}

/**
 * @brief Ensures directory exists, creates if missing
 * @param targetPath Path to check/create
 * @return fs::path object for the directory
 * @throws std::runtime_error if directory cannot be created
 */
inline fs::path CheckMissingFolder(const std::string& targetPath) 
{
    fs::path path(targetPath);
    std::error_code ec;
    
    if (!fs::exists(path, ec)) {
        if (!fs::create_directories(path, ec) || ec) {
            throw std::runtime_error("Failed to create directory: " + targetPath);
        }
    }
    return path;
}

/**
 * @brief Copies a directory recursively with filtering options
 * @param sourcePath Source directory path
 * @param destPath Destination directory path
 * @param fileFilter Filter function for files (return true to copy)
 * @param dirFilter Filter function for directories (return true to copy)
 * @throws std::runtime_error if operation fails
 */
inline void CopyFolder(
    const std::string& sourcePath,
    const std::string& destPath,
    const std::function<bool(const fs::path&)>& fileFilter = nullptr,
    const std::function<bool(const fs::path&)>& dirFilter = nullptr) 
{
    fs::path source(sourcePath);
    fs::path destination(destPath);
    
    if (!fs::exists(source)) {
        throw std::runtime_error("Source directory does not exist: " + sourcePath);
    }
    
    CheckMissingFolder(destPath);
    
    auto copyOptions = fs::copy_options::recursive | 
                      fs::copy_options::overwrite_existing;
    
    if (fileFilter || dirFilter) {
        for (const auto& entry : fs::recursive_directory_iterator(source)) {
            if (fs::is_directory(entry)) {
                if (!dirFilter || dirFilter(entry)) {
                    fs::path relative = fs::relative(entry, source);
                    fs::create_directories(destination / relative);
                }
            } else {
                if (!fileFilter || fileFilter(entry)) {
                    fs::path relative = fs::relative(entry, source);
                    fs::copy(entry, destination / relative, copyOptions);
                }
            }
        }
    } else {
        fs::copy(source, destination, copyOptions);
    }
}

/**
 * @brief Creates a symbolic link to a directory
 * @param sourcePath Target directory path
 * @param linkPath Path for the new link
 * @throws std::runtime_error if link creation fails
 */
inline void CreateSymbolicLink(
    const std::string& sourcePath,
    const std::string& linkPath) 
{
    std::error_code ec;
    fs::create_directory_symlink(sourcePath, linkPath, ec);
    if (ec) {
        throw std::runtime_error("Failed to create symbolic link: " + ec.message());
    }
}

/**
 * @brief Creates a new directory
 * @param targetPath Path for the new directory
 * @throws std::runtime_error if creation fails
 */
inline void CreateFolder(const std::string& targetPath) {
    std::error_code ec;
    if (!fs::create_directories(targetPath, ec)  ){
        return;
    }
}

/**
 * @brief Deletes a directory
 * @param targetPath Path to directory to delete
 * @param recursive Whether to delete contents recursively
 * @throws std::runtime_error if deletion fails
 */
inline void DeleteFolder(const std::string& targetPath, bool recursive = true) {
    std::error_code ec;
    if (recursive) {
        fs::remove_all(targetPath, ec);
    } else {
        fs::remove(targetPath, ec);
    }
    if (ec) {
        throw std::runtime_error("Failed to delete directory: " + targetPath);
    }
}

/**
 * @brief Gets a container path with optional prefix
 * @param targetPath Base path
 * @param namePrefix Prefix for container name
 * @return Full container path
 */
inline std::string GetContainerPath(
    const std::string& targetPath,
    const std::string& namePrefix = "FilesContainer") 
{
    fs::path path(targetPath);
    auto containerPath = path.parent_path() / namePrefix / path.stem();
    CheckMissingFolder(containerPath.string());
    return containerPath.string();
}

/**
 * @brief Gets directory entries with optional filtering
 * @param targetPath Directory path to scan
 * @param getFiles Whether to get files (true) or directories (false)
 * @param pattern Wildcard pattern for filtering
 * @param recursive Whether to search recursively
 * @return Vector of paths matching criteria
 */
inline std::vector<std::string> GetFileSystems(
    const std::string& targetPath,
    bool getFiles = true,
    const std::string& pattern = "*",
    bool recursive = false) 
{
    std::vector<std::string> results;
    
    if (!fs::exists(targetPath)) {
        return results;
    }
    
    auto scan = [&](const auto& dir_iter) {
        for (const auto& entry : dir_iter) {
            if ((getFiles && entry.is_regular_file()) || 
                (!getFiles && entry.is_directory())) {
                if (pattern == "*" || entry.path().filename().string().find(pattern) != std::string::npos) {
                    results.push_back(entry.path().string());
                }
            }
        }
    };
    
    if (recursive) {
        scan(fs::recursive_directory_iterator(targetPath));
    } else {
        scan(fs::directory_iterator(targetPath));
    }
    
    return results;
}

/**
 * @brief Gets the name of a directory from its path
 * @param targetPath Full directory path
 * @return Directory name
 */
inline std::string GetFolderName(const std::string& targetPath) {
    return fs::path(targetPath).filename().string();
}

/**
 * @brief Calculates total size of a directory
 * @param targetPath Directory path
 * @return Size in bytes
 */
inline uint64_t GetFolderSize(const std::string& targetPath) {
    uint64_t totalSize = 0;
    for (const auto& entry : fs::recursive_directory_iterator(targetPath)) {
        if (entry.is_regular_file()) {
            totalSize += entry.file_size();
        }
    }
    return totalSize;
}

/**
 * @brief Checks if directory is empty
 * @param targetPath Directory path
 * @return true if directory is empty
 */
inline bool FolderIsEmpty(const std::string& targetPath) {
    return fs::is_empty(targetPath);
}

/**
 * @brief Moves a directory to new location
 * @param sourcePath Original directory path
 * @param destPath New parent directory path
 * @throws std::runtime_error if operation fails
 */
inline void MoveFolder(const std::string& sourcePath, const std::string& destPath) {
    fs::path source(sourcePath);
    fs::path destination = fs::path(destPath) / source.filename();
    
    if (fs::exists(destination)) {
        destination = fs::path(ChangePath(sourcePath, destination.string()));
    }
    
    std::error_code ec;
    fs::rename(source, destination, ec);
    if (ec) {
        throw std::runtime_error("Failed to move directory: " + ec.message());
    }
}

/**
 * @brief Renames a directory
 * @param sourcePath Original directory path
 * @param newName New directory name
 * @throws std::runtime_error if operation fails
 */
inline void RenameFolder(const std::string& sourcePath, const std::string& newName) {
    fs::path source(sourcePath);
    fs::path newPath = source.parent_path() / newName;
    
    if (fs::exists(newPath)) {
        newPath = fs::path(ChangePath(sourcePath, newPath.string()));
    }
    
    std::error_code ec;
    fs::rename(source, newPath, ec);
    if (ec) {
        throw std::runtime_error("Failed to rename directory: " + ec.message());
    }
}

}

namespace dirMgr = BlossomLib::Modules::DirectoryManager;