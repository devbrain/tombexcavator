
set (src
        vfs/directory.cc
        vfs/file_system.cc

        physfs/directory.cc
        vfs_registry.cc
        )

set (hdr
        ${PROJECT_INCLUDE}/tomb-excavator/provider/vfs/directory.hh
        ${PROJECT_INCLUDE}/tomb-excavator/provider/vfs/file_system.hh

        ${PROJECT_INCLUDE}/tomb-excavator/provider/physfs/directory.hh

        ${PROJECT_INCLUDE}/tomb-excavator/provider/file_types.hh
        ${PROJECT_INCLUDE}/tomb-excavator/provider/vfs_registry.hh

        )

add_library(tombexcavator-provider SHARED ${src} ${hdr})

generate_export_header(tombexcavator-provider
             BASE_NAME PROVIDER_EXPORT
             EXPORT_MACRO_NAME PROVIDER_API
             EXPORT_FILE_NAME tombexcavator-provider_export.h
             STATIC_DEFINE PROVIDER_BUILT_AS_STATIC
)

target_include_directories(tombexcavator-provider
        PUBLIC ${PROJECT_CONFIG_INCLUDE} ${PROJECT_INCLUDE} ${CMAKE_CURRENT_BINARY_DIR}
        PRIVATE ${PROJECT_INCLUDE_PRIVATE}
        )

set_target_properties(tombexcavator-provider PROPERTIES CXX_STANDARD 17)

target_link_libraries(tombexcavator-provider
          PUBLIC
          tombexcavator-formats
          tombexcavator-bsw
          Threads::Threads
          ${CMAKE_DL_LIBS}

          PRIVATE
          thirdparty::mio
        )

target_compile_options(tombexcavator-provider PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -pedantic -fvisibility=hidden
        $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden> >
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)
