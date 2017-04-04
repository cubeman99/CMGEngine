// Log Utility Tests

#include <gtest/gtest.h>
#include <cmgCore/log/cmgLogging.h>


//-----------------------------------------------------------------------------
// LogUtility tests
//-----------------------------------------------------------------------------

TEST(LogUtility, Constructor)
{
	LogUtility utility;
	EXPECT_EQ(LogUtility::DEFAULT_LOG_LEVEL, utility.GetLogLevel());
	EXPECT_EQ(false, utility.IsTraceEnabled());
}

static void LogUtility_GettersAndSetters_TestCallback(const LogMessage& message)
{
	// this function is used to test as a pointer.
}

TEST(LogUtility, GettersAndSetters)
{
	LogUtility utility;

	utility.SetLogLevel(LogLevel::FATAL);
	EXPECT_EQ(LogLevel::FATAL, utility.GetLogLevel());
	utility.SetLogLevel(LogLevel::INFO);
	EXPECT_EQ(LogLevel::INFO, utility.GetLogLevel());

	utility.EnableTrace(true);
	EXPECT_EQ(true, utility.IsTraceEnabled());
	utility.EnableTrace(false);
	EXPECT_EQ(false, utility.IsTraceEnabled());
	
	utility.SetCallback((LogCallback) NULL);
	EXPECT_EQ((LogCallback) NULL, utility.GetCallback());
	utility.SetCallback(LogUtility_GettersAndSetters_TestCallback);
	EXPECT_EQ((LogCallback) LogUtility_GettersAndSetters_TestCallback, utility.GetCallback());
}



//-----------------------------------------------------------------------------
// LogStream tests
//-----------------------------------------------------------------------------

TEST(LogStream, LogStream_Constructor)
{
	// Test the constructor with default values.
	{
		LogStream logStream;
		EXPECT_EQ(LogLevel::DEFAULT, logStream.GetLevel());
		EXPECT_EQ("", logStream.GetFileName());
		EXPECT_EQ(-1, logStream.GetLineNumber());
		EXPECT_EQ("", logStream.GetBufferedText());
	}

	// Test the constructor with specified values #1.
	{
		LogStream logStream(LogLevel::ERROR, "file_name", 42);
		EXPECT_EQ(LogLevel::ERROR, logStream.GetLevel());
		EXPECT_EQ("file_name", logStream.GetFileName());
		EXPECT_EQ(42, logStream.GetLineNumber());
		EXPECT_EQ("", logStream.GetBufferedText());
	}

	// Test the constructor with specified values #2.
	{
		LogStream logStream(LogLevel::INFO, "file_name_2", 123);
		EXPECT_EQ(LogLevel::INFO, logStream.GetLevel());
		EXPECT_EQ("file_name_2", logStream.GetFileName());
		EXPECT_EQ(123, logStream.GetLineNumber());
		EXPECT_EQ("", logStream.GetBufferedText());
	}
}

static LogCallback callbackRan = NULL;

static void LogStream_Destructor_TestCallback(const LogMessage& message)
{
	callbackRan = LogStream_Destructor_TestCallback;
	EXPECT_EQ(LogLevel::WARN, message.level);
	EXPECT_EQ("test file name", message.fileName);
	EXPECT_EQ(92, message.lineNumber);
	EXPECT_EQ("test callback text", message.text);
}

TEST(LogStream, LogStream_Destructor)
{
	LogUtility logUtility;
	logUtility.SetLogLevel(LogLevel::ALL);
	logUtility.SetCallback(LogStream_Destructor_TestCallback);

	// Construct and destruct to test that the log callback is executed.
	callbackRan = NULL;
	{
		LogStream logStream(LogLevel::WARN, "test file name", 92, &logUtility);
		logStream << "test callback text";
	}
	EXPECT_EQ((LogCallback) LogStream_Destructor_TestCallback, callbackRan);

	// Construct and destruct with no log utility to test the callback doesn't execute.
	callbackRan = NULL;
	{
		LogStream logStream(LogLevel::WARN, "test file name", 92, NULL);
		logStream << "test callback text";
	}
	EXPECT_TRUE((LogCallback) NULL == callbackRan);
}

TEST(LogStream, LogStream_Buffer_String)
{
	LogStream logStream;

	logStream << "hello";
	EXPECT_EQ("hello", logStream.GetBufferedText());
}

TEST(LogStream, LogStream_Buffer_Concat)
{
	LogStream logStream;

	logStream << "hello";
	EXPECT_EQ("hello", logStream.GetBufferedText());

	logStream << "world";
	EXPECT_EQ("helloworld", logStream.GetBufferedText());
}

TEST(LogStream, LogStream_Buffer_Char)
{
	LogStream logStream;
	logStream << 'A' << 'B' << 'C';
	EXPECT_EQ("ABC", logStream.GetBufferedText());
}

TEST(LogStream, LogStream_Buffer_UnsignedChar)
{
	LogStream logStream;
	logStream << unsigned char('X') << unsigned char('Y');
	logStream << unsigned char('Z') << unsigned char(0x20);
	EXPECT_EQ("XYZ ", logStream.GetBufferedText());
}

TEST(LogStream, LogStream_Buffer_Short)
{
	LogStream logStream;
	logStream << short(-12366);
	EXPECT_EQ("-12366", logStream.GetBufferedText());
}

