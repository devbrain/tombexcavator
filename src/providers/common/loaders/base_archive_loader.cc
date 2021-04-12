//
// Created by igor on 22/03/2021.
//

#include <tomb-excavator/games/common/loaders/base_archive_loader.hh>
#include <tomb-excavator/bsw/exceptions.hh>

namespace games::common
{
    namespace detail
    {
        class graph
        {
            std::map<int, std::vector<int>> adj;
            bool is_cyclic_util(int v, std::set<int>& visited, std::set<int>& rs) const;
        public:
            void add_edge(int v, int w);
            [[nodiscard]] bool is_cyclic() const;
        };
        // -------------------------------------------------------------------------------------------------
        void graph::add_edge(int v, int w)
        {
            adj[v].push_back(w);
        }
        // -------------------------------------------------------------------------------------------------
        bool graph::is_cyclic_util(int v, std::set<int>& visited, std::set<int>& rs) const
        {
            if (visited.find(v) == visited.end())
            {
                visited.insert(v);
                rs.insert(v);

                auto itr = adj.find(v);
                if (itr == adj.end())
                {
                   for (auto i : itr->second)
                    {
                        if ((visited.find(i) == visited.end()) && is_cyclic_util(i, visited, rs))
                        {
                            return true;
                        } else
                        {
                            if (rs.find(i) != rs.end())
                            {
                                return true;
                            }
                        }
                    }
                }
            }
            rs.erase(v);
            return false;
        }
        // -------------------------------------------------------------------------------------------------
        bool graph::is_cyclic() const
        {
            std::set<int> visited;
            std::set<int> recStack;

            for (const auto& [i, edges] : adj)
            {
                if (is_cyclic_util(i, visited, recStack))
                {
                    return true;
                }
            }
            return false;
        }
        // ===================================================================================================
        class dependencies_cache
        {
        public:
            dependencies_cache(const archive_entry_loaders_registry& loaders,
                               std::shared_ptr<std::istream> istream,
                               const std::vector<fat_entry>& fat)
                    : m_loaders(loaders),
                      m_istream(istream),
                      m_fat(fat)
            {
                graph g;
                for (std::size_t j=0; j<m_fat.size(); j++)
                {
                    const auto& entry  = m_fat[j];
                    if (!entry.is_directory())
                    {
                        const auto& deps = entry.get_deps();
                        for (auto map_key : deps)
                        {
                            bool found = false;
                            for (std::size_t i = 0; i < m_fat.size(); i++)
                            {
                                if (!m_fat[i].is_directory())
                                {
                                    const auto& props = m_fat[i].get_props();
                                    if (props.exists(map_key))
                                    {
                                        auto[itr, inserted] = m_deps_map.insert(std::make_pair(map_key, i));
                                        if (!inserted)
                                        {
                                            RAISE_EX("Both fat entries ", i, " and ", itr->second,
                                                     " satisfies dependency ", map_key);
                                        }
                                        g.add_edge(j, i);
                                        found = true;
                                    }
                                }
                            }
                            if (!found)
                            {
                                RAISE_EX("Can not find entry which satisfies dependency key ", map_key);
                            }
                        }
                    }
                }
                if (g.is_cyclic())
                {
                    RAISE_EX("Cyclic dependency detected");
                }
            }

            loader_context_t load_deps(const fat_entry& e)
            {
                loaded_deps_t out;
                const auto& deps = e.get_deps();
                for (auto map_key : deps)
                {
                    auto i = m_deps_map.find(map_key);
                    if (i == m_deps_map.end())
                    {
                        RAISE_EX("Can not find entry for dependency key ", map_key);
                    }
                    out.insert(std::make_pair(map_key, get_by_fat_index(i->second)));
                }
                return std::make_tuple(e.get_props(), out);
            }
        private:
            std::shared_ptr<provider::file_content_t> get_by_fat_index(std::size_t index)
            {
                auto itr = m_cache.find(index);
                if (itr != m_cache.end())
                {
                    return itr->second;
                }
                auto fi = m_fat[index].get_file_info();
                auto ctx = load_deps(m_fat[index]);
                auto content = std::make_shared<provider::file_content_t>(m_loaders.read(*m_istream,
                                                                                         fi,
                                                                                         ctx));
                m_cache.insert(std::make_pair(index, content));
                return content;
            }
        private:
            const archive_entry_loaders_registry& m_loaders;
            std::shared_ptr<std::istream> m_istream;
            const std::vector<fat_entry>& m_fat;
            std::map<int, std::size_t> m_deps_map; // dependency -> fat entry
            std::map<std::size_t, std::shared_ptr<provider::file_content_t>> m_cache; // fat entry -> file content

        };

