
#if !defined(CALLABLE_FUNCTION_HPP_INCLUDED)
#define CALLABLE_FUNCTION_HPP_INCLUDED

#include "neutrino/core/utils/callable/helpers.hpp"
#include <cstddef>

namespace mpl {
    namespace detail {

        namespace impl {

            /** Define traits for a function type */
            template<typename Fun>
            struct function_traits;

            template<typename Ret, typename... Args>
            struct function_traits<Ret(Args...)> {
                typedef Ret function_type(Args...);
                typedef Ret return_type;
                static constexpr std::size_t argc = types_count<Args...>::value;

                template<std::size_t N>
                using argument_type = typename types_n<N, Args...>::type;
            };

            template<typename Ret, typename... Args>
            const std::size_t function_traits<Ret(Args...)>::argc;

        } // namespace

    } // namespace detail


    template<typename Func>
    struct function_traits : detail::impl::function_traits<detail::remove_cvref_t<Func>> {
    };

    template<typename Func>
    struct function_traits<Func*> : detail::impl::function_traits<detail::remove_cvref_t<Func>> {
    };
} // ns mpl
#endif // CALLABLE_FUNCTION_HPP_INCLUDED

