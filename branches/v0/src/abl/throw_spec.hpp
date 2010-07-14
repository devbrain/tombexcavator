#ifndef __ABL_THROW_SPEC_HPP__
#define __ABL_THROW_SPEC_HPP__

#include <stdexcept>
#include "thirdparty/predef/predef.h"

#if !defined (TE_NO_EXCEPTIONS)
#if defined (PREDEF_COMPILER_VISUALC)
	#define TE_THROW_SPEC(E) 
	#define TE_NOTHROW_SPEC throw ()
#else
	#define TE_THROW_SPEC(E) throw (E)
	#define TE_NOTHROW_SPEC throw ()
#endif
#define TE_THROW(E) throw E
#define TE_DISABLE_EXCEPTION(CODE) try { CODE } catch (...) {}
#else
#define TE_THROW_SPEC(E) 
#define TE_NOTHROW_SPEC
#define TE_THROW(E)
#define TE_DISABLE_EXCEPTION(CODE) do { CODE } while (false);
#endif
#endif
