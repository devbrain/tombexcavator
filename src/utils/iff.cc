//
// Created by igor on 04/04/2021.
//

#include <iostream>
#include <fstream>
#include <tomb-excavator/formats/iff/ea/ea.hh>
#include <tomb-excavator/formats/iff/iff_parser.hh>

namespace formats::iff
{
    class tester : public iff_events
    {
    public:
        tester()
                : m_level(0)
        {}
    private:
        bool on_start_group(std::istream& is, chunk_type type, uint64_t offset, std::size_t size) override
        {
            for (int i = 0; i < m_level; i++)
            {
                std::cout << " ";
            }
            uint32_t dword;
            is.read((char*)&dword, sizeof(dword));
            chunk_type tag (bsw::byte_order::from_big_endian(dword));

            std::cout << ">" << type << ";" << tag << " (" << offset << "," << size << ")" << std::endl;
            m_level++;
            return true;
        }

        void on_chunk([[maybe_unused]] std::istream& is, chunk_type type, uint64_t offset, std::size_t size) override
        {
            for (int i = 0; i < m_level; i++)
            {
                std::cout << " ";
            }
            std::cout << "  " << type.to_string() << " (" << offset << "," << size << ")" << std::endl;
        }

        void on_end_group(chunk_type type) override
        {
            for (int i = 0; i < m_level; i++)
            {
                std::cout << " ";
            }
            std::cout << "<" << type << std::endl;
            m_level--;
        }

        int m_level;
    };
}

int main(int argc, char* argv[])
{
    std::ifstream ifs("/home/igor/proj/ares/libiff/samples/TP_SEX.LBM", std::ios::in | std::ios::binary);
    formats::iff::tester ev;
    formats::iff::iff_parser<formats::iff::ea_iff>(ifs, &ev);
}