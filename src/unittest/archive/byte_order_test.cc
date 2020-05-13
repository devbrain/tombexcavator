#include <doctest.h>
#include <archive/byte_order.hh>

TEST_CASE("byte order - test flip")
{
    {
	int16_t norm = (int16_t) 0xAABB;
	int16_t flip = archive::byte_order::flip_bytes(norm);
	REQUIRE (int16_t(flip) == int16_t(0xBBAA));
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (flip == norm);
    }
    {
	int16_t norm = (int16_t) 0xAABB;
	int16_t flip = archive::byte_order::flip_bytes(norm);
	REQUIRE (flip == int16_t(0xBBAA));
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (flip == norm);
    }
    {
	int32_t norm = 0xAABBCCDD;
	int32_t flip = archive::byte_order::flip_bytes(norm);
	REQUIRE (uint32_t(flip) == 0xDDCCBBAA);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (flip == norm);
    }
    {
	uint32_t norm = 0xAABBCCDD;
	uint32_t flip = archive::byte_order::flip_bytes(norm);
	REQUIRE (flip == 0xDDCCBBAA);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (flip == norm);
    }
#if defined(ARCHIVE_HAVE_INT64)
    {
	int64_t norm = (int64_t(0x8899AABB) << 32) + 0xCCDDEEFF;
	int64_t flip = archive::byte_order::flip_bytes(norm);
	REQUIRE (flip == (int64_t(0xFFEEDDCC) << 32) + 0xBBAA9988);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (flip == norm);
    }
    {
	uint64_t norm = (uint64_t(0x8899AABB) << 32) + 0xCCDDEEFF;
	uint64_t flip = archive::byte_order::flip_bytes(norm);
	REQUIRE (flip == (uint64_t(0xFFEEDDCC) << 32) + 0xBBAA9988);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (flip == norm);
    }
#endif
}

TEST_CASE("byte order - test big endian")
{
#if defined(ARCHIVE_ARCH_IS_BIG_ENDIAN)
    //
    // big-endian systems
    //
    {
	int16_t norm = 4;
	int16_t flip = archive::byte_order::to_big_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	uint16_t norm = 4;
	uint16_t flip = archive::byte_order::to_big_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	int32_t norm = 4;
	int32_t flip = archive::byte_order::to_big_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	uint32_t norm = 4;
	uint32_t flip = archive::byte_order::to_big_endian(norm);
	REQUIRE (norm == flip);
    }
#if defined(ARCHIVE_HAVE_INT64)
    {
	int64_t norm = 4;
	int64_t flip = archive::byte_order::to_big_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	uint64_t norm = 4;
	uint64_t flip = archive::byte_order::to_big_endian(norm);
	REQUIRE (norm == flip);
    }
#endif

    {
	int16_t norm = 4;
	int16_t flip = archive::byte_order::from_big_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	uint16_t norm = 4;
	uint16_t flip = archive::byte_order::from_big_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	int32_t norm = 4;
	int32_t flip = archive::byte_order::from_big_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	uint32_t norm = 4;
	uint32_t flip = archive::byte_order::from_big_endian(norm);
	REQUIRE (norm == flip);
    }
#if defined(ARCHIVE_HAVE_INT64)
    {
	int64_t norm = 4;
	int64_t flip = archive::byte_order::from_big_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	uint64_t norm = 4;
	uint64_t flip = archive::byte_order::from_big_endian(norm);
	REQUIRE (norm == flip);
    }
#endif
#else
    //
    // little-endian systems
    //
    {
	int16_t norm = 4;
	int16_t flip = archive::byte_order::to_big_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint16_t norm = 4;
	uint16_t flip = archive::byte_order::to_big_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	int32_t norm = 4;
	int32_t flip = archive::byte_order::to_big_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint32_t norm = 4;
	uint32_t flip = archive::byte_order::to_big_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
#if defined(ARCHIVE_HAVE_INT64)
    {
	int64_t norm = 4;
	int64_t flip = archive::byte_order::to_big_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint64_t norm = 4;
	uint64_t flip = archive::byte_order::to_big_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
#endif

    {
	int16_t norm = 4;
	int16_t flip = archive::byte_order::from_big_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint16_t norm = 4;
	uint16_t flip = archive::byte_order::from_big_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	int32_t norm = 4;
	int32_t flip = archive::byte_order::from_big_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint32_t norm = 4;
	uint32_t flip = archive::byte_order::from_big_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
#if defined(ARCHIVE_HAVE_INT64)
    {
	int64_t norm = 4;
	int64_t flip = archive::byte_order::from_big_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint64_t norm = 4;
	uint64_t flip = archive::byte_order::from_big_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
#endif
#endif
}

TEST_CASE("byte order - test little endian")
{
#if !defined(POCO_ARCH_LITTLE_ENDIAN)
    //
    // big-endian systems
    //
    {
	int16_t norm = 4;
	int16_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	uint16_t norm = 4;
	uint16_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	int32_t norm = 4;
	int32_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	uint32_t norm = 4;
	uint32_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm == flip);
    }
#if defined(ARCHIVE_HAVE_INT64)
    {
	int64_t norm = 4;
	int64_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	uint64_t norm = 4;
	uint64_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm == flip);
    }
#endif

    {
	int16_t norm = 4;
	int16_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	uint16_t norm = 4;
	uint16_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	int32_t norm = 4;
	int32_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	uint32_t norm = 4;
	uint32_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm == flip);
    }
#if defined(ARCHIVE_HAVE_INT64)
    {
	int64_t norm = 4;
	int64_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm == flip);
    }
    {
	uint64_t norm = 4;
	uint64_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm == flip);
    }
