#include "base_stream.h"

struct WriteOnlyStream : virtual public BaseStream
{
// ============= CONSTRUCTORS/DESTRUCTORS =============
	
	WriteOnlyStream(size_t initialSize)
    : filePath(nullptr), fileSize(initialSize), currentPos(0), mappedData(nullptr)
    #ifdef _WIN32
    , hFile(INVALID_HANDLE_VALUE), hMapping(NULL)
    #else
    , fd(-1)
    #endif
{
    if (initialSize > 0) {
        mappedData = new uint8_t[initialSize];
        fileSize = initialSize;
    }
}

WriteOnlyStream(uint8_t* externalBuffer, size_t size)
    : filePath(nullptr), fileSize(size), currentPos(0), mappedData(externalBuffer)
    #ifdef _WIN32
    , hFile(INVALID_HANDLE_VALUE), hMapping(NULL)
    #else
    , fd(-1)
    #endif
{}


    /**
     * @brief Opens a file for memory-mapped writing
     * @param filePath Path to the file (C-string)
     * @param initialSize Initial file size (will be pre-allocated)
     */
    explicit WriteOnlyStream(const char* filePath, size_t initialSize = 0)
        : filePath(filePath), fileSize(initialSize), currentPos(0) {
        CreateFile();
        if (initialSize > 0) {
            Resize(initialSize);
        }
    }

    ~WriteOnlyStream() {
        #ifdef _WIN32
        if (mappedData) UnmapViewOfFile(mappedData);
        if (hMapping) CloseHandle(hMapping);
        if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
        #else
        if (mappedData) munmap(mappedData, fileSize);
        if (fd != -1) close(fd);
        #endif
    }

    // ============= STREAM OPERATIONS =============

    /**
     * @brief Gets the current write position
     */
    size_t Position() const { return currentPos; }

    /**
     * @brief Sets the current write position
     */
    void Seek(size_t pos) {
        if (pos > fileSize) {
            throw std::out_of_range("Position exceeds file size");
        }
        currentPos = pos;
    }

    /**
     * @brief Ensures all writes are flushed to disk
     */
    void Flush() {
        #ifdef _WIN32
        if (mappedData) {
            FlushViewOfFile(mappedData, fileSize);
            FlushFileBuffers(hFile);
        }
        #else
        if (mappedData) {
            msync(mappedData, fileSize, MS_SYNC);
            fsync(fd);
        }
        #endif
    }

    // ============= PRIMITIVE WRITE METHODS =============

    void WriteByte(uint8_t value) {
        EnsureSpace(1);
        mappedData[currentPos++] = value;
    }

    void WriteBool(bool value) {
        WriteByte(value ? 1 : 0);
    }

    void WriteInt16(int16_t value, Endianness endian = Endianness::LittleEndian) {
        value = ConvertEndian(value, endian);
        WriteBytes(&value, sizeof(int16_t));
    }

    void WriteUInt16(uint16_t value, Endianness endian = Endianness::LittleEndian) {
        value = ConvertEndian(value, endian);
        WriteBytes(&value, sizeof(uint16_t));
    }

    void WriteInt32(int32_t value, Endianness endian = Endianness::LittleEndian) {
        value = ConvertEndian(value, endian);
        WriteBytes(&value, sizeof(int32_t));
    }

    void WriteUInt32(uint32_t value, Endianness endian = Endianness::LittleEndian) {
        value = ConvertEndian(value, endian);
        WriteBytes(&value, sizeof(uint32_t));
    }

    void WriteInt64(int64_t value, Endianness endian = Endianness::LittleEndian) {
        value = ConvertEndian(value, endian);
        WriteBytes(&value, sizeof(int64_t));
    }

    void WriteUInt64(uint64_t value, Endianness endian = Endianness::LittleEndian) {
        value = ConvertEndian(value, endian);
        WriteBytes(&value, sizeof(uint64_t));
    }

    void WriteFloat(float value, Endianness endian = Endianness::LittleEndian) {
        uint32_t intValue;
        std::memcpy(&intValue, &value, sizeof(float));
        WriteUInt32(intValue, endian);
    }

    void WriteDouble(double value, Endianness endian = Endianness::LittleEndian) {
        uint64_t intValue;
        std::memcpy(&intValue, &value, sizeof(double));
        WriteUInt64(intValue, endian);
    }
	
	void WriteVarInt(int32_t value) {
    WriteUVarInt(static_cast<uint32_t>(value) );
}

