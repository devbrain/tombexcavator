
set (src
        io/memory_stream_buf.cc
        io/binary_reader.cc
        io/binary_writer.cc
        io/base64.cc

        thirdparty/whereami.c
        whereami.cc
        )

set (hdr
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/exceptions.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/byte_order.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/whereami.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/macros.hh

        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/unbuffered_stream_buf.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/buffered_stream_buf.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/buffered_bidi_stream_buf.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/memory_stream_buf.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/binary_reader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/binary_writer.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/base64.hh

        io/ios_init.hh
        thirdparty/whereami.h
)

build_te_library(bsw
        SOURCES ${src}
        HEADERS ${hdr}
        PUBLIC_LINK
            std::filesystem
            Threads::Threads
        PRIVATE_LINK
            thirdparty::mio
        )