        // ============================================================================================================
        class archive_directory : public abstract_directory
        {
        public:
            archive_directory(const archive_entry_loaders_registry& loaders,
                              std::shared_ptr<std::istream> istream,
                              const std::vector<fat_entry>& fat,
                              std::shared_ptr<dependencies_cache> cache,
                              std::size_t first_index, std::size_t length)
                    : m_loaders(loaders),
                      m_istream(istream),
                      m_fat(fat),
                      m_first_index(first_index),
                      m_length(length),
                      m_cache(cache)
            {

            }
        private:
            [[nodiscard]] std::size_t size() const override
            {
                return m_length;
            }
            [[nodiscard]] provider::file_content_t load(std::size_t index) const override
            {
                auto fi = m_fat[m_first_index + index].get_file_info();
                std::map<int, std::shared_ptr<provider::file_content_t>> needed_data;
                const auto& e = m_fat[m_first_index + index];
                auto ctx = m_cache->load_deps(e);
                return m_loaders.read(*m_istream, fi, ctx);
            }

            [[nodiscard]] provider::file_type_t type(std::size_t index) const override
            {
                auto fi = m_fat[m_first_index + index].get_file_info();
                return fi.type;
            }

            [[nodiscard]] std::string name(std::size_t index) const override
            {
                return m_fat[m_first_index + index].name();
            }
            [[nodiscard]] bool is_directory(std::size_t index) const override
            {
                return m_fat[m_first_index + index].is_directory();
            }
            [[nodiscard]] std::shared_ptr<abstract_directory> load_directory(std::size_t index) const override
            {
                auto di = m_fat[m_first_index + index].get_dir_info();
                return std::make_shared<archive_directory>(m_loaders, m_istream, m_fat, m_cache,
                                                           di.first_fat_index, di.entries_count);
            }
        private:
            const archive_entry_loaders_registry& m_loaders;
            std::shared_ptr<std::istream> m_istream;
            const std::vector<fat_entry>& m_fat;
            std::size_t m_first_index;
            std::size_t m_length;
            std::shared_ptr<dependencies_cache> m_cache;
        };
    }
    // ============================================================================================================
    base_archive_data_loader::base_archive_data_loader(std::string phys_name,
                                                       std::unique_ptr<archive_entry_loaders_registry> loaders)
            : physical_data_loader(std::move(phys_name)),
              m_loaders(std::move(loaders))
    {

    }
    // ------------------------------------------------------------------------------------------------------------
    void base_archive_data_loader::open(std::shared_ptr<provider::physfs::directory> dir)
    {
        auto istream = dir->open_file(physical_name());
        fat_builder builder;
        load_fat(*istream, builder);
        auto fat_info = builder.build();
        m_fat = std::move(std::get<0>(fat_info));
        auto sz = std::get<1>(fat_info);
        std::shared_ptr<std::istream> shared_stream(std::move(istream));
        auto cache = std::make_shared<detail::dependencies_cache>(*m_loaders, shared_stream, m_fat);
        m_root = std::make_shared<detail::archive_directory>(*m_loaders, shared_stream, m_fat, cache, 0, sz);
    }
    // ------------------------------------------------------------------------------------------------------------
    std::size_t base_archive_data_loader::size() const
    {
        return m_root->size();
    }
    // ------------------------------------------------------------------------------------------------------------
    provider::file_content_t base_archive_data_loader::load(std::size_t index) const
    {
        return m_root->load(index);
    }
    // ------------------------------------------------------------------------------------------------------------
    provider::file_type_t base_archive_data_loader::type(std::size_t index) const
    {
        return m_root->type(index);
    }
    // ------------------------------------------------------------------------------------------------------------
    std::string base_archive_data_loader::name(std::size_t index) const
    {
        return m_root->name(index);
    }
    // ------------------------------------------------------------------------------------------------------------
    bool base_archive_data_loader::is_directory(std::size_t index) const
    {
        return m_root->is_directory(index);
    }
    // ------------------------------------------------------------------------------------------------------------
    std::shared_ptr<abstract_directory> base_archive_data_loader::load_directory(std::size_t index) const
    {
        return m_root->load_directory(index);
    }
    // ------------------------------------------------------------------------------------------------------------
    archive_entry_loaders_registry& base_archive_data_loader::loaders()
    {
        return *m_loaders;
    }
    // ------------------------------------------------------------------------------------------------------------
    const archive_entry_loaders_registry& base_archive_data_loader::loaders() const
    {
        return *m_loaders;
    }
}
