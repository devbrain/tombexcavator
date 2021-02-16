//
// Created by igor on 14/02/2021.
//

#include <stdexcept>

#include "../../../include/formats/explode/mz/explode_exe_file.hh"

#include "formats/explode/mz/knowledge_dynamics.hh"
#include "formats/explode/mz/unlzexe.hh"
#include "formats/explode/mz/unpklite.hh"
#include "formats/explode/mz/unexepack.hh"


template<typename DECODER>
static void decode(formats::explode::mz::input_exe_file& iexe, std::vector<char>& out_buff)
{
    DECODER decoder(iexe);
    formats::explode::mz::full_exe_file fo(decoder.decomp_size());
    decoder.unpack(fo);
    formats::explode::mz::io::inmem_output ow(out_buff);
    fo.write(ow);
}

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
        catch (std::runtime_error& e)
        {
            return false;
        }
        return true;
    }
}
