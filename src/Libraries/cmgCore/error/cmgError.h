#ifndef _CMG_CORE_ERROR_H_
#define _CMG_CORE_ERROR_H_

#include <cmgCore/string/cmgString.h>


class Error
{
public:
	typedef int error_code_type;

	enum
	{
		kSuccess = 0,
		kFailure,
		kNullPointer,
		kInitialize,
		kAlreadyInitialized,
		kNoData,
		kCorruptData,
		kSizeMismatch,
		kPathIncorrect,
		kAccessDenied,
		kOperationNotPermitted,
		kFileNotFound,
		kNotAdirectory,
		kIsAdirectory,
		kFileExists,
		kFileCorrupt,
		kFileRead,
		kFileWrite,
		kInvalidIandle,
		kNotImplemented, // unimplemented function
		kOutOfMemory,
	};

public:
	Error();
	Error(error_code_type errorType, const char* fileName, const char* functionName, int lineNumber);
	Error(error_code_type errorType, const String& message, const char* fileName, const char* functionName, int lineNumber);
	Error(const Error& other);
	Error& operator =(Error& copy);
	~Error();
	
	// Error checking
	bool Passed() const;
	bool Failed() const;
	const Error& Ignore() const;
	const Error& Check() const;
	const Error& Uncheck() const;

	// Accessors
	const String& GetText() const;	
	int GetLineNumber() const;
	const char* GetFileName() const;
	const char* GetFunctionName() const;
	error_code_type GetErrorCode() const;

	static error_code_type ErrnoToErrorCode(errno_t error);
	static void HandleUncheckedError(const Error& error);

private:
	error_code_type m_error = kSuccess;
	const char* m_fileName = "";
	const char* m_functionName = "";
	int m_lineNumber = 0;
	String m_message;

	mutable bool m_errorCheckedByUser = false;
	mutable bool m_isHandled = false;
};

#define CMG_ERROR_MSG(_errorCode_, _message)\
  Error(_errorCode_, _message, __FILE__, __FUNCTION__, __LINE__)
#define CMG_ERROR(_errorCode_)\
  Error(_errorCode_, __FILE__, __FUNCTION__, __LINE__)

#define CMG_ERROR_SUCCESS \
  CMG_ERROR(Error::kSuccess)
#define CMG_ERROR_FAILURE \
  CMG_ERROR(Error::kFailure)
#define CMG_ERROR_NOT_IMPLEMENTED \
  CMG_ERROR(Error::kNotImplemented)
#define CMG_ERROR_FILE_NOT_FOUND \
  CMG_ERROR(Error::kFileNotFound)
#define CMG_ERROR_ACCESS_DENIED \
  CMG_ERROR(Error::kAccessDenied)
#define CMG_ERROR_FROM_ERRNO(_error) \
  CMG_ERROR(Error::ErrnoToErrorCode(_error))


#endif // _CMG_CORE_ERROR_H_