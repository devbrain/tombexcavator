add_library(docopt SHARED docopt.cpp docopt.h docopt_private.h docopt_util.h docopt_value.h)
add_library(te::docopt ALIAS docopt)
find_package(nlohmann_json REQUIRED)

generate_export_header(docopt
        EXPORT_MACRO_NAME DOCOPT_API
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/docopt/docopt_api.h
        )
target_link_libraries(docopt nlohmann_json::nlohmann_json)
target_compile_features(docopt PUBLIC cxx_std_17)
target_include_directories(docopt
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        ${CMAKE_SOURCE_DIR}/src
        )
target_compile_options(archive PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -fvisibility=hidden -fvisibility-inlines-hidden>
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)


