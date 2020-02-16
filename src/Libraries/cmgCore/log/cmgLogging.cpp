#include "cmgLogging.h"
#include <iostream>

const LogLevel LogUtility::DEFAULT_LOG_LEVEL = LogLevel::ALL;


LogUtility globalLogUtility;



//-----------------------------------------------------------------------------
// LogUtility
//-----------------------------------------------------------------------------

LogUtility::LogUtility() :
	m_level(DEFAULT_LOG_LEVEL),
	m_callback(nullptr),
	m_trace(false)
{

}

void LogUtility::HandleLogMessage(const LogMessage& message)
{
	if (message.level <= m_level)
	{
		// TODO: format the message
		std::cout << message.text << std::endl;

		// Invoke the callback
		if (m_callback != nullptr)
			m_callback(message);
	}
}



//-----------------------------------------------------------------------------
// LogStream
//-----------------------------------------------------------------------------

LogStream::LogStream(LogLevel level, const std::string& fileName,
	int lineNumber, LogUtility* logUtility) :
	m_level(level),
	m_fileName(fileName),
	m_lineNumber(lineNumber),
	m_logUtility(logUtility)
{
}

LogStream::~LogStream()
{
	// Create and send the message to the log utility.
	if (m_logUtility != nullptr)
	{
		LogMessage message;
		message.level = m_level;
		message.lineNumber = m_lineNumber;
		message.fileName = m_fileName;
		message.text = m_textStream.str();
		m_logUtility->HandleLogMessage(message);
	}
}


//-----------------------------------------------------------------------------
// Log Module
//-----------------------------------------------------------------------------


namespace cmg { namespace log {
	
	static LogUtility defaultLogUtility = LogUtility();

	static LogUtility* globalLogUtility = &defaultLogUtility;


	LogUtility* GetLogUtility()
	{
		return globalLogUtility;
	}

	void SetLogUtility(LogUtility* logUtility)
	{
		globalLogUtility = logUtility;
	}

	void SetLogUtilityToDefault()
	{
		globalLogUtility = &defaultLogUtility;
	}

};};