	void WriteUVarInt(uint32_t value) {
    while (value >= 0x80) {
        WriteByte(static_cast<uint8_t>((value & 0x7F) | 0x80));
        value >>= 7;
    }
    WriteByte(static_cast<uint8_t>(value));
}

void WriteZigZag(int32_t value) {
    uint32_t encoded = (value << 1) ^ (value >> 31);
    WriteUVarInt(encoded);
}

	void WriteVarInt64(int64_t value) {
    WriteUVarInt(static_cast<uint64_t>(value) );
}

void WriteUVarInt64(uint64_t value) {
    while (value >= 0x80) {
        WriteByte(static_cast<uint8_t>((value & 0x7F) | 0x80));
        value >>= 7;
    }
    WriteByte(static_cast<uint8_t>(value));
}

void WriteZigZag64(int64_t value) {
    uint64_t encoded = (value << 1) ^ (value >> 63);
    WriteVarInt64(encoded);
}


    // ============= BUFFER WRITE METHODS =============

    void WriteBytes(const void* data, size_t size) {
        EnsureSpace(size);
        std::memcpy(mappedData + currentPos, data, size);
        currentPos += size;
    }

    void WriteString(const char* str) {
        WriteBytes(str, std::strlen(str) );
    }

   void WriteCString(const char* str) {
        WriteBytes(str, std::strlen(str) + 1); // includes \0
    }


    void WritePadding(size_t count, uint8_t value = 0) {
        EnsureSpace(count);
        std::memset(mappedData + currentPos, value, count);
        currentPos += count;
    }

    // ============= FILE MANAGEMENT =============

    void Resize(size_t newSize) {
        if (newSize == fileSize) return;

        #ifdef _WIN32
        // Unmap current view
        if (mappedData) {
            FlushViewOfFile(mappedData, fileSize);
            UnmapViewOfFile(mappedData);
        }

        // Resize the file
        LARGE_INTEGER li;
        li.QuadPart = newSize;
        if (!SetFilePointerEx(hFile, li, NULL, FILE_BEGIN) || 
            !SetEndOfFile(hFile)) {
            throw std::runtime_error("Failed to resize file");
        }

        // Create new mapping
        if (hMapping) CloseHandle(hMapping);
        hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 
                                   0, newSize, NULL);
        if (hMapping == NULL) {
            throw std::runtime_error("Failed to recreate file mapping");
        }

        mappedData = static_cast<uint8_t*>(MapViewOfFile(
            hMapping, FILE_MAP_WRITE, 0, 0, newSize));
        if (mappedData == NULL) {
            throw std::runtime_error("Failed to remap view of file");
        }
        #else
        // Unmap current view
        if (mappedData) {
            msync(mappedData, fileSize, MS_SYNC);
            munmap(mappedData, fileSize);
        }

        // Resize the file
        if (ftruncate(fd, newSize)) {
            throw std::runtime_error("Failed to resize file");
        }

        // Create new mapping
        mappedData = static_cast<uint8_t*>(mmap(NULL, newSize, 
                                              PROT_READ | PROT_WRITE, 
                                              MAP_SHARED, fd, 0));
        if (mappedData == MAP_FAILED) {
            throw std::runtime_error("Failed to remap file");
        }
        #endif

        fileSize = newSize;
        if (currentPos > fileSize) {
            currentPos = fileSize;
        }
    }

    // ============= ACCESSORS =============

    /**
     * @brief Returns the path as a std::string
     */
    std::string GetPath() const {
        return std::string(filePath);
    }

private:

    #ifdef _WIN32
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMapping = NULL;
    #else
    int fd = -1;
    #endif

    void CreateFile() {
        #ifdef _WIN32
        hFile = CreateFileA(filePath, GENERIC_READ | GENERIC_WRITE, 
                          0, NULL, CREATE_ALWAYS, 
                          FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Failed to create file: " + std::string(filePath));
        }
        #else
        fd = open(filePath, O_RDWR | O_CREAT | O_TRUNC, 0666);
        if (fd == -1) {
            throw std::runtime_error("Failed to create file: " + std::string(filePath));
        }
        #endif
    }

    void EnsureSpace(size_t required) {
        if (currentPos + required > fileSize) {
            size_t newSize = CalculateNewSize(currentPos + required);
            Resize(newSize);
        }
    }

    size_t CalculateNewSize(size_t required) const {
        // Grow by at least 50% or required size, whichever is larger
        size_t newSize = fileSize + (fileSize / 2);
        return std::max(newSize, required);
    }

    // Delete copy operations
    WriteOnlyStream(const WriteOnlyStream&) = delete;
    WriteOnlyStream& operator=(const WriteOnlyStream&) = delete;
};
