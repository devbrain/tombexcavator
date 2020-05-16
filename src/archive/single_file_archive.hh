#ifndef ARCHIVE_SINGLE_FILE_ARCHIVE_HH__
#define ARCHIVE_SINGLE_FILE_ARCHIVE_HH__

#include <archive/archive.hh>
#include <map>
#include <string>

namespace archive
{
    // ---------------------------------------------------------------------------
    template <typename FileEntry>
    class file : public read_only_file
    {
    protected:
        virtual ssize_t do_read  (void* buff, size_t len) = 0;
    };
    // ---------------------------------------------------------------------------
    template <typename FileEntry>
    struct file_allocation_table
    {
        struct record
        {
            record (const FileEntry& fe, bool is_dir)
            : info (fe), is_directory(is_dir)
            {

            }
            FileEntry info;
            bool is_directory;
        };

        void put(const std::string& name, const FileEntry& fe, bool is_dir)
        {
            table.insert(std::make_pair(name, record(fe, is_dir)));
        }
        using map_t = std::map<std::string, record>;
        map_t table;
    };

    template <typename FileEntry>
    class directory_iterator : public dir_iterator
    {
    public:
        explicit directory_iterator(const file_allocation_table<FileEntry>& fat)
        : m_fat(fat),
        itr (fat.table.begin())
        {

        }

        bool has_next() override
        {
            return itr != m_fat.table.end();
        }

        std::string next() override
        {
            const auto x = *itr;
            itr++;
            return x.first;
        }
    private:
        const file_allocation_table<FileEntry>& m_fat;
        using map_t = typename file_allocation_table<FileEntry>::map_t ;
        typename map_t::const_iterator itr;
    };

    template <typename FileEntry>
    class single_file_archive : public read_only_inode
    {
    public:
        explicit single_file_archive(const file_allocation_table<FileEntry>& fat)
        : m_fat (fat) {}
    private:
        dir_iterator* directory_iterator() override
        {
            return new archive::directory_iterator<FileEntry>(m_fat);
        }
        read_only_file* open_read_only() override
        {
            return nullptr;
        }
    private:
        file_allocation_table<FileEntry> m_fat;
    };
}

#endif
