//
// Created by igor on 31/03/2021.
//

#include <tomb-excavator/games/common/loaders/fat.hh>
#include <utility>
#include <vector>
#include <queue>

namespace games::common
{
    fat_entry::fat_entry(std::string  name, file_info fileinfo)
            : m_name(std::move(name)),
              m_info(fileinfo)
    {

    }
    // ------------------------------------------------------------------------------------------------------------
    fat_entry::fat_entry(std::string  name, dir_info dirinfo)
            : m_name(std::move(name)),
              m_info(dirinfo)
    {

    }
    // ------------------------------------------------------------------------------------------------------------
    fat_entry::fat_entry(std::string  name, file_info fileinfo, props_map_t props)
            : m_name(std::move(name)),
              m_info(fileinfo),
              m_props(std::move(props))
    {

    }
    // ------------------------------------------------------------------------------------------------------------
    fat_entry::fat_entry(std::string  name, dir_info dirinfo, props_map_t props)
            : m_name(std::move(name)),
              m_info(dirinfo),
              m_props(std::move(props))
    {

    }
    // ------------------------------------------------------------------------------------------------------------
    std::string fat_entry::name() const noexcept
    {
        return m_name;
    }
    // ------------------------------------------------------------------------------------------------------------
    bool fat_entry::is_directory() const noexcept
    {
        return std::get_if<dir_info>(&m_info) != nullptr;
    }
    // ------------------------------------------------------------------------------------------------------------
    fat_entry::dir_info fat_entry::get_dir_info() const
    {
        return std::get<dir_info>(m_info);
    }
    // ------------------------------------------------------------------------------------------------------------
    fat_entry::file_info fat_entry::get_file_info() const
    {
        return std::get<file_info>(m_info);
    }
    // ------------------------------------------------------------------------------------------------------------
    const fat_entry::props_map_t& fat_entry::get_props() const
    {
        return m_props;
    }
    // ============================================================================================================
    struct directory;
    using dirent_t = std::tuple<std::string, fat_entry::props_map_t, directory*>;
    using file_t = std::tuple<std::string, fat_entry::props_map_t, fat_entry::file_info>;
    constexpr std::size_t BAD_INDEX = std::numeric_limits<std::size_t>::max();
    struct directory
    {

        explicit directory(directory* parent_dir)
        : parent(parent_dir), visited(false), fat_index(BAD_INDEX)
        {}

        ~directory()
        {
            for (auto& e : entries)
            {
                if (dirent_t* dirent =  std::get_if<dirent_t>(&e))
                {
                    delete std::get<2>(*dirent);
                }
            }
        }

        std::vector<directory*> dump(std::vector<fat_entry>& out) const
        {
            std::vector<directory*> adj;
            for (const auto& e : entries)
            {
                if (const dirent_t* dirent =  std::get_if<dirent_t>(&e))
                {
                    const auto&[name, props, d] = *dirent;
                    d->fat_index = out.size();
                    adj.push_back(d);
                    out.emplace_back(name, fat_entry::dir_info {BAD_INDEX, d->entries.size()}, props);
                }
                else
                {
                    const auto&[name, props, fi] = *std::get_if<file_t>(&e);
                    out.emplace_back(name, fi, props);
                }
            }
            return adj;
        }
        // for BFS
        mutable std::size_t fat_index;
        mutable bool visited;

        directory* parent;
        using entry_t = std::variant<file_t, dirent_t>;
        std::vector<entry_t> entries;
    };

    struct fat_builder::impl
    {
        impl()
        {
            root = new directory(nullptr);
            current = root;
        }

        ~impl()
        {
            delete (root);
        }
        directory* root;
        directory* current;
    };
    // ============================================================================================================
    fat_builder::fat_builder()
    {
        m_pimpl = std::make_unique<impl>();
    }
    // ------------------------------------------------------------------------------------------------------------
    fat_builder::~fat_builder() = default;
    // ------------------------------------------------------------------------------------------------------------
    void fat_builder::add_file(std::string name, fat_entry::file_info fi, fat_entry::props_map_t props)
    {
        m_pimpl->current->entries.emplace_back(std::make_tuple(std::move(name), std::move(props), fi));
    }
    // ------------------------------------------------------------------------------------------------------------
    void fat_builder::start_dir(std::string name, fat_entry::props_map_t props)
    {
        auto* new_dir = new directory(m_pimpl->current);
        m_pimpl->current->entries.emplace_back(std::make_tuple(std::move(name), std::move(props), new_dir));
        m_pimpl->current = new_dir;
    }
    // ------------------------------------------------------------------------------------------------------------
    void fat_builder::add_file(std::string name, fat_entry::file_info fi)
    {
        add_file(std::move(name), fi, {});
    }
    // ------------------------------------------------------------------------------------------------------------
    void fat_builder::start_dir(std::string name)
    {
        start_dir(std::move(name), {});
    }
    // ------------------------------------------------------------------------------------------------------------
    void fat_builder::end_dir()
    {
        m_pimpl->current = m_pimpl->current->parent;
    }
    // ------------------------------------------------------------------------------------------------------------
    std::tuple<std::vector<fat_entry>, std::size_t> fat_builder::build() const
    {
        auto root_size = m_pimpl->root->entries.size();
        std::vector<fat_entry> out;

        std::queue<directory*> queue;
        queue.push(m_pimpl->root);
        m_pimpl->root->visited = true;
        while (!queue.empty())
        {
            directory* current = queue.front();
            queue.pop();


            if (current->fat_index != BAD_INDEX)
            {
                std::get<fat_entry::dir_info>(out[current->fat_index].m_info).first_fat_index = out.size();
            }

            auto adj = current->dump(out);

            for (auto* d : adj)
            {
                if (!d->visited)
                {
                    queue.push(d);
                    d->visited = true;
                }
            }
        }
        return {out, root_size};
    }
    // ===============================================================================================
    fat_events::~fat_events() = default;
}