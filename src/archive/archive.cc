#include <stdexcept>
#include <archive/archive.hh>

namespace archive
{
    read_only_file::read_only_file()
    : m_pos(0)
    {

    }
    // -----------------------------------------------------------------------------
    ssize_t read_only_file::read  (void* buff, size_t len)
    {
        auto rc = do_read(buff, len);
        if (rc >= 0)
        {
            m_pos = m_pos + rc;
        }
        return rc;
    }
    // -----------------------------------------------------------------------------
    ssize_t read_only_file::write(void* , size_t )
    {
        throw std::runtime_error("Not implemented");
    }
    // -----------------------------------------------------------------------------
    bool read_only_file::seek(uint64_t, whence_type)
    {
        return false;
    }
    // -----------------------------------------------------------------------------
    uint64_t read_only_file::tell() const
    {
        return m_pos;
    }
    // -----------------------------------------------------------------------------
    bool read_only_file::truncate()
    {
        throw std::runtime_error("Not implemented");
    }
    // =============================================================================
    vfs::module::inode* read_only_inode::lookup(const char* name)
    {
        return find_by_name(name);
    }
    // -----------------------------------------------------------------------------
    vfs::module::directory_iterator* read_only_inode::get_directory_iterator()
    {
        return directory_iterator();
    }
    // -----------------------------------------------------------------------------
    bool read_only_inode::mkdir(const char* )
    {
        throw std::runtime_error("Not implemented");
    }
    // -----------------------------------------------------------------------------
    bool read_only_inode::mkfile(const char* )
    {
        throw std::runtime_error("Not implemented");
    }
    // -----------------------------------------------------------------------------
    int read_only_inode::unlink()
    {
        throw std::runtime_error("Not implemented");
    }
    // -----------------------------------------------------------------------------
    vfs::module::file* read_only_inode::open_file(open_mode_type mode_type)
    {
        if (mode_type == eVFS_OPEN_MODE_WRITE)
        {
            throw std::runtime_error("Not implemented");
        }
        return open_read_only();
    }
    // -----------------------------------------------------------------------------
    void read_only_inode::load_stat(stats&)
    {

    }
    // ==============================================================================
    vfs::module::inode* read_only_fs::load_root(const std::string& params)
    {
        return root(params);
    }
    // --------------------------------------------------------------------------
    int read_only_fs::sync()
    {
        return 1;
    }
    // --------------------------------------------------------------------------
    int read_only_fs::sync_inode(vfs::module::inode*)
    {
        return 1;
    }
    // --------------------------------------------------------------------------
    bool read_only_fs::is_readonly() const
    {
        return true;
    }
} // ns archive