//
// Created by igor on 17/05/2020.
//

#include "stargunner.hh"
#include "stargunner_fs.hh"


namespace stargunner
{
    file_ops::file_ops(std::vector<std::byte>& memory)
    : m_memory(std::move(memory)),
    m_input((const unsigned char*)m_memory.data(), m_memory.size())
    {

    }
    // -------------------------------------------------------------------------------------
    ssize_t file_ops::do_read  (void* buff, size_t len)
    {
        auto to_read = (len < m_input.bytes_remains()) ? len : m_input.bytes_remains();
        m_input.read_buff((char*)buff, len);
        return to_read;
    }
    // -------------------------------------------------------------------------------------
    bool file_ops::seek (uint64_t pos, enum whence_type whence)
    {
        try
        {
            switch (whence)
            {
                case whence_type::eVFS_SEEK_SET:
                    m_input.seek(pos);
                    break;
                case whence_type::eVFS_SEEK_END:
                    if (pos > m_memory.size())
                    {
                        return false;
                    }
                    m_input.seek(m_memory.size() - pos);
                    break;
                case whence_type::eVFS_SEEK_CUR:
                    m_input.seek(m_input.tell() + pos);
                    break;
            }
        }
        catch (...)
        {
            return false;
        }
        return true;
    }
    // -------------------------------------------------------------------------------------
    uint64_t file_ops::tell() const
    {
        return m_input.tell();
    }
    // =====================================================================================
    file_inode::file_inode(std::shared_ptr<dlt_archive> dlt, const fat_entry& fat)
    : archive::read_only_inode(vfs_inode_type::VFS_INODE_REGULAR),
    m_dlt(dlt),
    m_fat(fat)
    {

    }
    // ---------------------------------------------------------------------------------------
    archive::read_only_inode* file_inode::find_by_name(const char* name)
    {
        return nullptr;
    }
    // ---------------------------------------------------------------------------------------
    uint64_t file_inode::size()
    {
        return m_fat.out_size();
    }
    // ---------------------------------------------------------------------------------------
    archive::read_only_file* file_inode::open_read_only()
    {
        std::vector<std::byte> out;
        m_dlt->decompress(m_fat, out);
        return new file_ops(out);
    }
    // ---------------------------------------------------------------------------------------
    archive::dir_iterator* file_inode::directory_iterator()
    {
        return nullptr;
    }
    // ====================================================================================
    directory_inode::directory_inode(std::shared_ptr<dlt_archive> dlt, const std::string dir_name)
    : archive::read_only_inode(vfs_inode_type::VFS_INODE_DIRECTORY),
    m_dlt(dlt),
    m_fat(m_dlt->scan(dir_name)),
    m_dir_name (dir_name)
    {

    }
    // ---------------------------------------------------------------------------------------
    archive::read_only_inode* directory_inode::find_by_name(const char* name)
    {
        auto itr = m_fat.table.find(name);
        if (itr == m_fat.table.end())
        {
            return nullptr;
        }
        if (itr->second.is_directory)
        {
            std::string new_name = m_dir_name.empty() ? itr->first : m_dir_name + "\\" + itr->first;
            return new directory_inode(m_dlt, new_name);
        }
        return new file_inode(m_dlt, itr->second.info);
    }
    // ---------------------------------------------------------------------------------------
    uint64_t directory_inode::size()
    {
        return 0;
    }
    // ---------------------------------------------------------------------------------------
    archive::read_only_file* directory_inode::open_read_only()
    {
        return nullptr;
    }
    // ---------------------------------------------------------------------------------------
    archive::dir_iterator* directory_inode::directory_iterator()
    {
        return new archive::directory_iterator<fat_entry>(m_fat);
    }
    // ===================================================================================
    fs::fs()
    : archive::read_only_fs("stargunner"),
      m_dlt(nullptr)
    {

    }
    // ------------------------------------------------------------------------------------
    fs::~fs() = default;
    // ------------------------------------------------------------------------------------
    archive::read_only_inode* fs::root(const std::string& params)
    {
        m_dlt = std::make_shared<dlt_archive>(params);
        return new directory_inode(m_dlt, "");
    }
    // ------------------------------------------------------------------------------------
    size_t fs::max_name_length()
    {
        return 32;
    }
} // ns stargunner

REGISTER_VFS_MODULE(stargunner::fs)