//
// Created by igor on 08/04/2021.
//

#ifndef TOMBEXCAVATOR_OVERRIDE_HH
#define TOMBEXCAVATOR_OVERRIDE_HH

#include <utility>

namespace bsw
{
    template<class T>
    struct override_helper { using type=T; };

    template<class T>
    using override_helper_t = typename override_helper<T>::type;

    template<class R, class...Args>
    struct override_helper<R(*)(Args...)> {
        struct type {
            R(*f)(Args...);
            R operator()(Args...args)const { return f(std::forward<Args>(args)...); }
            type(R(*in)(Args...)):f(in) {}
        };
    };
    template<class R, class...Args>
    struct override_helper<R(&)(Args...)>:override_helper<R(*)(Args...)> {
        using override_helper<R(*)(Args...)>::override_helper;
    };

    template<class...Fs>
    struct overload:override_helper_t<Fs>... {
        using override_helper_t<Fs>::operator()...;
        overload(Fs...fs):override_helper_t<Fs>(std::move(fs))... {}
    };
}

#endif //TOMBEXCAVATOR_OVERRIDE_HH
