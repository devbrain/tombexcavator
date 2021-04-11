//
// Created by igor on 16/02/2021.
//

#include "picture_loader.hh"

namespace formats::image
{
    picture_loader& picture_loader::instance()
    {
        static picture_loader inst;
        return inst;
    }
    // ----------------------------------------------------------------------------------------
    void picture_loader::register_functions(tester_fn_ptr tester, loader_fn_ptr loader)
    {
        registry.emplace_back(tester, loader);
    }
    // ----------------------------------------------------------------------------------------
    bool picture_loader::load(const char* input, std::size_t input_length, picture& out) const
    {
        int i=0;
        for (auto& [tester, loader] : registry)
        {
            if (tester(input, input_length))
            {
                i++;
                return loader(input, input_length, out);
            }
        }
        return false;
    }
}