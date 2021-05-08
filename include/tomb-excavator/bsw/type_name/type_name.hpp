#ifndef ADAM_BADURA_TYPE_NAME_TYPE_NAME_HPP
#define ADAM_BADURA_TYPE_NAME_TYPE_NAME_HPP

#if defined(__clang__)
#	include <tomb-excavator/bsw/type_name/type_name.clang.hpp>
#elif defined(__GNUC__)
#	include <tomb-excavator/bsw/type_name/type_name.gcc.hpp>
#elif defined(_MSC_VER)
#	include <tomb-excavator/bsw/type_name/type_name.msvc.hpp>
#else
#	error Unsupported compiler.
#endif

#endif
