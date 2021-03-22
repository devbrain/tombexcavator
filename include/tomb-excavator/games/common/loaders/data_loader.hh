//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_DATA_LOADER_HH
#define TOMBEXCAVATOR_DATA_LOADER_HH

#include <tomb-excavator/export-games-common.h>
#include <tomb-excavator/provider/physfs/directory.hh>
#include <tomb-excavator/provider/file_types.hh>
#include <functional>
#include <utility>

namespace games::common
{
    /**
     * Base class for all data loaders
     */
    class GAMES_COMMON_API abstract_data_loader
    {
    public:
        virtual ~abstract_data_loader();

        virtual void open(std::shared_ptr<provider::physfs::directory> dir) = 0;

        [[nodiscard]] virtual std::size_t size() const = 0;
        [[nodiscard]] virtual provider::file_content_t load(std::size_t index) const = 0;
        [[nodiscard]] virtual provider::file_type_t    type(std::size_t index) const = 0;
        [[nodiscard]] virtual std::string name(std::size_t index) const = 0;
        [[nodiscard]] virtual bool accept(const provider::physfs::directory& dir) const = 0;
        [[nodiscard]] virtual bool is_directory(std::size_t index) const = 0;
    };
    // =============================================================================================
    class GAMES_COMMON_API physical_data_loader : public abstract_data_loader
    {
    public:
        explicit physical_data_loader(std::string phys_name);

        [[nodiscard]] bool accept(const provider::physfs::directory& dir) const override;
        [[nodiscard]] const std::string& physical_name() const;
    private:
        std::string m_phys_name;
    };
    // ================================================================================================
    namespace detail
    {
        template<typename FileType>
        class single_entry_data_loader : public abstract_data_loader
        {
        public:
            explicit single_entry_data_loader(std::string virt_name);
        protected:
            [[nodiscard]] std::size_t size() const override;
            [[nodiscard]] provider::file_type_t type(std::size_t index) const override;
            [[nodiscard]] bool is_directory(std::size_t index) const override;
            [[nodiscard]] std::string name(std::size_t index) const override;
        private:
            std::string m_virt_name;
        };
    }
    // ================================================================================================
    template <typename FileType>
    class single_entry_physical_data_loader : public detail::single_entry_data_loader<FileType>
    {
    public:
        using entry_loader_t = std::function<provider::file_content_t (std::istream&)>;
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
                : m_virt_name(std::move(virt_name))
        {

        }
        // --------------------------------------------------------------------------------------------
        template<typename FileType>
        std::size_t single_entry_data_loader<FileType>::size() const
        {
            return 1;
        }
        // --------------------------------------------------------------------------------------------
        template<typename FileType>
        provider::file_type_t single_entry_data_loader<FileType>::type([[maybe_unused]] std::size_t index) const
        {
            return provider::make_file_type<FileType>();
        }
        // --------------------------------------------------------------------------------------------
        template<typename FileType>
        bool single_entry_data_loader<FileType>::is_directory([[maybe_unused]] std::size_t index) const
        {
            return false;
        }
        // --------------------------------------------------------------------------------------------
        template<typename FileType>
        std::string single_entry_data_loader<FileType>::name([[maybe_unused]] std::size_t index) const
        {
            return m_virt_name;
        }
    } // ns detail
    // ===============================================================================
    template <typename FileType>
    single_entry_physical_data_loader<FileType>::single_entry_physical_data_loader(const std::string& virt_name,
                                                                         std::string phys_name,
                                                                         entry_loader_t ldr)
            : detail::single_entry_data_loader<FileType>(std::move(virt_name)),
              m_phys_name(std::move(phys_name)),
              m_loader(std::move(ldr))
    {

    }
    // --------------------------------------------------------------------------------------------
    template <typename FileType>
    void single_entry_physical_data_loader<FileType>::open(std::shared_ptr<provider::physfs::directory> dir)
    {
        m_dir = dir;
    }
    // --------------------------------------------------------------------------------------------
    template <typename FileType>
    provider::file_content_t single_entry_physical_data_loader<FileType>::load([[maybe_unused]] std::size_t index) const
    {
        auto is = m_dir->open_file(m_phys_name);
        return m_loader(*is);
    }
    // --------------------------------------------------------------------------------------------
    template <typename FileType>
    bool single_entry_physical_data_loader<FileType>::accept(const provider::physfs::directory& dir) const
    {
        return dir.contains(m_phys_name);
    }
    // ==============================================================================================
    template <typename FileType>
    single_entry_virtual_data_loader<FileType>::single_entry_virtual_data_loader(const std::string& virt_name)
            : detail::single_entry_data_loader<FileType>(std::move(virt_name))
    {

    }
    // --------------------------------------------------------------------------------------------
    template <typename FileType>
    void single_entry_virtual_data_loader<FileType>::open([[maybe_unused]] std::shared_ptr<provider::physfs::directory> dir)
    {
    }
    // --------------------------------------------------------------------------------------------
    template <typename FileType>
    provider::file_content_t single_entry_virtual_data_loader<FileType>::load([[maybe_unused]] std::size_t index) const
    {
        return this->read();
    }
    // --------------------------------------------------------------------------------------------
    template <typename FileType>
    bool single_entry_virtual_data_loader<FileType>::accept([[maybe_unused]] const provider::physfs::directory& dir) const
    {
        return true;
    }

}

#endif //TOMBEXCAVATOR_DATA_LOADER_HH
