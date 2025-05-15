#include "../utils/input/helper.h"
#include "base_stream.h"

/**
 * @struct ReadOnlyStream
 * @brief High-performance read-only binary stream using memory-mapped I/O
 */
 
struct ReadOnlyStream : virtual public BaseStream
{
// ============= CONSTRUCTORS/DESTRUCTORS =============
	
	    /**
     * @brief Default constructor.  Creates an empty stream.
     */
explicit ReadOnlyStream(size_t bufferSize)
    : filePath(nullptr), fileSize(bufferSize), currentPos(0),
      mappedData(malloc(bufferSize) ), isMemoryBuffer(true) {}


    /**
     * @brief Construct from file path (as C-string)
     * @param filePath Path to the file
     */
    explicit ReadOnlyStream(const char* path)
        : filePath(path), fileSize(0), currentPos(0), mappedData(nullptr) {
        OpenFile();
        MapFile();
    }

    /**
     * @brief Construct from memory buffer
     * @param data Pointer to memory buffer
     * @param size Size of buffer in bytes
     */
    ReadOnlyStream(const uint8_t* data, size_t size)
        : filePath(nullptr), fileSize(size), currentPos(0),
          mappedData(const_cast<uint8_t*>(data)), isMemoryBuffer(true) {}

    ~ReadOnlyStream() {
        #ifdef _WIN32
        if (mappedData && !isMemoryBuffer) UnmapViewOfFile(mappedData);
        if (hMapping) CloseHandle(hMapping);
        if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
        #else
        if (mappedData && !isMemoryBuffer) munmap(mappedData, fileSize);
        if (fd != -1) close(fd);
        #endif
    }

    std::string GetPath() const { return filePath ? std::string(filePath) : ""; }

    // ============= STREAM OPERATIONS =============

    size_t Position() const noexcept { return currentPos; }

    void Seek(size_t pos) {
        if (pos > fileSize) {
            throw std::out_of_range("Position exceeds file size");
        }
        currentPos = pos;
    }

    void Skip(size_t bytes) {
        if (currentPos + bytes > fileSize) {
            throw std::out_of_range("Skip would exceed file size");
        }
        currentPos += bytes;
    }

    explicit operator bool() const noexcept {
        return mappedData != nullptr;
    }

    int8_t ReadInt8() {
        CheckAvailable(sizeof(int8_t));
        return mappedData[currentPos++];
    }

    uint8_t ReadUInt8() {
        CheckAvailable(sizeof(uint8_t));
        return mappedData[currentPos++];
    }

    bool ReadBool() {
        return ReadUInt8() != 0;
    }

    char ReadChar() {
        return static_cast<char>(ReadUInt8());
    }

    int16_t ReadInt16(Endianness endian = Endianness::LittleEndian) {
        CheckAvailable(sizeof(int16_t));
        int16_t value;
        std::memcpy(&value, mappedData + currentPos, sizeof(int16_t));
        currentPos += sizeof(int16_t);
        return ConvertEndian(value, endian);
    }

    uint16_t ReadUInt16(Endianness endian = Endianness::LittleEndian) {
        return static_cast<uint16_t>(ReadInt16(endian));
    }
	
	int32_t ReadInt24(Endianness endian = Endianness::LittleEndian) {
    uint32_t inputValue = ReadUInt24(endian);

    if ((inputValue & 0x800000) != 0) {
        inputValue |= 0xFF000000;
    }

    return static_cast<int32_t>(inputValue);
}

	uint32_t ReadUInt24(Endianness endian = Endianness::LittleEndian) {
    CheckAvailable(3);
    uint32_t value = 0;
    std::memcpy(reinterpret_cast<uint8_t*>(&value), mappedData + currentPos, 3);
    currentPos += 3;
    return ConvertEndian(value, endian);
}

    int32_t ReadInt32(Endianness endian = Endianness::LittleEndian) {
        CheckAvailable(sizeof(int32_t));
        int32_t value;
        std::memcpy(&value, mappedData + currentPos, sizeof(int32_t));
        currentPos += sizeof(int32_t);
        return ConvertEndian(value, endian);
    }

