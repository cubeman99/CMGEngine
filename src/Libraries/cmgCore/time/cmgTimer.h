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

	double GetElapsedSeconds() const;
	double GetElapsedMilliseconds() const;


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
	ProfileSection(const std::string& name);
	~ProfileSection();

	inline std::string GetName() const { return m_name; }

	ProfileSection* GetSubSection(const std::string& sectionName);
	ProfileSection* operator [](const std::string& sectionName);
	ProfileSection* GetParentSection();

	std::vector<ProfileSection*>::iterator subsections_begin();
	std::vector<ProfileSection*>::iterator subsections_end();

	// Reset this section and all its sub-sections.
	void Reset();

	// Print profiling info for this section and all its sub-sections.
	void Print(std::ostream& outStream);

	void StartInvocation();
	double StopInvocation();

	int GetNumInvocations() const;
	double GetTotalTime() const;
	double GetAverageTime() const;
	double GetAverageTime(double dividend) const;
	
private:
	void Print(std::ostream& outStream, int depth);

	Timer m_timer;
	double m_totalTime;
	int m_numInvocations;
	std::string m_name;
	ProfileSection* m_parentSection;
	std::vector<ProfileSection*> m_subSections;
};


//-----------------------------------------------------------------------------
// Profiler
//-----------------------------------------------------------------------------
class Profiler
{
public:
	Profiler();
	~Profiler();
	
	void Reset();
	
	ProfileSection* GetSection(const std::string& sectionName);
	ProfileSection* operator [](const std::string& sectionName);

private:

};


#endif // _CMG_CORE_TIME_TIMER_H