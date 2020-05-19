#include "eval.hh"
#include "functions.hh"
#include <chaiscript/chaiscript.hpp>
namespace detail
{
    struct evaluator::impl
    {
        impl()
        {
            using namespace chaiscript;
            chai.add(fun(&list_modules::function, &m_list_modules), list_modules::name());
        }

        chaiscript::ChaiScript chai;
        list_modules m_list_modules;
    };
   // ---------------------------------------------------------------------------
    evaluator::evaluator()
    {
        m_pimpl = std::make_unique<impl>();
    }
    // ---------------------------------------------------------------------------
    evaluator::~evaluator() = default;
    // -----------------------------------------------------------------------
    void evaluator::eval(const std::string& txt)
    {
        m_pimpl->chai.eval(txt);
    }
}