    uint32_t ReadUInt32(Endianness endian = Endianness::LittleEndian) {
        return static_cast<uint32_t>(ReadInt32(endian));
    }

    int64_t ReadInt64(Endianness endian = Endianness::LittleEndian) {
        CheckAvailable(sizeof(int64_t));
        int64_t value;
        std::memcpy(&value, mappedData + currentPos, sizeof(int64_t));
        currentPos += sizeof(int64_t);
        return ConvertEndian(value, endian);
    }

    uint64_t ReadUInt64(Endianness endian = Endianness::LittleEndian) {
        return static_cast<uint64_t>(ReadInt64(endian));
    }

    float ReadFloat(Endianness endian = Endianness::LittleEndian) {
        uint32_t intValue = ReadUInt32(endian);
        float value;
        std::memcpy(&value, &intValue, sizeof(float));
        return value;
    }

    double ReadDouble(Endianness endian = Endianness::LittleEndian) {
        uint64_t intValue = ReadUInt64(endian);
        double value;
        std::memcpy(&value, &intValue, sizeof(double));
        return value;
    }
	
int32_t ReadVarInt() {
    int32_t varInt = 0;
    int integerBase = 0;
    uint8_t inputValue;

    do {
        if (integerBase == 35) {
            throw std::runtime_error("Not a 32-bits Integer");
        }

        inputValue = ReadUInt8();
        varInt |= static_cast<int32_t>(inputValue & 0x7F) << integerBase;
        integerBase += 7;
    }
    while ((inputValue & 0x80) != 0);

    return varInt;
}

uint32_t ReadUVarInt()
{
return static_cast<uint32_t>(ReadVarInt() );
}

int64_t ReadVarInt64() {
    int64_t varInt = 0;
    int integerBase = 0;
    uint8_t inputValue;

    do {
        if (integerBase == 70) {
            throw std::runtime_error("Not a 64-bits Integer");
        }

        inputValue = ReadUInt8();
        varInt |= static_cast<int64_t>(inputValue & 0x7F) << integerBase;
        integerBase += 7;
    }
    while ((inputValue & 0x80) != 0);

    return varInt;
}

uint32_t ReadUVarInt64()
{
return static_cast<uint64_t>(ReadVarInt64() );
}


int32_t ReadZigZag() {
    uint32_t inputValue = ReadUVarInt();

    if ((inputValue & 0b1) == 0) {
        return static_cast<int32_t>(inputValue >> 1);
    } else {
        return -static_cast<int32_t>((inputValue + 1) >> 1);
    }
}

int64_t ReadZigZag64() {
    uint64_t inputValue = ReadUVarLong();

    if ((inputValue & 0b1) == 0) {
        return static_cast<int64_t>(inputValue >> 1);
    } else {
        return -static_cast<int64_t>((inputValue + 1) >> 1);
    }
}


    void ReadBytes(uint8_t* buffer, size_t count) {
        CheckAvailable(count);
        std::memcpy(buffer, mappedData + currentPos, count);
        currentPos += count;
    }

const char* ReadString(size_t length) {
    CheckAvailable(length);
    const char* strPtr = reinterpret_cast<const char*>(mappedData + currentPos);
    currentPos += length;
    return strPtr;
}

const char* ReadCString() {
    const char* start = reinterpret_cast<const char*>(mappedData + currentPos);
    size_t maxLength = fileSize - currentPos;
    size_t length = 0;

    while (length < maxLength && start[length] != '\0') {
        length++;
    }

    if (length == maxLength) {
        throw std::runtime_error("Null terminator not found");
    }

    currentPos += length + 1;
    return start;
}


    char* ReadStrByLen(size_t lengthBytes = 4, Endianness endian = Endianness::LittleEndian) {
        size_t length = 0;
        switch (lengthBytes) {
            case 1: length = ReadUInt8(); break;
            case 2: length = ReadUInt16(endian); break;
            case 4: length = ReadUInt32(endian); break;
			case 8: length = ReadUInt64(endian); break;
            default: length = lengthBytes;
        }
		
        return ReadString(length);
    }
	
	// COMPARER
	
