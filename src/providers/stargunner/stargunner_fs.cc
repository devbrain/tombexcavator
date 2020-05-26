//
// Created by igor on 17/05/2020.
//

#include "stargunner.hh"
#include <sstream>
#include <providers/fat_based_fs.hh>
#include <providers/in_mem_file_ops.hh>

namespace stargunner
{
    struct fs_traits
    {
        using archive_reader = dlt_archive;
        static constexpr size_t MAX_NAME_LENGTH = 32;
        static constexpr const char * FS_TYPE = "stargunner";
        using fat_entry = stargunner::fat_entry;

        static bool is_directory(const fat_entry& fe);
        static uint64_t get_file_size(const fat_entry& fe);
        static void get_fat_entries(archive_reader* archive,
                                     const std::vector<std::string>& path,
                                     std::map<std::string, fat_entry>& out);

        using file_ops = provider::in_mem_file_ops;
        static file_ops create_file_ops(archive_reader* archive, const fat_entry& fat);

    };
    // ================================================================================
    // Implementation
    // ================================================================================
    bool fs_traits::is_directory(const fat_entry& fe)
    {
        return fe.is_directory();
    }
    // --------------------------------------------------------------------------------
    uint64_t fs_traits::get_file_size(const fat_entry& fe)
    {
        return fe.out_size();
    }
    // --------------------------------------------------------------------------------
    void fs_traits::get_fat_entries(archive_reader* archive,
                                const std::vector<std::string>& path,
                                std::map<std::string, fat_entry>& out)
    {
        std::ostringstream os;
        for (std::size_t i = 0; i<path.size(); i++)
        {
            os << path[i];
            if (i > 0 && i + 1 < path.size())
            {
                os << "\\";
            }
        }
        const std::string name = os.str();
        out = archive->scan(name);
    }
    // --------------------------------------------------------------------------------
    fs_traits::file_ops fs_traits::create_file_ops(archive_reader* archive, const fat_entry& fat)
    {
        std::vector<std::byte> out;
        archive->decompress(fat, out);
        return file_ops(out);
    }
}

REGISTER_FAT_VFS_MODULE(stargunner::fs_traits)

