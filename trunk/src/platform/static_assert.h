/**
 * @file   static_assert.h
 * @author Igor <igor.gutnik@gmail.com>
 * @date   Wed Aug 11 13:34:48 2010
 * 
 * @brief  Compile time assertions
 * 
 * 
 */

#ifndef __PLATFORM_STATIC_ASSERT_H__
#define __PLATFORM_STATIC_ASSERT_H__

#define STATIC_ASSERT(tag_name, x)            \
  typedef int static_assert_ ## tag_name[(x) * 2 - 1]

#endif
