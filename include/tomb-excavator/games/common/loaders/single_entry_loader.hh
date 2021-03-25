//
// Created by igor on 25/03/2021.
//

#ifndef TOMBEXCAVATOR_SINGLE_ENTRY_LOADER_HH
#define TOMBEXCAVATOR_SINGLE_ENTRY_LOADER_HH

#include <tomb-excavator/games/common/vfs/data_loader.hh>

namespace games::common
{
    namespace detail
    {
        template<typename FileType>
        class single_entry_data_loader : public base_single_entry_data_loader
        {
        public:
            explicit single_entry_data_loader(std::string virt_name);
        protected:
            [[nodiscard]] provider::file_type_t type(std::size_t index) const override;
        };
    }
    // ================================================================================================
    template<typename FileType>
    class single_entry_physical_data_loader : public detail::single_entry_data_loader<FileType>
    {
    public:
        using entry_loader_t = std::function<FileType(std::istream&)>;
    public:
        single_entry_physical_data_loader(const std::string& virt_name, std::string phys_name, entry_loader_t ldr);
    private:
        void open(std::shared_ptr<provider::physfs::directory> dir) override;
        [[nodiscard]] provider::file_content_t load(std::size_t index) const override;
        [[nodiscard]] bool accept(const provider::physfs::directory& dir) const override;
    private:
        std::string m_phys_name;
        std::shared_ptr<provider::physfs::directory> m_dir;
        entry_loader_t m_loader;
    };

    // ================================================================================================
    template<typename FileType>
    class single_entry_virtual_data_loader : public detail::single_entry_data_loader<FileType>
    {
    public:
        explicit single_entry_virtual_data_loader(const std::string& virt_name);
    private:
        void open(std::shared_ptr<provider::physfs::directory> dir) override;
        [[nodiscard]] provider::file_content_t load(std::size_t index) const override;
        [[nodiscard]] bool accept(const provider::physfs::directory& dir) const override;
    protected:
        [[nodiscard]] virtual FileType read() const = 0;
    };
    // ===============================================================================
    // Implementation
    // ===============================================================================
    namespace detail
    {
        template<typename FileType>
        single_entry_data_loader<FileType>::single_entry_data_loader(std::string virt_name)
                : base_single_entry_data_loader(std::move(virt_name))
        {

        }
        // --------------------------------------------------------------------------------------------
        template<typename FileType>
        provider::file_type_t single_entry_data_loader<FileType>::type([[maybe_unused]] std::size_t index) const
        {
            return provider::make_file_type<FileType>();
        }
    } // ns detail
    // ===============================================================================
    template<typename FileType>
    single_entry_physical_data_loader<FileType>::single_entry_physical_data_loader(const std::string& virt_name,
                                                                                   std::string phys_name,
                                                                                   entry_loader_t ldr)
            : detail::single_entry_data_loader<FileType>(std::move(virt_name)),
              m_phys_name(std::move(phys_name)),
              m_loader(std::move(ldr))
    {

    }
    // --------------------------------------------------------------------------------------------
    template<typename FileType>
    void single_entry_physical_data_loader<FileType>::open(std::shared_ptr<provider::physfs::directory> dir)
    {
        m_dir = dir;
    }
    // --------------------------------------------------------------------------------------------
    template<typename FileType>
    provider::file_content_t single_entry_physical_data_loader<FileType>::load([[maybe_unused]] std::size_t index) const
    {
        auto is = m_dir->open_file(m_phys_name);
        return m_loader(*is);
    }
    // --------------------------------------------------------------------------------------------
    template<typename FileType>
    bool single_entry_physical_data_loader<FileType>::accept(const provider::physfs::directory& dir) const
    {
        return dir.contains(m_phys_name);
    }
    // ==============================================================================================
    template<typename FileType>
    single_entry_virtual_data_loader<FileType>::single_entry_virtual_data_loader(const std::string& virt_name)
            : detail::single_entry_data_loader<FileType>(std::move(virt_name))
    {

    }
    // --------------------------------------------------------------------------------------------
    template<typename FileType>
    void
    single_entry_virtual_data_loader<FileType>::open([[maybe_unused]] std::shared_ptr<provider::physfs::directory> dir)
    {
    }
    // --------------------------------------------------------------------------------------------
    template<typename FileType>
    provider::file_content_t single_entry_virtual_data_loader<FileType>::load([[maybe_unused]] std::size_t index) const
    {
        return this->read();
    }
    // --------------------------------------------------------------------------------------------
    template<typename FileType>
    bool
    single_entry_virtual_data_loader<FileType>::accept([[maybe_unused]] const provider::physfs::directory& dir) const
    {
        return true;
    }
}

#endif //TOMBEXCAVATOR_SINGLE_ENTRY_LOADER_HH
