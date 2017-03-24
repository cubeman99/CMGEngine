#ifndef _CMG_CORE_IO_FILE_H_
#define _CMG_CORE_IO_FILE_H_

#include <cmgCore/io/cmgPath.h>
#include <cmgCore/cmgBase.h>
#include <cmgCore/error/cmgError.h>
#include <cmgCore/containers/cmgArray.h>


// Access policies
struct FileAccess
{
	typedef int value_type;

	enum
	{
		k_read,
		k_write,
		k_append,
		k_read_and_write,
		k_read_and_write_empty,
		k_read_and_append,
	};
};
	
// File type policies
struct FileType
{
	typedef int value_type;

	enum
	{
		k_text,
		k_binary,
	};
};

// File
class File
{
public:
	typedef FileAccess::value_type	file_access;
	typedef FileType::value_type	file_type;

public:
	File(const Path& path);
	File(const File& other);
	~File();

	// Open & close.
	Error	Open(file_access access, file_type type);
	bool	IsOpen() const;
	Error	Close();

	// Wread & write.
	Error	Read(unsigned char* destination, unsigned int size);
	Error	Write(const unsigned char* data, unsigned int size);
	Error	GetContents(String& out);
	Error	GetContents(Array<unsigned char>& out);

	static Error OpenAndGetContents(const Path& path, String& out);
	static Error OpenAndGetContents(const Path& path, Array<unsigned char>& out);

private:
	Path		m_path;
	FILE*		m_file;
	file_access	m_fileType;
	file_type	m_accessMode;
};


#endif // _CMG_CORE_IO_FILE_H_