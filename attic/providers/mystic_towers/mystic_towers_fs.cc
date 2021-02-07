#include "mystic_towers.hh"
#include "../in_mem_file_ops.hh"
#include "../fat_based_fs.hh"

namespace mystic
{
    struct fs_traits
    {
        using archive_reader = dat_archive;
        static constexpr size_t MAX_NAME_LENGTH = 12;
        static constexpr const char * FS_TYPE = "mystic_towers";
        using fat_entry = mystic::fat_entry;

        static bool is_directory(const fat_entry& fe);
        static uint64_t get_file_size(const fat_entry& fe);
        static void get_fat_entries(archive_reader* archive,
                                    const std::vector<std::string>& path,
                                    std::map<std::string, fat_entry>& out);

        using file_ops = provider::in_mem_file_ops;
        static file_ops create_file_ops(archive_reader* archive, const fat_entry& fat);
    };
    // implementation
    bool fs_traits::is_directory(const fat_entry&)
    {
        return false;
    }
    // --------------------------------------------------------------------------------------
    uint64_t fs_traits::get_file_size(const fat_entry& fe)
    {
        return fe.size;
    }
    // --------------------------------------------------------------------------------------
    void fs_traits::get_fat_entries(archive_reader* archive,
                                    const std::vector<std::string>&,
                                    std::map<std::string, fat_entry>& out)
    {
        out = archive->fat();
    }
    // --------------------------------------------------------------------------------------
    fs_traits::file_ops fs_traits::create_file_ops(archive_reader* archive, const fat_entry& fat)
    {
        return file_ops(archive->read(fat));
    }

} // ns mystic

REGISTER_FAT_VFS_MODULE(mystic::fs_traits)
