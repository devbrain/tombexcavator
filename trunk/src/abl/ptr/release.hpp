#ifndef __UTILS_GUARD_RELEASE_HPP__
#define __UTILS_GUARD_RELEASE_HPP__

#include <functional>
#include <utility>
#include "utils/guard/destructor.hpp"

namespace utils
{
    template <typename K, typename T>
    struct pair_releaser_c : public std::unary_function <std::pair<K,T*>, void>
    {
        void operator () (std::pair<K,T*> p) const
            {
                destructor_c <T> d(p.second);
                d ();
            }
    };

    template <typename T>
    struct releaser_c : public std::unary_function <T*, void>
    {
        void operator () (T *victim) const
            {
                destructor_c <T> d(victim);
                d ();
            }
    };

} // ns utils


#endif
