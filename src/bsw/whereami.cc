//
// Created by igor on 21/02/2021.
//

#include <vector>
#include <tomb-excavator/bsw/whereami.hh>
#include <tomb-excavator/bsw/exceptions.hh>
#include "bsw/thirdparty/whereami.h"

namespace
{
    using query_fn_t = int (*)(char* out, int capacity, int* dirname_length);
    std::string get_wai_path(query_fn_t fn)
    {
        int length = fn(nullptr, 0, nullptr);
        if (length <= 0)
        {
            return "";
        }

        std::vector<char> out(length + 1, '\0');
        fn(out.data(), length, &length);
        return out.data();
    }
}

namespace bsw
{
    std::filesystem::path get_executable_path()
    {
        auto res = get_wai_path(wai_getExecutablePath);
        if (res.empty())
        {
            RAISE_EX("Failed to determine executable path");
        }
        return res;
    }

    std::filesystem::path get_module_path()
    {
        auto res = get_wai_path(wai_getModulePath);
        if (res.empty())
        {
            RAISE_EX("Failed to determine module path");
        }
        return res;
    }
} // ns