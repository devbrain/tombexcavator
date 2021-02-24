//
// Created by igor on 16/02/2021.
//

#ifndef TOMBEXCAVATOR_PICTURE_LOADER_HH
#define TOMBEXCAVATOR_PICTURE_LOADER_HH

#include <cstddef>
#include <tuple>
#include <vector>

namespace formats::image
{
    struct picture;

    class picture_loader
    {
    public:
        using tester_fn_ptr = bool (*)(const char* input, std::size_t input_length);
        using loader_fn_ptr = bool (*)(const char* input, std::size_t input_length, picture& out);

        static picture_loader& instance();
        void register_functions(tester_fn_ptr tester, loader_fn_ptr loader);
        bool load(const char* input, std::size_t input_length, picture& out) const;
    private:
        picture_loader() = default;
        using func_t = std::tuple<tester_fn_ptr, loader_fn_ptr>;
        std::vector<func_t> registry;
    };
}

#endif //TOMBEXCAVATOR_PICTURE_LOADER_HH
