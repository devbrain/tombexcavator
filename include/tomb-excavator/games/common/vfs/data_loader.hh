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
#include <memory>

#include <tomb-excavator/msvc/c4251-begin.h>

namespace games::common
{

    class GAMES_COMMON_API abstract_directory
    {
    public:
        virtual ~abstract_directory();
        [[nodiscard]] virtual std::size_t size() const = 0;
        [[nodiscard]] virtual provider::file_content_t load(std::size_t index) const = 0;
        [[nodiscard]] virtual provider::file_type_t type(std::size_t index) const = 0;
        [[nodiscard]] virtual std::string name(std::size_t index) const = 0;
        [[nodiscard]] virtual bool is_directory(std::size_t index) const = 0;
        [[nodiscard]] virtual std::shared_ptr<abstract_directory> load_directory(std::size_t index) const = 0;
    };

    /**
     * Base class for all data loaders
     */
    class GAMES_COMMON_API abstract_data_loader : public abstract_directory
    {
    public:
        virtual void open(std::shared_ptr<provider::physfs::directory> dir) = 0;
        [[nodiscard]] virtual bool accept(const provider::physfs::directory& dir) const = 0;
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
    class GAMES_COMMON_API base_single_entry_data_loader : public abstract_data_loader
    {
    public:
        explicit base_single_entry_data_loader(std::string virt_name);
    protected:
        [[nodiscard]] std::size_t size() const override;
        [[nodiscard]] bool is_directory(std::size_t index) const override;
        [[nodiscard]] std::string name(std::size_t index) const override;
        [[nodiscard]] std::shared_ptr<abstract_directory> load_directory(std::size_t index) const override;
    private:
        std::string m_virt_name;
    };


#include <tomb-excavator/msvc/c4251-end.h>
}

#endif //TOMBEXCAVATOR_DATA_LOADER_HH
