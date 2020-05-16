//
// Created by igor on 16/05/2020.
//

#ifndef TOMBEXCAVATOR_STARGUNNER_HH
#define TOMBEXCAVATOR_STARGUNNER_HH

#include <vector>
#include <string>
#include <archive/archive.hh>
#include <archive/single_file_archive.hh>
#include <mio/mio.hpp>

namespace stargunner
{
    class fat_entry
    {
    public:
        fat_entry(std::size_t size, std::size_t out_size, const std::byte* data) noexcept;
        fat_entry() noexcept;
        fat_entry(const fat_entry&) = default;
        bool is_compressed() const noexcept ;
        std::size_t size() const noexcept ;
        std::size_t out_size() const noexcept ;
        const std::byte* stream () const noexcept ;
    private:
        std::size_t m_size;
        std::size_t m_out_size;
        const std::byte* m_stream;
    };

    class dlt_archive
    {
    public:
        dlt_archive (const std::string& path);
        archive::file_allocation_table<fat_entry> scan(const std::string& directory) const;
        static void decompress(const fat_entry& fe, std::vector<std::byte>& out);
        fat_entry get_entry(const std::string& name);
    private:
        mio::mmap_source m_source;
        std::map<std::string, fat_entry> m_fat;
    };
};


#endif //TOMBEXCAVATOR_STARGUNNER_HH
