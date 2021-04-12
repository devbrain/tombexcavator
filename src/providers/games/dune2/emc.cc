//
// Created by igor on 11/04/2021.
//

#include <iostream>
#include <sstream>
#include <cstdint>
#include <algorithm>

#include "emc.hh"
#include "chunked_file.hh"
#include <tomb-excavator/formats/iff/ea/ea_events.hh>
#include <tomb-excavator/formats/iff/ea/ea.hh>

static const char* scriptOpcodes[0x14] = {
        "Goto",
        "SetReturn",
        "PushOp",
        "PushWord",
        "PushByte",
        "PushVar",
        "PushLocalVar",
        "PushParameter",
        "Pop",
        "PopReg",
        "PopLocalVar",
        "PopParameter",
        "AddSP",
        "SubSP",
        "Execute",
        "IfNotGoto",
        "Negate",
        "Evaluate",
        "Return"
};


static const char* scriptOpcodesEvaluate[0x12] = {
        "IfAnd",
        "IfOr",
        "Equal",
        "NotEqual",
        "CompareLess",
        "CompareLessEqual",
        "CompareRight",
        "CompareRightEqual",
        "Add",
        "Subtract",
        "Multiply",
        "Divide",
        "ShiftRight",
        "ShiftLeft",
        "And",
        "Or",
        "DivideRemainder",
        "XOR"
};

struct emc_script
{
    explicit emc_script(emc::type_t t)
            : type(t)
    {

    }

    void decompile(std::ostream& os)
    {
        std::size_t opcode_idx = 0;
        std::size_t opcode_pos = 0;
        uint16_t opcode;
        uint16_t word;
        while (opcode_idx < data.size())
        {
            auto current_pos = opcode_pos;
            word = data[opcode_idx++];
            opcode_pos++;
            opcode = word >> 8u;
            opcode &= 0x1Fu;

            if (word & 0x8000u)
            {
                // Opcode uses 13 bits
                opcode = 0;
                word &= 0x7FFFu;
            } else
            {
                // Opcode only requires 1 byte
                if (word & 0x4000u)
                {
                    word &= 0xFFu;
                } else
                {
                    // Opcode uses the next WORD, grab it
                    if (word & 0x2000u)
                    {
                        word = data[opcode_idx++];
                        opcode_pos++;
                    }
                }
            }
            print(os, current_pos, opcode, word);
        }
    }

    void print(std::ostream& os, std::size_t current_pos, uint16_t opcode, uint16_t arg) const
    {
        const char* mnemonics = scriptOpcodes[opcode];
        os << current_pos << "\t" << "(" << opcode << ")" << mnemonics << "    ";
        os << transform_arg(opcode, arg) << std::endl;
    }

    std::string transform_arg(uint16_t opcode, uint16_t arg) const
    {
        static constexpr uint16_t EVALUATE = 17;
        static constexpr uint16_t IF_NOT_GOTO = 15;
        std::ostringstream os;
        switch (opcode)
        {
            case EVALUATE:
                os << scriptOpcodesEvaluate[arg];
                break;
            case IF_NOT_GOTO:
                os << script_label( arg & 0x7FFFu );
                break;
            default:
                os << arg;
        }
        return os.str();
    }

    int script_label(uint16_t arg) const
    {
        auto itr = std::find(offsets.begin(), offsets.end(), arg);
        if (itr == offsets.end())
        {
            return arg;
        }
        return itr - offsets.begin();
    }

    emc::type_t type;
    std::vector<uint16_t> offsets;
    std::vector<uint16_t> data;
};

// =============================================================================================================
games::common::archive_entry_loader::name_acceptor_t emc::accept_team()
{
    return games::westwood::westwood_entry_loader::by_name("team.emc");
}
// -----------------------------------------------------------------------------------------------------------
games::common::archive_entry_loader::name_acceptor_t emc::accept_build()
{
    return games::westwood::westwood_entry_loader::by_name("build.emc");
}
// -----------------------------------------------------------------------------------------------------------
games::common::archive_entry_loader::name_acceptor_t emc::accept_unit()
{
    return games::westwood::westwood_entry_loader::by_name("unit.emc");
}
// -----------------------------------------------------------------------------------------------------------
std::string emc::load_team(std::istream& is,
                           uint64_t offs, std::size_t size,
                           [[maybe_unused]] const games::common::fat_entry::props_map_t& props)
{
    return load(is, offs, size, eTEAM);
}
// -------------------------------------------------------------------------------------------------------------------
std::string emc::load_build(std::istream& is,
                            uint64_t offs, std::size_t size,
                            [[maybe_unused]] const games::common::fat_entry::props_map_t& props)
{
    return load(is, offs, size, eBUILD);
}
// -------------------------------------------------------------------------------------------------------------------
std::string emc::load_unit(std::istream& is,
                           uint64_t offs, std::size_t size,
                           [[maybe_unused]] const games::common::fat_entry::props_map_t& props)
{
    return load(is, offs, size, eUNIT);
}
// -------------------------------------------------------------------------------------------------------------------
std::string emc::load(std::istream& is,
                      uint64_t offs, std::size_t size,
                      type_t t)
{
    constexpr static auto ORDR = formats::iff::fourcc("ORDR");
    constexpr static auto DATA = formats::iff::fourcc("DATA");
    is.seekg(offs, std::ios::beg);
    chunked_file file(is, size);
    emc_script script(t);

    if (auto order_inf = file.find_section(ORDR); order_inf)
    {
        auto[offset, size] = *order_inf;
        if (size % 2 == 1)
        {
            RAISE_EX("Offsets size should be even");
        }
        size = size / 2;
        bsw::io::binary_reader rdr(is, bsw::io::binary_reader::BIG_ENDIAN_BYTE_ORDER);
        rdr.stream().seekg(offset, std::ios::beg);
        script.offsets.reserve(size);
        for (std::size_t i = 0; i < size; i++)
        {
            uint16_t ofs;
            rdr >> ofs;
            script.offsets.push_back(ofs);
        }
    } else
    {
        RAISE_EX("Can not find offsets");
    }

    if (auto data_inf = file.find_section(DATA); data_inf)
    {
        auto[offset, size] = *data_inf;
        if (size % 2 == 1)
        {
            RAISE_EX("Opcodes size should be even");
        }
        size = size / 2;
        bsw::io::binary_reader rdr(is, bsw::io::binary_reader::BIG_ENDIAN_BYTE_ORDER);
        rdr.stream().seekg(offset, std::ios::beg);

        for (std::size_t i = 0; i < size; i++)
        {
            uint16_t opcode;
            rdr >> opcode;
            script.data.push_back(opcode);
        }
    } else
    {
        RAISE_EX("Can not find data");
    }
    std::ostringstream  os;
    script.decompile(os);
    return os.str();
}