	   	bool CompareUInt8(const uint8_t target, ProgressCallback progress, 
		                   Endianness endian = Endianness::LittleEndian) 
		{
		  uint8_t source = ReadUInt8(endian);
		  auto result = source == target;
		  
		  if (!result && progress)
          {
           auto msg = fmt::format("UInt8 Mismatch at Address {}: read ({}) - expected ({})",
                                   pos, source, target);
           
           progress(0, -1, msg, true);
          }
         
          return result
        }
		  
	
	    bool CompareUInt16(const uint16_t target, ProgressCallback progress, 
		                   Endianness endian = Endianness::LittleEndian) 
		{
		  uint16_t source = ReadUInt16(endian);
		  auto result = source == target;
		  
		  if (!result && progress)
          {
           auto msg = fmt::format("UInt16 Mismatch at Address {}: read ({}) - expected ({})", 
                                   pos, source, target);

           progress(0, -1, msg, true);
          }
	
        return result;
        }
	
	bool CompareStr(const char* target, ProgressCallback progress)
	{
		size_t strLen = strlen(target);
		char* source = ReadString(strLen);
		
		auto result = memcmp(source, target, strLen) == 0;
		
		  if (!result && progress)
          {
           auto msg = fmt::format("Str Mismatch at Address {}: read \"{}\" - expected \"{}\"",
                                   pos, source, target);
                                   
           progress(0, -1, msg, true);
          }
		
		return result;
	}
	
	bool CheckSize64(size_t limit, ProgressCallback progress,
	                 Endianness endian = Endianness::LittleEndian)
	{
		auto size = static_cast<size_t>(ReadUInt64(endian) );
		auto result = size > limit;
		
		  if (!result && progress)
          {
			auto displaySize = Input::formatSize(size); 
			auto displayLimit = Input::formatSize(limit);
			
           auto msg = fmt::format("Size Exceded: {} - Limit: {}", displaySize, displayLimit);
           progress(0, -1, msg, true);
          }
		
		return result;
	}

    size_t Size() const noexcept { return fileSize; }
    const uint8_t* Data() const noexcept { return mappedData; }

private:

    #ifdef _WIN32
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMapping = NULL;
    #else
    int fd = -1;
    #endif

    void OpenFile() {
        #ifdef _WIN32
        hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ,
                            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            throw std::runtime_error(std::string("Failed to open file: ") + filePath);
        }

        LARGE_INTEGER size;
        if (!GetFileSizeEx(hFile, &size)) {
            CloseHandle(hFile);
            throw std::runtime_error("Failed to get file size");
        }
        fileSize = static_cast<size_t>(size.QuadPart);
        #else
        fd = open(filePath, O_RDONLY);
        if (fd == -1) {
            throw std::runtime_error(std::string("Failed to open file: ") + filePath);
        }

        struct stat st;
        if (fstat(fd, &st) == -1) {
            close(fd);
            throw std::runtime_error("Failed to get file size");
        }
        fileSize = static_cast<size_t>(st.st_size);
        #endif
    }

    void MapFile() {
        if (fileSize == 0) {
            mappedData = nullptr;
            return;
        }

        #ifdef _WIN32
        hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
        if (hMapping == NULL) {
            CloseHandle(hFile);
            throw std::runtime_error("Failed to create file mapping");
        }

        mappedData = static_cast<uint8_t*>(MapViewOfFile(
            hMapping, FILE_MAP_READ, 0, 0, fileSize));
        if (mappedData == NULL) {
            CloseHandle(hMapping);
            CloseHandle(hFile);
            throw std::runtime_error("Failed to map view of file");
        }
        #else
        mappedData = static_cast<uint8_t*>(mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fd, 0));
        if (mappedData == MAP_FAILED) {
            close(fd);
            throw std::runtime_error("Failed to map file");
        }

        madvise(mappedData, fileSize, MADV_SEQUENTIAL);
        #endif
    }

    void CheckAvailable(size_t required) {
        if (currentPos + required > fileSize) {
            throw std::out_of_range("Attempt to read beyond end of file");
        }
    }

    ReadOnlyStream(const ReadOnlyStream&) = delete;
    ReadOnlyStream& operator=(const ReadOnlyStream&) = delete;
};