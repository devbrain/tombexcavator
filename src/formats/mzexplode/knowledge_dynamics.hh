#ifndef __EXPLODE_KNOWLEDGE_DYNAMICS_HH__
#define __EXPLODE_KNOWLEDGE_DYNAMICS_HH__

#include <cstdint>
#include <cstddef>

#include "formats/mzexplode/exe_file.hh"

namespace formats
{
    namespace explode
    {

        class FORMATS_API knowledge_dynamics
        {
        public:
            explicit knowledge_dynamics(input_exe_file& inp);

            static bool accept(input_exe_file& inp);

            void unpack(output_exe_file& oexe);
            uint32_t decomp_size() const;
        private:
            input& m_file;

            uint32_t m_expected_size;
            uint32_t m_code_offs;

            uint16_t m_header[exe_file::MAX_HEADER_VAL];
        };
    } // ns explode
}
#endif
