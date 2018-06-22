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
	File(const Path& path);
	File(const File& other);
	~File();

	// Open & close.
	Error Open(FileAccess access, FileType type);
	bool IsOpen() const;
	Error Close();

	// Read & write.
	Error Read(void* destination, unsigned int size);
	Error Write(const void* data, unsigned int size);
	Error GetContents(String& out);
	Error GetContents(Array<unsigned char>& out);

	static Error OpenAndGetContents(const Path& path, String& out);
	static Error OpenAndGetContents(const Path& path, Array<unsigned char>& out);

private:
	Path m_path;
	FILE* m_file;
	FileType m_fileType;
	FileAccess m_accessMode;
};


#endif // _CMG_CORE_IO_FILE_H_