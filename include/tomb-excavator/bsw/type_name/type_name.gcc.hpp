#ifndef ADAM_BADURA_TYPE_NAME_TYPE_NAME_GCC_HPP
#define ADAM_BADURA_TYPE_NAME_TYPE_NAME_GCC_HPP

#if (__GNUC__ > 8) || (__GNUC__ == 8 && __GNUC_MINOR__ >= 1)
#	include <tomb-excavator/bsw/type_name/type_name.gcc.8_1.hpp>
#elif (__GNUC__ > 7) || (__GNUC__ == 7 && __GNUC_MINOR__ >= 3)
#	include <tomb-excavator/bsw/type_name/type_name.gcc.7_3.hpp>
#else
#	error When using GCC, at least version 7.3 is required.
#endif

#endif
