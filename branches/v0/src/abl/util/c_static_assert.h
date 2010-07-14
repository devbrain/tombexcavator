#ifndef __ABL_UTIL_C_STATIC_ASSERT_H__
#define __ABL_UTIL_C_STATIC_ASSERT_H__

#define C_STATIC_ASSERT(tag_name, x)            \
       typedef int cache_static_assert_ ## tag_name[(x) * 2 - 1]

#endif
