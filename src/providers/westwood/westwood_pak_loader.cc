//
// Created by igor on 25/03/2021.
//

#include <utility>
#include <tomb-excavator/games/westwood/westwood_pak_loader.hh>
#include <tomb-excavator/games/westwood/westwood_entry_loader.hh>
#include <tomb-excavator/bsw/io/binary_reader.hh>

namespace games::westwood
{
    pak_loader::pak_loader(std::string phys_name, std::initializer_list<common::archive_entry_loader> loaders)
            : common::archive_loader(std::move(phys_name), std::make_unique<westwood_entry_loader>()),
            m_is_first(true),
            m_is_last(false)
    {
        for (const auto& ldr : loaders)
        {
            this->loaders().add(ldr);
        }
    }
    // ---------------------------------------------------------------------------------------
    std::optional<pak_loader::fat_event_t> pak_loader::read_next_entry(std::istream& is)
    {
        if (m_is_last)
        {
            return std::nullopt;
        }
        bsw::io::binary_reader reader(is, bsw::io::binary_reader::LITTLE_ENDIAN_BYTE_ORDER);

        uint32_t offs;
        reader >> offs;
        char ch;
        if (offs == 0 || static_cast<uint32_t>(m_current_offs) + offs > m_file_size)
        {
            m_is_last = true;
            if (!m_is_first)
            {
                m_prev_event.size = m_file_size - m_prev_event.offset;
                return m_prev_event;
            }
            else
            {
                return std::nullopt;
            }
        } else
        {
            std::string name;
            while (true)
            {
                reader >> ch;
                if (ch != 0)
                {
                    name += ch;
                } else
                {
                    break;
                }
            }

            if (m_is_first)
            {
                m_prev_event.offset = offs;
                m_prev_event.name = name;
                m_is_first = false;
                return std::nullopt; // for first read
            }
            else
            {
                fat_file_event to_fire;
                to_fire.name = m_prev_event.name;
                to_fire.offset = m_prev_event.offset;
                to_fire.size = offs - m_prev_event.offset;

                m_prev_event.name = name;
                m_prev_event.offset = offs;
                return to_fire;
            }
        }
    }
    // ---------------------------------------------------------------------------------------
    void pak_loader::read_header(std::istream& is)
    {
        m_current_offs = is.tellg();
        is.seekg(0, std::ios::end);
        m_file_size = is.tellg();
        is.seekg(m_current_offs, std::ios::beg);
        read_next_entry(is);
    }
}
