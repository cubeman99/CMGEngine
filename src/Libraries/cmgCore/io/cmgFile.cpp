#include "cmgFile.h"
#include <cmgCore/cmgAssert.h>


//-----------------------------------------------------------------------------
// Constructors & Destructor
//-----------------------------------------------------------------------------
	
File::File() :
	m_path(""),
	m_file(nullptr)
{
}

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
	
Error File::Open(const Path& path, FileAccess access, FileType type)
{
	if (type != FileType::BINARY && type != FileType::TEXT)
		CMG_ASSERT_FALSE("Invalid file type (must be binary or text)");

	// Determine the open mode string.
	const char* mode;
	if (access == FileAccess::READ)
		mode = (type == FileType::BINARY ? "rb" : "r");
	else if (access == FileAccess::WRITE)
		mode = (type == FileType::BINARY ? "wb" : "w");
	else if (access == FileAccess::APPEND)
		mode = (type == FileType::BINARY ? "ab" : "a");
	else if (access == FileAccess::READ_AND_WRITE)
		mode = (type == FileType::BINARY ? "r+b" : "r+");
	else if (access == FileAccess::READ_AND_WRITE_EMPTY)
		mode = (type == FileType::BINARY ? "w+b" : "w+");
	else if (access == FileAccess::READ_AND_APPEND)
		mode = (type == FileType::BINARY ? "a+b" : "a+");
	else
		CMG_ASSERT_FALSE("Invalid file access mode");

	// Open the file
	errno_t fopenError = fopen_s(&m_file, path.c_str(), mode);

	m_fileType = type;
	m_accessMode = access;

	if (fopenError == ENOENT)
		return CMG_ERROR_FILE_NOT_FOUND;
	else if (fopenError == EISDIR)
		return CMG_ERROR(CommonErrorTypes::k_path_incorrect);
	else if (fopenError != 0)
		return CMG_ERROR_FAILURE;
	return CMG_ERROR_SUCCESS;
}

Error File::Open(const PathU16& path, FileAccess access, FileType type)
{
	if (type != FileType::BINARY && type != FileType::TEXT)
		CMG_ASSERT_FALSE("Invalid file type (must be binary or text)");

	// Determine the open mode string.
	const wchar_t* mode;
	if (access == FileAccess::READ)
		mode = (type == FileType::BINARY ? L"rb" : L"r");
	else if (access == FileAccess::WRITE)
		mode = (type == FileType::BINARY ? L"wb" : L"w");
	else if (access == FileAccess::APPEND)
		mode = (type == FileType::BINARY ? L"ab" : L"a");
	else if (access == FileAccess::READ_AND_WRITE)
		mode = (type == FileType::BINARY ? L"r+b" : L"r+");
	else if (access == FileAccess::READ_AND_WRITE_EMPTY)
		mode = (type == FileType::BINARY ? L"w+b" : L"w+");
	else if (access == FileAccess::READ_AND_APPEND)
		mode = (type == FileType::BINARY ? L"a+b" : L"a+");
	else
		CMG_ASSERT_FALSE("Invalid file access mode");

	// Open the file
	errno_t fopenError = _wfopen_s(&m_file, (const wchar_t*) path.c_str(), mode);

	m_fileType = type;
	m_accessMode = access;

	if (fopenError != 0)
		return CMG_ERROR_FAILURE;
	return CMG_ERROR_SUCCESS;
}

Error File::Open(FileAccess access, FileType type)
{
	return Open(m_path, access, type);
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

Error File::SeekFromStart(uint32 offset)
{
	CMG_ASSERT_MSG(IsOpen(), "Attempting to operate on a closed file");
	if (fseek(m_file, offset, SEEK_SET) != 0)
		return CMG_ERROR_FAILURE;
	return CMG_ERROR_SUCCESS;
}

uint32 File::Tell()
{
	CMG_ASSERT_MSG(IsOpen(), "Attempting to operate on a closed file");
	return (uint32) ftell(m_file);
}

Error File::SeekFromEnd(uint32 offset)
{
	CMG_ASSERT_MSG(IsOpen(), "Attempting to operate on a closed file");
	if (fseek(m_file, offset, SEEK_END) != 0)
		return CMG_ERROR_FAILURE;
	return CMG_ERROR_SUCCESS;
}

Error File::SeekFromCurrent(uint32 offset)
{
	CMG_ASSERT_MSG(IsOpen(), "Attempting to operate on a closed file");
	if (fseek(m_file, offset, SEEK_CUR) != 0)
		return CMG_ERROR_FAILURE;
	return CMG_ERROR_SUCCESS;
}

Error File::Read(void* destination, uint32 size)
{
	CMG_ASSERT_MSG(IsOpen(), "Attempting to read a file that is not open");

	if (fread((uint8*) destination, 1, size, m_file) != size)
		return CMG_ERROR(CommonErrorTypes::k_file_read);

	return CMG_ERROR_SUCCESS;
}
	
Error File::Write(const void* data, uint32 size)
{
	CMG_ASSERT_MSG(IsOpen(), "Attempting to write/append to a file that is not open");

	if (fwrite((uint8*) data, 1, size, m_file) != size)
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
	
Error File::GetContents(Array<uint8>& out)
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
	error = file.Open(FileAccess::READ, FileType::TEXT);
	if (error.Failed())
		return error.Uncheck();
	return file.GetContents(out);
}
	
Error File::OpenAndGetContents(const Path& path, Array<uint8>& out)
{
	Error error;
	File file(path);
	error = file.Open(FileAccess::READ, FileType::BINARY);
	if (error.Failed())
		return error.Uncheck();
	return file.GetContents(out);
}

Error File::OpenAndWriteContents(const Path & path, const String & text)
{
	Error error;
	File file(path);
	error = file.Open(FileAccess::WRITE, FileType::TEXT);
	if (error.Failed())
		return error.Uncheck();
	return file.Write(text.data(), text.length());
}

Error File::OpenAndWriteContents(const Path & path, const Array<uint8>& data)
{
	return OpenAndWriteContents(path, data.data(), data.size());
}

Error File::OpenAndWriteContents(const Path & path, const uint8 * data, size_t count)
{
	Error error;
	File file(path);
	error = file.Open(FileAccess::WRITE, FileType::BINARY);
	if (error.Failed())
		return error.Uncheck();
	return file.Write(data, count);
}
	

template<>
errno_t File::DoOpen<Path>(FILE** file, const Path& path, const char* mode)
{
	return fopen_s(file, path.c_str(), mode);
}

template<>
errno_t File::DoOpen<PathU16>(FILE** file, const PathU16& path, const char* mode)
{
	std::string modeStr(mode);
	std::wstring modeWStr(modeStr.begin(), modeStr.end());
	return _wfopen_s(file, (const wchar_t*) path.c_str(), modeWStr.c_str());
}
