// Log Utility Tests

#include <gtest/gtest.h>
#include <cmgCore/log/cmgLogging.h>


//-----------------------------------------------------------------------------
// LogStream tests
//-----------------------------------------------------------------------------

TEST(LogStream, LogStream_Constructor)
{
	LogStream logStream;

	// Test the constructor with default values.
	logStream = LogStream();
	EXPECT_EQ(LogLevel::DEFAULT, logStream.GetLevel());
	EXPECT_EQ("", logStream.GetFileName());
	EXPECT_EQ(-1, logStream.GetLineNumber());
	EXPECT_EQ("", logStream.GetBufferedText());

	// Test the constructor with specified values #1.
	logStream = LogStream(LogLevel::ERROR, "file_name", 42);
	EXPECT_EQ(LogLevel::ERROR, logStream.GetLevel());
	EXPECT_EQ("file_name", logStream.GetFileName());
	EXPECT_EQ(42, logStream.GetLineNumber());
	EXPECT_EQ("", logStream.GetBufferedText());

	// Test the constructor with specified values #2.
	logStream = LogStream(LogLevel::INFO, "file_name_2", 123);
	EXPECT_EQ(LogLevel::INFO, logStream.GetLevel());
	EXPECT_EQ("file_name_2", logStream.GetFileName());
	EXPECT_EQ(123, logStream.GetLineNumber());
	EXPECT_EQ("", logStream.GetBufferedText());
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
	// Construct and destruct to test that the log callback actually ran.
	callbackRan = NULL;
	{
		LogStream logStream = LogStream(LogLevel::WARN,
			"test file name", 92, LogStream_Destructor_TestCallback);
		logStream << "test callback text";
	}
	EXPECT_EQ((LogCallback) LogStream_Destructor_TestCallback, callbackRan);
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
	logStream << unsigned short(66234);
	EXPECT_EQ("66234", logStream.GetBufferedText());
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
	logStream << unsigned int(4294962000);
	EXPECT_EQ("4294962000", logStream.GetBufferedText());
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
	logStream << -3.14159265368;
	EXPECT_EQ("-3.14159265368", logStream.GetBufferedText());
}


//-----------------------------------------------------------------------------
// LogUtility tests
//-----------------------------------------------------------------------------

TEST(LogUtility, Constructor)
{
	LogUtility utility;
	EXPECT_EQ(LogUtility::DEFAULT_LOG_LEVEL, utility.GetLogLevel());
	EXPECT_EQ(false, utility.IsTraceEnabled());
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
}



//-----------------------------------------------------------------------------
// Log Macro Tests
//-----------------------------------------------------------------------------

static LogMessage testLogMessage;

static void LogMacros_TestCallback(const LogMessage& message)
{
	testLogMessage = message;
}

TEST(LogMacros, Macro_LOG_MSG)
{
	//LogUtility::GetInstance().SetLogLevel(LogLevel::INFO);
	////LogUtility::GetInstance().EnableTrace(false);
	//LogUtility::GetInstance().SetCallback(LogMacros_TestCallback);

	// Log a test message.
	testLogMessage.text = "??";
	CMG_LOG_MSG() << "hello";
	EXPECT_EQ(__LINE__ - 1, testLogMessage.lineNumber);
	EXPECT_EQ(__FILE__, testLogMessage.fileName);
	EXPECT_EQ(LogLevel::INFO, testLogMessage.level);
	EXPECT_EQ("hello", testLogMessage.text);
}

