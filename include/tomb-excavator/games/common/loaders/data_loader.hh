//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_DATA_LOADER_HH
#define TOMBEXCAVATOR_DATA_LOADER_HH

#include <tomb-excavator/export-games-common.h>
#include <tomb-excavator/provider/physfs/directory.hh>
#include <tomb-excavator/provider/file_types.hh>

namespace games::common
{
    class GAMES_COMMON_API data_loader
    {
    public:
        explicit data_loader(std::string phys_name);
        virtual ~data_loader();

        virtual void open(std::shared_ptr<provider::physfs::directory> dir) = 0;

        [[nodiscard]] virtual std::size_t size() const = 0;
        [[nodiscard]] virtual provider::file_content_t load(std::size_t index) const = 0;
        [[nodiscard]] virtual provider::file_type_t    type(std::size_t index) const = 0;
        [[nodiscard]] virtual std::string name(std::size_t index) const = 0;
        [[nodiscard]] const std::string& physical_name() const;
    private:
        std::string m_phys_name;
    };
    // ================================================================================================
    class GAMES_COMMON_API single_entry_data_loader : public data_loader
    {
    public:
        single_entry_data_loader(std::string virt_name, std::string phys_name);
    private:
        void open(std::shared_ptr<provider::physfs::directory> dir) override;
        [[nodiscard]] std::size_t size() const override;
        [[nodiscard]] provider::file_content_t load(std::size_t index) const override;
        [[nodiscard]] virtual provider::file_type_t type(std::size_t index) const = 0;
        [[nodiscard]] std::string name(std::size_t index) const override;
    protected:
        [[nodiscard]] virtual provider::file_content_t read(std::istream& is) const = 0;
    private:
        std::string m_virt_name;
        std::shared_ptr<provider::physfs::directory> m_dir;
    };
    // ================================================================================================
    template <typename FileType>
    class typed_single_entry_data_loader : public single_entry_data_loader
    {
    public:
        typed_single_entry_data_loader(std::string virt_name, std::string phys_name)
        : single_entry_data_loader(std::move(virt_name), std::move(phys_name))
        {

        }
    protected:
        [[nodiscard]] virtual FileType load_content(std::istream& is) const = 0;
    private:
        [[nodiscard]] provider::file_type_t type(std::size_t index) const override
        {
            return provider::make_file_type<FileType>();
        }

        [[nodiscard]] provider::file_content_t read(std::istream& is) const override
        {
            return this->load_content(is);
        }

    };

}

#endif //TOMBEXCAVATOR_DATA_LOADER_HH
