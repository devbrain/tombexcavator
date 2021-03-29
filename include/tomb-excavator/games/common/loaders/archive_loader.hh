//
// Created by igor on 22/03/2021.
//

#ifndef TOMBEXCAVATOR_ARCHIVE_LOADER_HH
#define TOMBEXCAVATOR_ARCHIVE_LOADER_HH

#include <tomb-excavator/games/common/vfs/data_loader.hh>
#include <variant>
#include <tuple>
#include <vector>
#include <cstdint>
#include <map>
#include <any>
#include <functional>

namespace games::common
{
    /**
     * Loader for archives
     */
    class GAMES_COMMON_API archive_data_loader : public physical_data_loader
    {
    public:
        class fat_entry
        {
        public:
            struct file_info
            {
                std::size_t size;
                uint64_t    offset; // absolute offset
                int         internal_type;
                provider::file_type_t type;
            };

            struct dir_info
            {
                std::size_t first_fat_index;
                std::size_t entries_count;
            };

            using props_map_t = std::map<int, std::any>;
        public:
            fat_entry(const std::string& name, file_info fileinfo);
            fat_entry(const std::string& name, dir_info dirinfo);
            fat_entry(const std::string& name, file_info fileinfo, props_map_t props);
            fat_entry(const std::string& name, dir_info dirinfo, props_map_t props);

            [[nodiscard]] std::string name() const noexcept;
            [[nodiscard]] bool is_directory() const noexcept;
            [[nodiscard]] dir_info get_dir_info() const;
            [[nodiscard]] file_info get_file_info() const;
            [[nodiscard]] const props_map_t& get_props() const;
        private:
            std::string m_name;
            std::variant<dir_info, file_info> m_info;
            props_map_t m_props;
        };
    public:
        using loader_t = std::function<provider::file_content_t (std::istream& is, const fat_entry::file_info& fi, const fat_entry::props_map_t& props)>;
        using loaders_map_t = std::map<int, loader_t>; // internal type to loader
    public:
        explicit archive_data_loader(std::string phys_name, loaders_map_t loaders);
    protected:
        [[nodiscard]] virtual std::vector<fat_entry> load_fat(std::istream& is) = 0;
    private:
        virtual void open(std::shared_ptr<provider::physfs::directory> dir) override;

        [[nodiscard]] virtual std::size_t size() const override;
        [[nodiscard]] virtual provider::file_content_t load(std::size_t index) const override;
        [[nodiscard]] virtual provider::file_type_t    type(std::size_t index) const override;
        [[nodiscard]] virtual std::string name(std::size_t index) const override;
        [[nodiscard]] virtual bool is_directory(std::size_t index) const override;
        [[nodiscard]] virtual std::shared_ptr<abstract_directory> load_directory(std::size_t index) const override;
    private:
        std::shared_ptr<abstract_directory> m_root;
        std::vector<fat_entry> m_fat;
        loaders_map_t m_loaders;
    };
}

#endif //TOMBEXCAVATOR_ARCHIVE_LOADER_HH
