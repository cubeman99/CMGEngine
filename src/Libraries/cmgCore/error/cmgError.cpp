#include "cmgError.h"
#include <cmgCore/cmgAssert.h>

namespace
{
	// Get the message text for the common error types enumeration.
	static const char* GetCommonErrorTypeName(int errorType)
	{
		switch (errorType)
		{
			case Error::kSuccess:
				return "success";
			case Error::kFailure:
				return "failure";
			case Error::kNullPointer:
				return "null pointer";
			case Error::kInitialize:
				return "initialize";
			case Error::kAlreadyInitialized:
				return "already initialized";
			case Error::kNoData:
				return "no data";
			case Error::kCorruptData:
				return "corrupt data";
			case Error::kSizeMismatch:
				return "size mismatch";
			case Error::kPathIncorrect:
				return "path incorrect";
			case Error::kAccessDenied:
				return "access denied";
			case Error::kOperationNotPermitted:
				return "operation not permitted";
			case Error::kFileNotFound:
				return "file not found";
			case Error::kNotAdirectory:
				return "not a directory";
			case Error::kIsAdirectory:
				return "is a directory";
			case Error::kFileExists:
				return "file exists";
			case Error::kFileCorrupt:
				return "file corrupt";
			case Error::kFileRead:
				return "file read";
			case Error::kFileWrite:
				return "file write";
			case Error::kInvalidIandle:
				return "invalid handle";
			case Error::kNotImplemented:
				return "not implemented";
			case Error::kOutOfMemory:
				return "out of memory";
			default:
				return "unknown error type";
		}
	}
};


//-----------------------------------------------------------------------------
// Constructors & Destructor
//-----------------------------------------------------------------------------

Error::Error()
{
}

Error::Error(error_code_type errorType, const char* fileName,
	const char* functionName, int lineNumber) :
	m_error(errorType),
	m_lineNumber(lineNumber),
	m_fileName(fileName),
	m_functionName(functionName)
{
	m_message = GetCommonErrorTypeName(errorType);
}

Error::Error(error_code_type errorType, const String& message, 
	const char* fileName, const char* functionName, int lineNumber) :
	m_error(errorType),
	m_lineNumber(lineNumber),
	m_fileName(fileName),
	m_functionName(functionName),
	m_message(message),
	m_errorCheckedByUser(false)
{
}

Error::Error(const Error& other) :
	m_error(other.m_error),
	m_fileName(other.m_fileName),
	m_functionName(other.m_functionName),
	m_lineNumber(other.m_lineNumber),
	m_message(other.m_message),
	m_errorCheckedByUser(other.m_errorCheckedByUser)
{
	other.Ignore();
}

Error& Error::operator =(Error& copy)
{
	m_error = copy.m_error;
	m_lineNumber = copy.m_lineNumber;
	m_fileName = copy.m_fileName;
	m_functionName = copy.m_functionName;
	m_message = copy.m_message;
	m_errorCheckedByUser = copy.m_errorCheckedByUser;
	copy.Ignore();
	return *this;
}

Error::~Error()
{
	if (!m_errorCheckedByUser && m_error != Error::kSuccess)
	{
		HandleUncheckedError(*this);
	}
}


//-----------------------------------------------------------------------------
// Error Checking
//-----------------------------------------------------------------------------

bool Error::Passed() const
{
	m_errorCheckedByUser = true;
	return (m_error == Error::kSuccess);
}

bool Error::Failed() const
{
	m_errorCheckedByUser = true;
	return (m_error != Error::kSuccess);
}

const Error& Error::Ignore() const
{
	m_errorCheckedByUser = true;
	return *this;
}

const Error& Error::Check() const
{
	m_errorCheckedByUser = true;
	return *this;
}

const Error& Error::Uncheck() const
{
	m_errorCheckedByUser = false;
	return *this;
}


//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------

const String& Error::GetText() const
{
	return m_message;
}
			
int Error::GetLineNumber() const
{
	return m_lineNumber;
}

const char* Error::GetFileName() const
{
	return m_fileName;
}

const char* Error::GetFunctionName() const
{
	return m_functionName;
}

Error::error_code_type Error::GetErrorCode() const
{
	return m_error;
}

Error::error_code_type Error::ErrnoToErrorCode(errno_t error)
{
	switch (error)
	{
		case 0: return Error::kSuccess;
		case EPERM: return Error::kOperationNotPermitted;
		case ENOENT: return Error::kFileNotFound;
		case ENOMEM: return Error::kOutOfMemory;
		case EACCES: return Error::kAccessDenied;
		case EEXIST: return Error::kFileExists;
		case ENOTDIR: return Error::kNotAdirectory;
		case EISDIR: return Error::kIsAdirectory;
		case ENOSPC: return Error::kOutOfMemory;
		case EROFS: return Error::kAccessDenied;
		case EIO: return Error::kFileRead;
		default: return Error::kFailure;
	}
}

void Error::HandleUncheckedError(const Error& error)
{
	cmg::core::console::SetConsoleColor(
		cmg::core::console::p_color::red,
		cmg::core::console::p_color::black);
	printf("An error went unchecked at %s, line %d\n", error.m_fileName, error.m_lineNumber);
	printf("Error reason: %s\n", error.m_message.c_str());
	cmg::core::console::SetConsoleColor(
		cmg::core::console::p_color::light_gray,
		cmg::core::console::p_color::black);
	CMG_DEBUGGER_BREAK();
}


