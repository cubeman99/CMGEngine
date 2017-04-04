#ifndef _CMG_CORE_LOG_LOGGING_H
#define _CMG_CORE_LOG_LOGGING_H

#include <cmgCore/string/cmgString.h>
#include <sstream>


//-----------------------------------------------------------------------------
// LogLevel
//-----------------------------------------------------------------------------

enum class LogLevel
{
	OFF = 0,
		FATAL,
		ERR,
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
	inline bool IsTraceEnabled() const { return m_trace; }
	inline LogLevel GetLogLevel() const { return m_level; }
	inline LogCallback GetCallback() { return m_callback; }

	// Setters
	inline void SetLogLevel(LogLevel level) { m_level = level; }
	inline void EnableTrace(bool trace) { m_trace = trace; }
	inline void SetCallback(LogCallback callback) { m_callback = callback; }

private:

	void HandleLogMessage(const LogMessage& message);

private:
	LogCallback m_callback;
	bool m_trace;
	LogLevel m_level;
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
			LogUtility* logUtility = nullptr);
	~LogStream();
	
	// Getters
	inline LogLevel GetLevel() const { return m_level; }
	inline std::string GetBufferedText() const { return m_textStream.str(); }
	inline std::string GetFileName() const { return m_fileName; }
	inline int GetLineNumber() const { return m_lineNumber; }
	inline LogUtility* GetLogUtility() const { return m_logUtility; }

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

private:
	LogUtility* m_logUtility;
	LogLevel m_level;
	std::stringstream m_textStream;
	std::string m_fileName;
	int m_lineNumber;
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
#define CMG_LOG_ERROR()		CMG_CREATE_LOG_STREAM(LogLevel::ERR)
#define CMG_LOG_WARN()		CMG_CREATE_LOG_STREAM(LogLevel::WARN)
#define CMG_LOG_INFO()		CMG_CREATE_LOG_STREAM(LogLevel::INFO)
#define CMG_LOG_NOTICE()	CMG_CREATE_LOG_STREAM(LogLevel::NOTICE)
#define CMG_LOG_DEBUG()		CMG_CREATE_LOG_STREAM(LogLevel::DEBUG)


#endif // _CMG_CORE_LOG_LOGGING_H