#ifndef __ABL_BYTE_ORDER_HPP__
#define __ABL_BYTE_ORDER_HPP__


#include "abl/abl_export.h"
#include "abl/util/c_types.h"


namespace abl 
{

  /// This class contains a number of static methods
/// to convert between big-endian and little-endian
/// integers of various sizes.
class ABL_API byte_order_c
	
{
public:
  static te_int16_t flipBytes(te_int16_t value);
  static te_uint16_t flipBytes(te_uint16_t value);
  static te_int32_t flipBytes(te_int32_t value);
  static te_uint32_t flipBytes(te_uint32_t value);
#if defined(TE_HAVE_INT64)
  static te_int64_t flipBytes(te_int64_t value);
  static te_uint64_t flipBytes(te_uint64_t value);
#endif

  static te_int16_t toBigEndian(te_int16_t value);
  static te_uint16_t toBigEndian (te_uint16_t value);
  static te_int32_t toBigEndian(te_int32_t value);
  static te_uint32_t toBigEndian (te_uint32_t value);
#if defined(TE_HAVE_INT64)
  static te_int64_t toBigEndian(te_int64_t value);
  static te_uint64_t toBigEndian (te_uint64_t value);
#endif

  static te_int16_t fromBigEndian(te_int16_t value);
  static te_uint16_t fromBigEndian (te_uint16_t value);
  static te_int32_t fromBigEndian(te_int32_t value);
  static te_uint32_t fromBigEndian (te_uint32_t value);
#if defined(TE_HAVE_INT64)
  static te_int64_t fromBigEndian(te_int64_t value);
  static te_uint64_t fromBigEndian (te_uint64_t value);
#endif

  static te_int16_t toLittleEndian(te_int16_t value);
  static te_uint16_t toLittleEndian (te_uint16_t value);
  static te_int32_t toLittleEndian(te_int32_t value);
  static te_uint32_t toLittleEndian (te_uint32_t value);
#if defined(TE_HAVE_INT64)
  static te_int64_t toLittleEndian(te_int64_t value);
  static te_uint64_t toLittleEndian (te_uint64_t value);
#endif

  static te_int16_t fromLittleEndian(te_int16_t value);
  static te_uint16_t fromLittleEndian (te_uint16_t value);
  static te_int32_t fromLittleEndian(te_int32_t value);
  static te_uint32_t fromLittleEndian (te_uint32_t value);
#if defined(TE_HAVE_INT64)
  static te_int64_t fromLittleEndian(te_int64_t value);
  static te_uint64_t fromLittleEndian (te_uint64_t value);
#endif

  static te_int16_t toNetwork(te_int16_t value);
  static te_uint16_t toNetwork (te_uint16_t value);
  static te_int32_t toNetwork(te_int32_t value);
  static te_uint32_t toNetwork (te_uint32_t value);
#if defined(TE_HAVE_INT64)
  static te_int64_t toNetwork(te_int64_t value);
  static te_uint64_t toNetwork (te_uint64_t value);
#endif

  static te_int16_t fromNetwork(te_int16_t value);
  static te_uint16_t fromNetwork (te_uint16_t value);
  static te_int32_t fromNetwork(te_int32_t value);
  static te_uint32_t fromNetwork (te_uint32_t value);
#if defined(TE_HAVE_INT64)
  static te_int64_t fromNetwork(te_int64_t value);
  static te_uint64_t fromNetwork (te_uint64_t value);
#endif
};


  //
  // inlines
  //
  inline te_uint16_t byte_order_c::flipBytes(te_uint16_t value)
  {
    return ((value >> 8) & 0x00FF) | ((value << 8) & 0xFF00);
  }


  inline te_int16_t byte_order_c::flipBytes(te_int16_t value)
  {
    return te_int16_t(flipBytes(te_uint16_t(value)));
  }


  inline te_uint32_t byte_order_c::flipBytes(te_uint32_t value)
  {
    return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00)
      | ((value << 8) & 0x00FF0000) | ((value << 24) & 0xFF000000);
  }


  inline te_int32_t byte_order_c::flipBytes(te_int32_t value)
  {
    return te_int32_t(flipBytes(te_uint32_t(value)));
  }


