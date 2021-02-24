
set (src
        io/memory_stream_buf.cc
        io/binary_reader.cc
        io/binary_writer.cc

        thirdparty/whereami.c
        whereami.cc
        )

set (hdr
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/exceptions.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/byte_order.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/whereami.hh

        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/unbuffered_stream_buf.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/buffered_stream_buf.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/buffered_bidi_stream_buf.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/memory_stream_buf.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/binary_reader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/bsw/io/binary_writer.hh

        io/ios_init.hh
        thirdparty/whereami.h

        )

add_library(tombexcavator-bsw SHARED ${src} ${hdr})

generate_export_header(tombexcavator-bsw
             BASE_NAME BSW_EXPORT
             EXPORT_MACRO_NAME BSW_API
             EXPORT_FILE_NAME tombexcavator-bsw_export.h
             STATIC_DEFINE BSW_BUILT_AS_STATIC
)

target_include_directories(tombexcavator-bsw
        PUBLIC ${PROJECT_CONFIG_INCLUDE} ${PROJECT_INCLUDE} ${CMAKE_CURRENT_BINARY_DIR}
        PRIVATE ${PROJECT_INCLUDE_PRIVATE}
        )

set_target_properties(tombexcavator-bsw PROPERTIES CXX_STANDARD 17)

target_link_libraries(tombexcavator-bsw
          PUBLIC
          std::filesystem
          Threads::Threads

          PRIVATE
          thirdparty::mio
        )

target_compile_options(tombexcavator-bsw PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -pedantic -fvisibility=hidden
        $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden> >
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)
