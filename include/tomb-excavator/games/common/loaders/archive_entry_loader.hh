//
// Created by igor on 02/04/2021.
//

#ifndef TOMBEXCAVATOR_ARCHIVE_ENTRY_LOADER_HH
#define TOMBEXCAVATOR_ARCHIVE_ENTRY_LOADER_HH

#include <tomb-excavator/export-games-common.h>
#include <tomb-excavator/provider/file_types.hh>
#include <tomb-excavator/games/common/loaders/fat.hh>

#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <optional>

namespace games::common
{

    class GAMES_COMMON_API archive_entry_loader
    {
    public:
        using name_acceptor_t = std::function<bool (const std::string& name)>;

        template <typename T>
        using entry_reader_t = std::function<T (std::istream& is,
                                                uint64_t offset,
                                                std::size_t size,
                                                const fat_entry::props_map_t& props)>;

        template <typename T>
        using free_entry_reader_t = T (*) (std::istream& is,
                                           uint64_t offset,
                                           std::size_t size,
                                           const fat_entry::props_map_t& props);

        archive_entry_loader(name_acceptor_t name_acceptor,
                             provider::file_type_t type,
                             entry_reader_t<provider::file_content_t> loader);


        [[nodiscard]] bool accept(const std::string& name) const;
        [[nodiscard]] provider::file_type_t get_file_type() const;
        [[nodiscard]] provider::file_content_t read(std::istream& is, uint64_t offset,
                                                    std::size_t size, const fat_entry::props_map_t& props) const;
    private:
        name_acceptor_t accept_fn;
        provider::file_type_t m_type;
        entry_reader_t<provider::file_content_t>  reader_fn;
    };

    template<typename T>
    archive_entry_loader make_entry_loader(archive_entry_loader::name_acceptor_t acceptor,
                                           archive_entry_loader::free_entry_reader_t<T> loader)
    {
        auto f = [loader](std::istream& is, uint64_t offset, std::size_t size, const fat_entry::props_map_t& props)
        {
            return provider::file_content_t{loader(is, offset, size, props)};
        };
        return {std::move(acceptor), provider::make_file_type<T>(), f};
    }

    template<typename T>
    archive_entry_loader make_entry_loader(archive_entry_loader::name_acceptor_t acceptor,
                                           archive_entry_loader::entry_reader_t<T> loader)
    {
        auto f = [loader](std::istream& is, uint64_t offset, std::size_t size, const fat_entry::props_map_t& props)
        {
            return provider::file_content_t{loader(is, offset, size, props)};
        };
        return {std::move(acceptor), provider::make_file_type<T>(), f};
    }

    // =============================================================================================================
    class GAMES_COMMON_API archive_entry_loaders_registry
    {
    public:
        archive_entry_loaders_registry(std::initializer_list<archive_entry_loader> inits);
        virtual ~archive_entry_loaders_registry();
        void add(archive_entry_loader loader);
        // internal id, type
        [[nodiscard]] virtual std::optional<std::tuple<int, provider::file_type_t>> get_file_metadata(const std::string& name) const;
        [[nodiscard]] provider::file_content_t read(std::istream& is, const fat_entry::file_info& fi, const fat_entry::props_map_t& props) const;
    private:
        std::vector<archive_entry_loader> m_loaders;
    };
    // =============================================================================================================
    class GAMES_COMMON_API simple_loaders_registry : public archive_entry_loaders_registry
    {
    public:
        simple_loaders_registry(std::initializer_list<archive_entry_loader> inits);

        [[nodiscard]] std::optional<std::tuple<int, provider::file_type_t>>
        get_file_metadata(const std::string& name) const override;

        static std::vector<char> load_as_vector(std::istream& is,
                                         uint64_t offset,
                                         std::size_t size);
    private:
        archive_entry_loader m_fallback;
        int                  m_fallback_id;
    };
}


#endif //TOMBEXCAVATOR_ARCHIVE_ENTRY_LOADER_HH
