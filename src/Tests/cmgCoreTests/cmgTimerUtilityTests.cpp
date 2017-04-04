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
	EXPECT_FLOAT_EQ(0.0f, timer.GetElapsedSeconds());
	EXPECT_FLOAT_EQ(0.0f, timer.GetElapsedMilliseconds());
}

TEST(Timer, Start)
{
	Timer timer;
	float t1, t2;

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
	float t1, t2;

	timer.Start();
	Sleep(50);

	// When stopped, make sure time does not change.

	timer.Stop();
	EXPECT_FALSE(timer.IsRunning());
	Sleep(50);

	t1 = timer.GetElapsedSeconds();
	Sleep(100);
	t2 = timer.GetElapsedSeconds();
	EXPECT_FLOAT_EQ(t1, t2);

}

TEST(Timer, UnitConversions)
{
	Timer timer;
	timer.Start();

	float seconds, milliseconds;

	// Make sure seconds is equivilent to milliseconds.
	// Allow 1 ms of error due to measurements happining
	// in different code statements.

	Sleep(100);
	seconds = timer.GetElapsedSeconds();
	milliseconds = timer.GetElapsedMilliseconds();
	EXPECT_NEAR(seconds * 1000.0f, milliseconds, 1.0f);

	Sleep(50);
	seconds = timer.GetElapsedSeconds();
	milliseconds = timer.GetElapsedMilliseconds();
	EXPECT_NEAR(seconds * 1000.0f, milliseconds, 1.0f);
}



//-----------------------------------------------------------------------------
// Profiler tests
//-----------------------------------------------------------------------------

TEST(ProfileSection, Constructor)
{
	ProfileSection section("graphics");
	EXPECT_EQ("graphics", section.GetName());
}

TEST(ProfileSection, AddSubSection)
{
	ProfileSection parent("parent");
	ProfileSection* child1 = parent.AddSubSection("child1");
	ProfileSection* child2 = parent.AddSubSection("child2");


}

TEST(Profiler, Constructor)
{
	//Profiler profiler;
}


TEST(Profiler, StartStopInvocation)
{
	Profiler profiler;

	//profiler.StartInvocation("physics.broadphase");
	//Sleep(100);
	//float seconds = profiler.StopInvocation("physics.broadphase");



}


