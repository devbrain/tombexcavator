//
// Created by igor on 22/03/2021.
//

#include <tomb-excavator/games/common/loaders/archive_loader.hh>
#include <tomb-excavator/bsw/exceptions.hh>

namespace games::common
{
    archive_data_loader::fat_entry::fat_entry(const std::string& name, file_info fileinfo)
    : m_name(name),
    m_info(fileinfo)
    {

    }
    // ------------------------------------------------------------------------------------------------------------
    archive_data_loader::fat_entry::fat_entry(const std::string& name, dir_info dirinfo)
            : m_name(name),
              m_info(dirinfo)
    {

    }
    // ------------------------------------------------------------------------------------------------------------
    archive_data_loader::fat_entry::fat_entry(const std::string& name, file_info fileinfo, props_map_t props)
            : m_name(name),
              m_info(fileinfo),
              m_props(std::move(props))
    {

    }
    // ------------------------------------------------------------------------------------------------------------
    archive_data_loader::fat_entry::fat_entry(const std::string& name, dir_info dirinfo, props_map_t props)
            : m_name(name),
              m_info(dirinfo),
              m_props(std::move(props))
    {

    }
    // ------------------------------------------------------------------------------------------------------------
    std::string archive_data_loader::fat_entry::name() const noexcept
    {
        return m_name;
    }
    // ------------------------------------------------------------------------------------------------------------
    bool archive_data_loader::fat_entry::is_directory() const noexcept
    {
        return std::get_if<dir_info>(&m_info) != nullptr;
    }
    // ------------------------------------------------------------------------------------------------------------
    archive_data_loader::fat_entry::dir_info archive_data_loader::fat_entry::get_dir_info() const
    {
        return std::get<dir_info>(m_info);
    }
    // ------------------------------------------------------------------------------------------------------------
    archive_data_loader::fat_entry::file_info archive_data_loader::fat_entry::get_file_info() const
    {
        return std::get<file_info>(m_info);
    }
    // ------------------------------------------------------------------------------------------------------------
    const archive_data_loader::fat_entry::props_map_t& archive_data_loader::fat_entry::get_props() const
    {
        return m_props;
    }
    // ============================================================================================================
    namespace detail
    {
        class archive_directory : public abstract_directory
        {
        public:
            archive_directory(const archive_data_loader::loaders_map_t& loaders,
                              std::shared_ptr<std::istream> istream,
                              const std::vector<archive_data_loader::fat_entry>& fat,
                              std::size_t first_index, std::size_t length)
                              : m_loaders(loaders),
                                m_istream(istream),
                                m_fat (fat),
                                m_first_index(first_index),
                                m_length(length)
            {

            }
        private:
            [[nodiscard]] virtual std::size_t size() const override
            {
                return m_length;
            }
            [[nodiscard]] virtual provider::file_content_t load(std::size_t index) const override
            {
                auto fi = m_fat[m_first_index + index].get_file_info();
                auto itr = m_loaders.find(fi.internal_type);
                return itr->second(*m_istream, fi, m_fat[m_first_index + index].get_props());
            }

            [[nodiscard]] virtual provider::file_type_t type(std::size_t index) const override
            {
                auto fi = m_fat[m_first_index + index].get_file_info();
                return fi.type;
            }

            [[nodiscard]] virtual std::string name(std::size_t index) const override
            {
                return m_fat[m_first_index + index].name();
            }
            [[nodiscard]] virtual bool is_directory(std::size_t index) const override
            {
                return m_fat[m_first_index + index].is_directory();
            }
            [[nodiscard]] virtual std::shared_ptr<abstract_directory> load_directory(std::size_t index) const override
            {
                auto di = m_fat[m_first_index+index].get_dir_info();
                return std::make_shared<archive_directory>(m_loaders, m_istream, m_fat, di.first_fat_index, di.entries_count);
            }
        private:
            const archive_data_loader::loaders_map_t&  m_loaders;
            std::shared_ptr<std::istream> m_istream;
            const std::vector<archive_data_loader::fat_entry>& m_fat;
            std::size_t m_first_index;
            std::size_t m_length;
        };
    }
    // ============================================================================================================
    archive_data_loader::archive_data_loader(std::string phys_name, loaders_map_t loaders)
    : physical_data_loader(std::move(phys_name)),
      m_loaders(std::move(loaders))
    {

    }
    // ------------------------------------------------------------------------------------------------------------
    void archive_data_loader::open(std::shared_ptr<provider::physfs::directory> dir)
    {
        auto istream = dir->open_file(physical_name());
        m_fat = load_fat(*istream);

        //auto di = m_fat[0].get_dir_info();
        m_root = std::make_shared<detail::archive_directory>(m_loaders, std::move(istream), m_fat, 0, m_fat.size());
    }
    // ------------------------------------------------------------------------------------------------------------
    std::size_t archive_data_loader::size() const
    {
        return m_root->size();
    }
    // ------------------------------------------------------------------------------------------------------------
    provider::file_content_t archive_data_loader::load(std::size_t index) const
    {
        return m_root->load(index);
    }
    // ------------------------------------------------------------------------------------------------------------
    provider::file_type_t archive_data_loader::type(std::size_t index) const
    {
        return m_root->type(index);
    }
    // ------------------------------------------------------------------------------------------------------------
    std::string archive_data_loader::name(std::size_t index) const
    {
        return m_root->name(index);
    }
    // ------------------------------------------------------------------------------------------------------------
    bool archive_data_loader::is_directory(std::size_t index) const
    {
        return m_root->is_directory(index);
    }
    // ------------------------------------------------------------------------------------------------------------
    std::shared_ptr<abstract_directory> archive_data_loader::load_directory(std::size_t index) const
    {
        return m_root->load_directory(index);
    }
}
