#ifndef _CMG_BASE_H_
#define _CMG_BASE_H_


//-----------------------------------------------------------------------------
// Raw Data Types
//-----------------------------------------------------------------------------

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef unsigned long uint64;
typedef int int32;
typedef short int16;
typedef char int8;
typedef long int64;
typedef float float32;
typedef double float64;


//-----------------------------------------------------------------------------
// NULL define
//-----------------------------------------------------------------------------

#include <cstddef>
#define CMG_NULL NULL


//-----------------------------------------------------------------------------
// Platform specific
//-----------------------------------------------------------------------------

#define CMG_MAIN main


//-----------------------------------------------------------------------------
// Inline
//-----------------------------------------------------------------------------

#define CMG_INLINE inline
// NOTE: __forceinline increases build times substantially
#define CMG_FORCE_INLINE inline



//-----------------------------------------------------------------------------
// Miscellaneous
//-----------------------------------------------------------------------------

// auto
#define auto_cref  const auto&
#define auto_ref   auto&

// Sometimes we have to pass templates in macros where commas don't work.
// There we'll have to use this macro
#define CMG_COMMA ,


//-----------------------------------------------------------------------------
// Unused Macros
//-----------------------------------------------------------------------------
// Idea taken from WildMagic5
// Avoid warnings about unused variables. This is designed for variables
// that are exposed in debug configurations but are hidden in release
// configurations.
//
// Example use in error checking:
//   int result = DoSomething();
//   CMG_UNUSED(result);
//   CMG_ASSERT_MSG(result, "Failed to do something correctly");
//-----------------------------------------------------------------------------
#define CMG_UNUSED(variable) (void)variable
#define CMG_UNUSED_2(variable1, variable2) CMG_UNUSED(variable1); CMG_UNUSED(variable2)
#define CMG_UNUSED_3(variable1, variable2, variable3) CMG_UNUSED_2(variable1, variable2); CMG_UNUSED(variable3)
#define CMG_UNUSED_4(variable1, variable2, variable3, variable4) CMG_UNUSED_3(variable1, variable2, variable3); CMG_UNUSED(variable4)
#define CMG_UNUSED_5(variable1, variable2, variable3, variable4, variable5) CMG_UNUSED_4(variable1, variable2, variable3, variable4); CMG_UNUSED(variable5)
#define CMG_UNUSED_6(variable1, variable2, variable3, variable4, variable5, variable6) CMG_UNUSED_5(variable1, variable2, variable3, variable4, variable5); CMG_UNUSED(variable6)
#define CMG_UNUSED_7(variable1, variable2, variable3, variable4, variable5, variable6, variable7) CMG_UNUSED_6(variable1, variable2, variable3, variable4, variable5, variable6); CMG_UNUSED(variable7)


#include <memory.h>
#define CMG_MALLOC(_size_)					malloc(_size_)
#define CMG_CALLOC(_numElements_, _size_)	calloc(_numElements_, _size_)
#define CMG_REALLOC(_ptr_, _size_)			realloc(_ptr_, _size_)
#define CMG_FREE(_ptr_)						free(_ptr_)


#endif