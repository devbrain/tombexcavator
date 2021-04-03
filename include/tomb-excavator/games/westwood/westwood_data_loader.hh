//
// Created by igor on 25/03/2021.
//

#ifndef TOMBEXCAVATOR_WESTWOOD_DATA_LOADER_HH
#define TOMBEXCAVATOR_WESTWOOD_DATA_LOADER_HH

#include <tomb-excavator/export-games-westwood.h>
#include <tomb-excavator/games/common/vfs/data_loader.hh>
#include <tomb-excavator/games/common/loaders/base_archive_loader.hh>
#include <string>
#include <tuple>
#include <vector>
#include <memory>
#include <map>
#include <tomb-excavator/msvc/c4251-begin.h>

namespace games::westwood
{
    class GAMES_WESTWOOD_API root_directory_loader : public common::abstract_data_loader
    {
    public:
        using file_names_t = std::initializer_list<std::string>;
    public:
        root_directory_loader(file_names_t pak_files);
    protected:
        virtual std::unique_ptr<common::physical_data_loader> create_pak_loader(const std::string& pak_name) = 0;
    private:
        [[nodiscard]] std::size_t size() const override;
        [[nodiscard]] provider::file_content_t load(std::size_t index) const override;
        [[nodiscard]] provider::file_type_t type(std::size_t index) const override;
        [[nodiscard]] std::string name(std::size_t index) const override;
        [[nodiscard]] bool is_directory(std::size_t index) const override;
        [[nodiscard]] std::shared_ptr<abstract_directory> load_directory(std::size_t index) const override;
        void open(std::shared_ptr<provider::physfs::directory> dir) override;
        [[nodiscard]] bool accept(const provider::physfs::directory& dir) const override;
    private:
        using archive_loader_t = std::tuple<std::string, std::shared_ptr<common::physical_data_loader>>;
        std::vector<archive_loader_t> m_archive_loaders;
    };
}

#include <tomb-excavator/msvc/c4251-end.h>
#endif //TOMBEXCAVATOR_WESTWOOD_DATA_LOADER_HH
