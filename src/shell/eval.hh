#ifndef SHELL_EVAL_HH
#define SHELL_EVAL_HH

#include <memory>
#include <bsw/singleton.hh>
namespace detail
{
    class evaluator
    {
    public:
        evaluator();
        ~evaluator();

        void eval(const std::string& txt);
    private:
        struct impl;
        std::unique_ptr<impl> m_pimpl;
    };
}

using evaluator = core::singleton<detail::evaluator>;

#endif