#endif
#else
    //
    // little-endian systems
    //
    {
	int16_t norm = 4;
	int16_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint16_t norm = 4;
	uint16_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	int32_t norm = 4;
	int32_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint32_t norm = 4;
	uint32_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
#if defined(ARCHIVE_HAVE_INT64)
    {
	int64_t norm = 4;
	int64_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint64_t norm = 4;
	uint64_t flip = archive::byte_order::to_little_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
#endif

    {
	int16_t norm = 4;
	int16_t flip = archive::byte_order::from_little_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint16_t norm = 4;
	uint16_t flip = archive::byte_order::from_little_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	int32_t norm = 4;
	int32_t flip = archive::byte_order::from_little_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint32_t norm = 4;
	uint32_t flip = archive::byte_order::from_little_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
#if defined(ARCHIVE_HAVE_INT64)
    {
	int64_t norm = 4;
	int64_t flip = archive::byte_order::from_little_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint64_t norm = 4;
	uint64_t flip = archive::byte_order::from_little_endian(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
#endif
#endif
}

TEST_CASE("byte order - byte order network")
{
#if defined(ARCHIVE_ARCH_IS_BIG_ENDIAN)
    //
    // big-endian systems
    //
    {
	int16_t norm = 4;
	int16_t flip = archive::byte_order::to_network(norm);
	REQUIRE (norm == flip);
    }
    {
	uint16_t norm = 4;
	uint16_t flip = archive::byte_order::to_network(norm);
	REQUIRE (norm == flip);
    }
    {
	int32_t norm = 4;
	int32_t flip = archive::byte_order::to_network(norm);
	REQUIRE (norm == flip);
    }
    {
	uint32_t norm = 4;
	uint32_t flip = archive::byte_order::to_network(norm);
	REQUIRE (norm == flip);
    }
#if defined(ARCHIVE_HAVE_INT64)
    {
	int64_t norm = 4;
	int64_t flip = archive::byte_order::to_network(norm);
	REQUIRE (norm == flip);
    }
    {
	uint64_t norm = 4;
	uint64_t flip = archive::byte_order::to_network(norm);
	REQUIRE (norm == flip);
    }
#endif

    {
	int16_t norm = 4;
	int16_t flip = archive::byte_order::from_network(norm);
	REQUIRE (norm == flip);
    }
    {
	uint16_t norm = 4;
	uint16_t flip = archive::byte_order::from_network(norm);
	REQUIRE (norm == flip);
    }
    {
	int32_t norm = 4;
	int32_t flip = archive::byte_order::from_network(norm);
	REQUIRE (norm == flip);
    }
    {
	uint32_t norm = 4;
	uint32_t flip = archive::byte_order::from_network(norm);
	REQUIRE (norm == flip);
    }
#if defined(ARCHIVE_HAVE_INT64)
    {
	int64_t norm = 4;
	int64_t flip = archive::byte_order::from_network(norm);
	REQUIRE (norm == flip);
    }
    {
	uint64_t norm = 4;
	uint64_t flip = archive::byte_order::from_network(norm);
	REQUIRE (norm == flip);
    }
#endif
#else
    //
    // little-endian systems
    //
    {
	int16_t norm = 4;
	int16_t flip = archive::byte_order::to_network(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint16_t norm = 4;
	uint16_t flip = archive::byte_order::to_network(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	int32_t norm = 4;
	int32_t flip = archive::byte_order::to_network(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint32_t norm = 4;
	uint32_t flip = archive::byte_order::to_network(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
#if defined(ARCHIVE_HAVE_INT64)
    {
	int64_t norm = 4;
	int64_t flip = archive::byte_order::to_network(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint64_t norm = 4;
	uint64_t flip = archive::byte_order::to_network(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
#endif

    {
	int16_t norm = 4;
	int16_t flip = archive::byte_order::from_network(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint16_t norm = 4;
	uint16_t flip = archive::byte_order::from_network(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	int32_t norm = 4;
	int32_t flip = archive::byte_order::from_network(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint32_t norm = 4;
	uint32_t flip = archive::byte_order::from_network(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
#if defined(ARCHIVE_HAVE_INT64)
    {
	int64_t norm = 4;
	int64_t flip = archive::byte_order::from_network(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
    {
	uint64_t norm = 4;
	uint64_t flip = archive::byte_order::from_network(norm);
	REQUIRE (norm != flip);
	flip = archive::byte_order::flip_bytes(flip);
	REQUIRE (norm == flip);
    }
#endif
#endif
}

