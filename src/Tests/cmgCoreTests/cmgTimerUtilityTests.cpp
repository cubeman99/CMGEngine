// Timer Utility Tests

#include <gtest/gtest.h>
#include <cmgCore/time/cmgTimer.h>
#include <Windows.h>


//-----------------------------------------------------------------------------
// Timer tests
//-----------------------------------------------------------------------------

TEST(Timer, Constructor)
{
	Timer timer;

	// The timer should be by default stopped.
	EXPECT_FALSE(timer.IsRunning());
	
	// Make sure no time has elapsed if not started.
	EXPECT_DOUBLE_EQ(0.0, timer.GetElapsedSeconds());
	EXPECT_DOUBLE_EQ(0.0, timer.GetElapsedMilliseconds());
}

TEST(Timer, Start)
{
	Timer timer;
	double t1, t2;

	// When started, make sure time always goes up.

	timer.Start();
	EXPECT_TRUE(timer.IsRunning());
	Sleep(50);

	t1 = timer.GetElapsedSeconds();
	Sleep(100);
	t2 = timer.GetElapsedSeconds();
	EXPECT_GT(t2, t1);

	t1 = timer.GetElapsedSeconds();
	Sleep(20);
	t2 = timer.GetElapsedSeconds();
	EXPECT_GT(t2, t1);
}

TEST(Timer, Stop)
{
	Timer timer;
	double t1, t2;

	timer.Start();
	Sleep(50);

	// When stopped, make sure time does not change.

	timer.Stop();
	EXPECT_FALSE(timer.IsRunning());
	Sleep(50);

	t1 = timer.GetElapsedSeconds();
	Sleep(100);
	t2 = timer.GetElapsedSeconds();
	EXPECT_DOUBLE_EQ(t1, t2);

}

TEST(Timer, UnitConversions)
{
	Timer timer;
	timer.Start();

	double seconds, milliseconds;

	// Make sure seconds is equivilent to milliseconds.
	// Allow 1 ms of error due to measurements happining
	// in different code statements.

	Sleep(100);
	seconds = timer.GetElapsedSeconds();
	milliseconds = timer.GetElapsedMilliseconds();
	EXPECT_NEAR(seconds * 1000.0, milliseconds, 1.0);

	Sleep(50);
	seconds = timer.GetElapsedSeconds();
	milliseconds = timer.GetElapsedMilliseconds();
	EXPECT_NEAR(seconds * 1000.0, milliseconds, 1.0);
}



//-----------------------------------------------------------------------------
// ProfileSection tests
//-----------------------------------------------------------------------------

TEST(ProfileSection, Constructor)
{
	ProfileSection section1("graphics");
	EXPECT_EQ("graphics", section1.GetName());
	ProfileSection section2("physics");
	EXPECT_EQ("physics", section2.GetName());
}

TEST(ProfileSection, GetSubSection)
{
	ProfileSection parent("parent");

	ProfileSection* child1 = parent.GetSubSection("child1");
	ASSERT_TRUE(child1 != nullptr);
	EXPECT_EQ("child1", child1->GetName());

	ProfileSection* child2 = parent["child2"];
	ASSERT_TRUE(child2 != nullptr);
	ASSERT_TRUE(child1 != child2);
	EXPECT_EQ("child2", child2->GetName());
	
	ProfileSection* child1Again = parent.GetSubSection("child1");
	EXPECT_TRUE(child1Again == child1);
}

TEST(ProfileSection, IterateSubSections)
{
	ProfileSection parent("parent");

	// Create 4 children.
	ProfileSection* children[4];
	children[0] = parent.GetSubSection("child1");
	children[1] = parent.GetSubSection("child2");
	children[2] = parent.GetSubSection("child3");
	children[3] = parent.GetSubSection("child4");

	// Iterate the the children and mark off
	// how many times each one was found.
	int count[4] = { 0 };
	for (auto it = parent.subsections_begin();
		it != parent.subsections_end(); ++it)
	{
		for (unsigned int i = 0; i < 4; ++i)
		{
			if (*it == children[i])
				count[i]++;
		}
	}

	// Make sure we iterate each child once and only once.
	EXPECT_EQ(1, count[0]);
	EXPECT_EQ(1, count[1]);
	EXPECT_EQ(1, count[2]);
	EXPECT_EQ(1, count[3]);
}

TEST(ProfileSection, GetParentSection)
{
	ProfileSection parent("parent");
	EXPECT_TRUE(nullptr == parent.GetParentSection());

	ProfileSection* child1 = parent.GetSubSection("child1");
	EXPECT_TRUE(&parent == child1->GetParentSection());

	ProfileSection* child1b = child1->GetSubSection("child1b");
	EXPECT_TRUE(child1 == child1b->GetParentSection());

	ProfileSection* child2 = parent.GetSubSection("child2");
	EXPECT_TRUE(&parent == child2->GetParentSection());
}

