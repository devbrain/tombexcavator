add_executable(extractor extractor.cc)
target_link_libraries(extractor vfs::vfs te::docopt)
target_compile_definitions(extractor
        PRIVATE
        ONYX_MODULE_NAME=extractor
        $<$<CXX_COMPILER_ID:MSVC>:
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_DEPRECATE
        NOMINMAX >
        )

target_compile_options(extractor PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall>
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)

target_compile_features(extractor PUBLIC cxx_std_17)