#if defined(TE_HAVE_INT64)
  inline te_uint64_t byte_order_c::flipBytes(te_uint64_t value)
  {
    te_uint32_t hi = te_uint32_t(value >> 32);
    te_uint32_t lo = te_uint32_t(value & 0xFFFFFFFF);
    return te_uint64_t(flipBytes(hi)) | (te_uint64_t(flipBytes(lo)) << 32);
  }


  inline te_int64_t byte_order_c::flipBytes(te_int64_t value)
  {
    return te_int64_t(flipBytes(te_uint64_t(value)));
  }
#endif // TE_HAVE_INT64


  //
  // some macro trickery to automate the method implementation
  //
#define ABL_IMPLEMENT_BYTEORDER_NOOP_(op, type)	\
  inline type byte_order_c::op(type value)	\
  {						\
    return value;				\
  }
#define ABL_IMPLEMENT_BYTEORDER_FLIP_(op, type)	\
  inline type byte_order_c::op(type value)	\
  {						\
    return flipBytes(value);			\
  }


#if defined(TE_HAVE_INT64)
#define ABL_IMPLEMENT_BYTEORDER_NOOP(op)		\
  ABL_IMPLEMENT_BYTEORDER_NOOP_(op, te_int16_t)		\
    ABL_IMPLEMENT_BYTEORDER_NOOP_(op, te_uint16_t)	\
    ABL_IMPLEMENT_BYTEORDER_NOOP_(op, te_int32_t)	\
    ABL_IMPLEMENT_BYTEORDER_NOOP_(op, te_uint32_t)	\
    ABL_IMPLEMENT_BYTEORDER_NOOP_(op, te_int64_t)	\
    ABL_IMPLEMENT_BYTEORDER_NOOP_(op, te_uint64_t)
#define ABL_IMPLEMENT_BYTEORDER_FLIP(op)		\
  ABL_IMPLEMENT_BYTEORDER_FLIP_(op, te_int16_t)		\
    ABL_IMPLEMENT_BYTEORDER_FLIP_(op, te_uint16_t)	\
    ABL_IMPLEMENT_BYTEORDER_FLIP_(op, te_int32_t)	\
    ABL_IMPLEMENT_BYTEORDER_FLIP_(op, te_uint32_t)	\
    ABL_IMPLEMENT_BYTEORDER_FLIP_(op, te_int64_t)	\
    ABL_IMPLEMENT_BYTEORDER_FLIP_(op, te_uint64_t)
#else
#define ABL_IMPLEMENT_BYTEORDER_NOOP(op)		\
  ABL_IMPLEMENT_BYTEORDER_NOOP_(op, te_int16_t)		\
    ABL_IMPLEMENT_BYTEORDER_NOOP_(op, te_uint16_t)	\
    ABL_IMPLEMENT_BYTEORDER_NOOP_(op, te_int32_t)	\
    ABL_IMPLEMENT_BYTEORDER_NOOP_(op, te_uint32_t)
#define ABL_IMPLEMENT_BYTEORDER_FLIP(op)		\
  ABL_IMPLEMENT_BYTEORDER_FLIP_(op, te_int16_t)		\
    ABL_IMPLEMENT_BYTEORDER_FLIP_(op, te_uint16_t)	\
    ABL_IMPLEMENT_BYTEORDER_FLIP_(op, te_int32_t)	\
    ABL_IMPLEMENT_BYTEORDER_FLIP_(op, te_uint32_t)
#endif


#if defined(TE_ARCH_BIG_ENDIAN)
#define ABL_IMPLEMENT_BYTEORDER_BIG ABL_IMPLEMENT_BYTEORDER_NOOP
#define ABL_IMPLEMENT_BYTEORDER_LIT ABL_IMPLEMENT_BYTEORDER_FLIP
#else
#define ABL_IMPLEMENT_BYTEORDER_BIG ABL_IMPLEMENT_BYTEORDER_FLIP
#define ABL_IMPLEMENT_BYTEORDER_LIT ABL_IMPLEMENT_BYTEORDER_NOOP
#endif


  ABL_IMPLEMENT_BYTEORDER_BIG(toBigEndian)
    ABL_IMPLEMENT_BYTEORDER_BIG(fromBigEndian)
    ABL_IMPLEMENT_BYTEORDER_BIG(toNetwork)
    ABL_IMPLEMENT_BYTEORDER_BIG(fromNetwork)
    ABL_IMPLEMENT_BYTEORDER_LIT(toLittleEndian)
    ABL_IMPLEMENT_BYTEORDER_LIT(fromLittleEndian)


    } 


#endif 
