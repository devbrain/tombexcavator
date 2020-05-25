#ifndef MYSTIC_TOWERS_FS_HH
#define MYSTIC_TOWERS_FS_HH

#include <archive/archive.hh>
#include "mystic_towers.hh"
namespace mystic
{
    class file_ops : public archive::read_only_file
    {
    public:
        file_ops(std::vector<std::byte>& memory);
    private:
        ssize_t do_read  (void* buff, size_t len) override ;
        bool seek (uint64_t pos, enum whence_type whence) override;
        uint64_t tell() const override ;
    private:
        std::vector<std::byte> m_memory;
    };
    // ---------------------------------------------------------------------------------
    class file_inode : public archive::read_only_inode
    {
    public:
        explicit file_inode(std::shared_ptr<dat_archive> dlt, const fat_entry& fat);
    private:
        archive::read_only_inode* find_by_name(const char* name) override ;
        uint64_t size() override ;
        archive::read_only_file* open_read_only() override ;
        archive::dir_iterator* directory_iterator() override ;
    private:
        std::shared_ptr<dat_archive> m_dlt;
        fat_entry m_fat;
    };
    // ---------------------------------------------------------------------------------
    class directory_inode : public archive::read_only_inode
    {
    public:
        explicit directory_inode(std::shared_ptr<dat_archive> dlt);
    private:
        archive::read_only_inode* find_by_name(const char* name) override ;
        uint64_t size() override ;
        archive::read_only_file* open_read_only() override ;
        archive::dir_iterator* directory_iterator() override ;
    private:
        std::shared_ptr<dat_archive> m_dlt;
        archive::file_allocation_table<fat_entry> m_fat;
    };
    // --------------------------------------------------------------------------------
    class fs : public archive::read_only_fs
    {
    public:
        fs();
        ~fs();
    private:
        archive::read_only_inode* root(const std::string& params) override;
        size_t max_name_length() override;
    private:
        std::shared_ptr<dat_archive> m_dat;
    };
} // ns mystic
#endif
