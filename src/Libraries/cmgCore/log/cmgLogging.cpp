#include "cmgLogging.h"


const LogLevel LogUtility::DEFAULT_LOG_LEVEL = LogLevel::WARN;


LogUtility globalLogUtility;



//-----------------------------------------------------------------------------
// LogUtility
//-----------------------------------------------------------------------------

LogUtility::LogUtility()
{

}

void LogUtility::HandleLogMessage(const LogMessage& logMessage)
{
	
}



//-----------------------------------------------------------------------------
// LogStream
//-----------------------------------------------------------------------------

LogStream::LogStream(LogLevel level, const std::string& fileName, int lineNumber, LogUtility* logUtility)
{
}

LogStream::~LogStream()
{
}


LogStream& LogStream::operator <<(char x)
{
	return *this;
}

LogStream& LogStream::operator <<(unsigned char x)
{
	return *this;
}

LogStream& LogStream::operator <<(short x)
{
	return *this;
}

LogStream& LogStream::operator <<(unsigned short x)
{
	return *this;
}

LogStream& LogStream::operator <<(int x)
{
	return *this;
}

LogStream& LogStream::operator <<(unsigned int x)
{
	return *this;
}

LogStream& LogStream::operator <<(long x)
{
	return *this;
}

LogStream& LogStream::operator <<(unsigned long x)
{
	return *this;
}

LogStream& LogStream::operator <<(float x)
{
	return *this;
}

LogStream& LogStream::operator <<(double x)
{
	return *this;
}

LogStream& LogStream::operator <<(const std::string& str)
{
	return *this;
}


//-----------------------------------------------------------------------------
// Log Module
//-----------------------------------------------------------------------------



namespace cmg { namespace log {

	LogUtility* GetLogUtility()
	{
		return NULL;
	}

	void SetLogUtility(LogUtility* logUtility)
	{
	}

	void SetLogUtilityToDefault()
	{
	}

};};

