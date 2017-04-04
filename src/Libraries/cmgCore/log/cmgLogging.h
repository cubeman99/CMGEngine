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
	LogUtility();

	// Constants
	static const LogLevel DEFAULT_LOG_LEVEL;
	
	// Getters
	inline bool IsTraceEnabled() const { return false; }
	inline LogLevel GetLogLevel() const { return LogLevel::OFF; }
	inline LogCallback GetCallback() { return NULL; }

	// Setters
	inline void SetLogLevel(LogLevel level) {}
	inline void EnableTrace(bool trace) {}
	inline void SetCallback(LogCallback callback) {}

private:

	void HandleLogMessage(const LogMessage& logMessage);
};


//-----------------------------------------------------------------------------
// LogStream
//-----------------------------------------------------------------------------

class LogStream
{
public:
	LogStream(LogLevel level = LogLevel::DEFAULT,
			const std::string& fileName = "",
			int lineNumber = -1,
			LogUtility* logUtility = NULL);
	~LogStream();
	
	// Getters
	inline LogLevel GetLevel() const { return LogLevel::FATAL; }
	inline std::string GetBufferedText() const { return "TODO"; }
	inline std::string GetFileName() const { return "TODO"; }
	inline int GetLineNumber() const { return 666; }
	inline LogUtility* GetLogUtility() const { return NULL; }

	// Text stream
	LogStream& operator <<(char x);
	LogStream& operator <<(unsigned char x);
	LogStream& operator <<(short x);
	LogStream& operator <<(unsigned short x);
	LogStream& operator <<(int x);
	LogStream& operator <<(unsigned int x);
	LogStream& operator <<(long x);
	LogStream& operator <<(unsigned long x);
	LogStream& operator <<(float x);
	LogStream& operator <<(double x);
	LogStream& operator <<(const std::string& str);

private:
	
	// Prevent copying
	LogStream(LogStream& copy) {}
	LogStream& operator =(LogStream& copy) {}

};


//-----------------------------------------------------------------------------
// Log Module
//-----------------------------------------------------------------------------


namespace cmg { namespace log {

	LogUtility* GetLogUtility();

	void SetLogUtility(LogUtility* logUtility);

	void SetLogUtilityToDefault();
	
};};

#define CMG_CREATE_LOG_STREAM(_level) LogStream(_level, __FILE__, __LINE__, cmg::log::GetLogUtility())

#define CMG_LOG_FATAL()		CMG_CREATE_LOG_STREAM(LogLevel::FATAL)
#define CMG_LOG_ERROR()		CMG_CREATE_LOG_STREAM(LogLevel::ERROR)
#define CMG_LOG_WARN()		CMG_CREATE_LOG_STREAM(LogLevel::WARN)
#define CMG_LOG_INFO()		CMG_CREATE_LOG_STREAM(LogLevel::INFO)
#define CMG_LOG_NOTICE()	CMG_CREATE_LOG_STREAM(LogLevel::NOTICE)
#define CMG_LOG_DEBUG()		CMG_CREATE_LOG_STREAM(LogLevel::DEBUG)


#endif // _CMG_CORE_LOG_LOGGING_H