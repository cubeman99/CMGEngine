#include "cmgFile.h"
#include <cmgCore/cmgAssert.h>


/*
// Yes, this is a lot of repetition but this is done only once and in
// return we get type safety (that is, we will never have to find out at
// run-time that we used incorrect file access parameters)

FILE* DoOpen(const char* a_fileName,
                file_modes::Read, file_modes::Ascii)
{ return fopen(a_fileName, "r"); }

FILE* DoOpen(const char* a_fileName,
                file_modes::Read, file_modes::Binary)
{ return fopen(a_fileName, "rb"); }

FILE* DoOpen(const char* a_fileName,
                file_modes::Write, file_modes::Ascii)
{ return fopen(a_fileName, "w"); }

FILE* DoOpen(const char* a_fileName,
                file_modes::Write, file_modes::Binary)
{ return fopen(a_fileName, "wb"); }

FILE* DoOpen(const char* a_fileName,
                file_modes::Append, file_modes::Ascii)
{ return fopen(a_fileName, "a"); }

FILE* DoOpen(const char* a_fileName,
                file_modes::Append, file_modes::Binary)
{ return fopen(a_fileName, "ab"); }

FILE* DoOpen(const char* a_fileName,
                file_modes::ReadAndWrite, file_modes::Ascii)
{ return fopen(a_fileName, "r+"); }

FILE* DoOpen(const char* a_fileName,
                file_modes::ReadAndWrite, file_modes::Binary)
{ return fopen(a_fileName, "r+b"); }

FILE* DoOpen(const char* a_fileName,
                file_modes::ReadAndWriteEmpty, file_modes::Ascii)
{ return fopen(a_fileName, "w+"); }

FILE* DoOpen(const char* a_fileName,
                file_modes::ReadAndWriteEmpty, file_modes::Binary)
{ return fopen(a_fileName, "w+b"); }

FILE* DoOpen(const char* a_fileName,
                file_modes::ReadAndAppend, file_modes::Ascii)
{ return fopen(a_fileName, "a+"); }

FILE* DoOpen(const char* a_fileName,
                file_modes::ReadAndAppend, file_modes::Binary)
{ return fopen(a_fileName, "a+b"); }

	
#define FILE_TEMP    typename T_AccessPolicy, typename T_FileFormat
#define FILE_PARAMS  T_AccessPolicy, T_FileFormat
#define FILE_TYPE    typename File
*/

//-----------------------------------------------------------------------------
// Constructors & Destructor
//-----------------------------------------------------------------------------
	
File::File(const Path& path) :
	m_path(path),
	m_file(nullptr)
{
}

File::File(const File& other) :
	m_path(other.m_path),
	m_file(nullptr)
{
}
	
File::~File()
{
	Close();
}


//-----------------------------------------------------------------------------
// Open & Close
//-----------------------------------------------------------------------------
	
Error File::Open(file_access access, file_type type)
{
	if (type != FileType::k_binary && type != FileType::k_text)
		CMG_ASSERT_FALSE("Invalid file type (must be binary or text)");
	if (!m_path.HasFilename())
		return CMG_ERROR(CommonErrorTypes::k_path_incorrect);
				
	// Determine the open mode string.
	const char* mode;
	if (access == FileAccess::k_read)
		mode = (type == FileType::k_binary ? "rb" : "r");
	else if (access == FileAccess::k_write)
		mode = (type == FileType::k_binary ? "wb" : "w");
	else if (access == FileAccess::k_append)
		mode = (type == FileType::k_binary ? "ab" : "a");
	else if (access == FileAccess::k_read_and_write)
		mode = (type == FileType::k_binary ? "r+b" : "r+");
	else if (access == FileAccess::k_read_and_write_empty)
		mode = (type == FileType::k_binary ? "w+b" : "w+");
	else if (access == FileAccess::k_read_and_append)
		mode = (type == FileType::k_binary ? "a+b" : "a+");
	else
		CMG_ASSERT_FALSE("Invalid file access mode");
		
	// Open the file.
	errno_t fopenError = fopen_s(&m_file, m_path.c_str(), mode);

	m_fileType = type;
	m_accessMode = access;

	if (fopenError != 0)
		return CMG_ERROR_FAILURE;
	return CMG_ERROR_SUCCESS;
}
	
bool File::IsOpen() const
{
	return (m_file != nullptr);
}
	
Error File::Close()
{
	if (m_file)
	{
		if (fclose(m_file) == EOF)
		{
			return CMG_ERROR_FAILURE;
		}
	}

	m_file = nullptr;
	return CMG_ERROR_SUCCESS;
}


//-----------------------------------------------------------------------------
// Read & Write
//-----------------------------------------------------------------------------
	
Error File::Read(unsigned char* destination, unsigned int size)
{
	CMG_ASSERT_MSG(IsOpen(), "Attempting to read a file that is not open");

	if (fread(destination, 1, size, m_file) != size)
		return CMG_ERROR(CommonErrorTypes::k_file_read);

	return CMG_ERROR_SUCCESS; 
}
	
Error File::Write(const unsigned char* data, unsigned int size)
{
	CMG_ASSERT_MSG(IsOpen(), "Attempting to write/append to a file that is not open");

	if (fwrite(data, 1, size, m_file) != size)
		return CMG_ERROR(CommonErrorTypes::k_file_write);

	return CMG_ERROR_SUCCESS; 
}
	
Error File::GetContents(String& out)
{
	CMG_ASSERT_MSG(m_file, "No file to read - did you forget to call Open()?");

	if (fseek(m_file, 0, SEEK_END) != 0)
		return CMG_ERROR_FAILURE;

	long fileSize = ftell(m_file);
	if (fileSize < 0)
		return CMG_ERROR_FAILURE;

	rewind(m_file);

	const size_t fileSizeInclNull = fileSize + 1;
	char* buffer = (char*) CMG_MALLOC(sizeof(char) * fileSizeInclNull);

	size_t result = fread(buffer, 1, fileSizeInclNull, m_file);
	if (result == 0)
	{
		CMG_FREE(buffer);
		return CMG_ERROR_FAILURE;
	}

	buffer[result] = '\0';
	out.assign(buffer, buffer + result);

	CMG_FREE(buffer);

	return CMG_ERROR_SUCCESS;
}
	
Error File::GetContents(Array<unsigned char>& out)
{
	CMG_ASSERT_MSG(m_file, "No file to read - did you forget to call Open()?");

	if (fseek(m_file, 0, SEEK_END) != 0)
		return CMG_ERROR_FAILURE;

	long fileSize = ftell(m_file);
	if (fileSize < 0)
		return CMG_ERROR_FAILURE;

	rewind(m_file);

	//const size_t fileSizeInclNull = fileSize;
	char* buffer = (char*) CMG_MALLOC(sizeof(char) * fileSize);

	size_t result = fread(buffer, 1, fileSize, m_file);
	if (result == 0)
		return CMG_ERROR_FAILURE;

	out.assign(buffer, buffer + result);

	CMG_FREE(buffer);

	return CMG_ERROR_SUCCESS;
}

	
Error File::OpenAndGetContents(const Path& path, String& out)
{
	Error error;
	File file(path);
	error = file.Open(FileAccess::k_read, FileType::k_text);
	if (error.Failed())
		return error;
	return file.GetContents(out);
}
	
Error File::OpenAndGetContents(const Path& path, Array<unsigned char>& out)
{
	Error error;
	File file(path);
	error = file.Open(FileAccess::k_read, FileType::k_binary);
	if (error.Failed())
		return error;
	return file.GetContents(out);
}
	
