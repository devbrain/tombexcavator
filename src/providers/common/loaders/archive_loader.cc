//
// Created by igor on 30/03/2021.
//

#include <tomb-excavator/games/common/loaders/archive_loader.hh>
#include <tomb-excavator/bsw/override.hh>

namespace games::common
{
    // ======================================================================================================
    archive_loader::archive_loader(std::string physical_name, std::unique_ptr<archive_entry_loaders_registry> loaders)
    : base_archive_data_loader(std::move(physical_name), std::move(loaders))
    {

    }
    // -----------------------------------------------------------------------------------------------------
    void archive_loader::load_fat(std::istream& is, fat_events& builder)
    {
        read_header(is);
        while (true)
        {
            auto ev = read_next_entry(is);
            if (!ev)
            {
                break;
            }
            std::visit(bsw::overload(
                    [&builder, this](const fat_file_event& e)
                    {
                        auto file_md = loaders().get_file_metadata(e.name);
                        if (!file_md)
                        {
                            RAISE_EX("Can not deduce metadata for ", e.name);
                        }
                        auto [internal_type, dto_type] = *file_md;
                        fat_entry_t::file_info fi {e.size, e.offset, internal_type, dto_type};
                        builder.add_file(e.name, fi, e.props, e.deps);
                    },
                    [&builder](const fat_dir_start_event& e)
                    {
                        builder.start_dir(e.name, e.props);
                    },
                    [&builder](const fat_dir_end_event& )
                    {
                        builder.end_dir();
                    }), *ev);
        }
    }

}