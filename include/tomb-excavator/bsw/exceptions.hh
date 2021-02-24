#ifndef __EXPLODE_EXCEPTIONS_HH__
#define __EXPLODE_EXCEPTIONS_HH__

#include <stdexcept>
#include <sstream>
#include <tomb-excavator/bsw_export.h>

namespace bsw
{
    class BSW_API exception : public std::runtime_error
    {
    public:
        template<typename ... Args>
        exception(const char* function, const char* source, int line, Args&& ... args);
    private:
        template<typename ... Args>
        static std::string _create(const char* function, const char* source, int line, Args&& ... args);

    };
} // ns bsw
// ==========================================================================
// Implementation
// ==========================================================================
namespace bsw
{

    template<typename ... Args>
    inline
    exception::exception(const char* function, const char* source, int line, Args&& ... args)
            : std::runtime_error(_create(function, source, line, std::forward<Args>(args)...))
    {

    }
    // --------------------------------------------------------------------------------------
    template<typename ... Args>
    inline
    std::string exception::_create(const char* function, const char* source, int line, Args&& ... args)
    {
        std::ostringstream os;

        os << "Error at " << function << " " << source << "@" << line;
        if constexpr (sizeof...(args) > 0)
        {
            os << " :";
            ((os << ' ' << std::forward<Args>(args)), ...);
        }
        return os.str();
    }
}

#if defined(_MSC_VER)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define RAISE_EX(...) throw ::bsw::exception(__PRETTY_FUNCTION__, __FILE__, __LINE__,  ##__VA_ARGS__)


#endif
