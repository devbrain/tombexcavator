#ifndef __ABL_THROW_SPEC_HPP__
#define __ABL_THROW_SPEC_HPP__

#include <stdexcept>
#include "thirdparty/predef/predef.h"

#if defined (PREDEF_COMPILER_VISUALC)
	#define TE_THROW_SPEC(E) 
	#define TE_NOTHROW_SPEC throw ()
	#define TE_THROW(E) throw E
#else
	#define TE_THROW_SPEC(E) throw (E)
	#define TE_NOTHROW_SPEC throw ()
	#define TE_THROW(E) throw E
#endif
#endif
