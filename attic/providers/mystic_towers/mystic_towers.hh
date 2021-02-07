//
// Created by igor on 25/05/2020.
//

#ifndef TOMBEXCAVATOR_MYSTIC_TOWERS_HH
#define TOMBEXCAVATOR_MYSTIC_TOWERS_HH

#include <string>
#include <map>
#include <mio/mio.hpp>

namespace mystic
{
    struct fat_entry
    {
        fat_entry (std::uint32_t offset_, std::uint32_t size_);
        std::uint32_t offset;
        std::uint32_t size;
    };

    class dat_archive
    {
    public:
        dat_archive (const std::string& path);
        const std::map<std::string, fat_entry>& fat () const;
        std::vector<std::byte> read(const fat_entry& fe) const;
    private:
        mio::mmap_source m_source;
        std::map<std::string, fat_entry> m_fat;
    };
}



#endif //TOMBEXCAVATOR_MYSTIC_TOWERS_HH
