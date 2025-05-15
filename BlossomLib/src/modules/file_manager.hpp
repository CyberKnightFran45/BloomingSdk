#pragma once

#include "../global/callback.h"
#include "../global/typedefs/streams/readonly_stream.h"
#include "../global/typedefs/streams/writeonly_stream.h"
#include <string>
#include <memory>
#include <functional>
#include <filesystem>

namespace fs = std::filesystem;

namespace BlossomLib::Modules::FileManager
{
// ================== CORE STREAM OPERATIONS ==================

/**
 * @brief Opens a file as read-only stream with optimized settings
 * @param filePath Path to the file
 */
 
inline std::unique_ptr<ReadOnlyStream> openRead(const char* filePath)
{
    return std::make_unique<ReadOnlyStream>(filePath);
}

/**
 * @brief Opens a file as write-only stream with optimized settings
 * @param filePath Path to the file
 */
 
inline std::unique_ptr<WriteOnlyStream> openWrite(const char* filePath)
{
    return std::make_unique<WriteOnlyStream>(filePath);
}

/**
 * @brief High-performance buffer processing between streams
 * @param input Source read-only stream
 * @param output Target write-only stream
 * @param maxBytes Maximum bytes to process (-1 for unlimited)
 * @param processFunc Optional processing callback (nullptr for direct copy)
 * @throws std::runtime_error if stream operations fail
 */

inline void process(ReadOnlyStream& input, WriteOnlyStream& output, int64_t maxBytes, ProgressCallback progress,
                    std::function<size_t(const uint8_t* inData, size_t dataSize, void*& outData)> transform = nullptr)
{
    size_t totalProcessed = 0;
    bool unlimited = maxBytes < 0;
	
    const uint8_t* fileData = input.Data();
    size_t fileSize = input.Size();
	
    size_t limit = unlimited ? fileSize : std::min(static_cast<size_t>(maxBytes), fileSize);

    for (; totalProcessed < limit;)
    {
        size_t toProcess = limit - totalProcessed;
        const uint8_t* currentPtr = fileData + totalProcessed;

        try 
		{
            if (transform) {
                void* transformed = nullptr;
                size_t outSize = transform(currentPtr, toProcess, transformed);
                output.WriteBytes(transformed, outSize);
                free(transformed);
            } else {
                output.WriteBytes(currentPtr, toProcess);
            }

            totalProcessed += toProcess;

            if (progress) {
                std::string msg = fmt::format("Processed {}/{} bytes", totalProcessed, limit);
                progress(totalProcessed, limit, msg, false);
            }
        }
		
        catch (const std::exception& ex) {
            if (progress) {
                std::string msg = fmt::format("Error during processing: {}", ex.what());
                progress(totalProcessed, limit, msg, true);
            }
            break;
        }
    }

    output.Flush();
}

// ================== FILE OPERATIONS ==================

/**
 * @brief Copies a file with optional overwrite
 * @param sourcePath Source file path
 * @param destPath Destination directory path
 * @param replaceFile Whether to replace existing file
 * @throws std::runtime_error if operation fails
 */
inline void copy(
    const std::string& sourcePath,
    const std::string& destPath,
    bool replaceFile = false)
{
    fs::path source(sourcePath);
    fs::path destination(destPath);

    if (!fs::exists(source)) {
        throw std::runtime_error("Source file does not exist: " + sourcePath);
    }

    if (fs::is_directory(destination)) {
        destination /= source.filename();
    }

    if (fs::exists(destination) && !replaceFile) {
        throw std::runtime_error("Destination file exists: " + destination.string());
    }

    fs::copy_options options = replaceFile ? 
        fs::copy_options::overwrite_existing : 
        fs::copy_options::none;
    
    fs::copy_file(source, destination, options);
}

/**
 * @brief Moves/renames a file with optional overwrite
 * @param sourcePath Source file path
 * @param destPath Destination path
 * @param replaceFile Whether to replace existing file
 * @throws std::runtime_error if operation fails
 */
inline void move(
    const std::string& sourcePath,
    const std::string& destPath,
    bool replaceFile = false)
{
    fs::path source(sourcePath);
    fs::path destination(destPath);

    if (!fs::exists(source)) {
        throw std::runtime_error("Source file does not exist: " + sourcePath);
    }

    if (fs::exists(destination)) {
        if (replaceFile) {
            fs::remove(destination);
        } else {
            throw std::runtime_error("Destination file already exists: " + destPath);
        }
    }

    fs::rename(source, destination);
}

}

namespace fileMgr = BlossomLib::Modules::FileManager;