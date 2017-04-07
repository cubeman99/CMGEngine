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

double Timer::GetElapsedSeconds() const
{
	if (m_isRunning)
		return (cmgGetCurrentTime() - m_startTime);
	else
		return (m_stopTime - m_startTime);
}

double Timer::GetElapsedMilliseconds() const
{
	return (GetElapsedSeconds() * 1000.0);
}


//-----------------------------------------------------------------------------
// ProfileSection
//-----------------------------------------------------------------------------

ProfileSection::ProfileSection(const std::string& name) :
	m_name(name),
	m_numInvocations(0),
	m_totalTime(0.0),
	m_parentSection(nullptr)
{
}

ProfileSection::~ProfileSection()
{
	// Delete all sub-sections.
	for (unsigned int i = 0; i < m_subSections.size(); ++i)
		delete m_subSections[i];
	m_subSections.clear();
}
	
ProfileSection* ProfileSection::GetSubSection(const std::string& sectionName)
{
	// Find if the sub-section already exists.
	for (unsigned int i = 0; i < m_subSections.size(); ++i)
	{
		if (m_subSections[i]->m_name == sectionName)
			return m_subSections[i];
	}

	// If it does not exist, then create it.
	ProfileSection* newSection = new ProfileSection(sectionName);
	newSection->m_parentSection = this;
	m_subSections.push_back(newSection);
	return newSection;
}


ProfileSection* ProfileSection::operator [](const std::string& sectionName)
{
	return GetSubSection(sectionName);
}

ProfileSection* ProfileSection::GetParentSection()
{
	return m_parentSection;
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
	// Reset this section.
	m_numInvocations = 0;
	m_totalTime = 0.0;

	// Reset all sub-sections.
	for (unsigned int i = 0; i < m_subSections.size(); ++i)
		m_subSections[i]->Reset();
}

void ProfileSection::Print(std::ostream& outStream)
{
	Print(outStream, 0);
}

void ProfileSection::StartInvocation()
{
	m_timer.Start();
}

double ProfileSection::StopInvocation()
{
	m_timer.Stop();
	double dt = m_timer.GetElapsedSeconds();
	m_totalTime += dt;
	++m_numInvocations;
	return dt;
}

int ProfileSection::GetNumInvocations() const
{
	return m_numInvocations;
}

double ProfileSection::GetTotalTime() const
{
	return m_totalTime;
}

double ProfileSection::GetAverageTime() const
{
	if (m_numInvocations != 0)
		return (m_totalTime / (double) m_numInvocations);
	return 0.0;
}

double ProfileSection::GetAverageTime(double dividend) const
{
	if (dividend != 0.0)
		return (m_totalTime / dividend);
	return 0.0;
}


void ProfileSection::Print(std::ostream& outStream, int depth)
{
	// PRINT FORMAT:
	//  - root : 12.23 ms
	//    - child A : 12.23 ms
	//    - child B : 12.23 ms
	//    - child B : 12.23 ms
	//    - (other) : 12.23 ms

	// Print info for this section.
	outStream << " ";
	for (int i = 0; i < depth; ++i)
		outStream << "  ";
	outStream << "- " << m_name << " : " << (m_totalTime * 1000.0) << " ms" << std::endl;

	// Print all sub-sections.
	for (unsigned int i = 0; i < m_subSections.size(); ++i)
		m_subSections[i]->Print(outStream, depth + 1);
}

