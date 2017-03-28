// Log Utility Tests

#include <gtest/gtest.h>


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


class LogUtility
{
public:
	LogUtility();

	// getters
	bool IsTraceEnabled() const;
	LogLevel GetLogLevel() const;

	// setters
	void SetLogLevel(LogLevel level);
	void EnableTrace(bool trace);

private:
};


class LogStream
{

public:
	LogStream(LogLevel level = LogLevel::DEFAULT, const std::string& fileName = "", int lineNumber = -1) {}
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

TEST(LogStream, LogStream_Destructor)
{
	// TODO: LogStream_Destructor
	EXPECT_TRUE(false);
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

TEST(LogStream, LogStream_Buffer_Int)
{
	LogStream logStream;
	logStream << int(12345678);
	EXPECT_EQ("12345678", logStream.GetBufferedText());
}

TEST(LogStream, LogStream_Buffer_Unsigned_Int)
{
	LogStream logStream;
	logStream << unsigned int(56);
	EXPECT_EQ("56", logStream.GetBufferedText());
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
	logStream << 12.25;
	EXPECT_EQ("12.25", logStream.GetBufferedText());
}


//-----------------------------------------------------------------------------
// LogUtility tests
//-----------------------------------------------------------------------------
