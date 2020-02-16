// ECS Tests

#include <gtest/gtest.h>
#include <cmgCore/cmg_core.h>


TEST(String, Split)
{
	Array<String> result;
	uint32 count;

	result = cmg::string::Split("hello world   how are u", " ");
	EXPECT_EQ(result.size(), 5);
	EXPECT_EQ(result[0], "hello");
	EXPECT_EQ(result[1], "world");
	EXPECT_EQ(result[2], "how");
	EXPECT_EQ(result[3], "are");
	EXPECT_EQ(result[4], "u");

	result.clear();
	count = cmg::string::Split(result, "hello world   how are u", " ");
	EXPECT_EQ(count, 5);
	EXPECT_EQ(result.size(), 5);
	EXPECT_EQ(result[0], "hello");
	EXPECT_EQ(result[1], "world");
	EXPECT_EQ(result[2], "how");
	EXPECT_EQ(result[3], "are");
	EXPECT_EQ(result[4], "u");

	result.clear();
	count = cmg::string::Split(result, "hello world   how are u", " ", 2);
	EXPECT_EQ(count, 2);
	EXPECT_EQ(result.size(), 2);
	EXPECT_EQ(result[0], "hello");
	EXPECT_EQ(result[1], "world");

	result.clear();
	count = cmg::string::Split(result, "quick brown ", " ");
	EXPECT_EQ(count, 2);
	EXPECT_EQ(result.size(), 2);
	EXPECT_EQ(result[0], "quick");
	EXPECT_EQ(result[1], "brown");

	result.clear();
	count = cmg::string::Split(result, "milk, eggs, ham, bacon", ", ");
	EXPECT_EQ(count, 4);
	EXPECT_EQ(result.size(), 4);
	EXPECT_EQ(result[0], "milk");
	EXPECT_EQ(result[1], "eggs");
	EXPECT_EQ(result[2], "ham");
	EXPECT_EQ(result[3], "bacon");
}

TEST(String, Trim)
{
	String str = "  word  ";
	cmg::string::TrimIP(str);
	EXPECT_EQ(str, "word");
	str = "  word  ";
	cmg::string::TrimLeftIP(str);
	EXPECT_EQ(str, "word  ");
	str = "  word  ";
	cmg::string::TrimRightIP(str);
	EXPECT_EQ(str, "  word");

	EXPECT_EQ(cmg::string::Trim(String("hello")), "hello");
	EXPECT_EQ(cmg::string::Trim(String(" hello")), "hello");
	EXPECT_EQ(cmg::string::Trim(String("   hello     ")), "hello");
	EXPECT_EQ(cmg::string::Trim(String("    hello ")), "hello");

	EXPECT_EQ(cmg::string::TrimLeft(String("hello")), "hello");
	EXPECT_EQ(cmg::string::TrimLeft(String(" hello")), "hello");
	EXPECT_EQ(cmg::string::TrimLeft(String("   hello     ")), "hello     ");
	EXPECT_EQ(cmg::string::TrimLeft(String("    hello ")), "hello ");

	EXPECT_EQ(cmg::string::TrimRight(String("hello")), "hello");
	EXPECT_EQ(cmg::string::TrimRight(String(" hello")), " hello");
	EXPECT_EQ(cmg::string::TrimRight(String("   hello     ")), "   hello");
	EXPECT_EQ(cmg::string::TrimRight(String("    hello ")), "    hello");
}

TEST(String, ToLower)
{
	EXPECT_EQ(cmg::string::ToLower(String("Hello")), "hello");
	EXPECT_EQ(cmg::string::ToLower(String("HELLO")), "hello");
	EXPECT_EQ(cmg::string::ToLower(String("hello")), "hello");
	EXPECT_EQ(cmg::string::ToLower(std::u16string(u"HELLO")), u"hello");
}

TEST(String, ToUpper)
{
	EXPECT_EQ(cmg::string::ToUpper(String("Hello")), "HELLO");
	EXPECT_EQ(cmg::string::ToUpper(String("HELLO")), "HELLO");
	EXPECT_EQ(cmg::string::ToUpper(String("hello")), "HELLO");
	EXPECT_EQ(cmg::string::ToUpper(std::u16string(u"hello")), u"HELLO");
}

TEST(String, EndsWith)
{
	String str = "hello";
	EXPECT_TRUE(cmg::string::EndsWith<String>(str, "hello"));
	EXPECT_TRUE(cmg::string::EndsWith<String>(str, "ello"));
	EXPECT_TRUE(cmg::string::EndsWith<String>(str, "llo"));
	EXPECT_TRUE(cmg::string::EndsWith<String>(str, "o"));
	EXPECT_FALSE(cmg::string::EndsWith<String>(str, "hell"));
	EXPECT_FALSE(cmg::string::EndsWith<String>(str, "ahello"));
	EXPECT_TRUE(cmg::string::EndsWith<std::u16string>(u"hello", u"llo"));
	EXPECT_FALSE(cmg::string::EndsWith<std::u16string>(u"hello", u"hell"));
}

TEST(String, BeginsWith)
{
	String str = "hello";
	EXPECT_TRUE(cmg::string::BeginsWith<String>(str, "hello"));
	EXPECT_TRUE(cmg::string::BeginsWith<String>(str, "hell"));
	EXPECT_TRUE(cmg::string::BeginsWith<String>(str, "he"));
	EXPECT_TRUE(cmg::string::BeginsWith<String>(str, "h"));
	EXPECT_FALSE(cmg::string::BeginsWith<String>(str, "hello world"));
	EXPECT_FALSE(cmg::string::BeginsWith<String>(str, "what"));
	EXPECT_TRUE(cmg::string::BeginsWith<std::u16string>(u"hello", u"hell"));
	EXPECT_FALSE(cmg::string::BeginsWith<std::u16string>(u"hello", u"llo"));
}


struct TestEvent : public cmg::Event
{
	TestEvent(const String& text) :
		text{text}
	{}
	String text;
};



TEST(EventSystem, Test)
{
	cmg::EventManager bus;

	struct TestClass : public cmg::EventSubscriber
	{
		String myText = "";

		void Handler(TestEvent* e)
		{
			myText = e->text;
		}
	};
	TestEvent event1("hello");
	TestEvent event2("world");
	{

		TestClass a;

		bus.Subscribe(&a, &TestClass::Handler);
		bus.Publish(&event1);
		EXPECT_EQ(a.myText, "hello");
		bus.Publish(&event2);
		EXPECT_EQ(a.myText, "world");
		bus.Unsubscribe<TestEvent>(&a);
		bus.Publish(&event1);
		EXPECT_EQ(a.myText, "world");

		bus.Subscribe(&a, &TestClass::Handler);
		bus.Publish(&event1);
		EXPECT_EQ(a.myText, "hello");
		bus.Publish(&event2);
		EXPECT_EQ(a.myText, "world");
		bus.UnsubscribeAll(&a);
		bus.Publish(&event1);
		EXPECT_EQ(a.myText, "world");

		bus.Subscribe(&a, &TestClass::Handler);
	}
	bus.Publish(&event1);
}
