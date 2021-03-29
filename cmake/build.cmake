function (build_te_library TARGET)

    string(TOUPPER ${TARGET} UPPER_TARGET)
    cmake_parse_arguments(BTL
            "" # list of names of the boolean arguments (only defined ones will be true)
            "" # list of names of mono-valued arguments
            "SOURCES;HEADERS;PUBLIC_LINK;PRIVATE_LINK"
            ${ARGN})

    set(LIBNAME "tombexcavator-${TARGET}")

    add_library(${LIBNAME} SHARED ${BTL_SOURCES} ${BTL_HEADERS})

    generate_export_header(${LIBNAME}
            BASE_NAME "${UPPER_TARGET}_EXPORT"
            EXPORT_MACRO_NAME "${UPPER_TARGET}_API"
            EXPORT_FILE_NAME "${PROJECT_CONFIG_INCLUDE}/tomb-excavator/export-${TARGET}.h"
            STATIC_DEFINE "${UPPER_TARGET}_BUILT_AS_STATIC"
            )

    target_include_directories(${LIBNAME}
            PUBLIC ${PROJECT_INCLUDE} ${PROJECT_CONFIG_INCLUDE}
            PRIVATE ${PROJECT_INCLUDE_PRIVATE}
            )

    set_target_properties(${LIBNAME} PROPERTIES CXX_STANDARD 17)

    set (LINK_LIBS)
    if (BTL_PUBLIC_LINK)
        list(APPEND LINK_LIBS PUBLIC ${BTL_PUBLIC_LINK})
    endif()

    if (BTL_PRIVATE_LINK)
        list(APPEND LINK_LIBS PRIVATE ${BTL_PRIVATE_LINK})
    endif()

    target_link_libraries(${LIBNAME} ${LINK_LIBS})


    target_compile_options(${LIBNAME} PRIVATE
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -pedantic -fvisibility=hidden
            $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden> >
            $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)
endfunction()