TEST(ProfileSection, StartStopInvocation)
{
	ProfileSection section("abc");

	section.StartInvocation();
	Sleep(100);
	double dt = section.StopInvocation();
	
	EXPECT_NEAR(100.0, dt * 1000.0, 1.0);
	EXPECT_DOUBLE_EQ(dt, section.GetTotalTime());
	EXPECT_EQ(1, section.GetNumInvocations());
	
	section.StartInvocation();
	Sleep(100);
	dt = section.StopInvocation();
	
	EXPECT_NEAR(100.0, dt * 1000.0, 1.0);
	EXPECT_EQ(2, section.GetNumInvocations());
}

TEST(ProfileSection, GetAverageTime)
{
	ProfileSection section("test");

	for (unsigned int i = 0; i < 4; ++i)
	{
		section.StartInvocation();
		Sleep(25);
		section.StopInvocation();
	}
		
	EXPECT_DOUBLE_EQ(section.GetTotalTime() / (double) section.GetNumInvocations(), section.GetAverageTime());
	EXPECT_DOUBLE_EQ(section.GetTotalTime() / 3.14, section.GetAverageTime(3.14));
}

TEST(ProfileSection, Reset)
{
	ProfileSection section("test");
	ProfileSection* child = section.GetSubSection("child");
	ProfileSection* childChild = child->GetSubSection("child's child");

	section.StartInvocation();
	{
		Sleep(20);

		child->StartInvocation();
		{
			Sleep(20);

			childChild->StartInvocation();
			{
				Sleep(20);
			}
			childChild->StopInvocation();
		}
		child->StopInvocation();
	}
	section.StopInvocation();
		
	EXPECT_GT(section.GetAverageTime(), 0.0);
	EXPECT_GT(section.GetTotalTime(), 0.0);
	EXPECT_EQ(1, section.GetNumInvocations());
	EXPECT_GT(child->GetAverageTime(), 0.0);
	EXPECT_GT(child->GetTotalTime(), 0.0);
	EXPECT_EQ(1, child->GetNumInvocations());
	EXPECT_GT(childChild->GetAverageTime(), 0.0);
	EXPECT_GT(childChild->GetTotalTime(), 0.0);
	EXPECT_EQ(1, childChild->GetNumInvocations());

	// Reset the section and make sure things go to zero.
	// Make sure children get reset too.
	section.Reset();
	EXPECT_DOUBLE_EQ(0.0, section.GetAverageTime());
	EXPECT_DOUBLE_EQ(0.0, section.GetTotalTime());
	EXPECT_EQ(0, section.GetNumInvocations());
	EXPECT_DOUBLE_EQ(0.0, child->GetAverageTime());
	EXPECT_DOUBLE_EQ(0.0, child->GetTotalTime());
	EXPECT_EQ(0, child->GetNumInvocations());
	EXPECT_DOUBLE_EQ(0.0, childChild->GetAverageTime());
	EXPECT_DOUBLE_EQ(0.0, childChild->GetTotalTime());
	EXPECT_EQ(0, childChild->GetNumInvocations());
}

TEST(ProfileSection, Print)
{
	// Create the profiling section hierarchy.
	ProfileSection root("test");
	ProfileSection* sectionA	= root.GetSubSection("a");
	ProfileSection* sectionAA	= sectionA->GetSubSection("a");
	ProfileSection* sectionAB	= sectionA->GetSubSection("b");
	ProfileSection* sectionB	= root.GetSubSection("b");
	ProfileSection* sectionBA	= sectionB->GetSubSection("a");
	ProfileSection* sectionBB	= sectionB->GetSubSection("b");
	ProfileSection* sectionBC	= sectionB->GetSubSection("c");
	ProfileSection* sectionC	= root.GetSubSection("c");

	// Run some test invocations.
	root.StartInvocation();
	{
		sectionA->StartInvocation();
		{
			sectionAA->StartInvocation();
				Sleep(10);
			sectionAA->StopInvocation();
			sectionAB->StartInvocation();
				Sleep(20);
			sectionAB->StopInvocation();
			sectionAB->StartInvocation();
				Sleep(40);
			sectionAB->StopInvocation();
		}
		sectionA->StopInvocation();
		
		sectionB->StartInvocation();
		{
			sectionBA->StartInvocation();
				Sleep(10);
			sectionBA->StopInvocation();
			sectionBB->StartInvocation();
				Sleep(20);
			sectionBB->StopInvocation();
			sectionBC->StartInvocation();
				Sleep(30);
			sectionBC->StopInvocation();
		}
		sectionB->StopInvocation();
		
		sectionC->StartInvocation();
		{
			Sleep(20);
		}
		sectionC->StopInvocation();
	}
	root.StopInvocation();

	// Print out the profiling info.
	root.Print(std::cout);
}




//-----------------------------------------------------------------------------
// Profiler tests
//-----------------------------------------------------------------------------

//TEST(Profiler, Constructor)
//{
	//Profiler profiler;
//}

