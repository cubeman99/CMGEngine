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

	EXPECT_EQ(cmg::string::Trim("hello"), "hello");
	EXPECT_EQ(cmg::string::Trim(" hello"), "hello");
	EXPECT_EQ(cmg::string::Trim("   hello     "), "hello");
	EXPECT_EQ(cmg::string::Trim("    hello "), "hello");

	EXPECT_EQ(cmg::string::TrimLeft("hello"), "hello");
	EXPECT_EQ(cmg::string::TrimLeft(" hello"), "hello");
	EXPECT_EQ(cmg::string::TrimLeft("   hello     "), "hello     ");
	EXPECT_EQ(cmg::string::TrimLeft("    hello "), "hello ");

	EXPECT_EQ(cmg::string::TrimRight("hello"), "hello");
	EXPECT_EQ(cmg::string::TrimRight(" hello"), " hello");
	EXPECT_EQ(cmg::string::TrimRight("   hello     "), "   hello");
	EXPECT_EQ(cmg::string::TrimRight("    hello "), "    hello");

	EXPECT_EQ(cmg::string::ToLower("Hello"), "hello");
	EXPECT_EQ(cmg::string::ToLower("HELLO"), "hello");
	EXPECT_EQ(cmg::string::ToLower("hello"), "hello");
	EXPECT_EQ(cmg::string::ToUpper("Hello"), "HELLO");
	EXPECT_EQ(cmg::string::ToUpper("HELLO"), "HELLO");
	EXPECT_EQ(cmg::string::ToUpper("hello"), "HELLO");
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
