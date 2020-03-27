#include "cmgLogging.h"
#include <cmgCore/cmgConsole.h>
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
		const char* levelName = "";
		uint32 color = cmg::core::console::p_color::light_gray;
		if (message.level == LogLevel::DEBUG)
		{
			levelName = "DEBUG";
		}
		else if (message.level == LogLevel::INFO)
		{
			levelName = "INFO";
		}
		else if (message.level == LogLevel::NOTICE)
		{
			levelName = "NOTICE";
		}
		else if (message.level == LogLevel::WARN)
		{
			levelName = "WARN";
			color = cmg::core::console::p_color::yellow;
		}
		else if (message.level == LogLevel::ERR)
		{
			levelName = "ERR";
			color = cmg::core::console::p_color::red;
		}
		else if (message.level == LogLevel::FATAL)
		{
			levelName = "FATAL";
			color = cmg::core::console::p_color::red;
		}

		// Format the timestamp
		char timestamp[256];
		time_t rawtime;
		struct tm timeinfo;
		time (&rawtime);
		localtime_s(&timeinfo, &rawtime);
		strftime(timestamp, sizeof(timestamp), "%H:%M:%S", &timeinfo);

		cmg::core::console::SetConsoleColor(
			color, cmg::core::console::p_color::black);

		std::cout << levelName << " : " << timestamp << " : " << message.text << std::endl;

		cmg::core::console::SetConsoleColor(
			cmg::core::console::p_color::light_gray,
			cmg::core::console::p_color::black);

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