TEST(LogStream, LogStream_Buffer_UnsignedShort)
{
	LogStream logStream;
	logStream << unsigned short(65530);
	EXPECT_EQ("65530", logStream.GetBufferedText());
}

TEST(LogStream, LogStream_Buffer_Int)
{
	LogStream logStream;
	logStream << int(12345678);
	EXPECT_EQ("12345678", logStream.GetBufferedText());
	logStream << int(-32);
	EXPECT_EQ("12345678-32", logStream.GetBufferedText());
}

TEST(LogStream, LogStream_Buffer_UnsignedInt)
{
	LogStream logStream;
	logStream << unsigned int(4294967295);
	EXPECT_EQ("4294967295", logStream.GetBufferedText());
}

TEST(LogStream, LogStream_Buffer_Long)
{
	LogStream logStream;
	logStream << long(-21474848);
	EXPECT_EQ("-21474848", logStream.GetBufferedText());
}

TEST(LogStream, LogStream_Buffer_UnsignedLong)
{
	LogStream logStream;
	logStream << unsigned long(4294967294);
	EXPECT_EQ("4294967294", logStream.GetBufferedText());
}

TEST(LogStream, LogStream_Buffer_Float)
{
	LogStream logStream;
	logStream << 12.25f;
	EXPECT_EQ("12.25", logStream.GetBufferedText());
}

TEST(LogStream, LogStream_Buffer_Double)
{
	LogStream logStream;
	logStream << -3.141;
	EXPECT_EQ("-3.141", logStream.GetBufferedText());
}



//-----------------------------------------------------------------------------
// Log Module
//-----------------------------------------------------------------------------

TEST(LogModule, GettersAndSetters)
{
	LogUtility logUtilityA;
	LogUtility logUtilityB;

	cmg::log::SetLogUtility(&logUtilityA);
	EXPECT_EQ(&logUtilityA, cmg::log::GetLogUtility());
	cmg::log::SetLogUtility(&logUtilityB);
	EXPECT_EQ(&logUtilityB, cmg::log::GetLogUtility());
	cmg::log::SetLogUtility(NULL);
	EXPECT_TRUE(NULL == cmg::log::GetLogUtility());
	cmg::log::SetLogUtilityToDefault();
	EXPECT_TRUE(NULL != cmg::log::GetLogUtility());
}

TEST(LogModule, LogMacros_LogLevels)
{
	cmg::log::SetLogUtility(NULL);
	EXPECT_EQ(LogLevel::FATAL,	CMG_LOG_FATAL().GetLevel());
	EXPECT_EQ(LogLevel::ERROR,	CMG_LOG_ERROR().GetLevel());
	EXPECT_EQ(LogLevel::WARN,	CMG_LOG_WARN().GetLevel());
	EXPECT_EQ(LogLevel::INFO,	CMG_LOG_INFO().GetLevel());
	EXPECT_EQ(LogLevel::NOTICE,	CMG_LOG_NOTICE().GetLevel());
	EXPECT_EQ(LogLevel::DEBUG,	CMG_LOG_DEBUG().GetLevel());
}

static LogMessage testLogMessage;

static void LogMacros_MessageData_TestCallback(const LogMessage& message)
{
	testLogMessage = message;
}

TEST(LogModule, LogMacros_MessageData)
{
	LogUtility logUtility;
	logUtility.SetLogLevel(LogLevel::WARN);
	logUtility.EnableTrace(false);
	logUtility.SetCallback(LogMacros_MessageData_TestCallback);
	cmg::log::SetLogUtility(&logUtility);

	LogMessage nullLogMessage;
	nullLogMessage.text = "??";
	nullLogMessage.lineNumber = -1;
	nullLogMessage.fileName = "??";
	nullLogMessage.level = LogLevel::OFF;
		
	// Log a test message.
	testLogMessage = nullLogMessage;
	CMG_LOG_WARN() << "test 1";
	EXPECT_EQ(__LINE__ - 1, testLogMessage.lineNumber);
	EXPECT_EQ(__FILE__, testLogMessage.fileName);
	EXPECT_EQ(LogLevel::WARN, testLogMessage.level);
	EXPECT_EQ("test 1", testLogMessage.text);
	
	// Log a test message that is too high of a level.
	logUtility.SetLogLevel(LogLevel::WARN);
	testLogMessage = nullLogMessage;
	CMG_LOG_INFO() << "test 2";
	EXPECT_NE(__LINE__ - 1, testLogMessage.lineNumber);
	EXPECT_NE(__FILE__, testLogMessage.fileName);
	EXPECT_NE(LogLevel::INFO, testLogMessage.level);
	EXPECT_NE("test 2", testLogMessage.text);

	// Try again with log level set to all.
	logUtility.SetLogLevel(LogLevel::ALL);
	testLogMessage = nullLogMessage;
	CMG_LOG_NOTICE() << "test " << 2;
	EXPECT_EQ(__LINE__ - 1, testLogMessage.lineNumber);
	EXPECT_EQ(__FILE__, testLogMessage.fileName);
	EXPECT_EQ(LogLevel::NOTICE, testLogMessage.level);
	EXPECT_EQ("test 2", testLogMessage.text);

	cmg::log::SetLogUtilityToDefault();
}

