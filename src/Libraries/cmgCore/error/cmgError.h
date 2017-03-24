#ifndef _CMG_CORE_ERROR_H_
#define _CMG_CORE_ERROR_H_

#include <cmgCore/string/cmgString.h>

	
// NOTE: Taken from 2LOC
struct CommonErrorTypes
{
	enum
	{
		k_success = 0,
		k_failure,
		k_null_pointer,
		k_initialize,
		k_already_initialized,
		k_no_data,                    // used for null
		k_corrupt_data,
		k_size_mismatch,
		k_path_incorrect,
		k_file_not_found,
		k_file_corrupt,
		k_file_read,
		k_file_write,
		k_invalid_handle,
	};
};
	
typedef int error_code_type;


// Error stuff taken from 2LOC
class Error
{
public:
	Error();
	Error(int errorType, int line, const char* file);
	Error(int errorType, const String& message, int line, const char* file);
	Error(const Error& other);
	Error& operator =(Error& copy);
	~Error();
	
	// Error checking
	bool Succeeded() const;
	bool Failed() const;
	void Ignore() const;
	void Check() const;
	void Uncheck() const;

	// Accessors
	const String&	GetText() const;	
	int				GetLineNumber() const;
	const char*		GetFileName() const;
	error_code_type	GetErrorCode() const;

private:
	error_code_type	m_error;	// Error code
	const char*		m_file;		// File name
	int				m_line;		// Line number
	String			m_message;	// Message text
	mutable bool	m_errorCheckedByUser;
	//char			padding[3];
};

#define CMG_ERROR_MSG(_errorCode_, _message)\
  Error(_errorCode_, _message, __LINE__, __FILE__)
#define CMG_ERROR(_errorCode_)\
  Error(_errorCode_, __LINE__, __FILE__)

#define CMG_ERROR_SUCCESS \
  CMG_ERROR(CommonErrorTypes::k_success)
#define CMG_ERROR_FAILURE \
  CMG_ERROR(CommonErrorTypes::k_failure)


#endif // _CMG_CORE_ERROR_H_