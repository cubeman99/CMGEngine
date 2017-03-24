#ifndef _CMG_ASSERT_H_
#define _CMG_ASSERT_H_

//#include <cmgCore/cmgBase.h>
#include <cmgCore/cmgConsole.h>
#include <stdio.h>


namespace cmg { namespace core { namespace assert {

	bool AlwaysReturnFalse();

	// Returns true if a debugger is currently present.
	bool IsInDebugSession();

	void HardExit(int exitCode);

};};};


//-----------------------------------------------------------------------------
// Debugger Breaks
//-----------------------------------------------------------------------------

// if (Platform::IsInDebugSession())

// Trigger a break in the debugger if there is a debugger present.
#define CMG_DEBUGGER_BREAK() \
do {											\
	if (cmg::core::assert::IsInDebugSession())	\
		{ __debugbreak(); }						\
	else										\
		{ cmg::core::assert::HardExit(-1); }	\
} while ((void) 0, 0)


//-----------------------------------------------------------------------------
// Assertion
//-----------------------------------------------------------------------------
namespace Assertion
{
	/*
	// Information about an assertion failure.
	struct AssertInfo
	{
		const char*	pMessage;		// The message that goes with the Assert.
		const char* pCondition;		// The code of the condition used with the Assert macro.
		const char*	pFile;			// The name of the file where Assert was called.
		const int	line;			// The line number in the file where Assert was called.
		bool		isFatal;		// Is failing this assertion fatal?

		AssertInfo(const char* pMessage, const char* pCondition,
				   const char* pFile, int line, bool isFatal) :
			pMessage(pMessage),
			pCondition(pCondition),
			pFile(pFile),
			line(line),
			isFatal(isFatal)
		{}
	};

	// The possible actions in response to an assertion failure.
	enum AssertionResponse
	{
		ASSERTION_RESPONSE_IGNORE = 0,	// Ignore the assertion and continue program execution.
		ASSERTION_RESPONSE_BREAK,		// Raise a breakpoint to the debugger.
		ASSERTION_RESPONSE_ABORT,		// Exit the program immediately.
		ASSERTION_RESPONSE_RETRY,		// Reevaluate the assertion condition, and continue program execution if it passes.
	};

	// Function prototype for an Assertion handler.
	typedef AssertionResponse (*AssertionHandler)(const AssertInfo&);

	// The default assertion handler.
	AssertionResponse DefaultAssertionHandler(const AssertInfo& assertInfo);
	
	// Return the default assertion response action for the given assertion info.
	AssertionResponse GetDefaultAssertionResponse(const AssertInfo& assertInfo);
	
	// Show the assertion dialogue, which prompts the user for a response action.
	AssertionResponse ShowAssertionDialogue(const AssertInfo& assertInfo);

	// Set the assertion handler.
	void SetAssertionHandler(AssertionHandler handler);
	
	//-----------------------------------------------------------------------------
	// Engine assertion internals.
	//-----------------------------------------------------------------------------
	namespace Internal
	{
		extern AssertionHandler g_assertionHandler;
		void _ExitOnFatalAssert(const AssertInfo& assertInfo);
		void _AssertionAbortResponse(const AssertInfo& assertInfo);
	}
	*/
};


//-----------------------------------------------------------------------------
// Purpose: Assert a condition.
// Input  : _exp        - The expression to check.
//          _msg        - A character array for the assertion message.
//          _executeExp - An expression that will be executed upon failure.
//-----------------------------------------------------------------------------
#define _AssertMsg( _exp, _msg, _executeExp, _isFatal )	\
	do {																		\
		if (!(_exp)) 															\
		{ 																		\
			Assertion::AssertInfo _assertInfo(static_cast<const char*>(_msg), #_exp, __FILE__, __LINE__, _isFatal);	\
			Assertion::AssertionResponse _response = Assertion::Internal::g_assertionHandler(_assertInfo);	\
			_executeExp;														\
			if (_response == Assertion::ASSERTION_RESPONSE_BREAK)				\
				CMG_DEBUGGER_BREAK();											\
			else if (_isFatal)													\
				Assertion::Internal::_ExitOnFatalAssert(_assertInfo);			\
			else if (_response == Assertion::ASSERTION_RESPONSE_ABORT)			\
			{																	\
				Assertion::Internal::_AssertionAbortResponse(_assertInfo);		\
			}																	\
		}																		\
	} while (0)

//-----------------------------------------------------------------------------
// Purpose: Assert a condition that will only fail once.
// Input  : _exp        - The expression to check.
//          _msg        - A character array for the assertion message.
//          _executeExp - An expression that will be executed upon failure.
//-----------------------------------------------------------------------------
#define _AssertMsgOnce( _exp, _msg, _bFatal ) \
	do {																	\
		static bool fAsserted;												\
		if (!fAsserted)														\
		{ 																	\
			_AssertMsg( _exp, _msg, (fAsserted = true), _bFatal );			\
		}																	\
	} while (0)


// Assert with no message.
#define CMG_ASSERT(_Expression) \
	do {\
		if (!(_Expression))\
		{\
			cmg::core::console::SetConsoleColor(cmg::core::console::p_color::red, cmg::core::console::p_color::black);\
			printf("\nASSERTION FAILURE: (%s)\n%s(%i)\n\n", (#_Expression), __FILE__, __LINE__);\
			cmg::core::console::SetConsoleColor(cmg::core::console::p_color::light_gray, cmg::core::console::p_color::black);\
			CMG_DEBUGGER_BREAK();\
		}\
	} while((void) 0, 0)

// Assert with a message.
#define CMG_ASSERT_MSG(_Expression, _Msg) \
	do {\
		if (!(_Expression))\
		{\
			cmg::core::console::SetConsoleColor(cmg::core::console::p_color::red, cmg::core::console::p_color::black);\
			printf("\nASSERTION FAILURE: (%s)\n%s(%i)\n%s\n\n", (#_Expression), __FILE__, __LINE__, _Msg);\
			cmg::core::console::SetConsoleColor(cmg::core::console::p_color::light_gray, cmg::core::console::p_color::black);\
			CMG_DEBUGGER_BREAK();\
		}\
	} while((void) 0, 0)

// Assert that always fails.
#define CMG_ASSERT_FALSE(_Msg) \
  CMG_ASSERT_MSG(cmg::core::assert::AlwaysReturnFalse(), _Msg)

#define CMG_ASSERT_UNIMPLEMENTED_FUNCTION() \
	CMG_ASSERT_FALSE("Unimplemented function")



#endif // _CMG_ASSERT_H_

