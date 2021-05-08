
set (src
        io/memory_stream_buf.cc
        io/binary_reader.cc
        io/binary_writer.cc
        io/base64.cc

        thirdparty/whereami.c
        whereami.cc
        string_tokenizer.cc
        ascii.cc
        )

set (hdr
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/exceptions.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/byte_order.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/whereami.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/macros.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/string_tokenizer.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/ascii.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/string_utils.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/string_constant.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/override.hh

        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/unbuffered_stream_buf.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/buffered_stream_buf.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/buffered_bidi_stream_buf.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/memory_stream_buf.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/binary_reader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/binary_writer.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/base64.hh


        ${PROJECT_INCLUDE}/tomb-excavator/bsw/type_name/type_name.clang.5.hpp
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/type_name/type_name.clang.hpp
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/type_name/type_name.gcc.7_3.hpp
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/type_name/type_name.gcc.8_1.hpp
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/type_name/type_name.gcc.hpp
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/type_name/type_name.hpp
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/type_name/type_name.msvc.19_21.hpp
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/type_name/type_name.msvc.hpp


        io/ios_init.hh
        thirdparty/whereami.h
)

build_te_library(bsw
        SOURCES ${src}
        HEADERS ${hdr}
        PUBLIC_LINK
            std::filesystem
            Threads::Threads
            pfr
            thirdparty::zlibng
        PRIVATE_LINK
            thirdparty::mio
        )
