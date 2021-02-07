#ifndef PROVIDER_FAT_BASED_FS_HH
#define PROVIDER_FAT_BASED_FS_HH

#include "read_only_fs.hh"
#include <memory>
#include <map>
#include <vector>

/*
 * ARCHIVE TRAITS Requirements
 *  typedef archive_reader := reader for archives. should be constructable with params string
 *  constexpr size_t MAX_NAME_LENGTH := maximal length of the file name
 *  static constexpr const char * FS_TYPE := fs type
 *  typedef fat_entry := fat entry data
 *  static bool is_directory(const fat_entry& fe); := returns true if fat_entry is directory
 *  static uint64_t get_file_size(const fat_entry& fe) := returns file size
 *  static void get_fat_enties(archive_reader* archive,
 *                             const std::vector<std::string>& path,
 *                             std::map<std::string, fat_entry>& out) := populate fat entries by path
 *  struct file_ops := data structure for file ops
 *  {
 *    ssize_t do_read  (void* buff, size_t len);
 *    bool seek (uint64_t pos, enum whence_type whence);
 *    uint64_t tell() const;
 *  }
 *  static file_ops = create_file_ops(archive_reader* archive, const fat_entry&);

 */
namespace provider
{
    template <typename ARCHIVE_TRAITS>
    class fat_based_file_ops : public provider::read_only_file
    {
        using file_ops_t = typename ARCHIVE_TRAITS::file_ops;
    public:
        fat_based_file_ops(file_ops_t file_ops);
    private:
        ssize_t do_read  (void* buff, size_t len) override ;
        bool seek (uint64_t pos, enum whence_type whence) override;
        uint64_t tell() const override ;
    private:
        file_ops_t m_file_ops;
    };
    // --------------------------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    class fat_based_file_inode : public read_only_inode
    {
        using archive_reader_t = typename ARCHIVE_TRAITS::archive_reader;
        using fat_entry_t = typename ARCHIVE_TRAITS::fat_entry;
    public:
        fat_based_file_inode(std::shared_ptr<archive_reader_t> archive_reader, const fat_entry_t& fat);
    private:
        read_only_inode* find_by_name(const char* name) override ;
        uint64_t size() override ;
        read_only_file* open_read_only() override ;
        dir_iterator* directory_iterator() override ;
    private:
        std::shared_ptr<archive_reader_t> m_archive;
        fat_entry_t m_fat;
    };
    // --------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    class fat_based_directory_iterator : public dir_iterator
    {
        using fat_entry_t = typename ARCHIVE_TRAITS::fat_entry;
        using map_t = std::map<std::string, fat_entry_t>;
    public:
        explicit fat_based_directory_iterator(const map_t& fat);
        bool has_next() override;
        std::string next() override;
    private:
        const map_t& m_fat;
        typename map_t::const_iterator itr;
    };
    // --------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    class fat_based_directory_inode : public read_only_inode
    {
        using archive_reader_t = typename ARCHIVE_TRAITS::archive_reader;
        using fat_entry_t = typename ARCHIVE_TRAITS::fat_entry;
    public:
        fat_based_directory_inode(std::shared_ptr<archive_reader_t> archive_reader, std::vector<std::string> path);
    private:
        read_only_inode* find_by_name(const char* name) override ;
        uint64_t size() override ;
        read_only_file* open_read_only() override ;
        dir_iterator* directory_iterator() override ;
    private:
        std::shared_ptr<archive_reader_t> m_archive;
        std::map<std::string, fat_entry_t> m_fat;
        std::vector<std::string> m_dir_names;
    };
    // --------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    class fat_based_fs : public read_only_fs
    {
    private:
        using archive_reader_t = typename ARCHIVE_TRAITS::archive_reader;
    public:
        fat_based_fs();
        ~fat_based_fs() override ;
    private:
        read_only_inode* root(const std::string& params) override ;
        size_t max_name_length() override ;
    private:
        std::shared_ptr<archive_reader_t> m_archive;
    };
} // ns provider
#define REGISTER_FAT_VFS_MODULE(ARCHIVE_TRAITS) REGISTER_VFS_MODULE(::provider::fat_based_fs<ARCHIVE_TRAITS>)
// ===========================================================================================
// IMPLEMENTATION
// ===========================================================================================
namespace provider
{
    template <typename ARCHIVE_TRAITS>
    fat_based_fs<ARCHIVE_TRAITS>::fat_based_fs()
    : read_only_fs(ARCHIVE_TRAITS::FS_TYPE)
    {

    }
    // ---------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    fat_based_fs<ARCHIVE_TRAITS>::~fat_based_fs() = default;
    // ---------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    read_only_inode* fat_based_fs<ARCHIVE_TRAITS>::root(const std::string& params)
    {
        m_archive = std::make_shared<archive_reader_t>(params);
        std::vector<std::string> parents;
        return new fat_based_directory_inode<ARCHIVE_TRAITS>(m_archive, parents);
    }
    // ---------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    size_t fat_based_fs<ARCHIVE_TRAITS>::max_name_length()
    {
        return ARCHIVE_TRAITS::MAX_NAME_LENGTH;
    }
    // =======================================================================================
    template <typename ARCHIVE_TRAITS>
    fat_based_directory_inode<ARCHIVE_TRAITS>::fat_based_directory_inode(std::shared_ptr<archive_reader_t> archive_reader,
            std::vector<std::string> parents)
            : read_only_inode(vfs_inode_type::VFS_INODE_DIRECTORY),
            m_archive(archive_reader),
            m_dir_names(std::move(parents))
    {
        ARCHIVE_TRAITS::get_fat_entries(archive_reader.get(), m_dir_names, m_fat);
    }
    // ---------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    read_only_inode* fat_based_directory_inode<ARCHIVE_TRAITS>::find_by_name(const char* name)
    {
        auto itr = m_fat.find(name);
        if (itr == m_fat.end())
        {
            return nullptr;
        }
        if (ARCHIVE_TRAITS::is_directory(itr->second))
        {
            auto parents = m_dir_names;
            parents.push_back(name);
            return new fat_based_directory_inode<ARCHIVE_TRAITS>(m_archive, parents);
        }
        return new fat_based_file_inode<ARCHIVE_TRAITS>(m_archive, itr->second);
    }
    // ---------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    uint64_t fat_based_directory_inode<ARCHIVE_TRAITS>::size()
    {
        return 0;
    }
    // ---------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    read_only_file* fat_based_directory_inode<ARCHIVE_TRAITS>::open_read_only()
    {
        return nullptr;
    }
    // ---------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    dir_iterator* fat_based_directory_inode<ARCHIVE_TRAITS>::directory_iterator()
    {
        return new fat_based_directory_iterator<ARCHIVE_TRAITS>(m_fat);
    }
    // =======================================================================================
    template <typename ARCHIVE_TRAITS>
    fat_based_directory_iterator<ARCHIVE_TRAITS>::fat_based_directory_iterator(const map_t& fat)
    : m_fat(fat),
    itr (fat.begin())
    {

    }
    // ----------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    bool fat_based_directory_iterator<ARCHIVE_TRAITS>::has_next()
    {
        return itr != m_fat.end();
    }
    // ----------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    std::string fat_based_directory_iterator<ARCHIVE_TRAITS>::next()
    {
        const auto x = *itr;
        itr++;
        return x.first;
    }
    // =========================================================================================
    template <typename ARCHIVE_TRAITS>
    fat_based_file_inode<ARCHIVE_TRAITS>::fat_based_file_inode(std::shared_ptr<archive_reader_t> archive_reader, const fat_entry_t& fat)
    : read_only_inode(vfs_inode_type::VFS_INODE_REGULAR),
    m_archive(archive_reader),
    m_fat(fat)
    {

    }
    // -------------------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    read_only_inode* fat_based_file_inode<ARCHIVE_TRAITS>::find_by_name(const char* name)
    {
        return nullptr;
    }
    // -------------------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    uint64_t fat_based_file_inode<ARCHIVE_TRAITS>::size()
    {
        return ARCHIVE_TRAITS::get_file_size(m_fat);
    }
    // -------------------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    read_only_file* fat_based_file_inode<ARCHIVE_TRAITS>::open_read_only()
    {
        return new fat_based_file_ops<ARCHIVE_TRAITS>(ARCHIVE_TRAITS::create_file_ops(m_archive.get(), m_fat));
    }
    // -------------------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    dir_iterator* fat_based_file_inode<ARCHIVE_TRAITS>::directory_iterator()
    {
        return nullptr;
    }
    // ==================================================================================================
    template <typename ARCHIVE_TRAITS>
    fat_based_file_ops<ARCHIVE_TRAITS>::fat_based_file_ops(file_ops_t file_ops)
    : m_file_ops(std::move(file_ops))
    {
    }
    // -------------------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    ssize_t fat_based_file_ops<ARCHIVE_TRAITS>::do_read (void* buff, size_t len)
    {
        return m_file_ops.do_read(buff, len);
    }
    // -------------------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    bool fat_based_file_ops<ARCHIVE_TRAITS>::seek (uint64_t pos, enum whence_type whence)
    {
        return m_file_ops.seek(pos, whence);
    }
    // -------------------------------------------------------------------------------------------------
    template <typename ARCHIVE_TRAITS>
    uint64_t fat_based_file_ops<ARCHIVE_TRAITS>::tell() const
    {
        return m_file_ops.tell();
    }
} // ns provider


#endif
