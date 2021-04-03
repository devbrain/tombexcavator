//
// Created by igor on 30/03/2021.
//

#include <tomb-excavator/games/common/loaders/archive_loader.hh>

namespace games::common
{
    namespace detail
    {

        template<class T>
        struct override_helper { using type=T; };

        template<class T>
        using override_helper_t = typename override_helper<T>::type;

        template<class R, class...Args>
        struct override_helper<R(*)(Args...)> {
            struct type {
                R(*f)(Args...);
                R operator()(Args...args)const { return f(std::forward<Args>(args)...); }
                type(R(*in)(Args...)):f(in) {}
            };
        };
        template<class R, class...Args>
        struct override_helper<R(&)(Args...)>:override_helper<R(*)(Args...)> {
            using override_helper<R(*)(Args...)>::override_helper;
        };

        template<class...Fs>
        struct overload:override_helper_t<Fs>... {
            using override_helper_t<Fs>::operator()...;
            overload(Fs...fs):override_helper_t<Fs>(std::move(fs))... {}
        };
    } // ns detail
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
            std::visit(detail::overload(
                    [&builder, this](const fat_file_event& e)
                    {
                        auto file_md = loaders().get_file_metadata(e.name);
                        if (!file_md)
                        {
                            RAISE_EX("Can not deduce metadata for ", e.name);
                        }
                        auto [internal_type, dto_type] = *file_md;
                        fat_entry_t::file_info fi {e.size, e.offset, internal_type, dto_type};
                        if (e.props)
                        {
                            builder.add_file(e.name, fi, *e.props);
                        }
                        else
                        {
                            builder.add_file(e.name, fi);
                        }
                    },
                    [&builder](const fat_dir_start_event& e)
                    {
                        if (e.props)
                        {
                            builder.start_dir(e.name, *e.props);
                        }
                        else
                        {
                            builder.start_dir(e.name);
                        }
                    },
                    [&builder](const fat_dir_end_event& )
                    {
                        builder.end_dir();
                    }), *ev);
        }
    }

}