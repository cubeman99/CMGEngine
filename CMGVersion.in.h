#ifndef _CMG_ENGINE_CONFIG_H_
#define _CMG_ENGINE_CONFIG_H_

// This is a configured options and settings file for the Engine


//-----------------------------------------------------------------------------
// Version number:
//-----------------------------------------------------------------------------

#define CMG_VERSION_MAJOR @CMG_VERSION_MAJOR@	// The engine's major version number.
#define CMG_VERSION_MINOR @CMG_VERSION_MINOR@	// The engine's minor version number.
#define CMG_VERSION_STRING "@CMG_VERSION_MAJOR@.@CMG_VERSION_MINOR@"	// A string containing the engine's version number.

//-----------------------------------------------------------------------------
// Other options:
//-----------------------------------------------------------------------------

#cmakedefine INCLUDE_TESTS	// Are tests included in the engine build?


#endif