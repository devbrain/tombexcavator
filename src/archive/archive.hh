#ifndef ARCHIVE_HH__
#define ARCHIVE_HH__

#include <vfs/api/vfs_module.h>
#include <archive/archive_api.h>

#include <bsw/warn/push.hh>
#include <bsw/warn/dll_interface_base>

namespace archive
{
    class ARCHIVE_API read_only_file : public vfs::module::file
    {
    public:
        read_only_file();
    protected:
        virtual ssize_t do_read  (void* buff, size_t len) = 0;
        bool seek (uint64_t pos, enum whence_type whence) override;
    private:
        ssize_t read  (void* buff, size_t len) override ;
        ssize_t write (void* buff, size_t len) override ;
        uint64_t tell() const override ;
        bool truncate() override;
    };
    // --------------------------------------------------------------------
    class ARCHIVE_API dir_iterator : public vfs::module::directory_iterator
    {
    protected:
        virtual bool has_next() = 0;
        virtual std::string next() = 0;
    };
    // --------------------------------------------------------------------
    class ARCHIVE_API read_only_inode : public vfs::module::inode
    {
    public:
        explicit read_only_inode(vfs_inode_type type);
    protected:
        void load_stat(stats& output) override ;
        virtual read_only_inode* find_by_name(const char* name) = 0;
        virtual uint64_t size() = 0;
        virtual read_only_file* open_read_only() = 0;
        virtual dir_iterator* directory_iterator() = 0;
    private:
        vfs::module::inode* lookup(const char* name) override;
        vfs::module::directory_iterator* get_directory_iterator() override;
        bool mkdir(const char* name) override ;
        bool mkfile(const char* name) override ;
        int unlink() override ;
        vfs::module::file* open_file(open_mode_type mode_type) override ;
    };
    // --------------------------------------------------------------------
    class ARCHIVE_API read_only_fs : public vfs::module::filesystem
    {
    public:
        read_only_fs(const std::string fstype);
        virtual read_only_inode* root(const std::string& params) = 0;
        virtual size_t max_name_length() = 0;
    private:
        vfs::module::inode* load_root(const std::string& params) override ;
        int sync() override ;
        int sync_inode(vfs::module::inode* inod) override ;
        bool is_readonly() const override ;
    };
} // ns archive
#include <bsw/warn/pop.hh>
#endif

