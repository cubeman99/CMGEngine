#ifndef _CMG_CORE_IO_FILE_H_
#define _CMG_CORE_IO_FILE_H_

#include <cmgCore/io/cmgPath.h>
#include <cmgCore/cmgBase.h>
#include <cmgCore/error/cmgError.h>
#include <cmgCore/containers/cmgArray.h>


// Access policies
enum class FileAccess
{
	READ,
	WRITE,
	APPEND,
	READ_AND_WRITE,
	READ_AND_WRITE_EMPTY,
	READ_AND_APPEND,
};
	

// File type policies
enum class FileType
{
	TEXT,
	BINARY,
};


// File
class File
{
public:
	File();
	File(const Path& path);
	File(const File& other);
	~File();

	// Open & close
	Error Open(const Path& path, FileAccess access, FileType type);
	Error Open(const PathU16& path, FileAccess access, FileType type);
	Error Open(FileAccess access, FileType type);
	bool IsOpen() const;
	Error Close();

	// Read & write
	uint32 Tell();
	Error SeekFromEnd(uint32 offset);
	Error SeekFromStart(uint32 offset);
	Error SeekFromCurrent(uint32 offset);
	Error Read(void* destination, uint32 size);
	Error Write(const void* data, uint32 size);
	Error GetContents(String& out);
	Error GetContents(Array<uint8>& out);

	// Static methods
	static Error OpenAndGetContents(const Path& path, String& out);
	static Error OpenAndGetContents(const Path& path, Array<uint8>& out);
	static Error OpenAndWriteContents(const Path& path, const String& text);
	static Error OpenAndWriteContents(const Path& path, const Array<uint8>& data);
	static Error OpenAndWriteContents(const Path& path, const uint8* data, size_t count);

private:
	template<class T_Path>
	static errno_t DoOpen(FILE** file, const T_Path& path, const char* mode)
	{
		return -1;
	}

	Path m_path;
	FILE* m_file;
	FileType m_fileType;
	FileAccess m_accessMode;
};

extern template errno_t File::DoOpen<Path>(FILE** file, const Path& path, const char* mode);
extern template errno_t File::DoOpen<PathU16>(FILE** file, const PathU16& path, const char* mode);


#endif // _CMG_CORE_IO_FILE_H_