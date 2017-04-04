#ifndef _CMG_CORE_LOG_LOGGING_H
#define _CMG_CORE_LOG_LOGGING_H

#include <cmgCore/string/cmgString.h>


//-----------------------------------------------------------------------------
// LogLevel
//-----------------------------------------------------------------------------

enum class LogLevel
{
	OFF = 0,
		FATAL,
		ERROR,
		WARN,
		INFO,
		NOTICE,
		DEBUG,
	ALL,

	DEFAULT = WARN,
};


//-----------------------------------------------------------------------------
// LogMessage
//-----------------------------------------------------------------------------

struct LogMessage
{
	LogLevel		level;
	std::string		fileName;
	int				lineNumber;
	std::string		text;
};


//-----------------------------------------------------------------------------
// LogUtility
//-----------------------------------------------------------------------------

typedef void (*LogCallback) (const LogMessage& message);

class LogUtility
{
	friend class LogStream;

public:
	LogUtility() {}

	// Constants
	static const LogLevel DEFAULT_LOG_LEVEL;
	
	// Getters
	bool IsTraceEnabled() const { return false; }
	LogLevel GetLogLevel() const { return LogLevel::OFF; }
	LogCallback GetCallback() { return NULL; }

	// Setters
	void SetLogLevel(LogLevel level) {}
	void EnableTrace(bool trace) {}
	void SetCallback(LogCallback callback) {}

private:

	void HandleLogMessage(const LogMessage& logMessage);
};


//-----------------------------------------------------------------------------
// LogStream
//-----------------------------------------------------------------------------

class LogStream
{
public:
	LogStream(LogLevel level = LogLevel::DEFAULT, const std::string& fileName = "", int lineNumber = -1, LogCallback callback = NULL) {}
	~LogStream() {}

	LogStream& operator <<(char x) { return *this; }
	LogStream& operator <<(unsigned char x) { return *this; }
	LogStream& operator <<(short x) { return *this; }
	LogStream& operator <<(unsigned short x) { return *this; }
	LogStream& operator <<(int x) { return *this; }
	LogStream& operator <<(unsigned int x) { return *this; }
	LogStream& operator <<(long x) { return *this; }
	LogStream& operator <<(unsigned long x) { return *this; }
	LogStream& operator <<(float x) { return *this; }
	LogStream& operator <<(double x) { return *this; }
	LogStream& operator <<(const std::string& str) { return *this; }
	
	LogLevel GetLevel() const { return LogLevel::FATAL; }
	std::string GetBufferedText() const { return "TODO"; }
	std::string GetFileName() const { return "TODO"; }
	int GetLineNumber() const { return 666; }

private:
};


//-----------------------------------------------------------------------------
// Log Macros
//-----------------------------------------------------------------------------

extern LogUtility globalLogUtility;

#define CMG_LOG_MSG() LogStream(LogLevel::INFO, __FILE__, __LINE__, globalLogUtility.GetCallback())



#endif // _CMG_CORE_LOG_LOGGING_H