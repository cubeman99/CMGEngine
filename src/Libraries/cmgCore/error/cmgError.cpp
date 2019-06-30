#include "cmgError.h"
#include <cmgCore/cmgAssert.h>

namespace
{
	// Get the message text for the common error types enumeration.
	static const char* GetCommonErrorTypeName(int errorType)
	{
		switch (errorType)
		{
			case CommonErrorTypes::k_success:				return "success";
			case CommonErrorTypes::k_failure:				return "failure";
			case CommonErrorTypes::k_null_pointer:			return "null pointer";
			case CommonErrorTypes::k_initialize:			return "initialize";
			case CommonErrorTypes::k_already_initialized:	return "already initialized";
			case CommonErrorTypes::k_no_data:				return "no data";
			case CommonErrorTypes::k_corrupt_data:			return "corrupt data";
			case CommonErrorTypes::k_size_mismatch:			return "size mismatch";
			case CommonErrorTypes::k_path_incorrect:		return "path incorrect";
			case CommonErrorTypes::k_file_not_found:		return "file not found";
			case CommonErrorTypes::k_file_corrupt:			return "file corrupt";
			case CommonErrorTypes::k_file_read:				return "file read";
			case CommonErrorTypes::k_file_write:			return "file write";
			case CommonErrorTypes::k_invalid_handle:		return "invalid handle";
			default: return "";
		};
	}
};


//-----------------------------------------------------------------------------
// Constructors & Destructor
//-----------------------------------------------------------------------------

Error::Error() :
	m_error(CommonErrorTypes::k_success),
	m_line(0),
	m_file(""),
	m_message(""),
	m_errorCheckedByUser(false)
{
}

Error::Error(int errorType, int line, const char* file) :
	m_error(errorType),
	m_line(line),
	m_file(file),
	m_errorCheckedByUser(false)
{
	m_message = GetCommonErrorTypeName(errorType);
}

Error::Error(int errorType, const String& message, int line, const char* file) :
	m_error(errorType),
	m_line(line),
	m_file(file),
	m_message(message),
	m_errorCheckedByUser(false)
{
}

Error::Error(const Error& other) :
	m_error(other.m_error),
	m_line(other.m_line),
	m_file(other.m_file),
	m_message(other.m_message),
	m_errorCheckedByUser(other.m_errorCheckedByUser)
{
	other.Ignore();
}

Error& Error::operator =(Error& copy)
{
	m_error = copy.m_error;
	m_line = copy.m_line;
	m_file = copy.m_file;
	m_message = copy.m_message;
	m_errorCheckedByUser = copy.m_errorCheckedByUser;
	copy.Ignore();
	return *this;
}

Error::~Error()
{
	if (!m_errorCheckedByUser && m_error != CommonErrorTypes::k_success)
	{
		// TODO: Unchecked failure.
		CMG_ASSERT_FALSE(m_message.c_str());//"An error went unchecked!");
		printf("Error: %s", m_message.c_str());
	}
}


//-----------------------------------------------------------------------------
// Error Checking
//-----------------------------------------------------------------------------

bool Error::Succeeded() const
{
	m_errorCheckedByUser = true;
	return (m_error == CommonErrorTypes::k_success);
}

bool Error::Passed() const
{
	m_errorCheckedByUser = true;
	return (m_error == CommonErrorTypes::k_success);
}

bool Error::Failed() const
{
	m_errorCheckedByUser = true;
	return (m_error != CommonErrorTypes::k_success);
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
	return m_line;
}

const char* Error::GetFileName() const
{
	return m_file;
}

error_code_type Error::GetErrorCode() const
{
	return m_error;
}


