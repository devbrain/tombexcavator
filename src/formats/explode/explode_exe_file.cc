//
// Created by igor on 14/02/2021.
//

#include <stdexcept>

#include "tomb-excavator/formats/explode/mz/explode_exe_file.hh"

#include "formats/explode/mz/knowledge_dynamics.hh"
#include "formats/explode/mz/unlzexe.hh"
#include "formats/explode/mz/unpklite.hh"
#include "formats/explode/mz/unexepack.hh"

namespace
{
    template<typename DECODER>
    void decode(formats::explode::mz::input_exe_file& iexe, std::vector<char>& out_buff)
    {
        DECODER decoder(iexe);
        formats::explode::mz::full_exe_file fo(decoder.decomp_size());
        decoder.unpack(fo);
        formats::explode::mz::io::inmem_output ow(out_buff);
        fo.write(ow);
    }
} // anon. ns
// ========================================================================================================
namespace formats::explode::mz
{
    bool explode_exe_file(const char* input, std::size_t input_size, std::vector<char>& output)
    {
        try
        {
            io::inmem_input inp((unsigned char*) (input), input_size);

            input_exe_file iexe(inp);
            if (unlzexe::accept(iexe))
            {
                decode<unlzexe>(iexe, output);
            } else
            {
                if (unpklite::accept(iexe))
                {
                    decode<unpklite>(iexe, output);
                } else
                {
                    if (iexe.is_exepack())
                    {
                        decode<unexepack>(iexe, output);
                    } else
                    {
                        if (knowledge_dynamics::accept(iexe))
                        {
                            decode<knowledge_dynamics>(iexe, output);
                        } else
                        {
                            return false;
                        }
                    }
                }
            }
        }
        catch (std::runtime_error& )
        {
            return false;
        }
        return true;
    }
    // ===========================================================================================
    std::unique_ptr<bsw::io::binary_reader> explode_exe_file(std::istream& istream)
    {
        auto current = istream.tellg();
        istream.seekg(0, std::ios::end);
        auto fsize = istream.tellg() - current;
        istream.seekg(current, std::ios::beg);

        auto input = std::make_shared<std::vector<char>>(fsize);
        istream.read(input->data(), fsize);

        constexpr auto order = bsw::io::binary_reader::LITTLE_ENDIAN_BYTE_ORDER;

        if (auto output = std::make_shared<std::vector<char>>();
            formats::explode::mz::explode_exe_file(input->data(), input->size(), *output))
        {
            return std::make_unique<bsw::io::memory_binary_reader>(output, order);
        }
        return std::make_unique<bsw::io::memory_binary_reader>(input, order);
    }
}
