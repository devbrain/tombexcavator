
#if !defined(CALLABLE_FUNCTOR_HPP_INCLUDED)
#define CALLABLE_FUNCTOR_HPP_INCLUDED

#include "neutrino/core/utils/callable/helpers.hpp"
#include "neutrino/core/utils/callable/function.hpp"
#include "neutrino/core/utils/callable/member_function.hpp"
namespace mpl {
    namespace detail {

        template<typename Class>
        using call_operator_traits = member_function_traits<decltype(&Class::operator())>;

        // classes with operator()
        template<typename Class>
        struct functor_traits : function_traits<typename call_operator_traits<Class>::function_type> {
            typedef call_operator_traits<Class> call_operator;
        };

    } // namespace detail


    template<typename Class>
    struct functor_traits : detail::functor_traits<detail::remove_cvref_t<Class>> {
    };
} // ns mpl
#endif // CALLABLE_FUNCTOR_HPP_INCLUDED

