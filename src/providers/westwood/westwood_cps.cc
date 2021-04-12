//
// Created by igor on 12/04/2021.
//

#include <tomb-excavator/games/westwood/westwood_cps.hh>
#include <tomb-excavator/bsw/io/binary_reader.hh>
#include <tomb-excavator/bsw/exceptions.hh>

namespace games::westwood
{

    struct cps_header
    {
        uint16_t file_size;
        uint16_t compression_type;
        uint32_t uncompressed_size;
        uint16_t palette_size;
    };


    provider::dto::picture load_cps(std::istream &is, std::size_t entry_size)
    {
        if (entry_size < sizeof(cps_header))
        {
            RAISE_EX("CPS is corrupted");
        }
        bsw::io::binary_reader rd(is, bsw::io::binary_reader::LITTLE_ENDIAN_BYTE_ORDER);
        auto current = is.tellg();
        cps_header header{10,10,10,10};
        rd >> header.file_size >> header.compression_type >> header.uncompressed_size >> header.palette_size;

        if (header.palette_size > 0)
        {
            // TODO
        }

    }
}