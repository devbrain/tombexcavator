//
// Created by igor on 22/03/2021.
//

#include <tomb-excavator/games/common/loaders/base_archive_loader.hh>
#include <tomb-excavator/bsw/exceptions.hh>

namespace games::common
{

    // ============================================================================================================
    namespace detail
    {
        class archive_directory : public abstract_directory
        {
        public:
            archive_directory(const archive_entry_loaders_registry& loaders,
                              std::shared_ptr<std::istream> istream,
                              const std::vector<fat_entry>& fat,
                              std::size_t first_index, std::size_t length)
                              : m_loaders(loaders),
                                m_istream(istream),
                                m_fat (fat),
                                m_first_index(first_index),
                                m_length(length)
            {

            }
        private:
            [[nodiscard]] std::size_t size() const override
            {
                return m_length;
            }
            [[nodiscard]] provider::file_content_t load(std::size_t index) const override
            {
                auto fi = m_fat[m_first_index + index].get_file_info();
                return m_loaders.read(*m_istream, fi, m_fat[m_first_index + index].get_props());
            }

            [[nodiscard]] provider::file_type_t type(std::size_t index) const override
            {
                auto fi = m_fat[m_first_index + index].get_file_info();
                return fi.type;
            }

            [[nodiscard]] std::string name(std::size_t index) const override
            {
                return m_fat[m_first_index + index].name();
            }
            [[nodiscard]] bool is_directory(std::size_t index) const override
            {
                return m_fat[m_first_index + index].is_directory();
            }
            [[nodiscard]] std::shared_ptr<abstract_directory> load_directory(std::size_t index) const override
            {
                auto di = m_fat[m_first_index+index].get_dir_info();
                return std::make_shared<archive_directory>(m_loaders, m_istream, m_fat, di.first_fat_index, di.entries_count);
            }
        private:
            const archive_entry_loaders_registry&  m_loaders;
            std::shared_ptr<std::istream> m_istream;
            const std::vector<fat_entry>& m_fat;
            std::size_t m_first_index;
            std::size_t m_length;
        };
    }
    // ============================================================================================================
    base_archive_data_loader::base_archive_data_loader(std::string phys_name, std::unique_ptr<archive_entry_loaders_registry> loaders)
    : physical_data_loader(std::move(phys_name)),
      m_loaders(std::move(loaders))
    {

    }
    // ------------------------------------------------------------------------------------------------------------
    void base_archive_data_loader::open(std::shared_ptr<provider::physfs::directory> dir)
    {
        auto istream = dir->open_file(physical_name());
        fat_builder builder;
        load_fat(*istream, builder);
        auto fat_info = builder.build();
        m_fat = std::move(std::get<0>(fat_info));
        auto sz = std::get<1>(fat_info);
        m_root = std::make_shared<detail::archive_directory>(*m_loaders, std::move(istream), m_fat, 0, sz);
    }
    // ------------------------------------------------------------------------------------------------------------
    std::size_t base_archive_data_loader::size() const
    {
        return m_root->size();
    }
    // ------------------------------------------------------------------------------------------------------------
    provider::file_content_t base_archive_data_loader::load(std::size_t index) const
    {
        return m_root->load(index);
    }
    // ------------------------------------------------------------------------------------------------------------
    provider::file_type_t base_archive_data_loader::type(std::size_t index) const
    {
        return m_root->type(index);
    }
    // ------------------------------------------------------------------------------------------------------------
    std::string base_archive_data_loader::name(std::size_t index) const
    {
        return m_root->name(index);
    }
    // ------------------------------------------------------------------------------------------------------------
    bool base_archive_data_loader::is_directory(std::size_t index) const
    {
        return m_root->is_directory(index);
    }
    // ------------------------------------------------------------------------------------------------------------
    std::shared_ptr<abstract_directory> base_archive_data_loader::load_directory(std::size_t index) const
    {
        return m_root->load_directory(index);
    }
    // ------------------------------------------------------------------------------------------------------------
    archive_entry_loaders_registry& base_archive_data_loader::loaders ()
    {
        return *m_loaders;
    }
    // ------------------------------------------------------------------------------------------------------------
    const archive_entry_loaders_registry& base_archive_data_loader::loaders () const
    {
        return *m_loaders;
    }
}
