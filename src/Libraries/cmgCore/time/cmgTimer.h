#ifndef _CMG_CORE_TIME_TIMER_H
#define _CMG_CORE_TIME_TIMER_H

#include <cmgCore/string/cmgString.h>
#include <sstream>


//-----------------------------------------------------------------------------
// Timer - timer used to measure elapsed time.
//-----------------------------------------------------------------------------
class Timer
{
public:
	Timer();
	~Timer();

	// Start the timer. This also resets the timer if it is already started.
	void Start();

	// Stop the timer.
	void Stop();

	inline bool IsRunning() const { return m_isRunning; }

	float GetElapsedSeconds() const;
	float GetElapsedMilliseconds() const;


private:
	double m_startTime;
	double m_stopTime;
	bool m_isRunning;
};


//-----------------------------------------------------------------------------
// ProfileSection
//-----------------------------------------------------------------------------
class ProfileSection
{
public:
	ProfileSection(const std::string& name) {}

	inline const std::string& GetName() const { return "?"; }

	ProfileSection* AddSubSection(const std::string& name)
	{
		return nullptr;
	}

	void StartInvocation() {}
	float StopInvocation() { return -1.0f; }

private:
};


//-----------------------------------------------------------------------------
// Profiler
//-----------------------------------------------------------------------------
class Profiler
{
public:
	Profiler();
	~Profiler();
	
	void StartInvocation(const std::string& label) {}
	float StopInvocation(const std::string& label) { return -1.0f; }


private:

};


#endif // _CMG_CORE_TIME_TIMER_H