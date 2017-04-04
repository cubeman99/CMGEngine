#include "cmgTimer.h"
#include <cmgCore/log/cmgLogging.h>


//-----------------------------------------------------------------------------
// Native timer implementations
//-----------------------------------------------------------------------------

// Native timer implementation code by thebennybox 
// Modified by David Jordan
// https://github.com/BennyQBD/3DEngineCpp/blob/master/src/core/timing.cpp


// Define which operating system we are on.
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WIN64)
#define OS_WINDOWS
#elif defined(__linux__)
#define OS_LINUX
#elif __cplusplus >= 201103L
#define OS_OTHER_CPP11
#else
#define OS_OTHER
#endif

#if defined(OS_WINDOWS)

#include <Windows.h>
#include <iostream>
static double g_freq;
static bool g_timerInitialized = false;

// Get the current time in seconds.
static double cmgGetCurrentTime()
{
	// Initialize the timer if it hasn't been aleady.
	if (!g_timerInitialized)
	{
		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li))
		{
			CMG_LOG_ERROR() << "QueryPerformanceFrequency failed in timer initialization\n";
		}

		g_freq = double(li.QuadPart);
		g_timerInitialized = true;
	}

	// Query the performance counter to 
	LARGE_INTEGER li;
	if (!QueryPerformanceCounter(&li))
	{
		CMG_LOG_ERROR() << "QueryPerformanceCounter failed in get time!\n";
	}

	return (double(li.QuadPart) / g_freq);
}

#endif


//-----------------------------------------------------------------------------
// Timer
//-----------------------------------------------------------------------------

Timer::Timer() :
	m_isRunning(false),
	m_startTime(0.0),
	m_stopTime(0.0)
{
}

Timer::~Timer()
{
}

void Timer::Start()
{
	m_isRunning = true;
	m_startTime = cmgGetCurrentTime();
}

void Timer::Stop()
{
	if (m_isRunning)
	{
		m_isRunning = false;
		m_stopTime = cmgGetCurrentTime();
	}
}

float Timer::GetElapsedSeconds() const
{
	if (m_isRunning)
		return (float) (cmgGetCurrentTime() - m_startTime);
	else
		return (float) (m_stopTime - m_startTime);
}

float Timer::GetElapsedMilliseconds() const
{
	return (GetElapsedSeconds() * 1000.0f);
}


//-----------------------------------------------------------------------------
// ProfileSection
//-----------------------------------------------------------------------------

ProfileSection::ProfileSection(const std::string& name)
{
}

ProfileSection::~ProfileSection()
{
}
	
ProfileSection* ProfileSection::GetSubSection(const std::string& sectionName)
{
	return nullptr;
}


ProfileSection* ProfileSection::operator [](const std::string& sectionName)
{
	return nullptr;
}

ProfileSection* ProfileSection::GetParentSection()
{
	return nullptr;
}

std::vector<ProfileSection*>::iterator ProfileSection::subsections_begin()
{
	return m_subSections.begin();
}

std::vector<ProfileSection*>::iterator ProfileSection::subsections_end()
{
	return m_subSections.end();
}

void ProfileSection::Reset()
{
}

void ProfileSection::Print(std::ostream& outStream)
{

}

void ProfileSection::StartInvocation()
{
}

double ProfileSection::StopInvocation()
{
	return 0.0;
}

int ProfileSection::GetNumInvocations() const
{
	return -1;
}

double ProfileSection::GetTotalTime() const
{
	return 0.0;
}

double ProfileSection::GetAverageTime() const
{
	return 0.0;
}

double ProfileSection::GetAverageTime(double dividend) const
{
	return 0.0;
}


void ProfileSection::Print(std::ostream& outStream, int depth)
{

}




//-----------------------------------------------------------------------------
// Profiler
//-----------------------------------------------------------------------------

Profiler::Profiler()
{
}

Profiler::~Profiler()
{
}
	
void Profiler::Reset()
{
}

ProfileSection* Profiler::GetSection(const std::string& sectionName)
{
	return nullptr;
}

ProfileSection* Profiler::operator [](const std::string& sectionName)
{
	return nullptr;
}

