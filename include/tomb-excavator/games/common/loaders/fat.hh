//
// Created by igor on 31/03/2021.
//

#ifndef TOMBEXCAVATOR_FAT_HH
#define TOMBEXCAVATOR_FAT_HH

#include <cstdint>
#include <map>
#include <set>
#include <any>
#include <vector>
#include <memory>
#include <tuple>

#include <tomb-excavator/export-games-common.h>
#include <tomb-excavator/provider/file_types.hh>
#include <tomb-excavator/bsw/exceptions.hh>

#include <tomb-excavator/msvc/c4251-begin.h>

namespace games::common
{

    namespace detail
    {
        template<typename Key>
        class props_map
        {
        public:
            using key_t = Key;
            using props_map_t = std::map<Key, std::any>;
            using init_list = std::initializer_list<typename props_map_t::value_type>;
            using map_keys_t = std::set<key_t>;
        public:
            props_map() = default;

            props_map(init_list ilist);

            template <typename T>
            void put(Key key, const T& value);

            template <typename T>
            T get(Key key) const;

            [[nodiscard]] bool exists(Key key) const;

            template<typename T>
            [[nodiscard]] bool has_type(Key key) const;
        private:
            props_map_t m_props;
        };
    } // ns detail

    class GAMES_COMMON_API fat_builder;
    class GAMES_COMMON_API fat_entry
    {
        friend class fat_builder;
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

        using props_map_t = detail::props_map<int>;
        using dependencies_t = props_map_t::map_keys_t;
    public:
        fat_entry(std::string  name, file_info fileinfo);
        fat_entry(std::string  name, dir_info dirinfo);
        fat_entry(std::string  name, file_info fileinfo, props_map_t props);
        fat_entry(std::string  name, file_info fileinfo, props_map_t props, dependencies_t deps);
        fat_entry(std::string  name, dir_info dirinfo, props_map_t props);

        [[nodiscard]] std::string name() const noexcept;
        [[nodiscard]] bool is_directory() const noexcept;
        [[nodiscard]] dir_info get_dir_info() const;
        [[nodiscard]] file_info get_file_info() const;
        [[nodiscard]] const props_map_t& get_props() const noexcept;
        [[nodiscard]] const dependencies_t& get_deps() const noexcept;
    private:
        std::string m_name;
        std::variant<dir_info, file_info> m_info;
        props_map_t m_props;
        dependencies_t m_deps;
    };

    using loaded_deps_t = std::map<fat_entry::props_map_t::key_t, std::shared_ptr<provider::file_content_t>>;
    using loader_context_t = std::tuple<fat_entry::props_map_t, loaded_deps_t>;

    struct GAMES_COMMON_API fat_events
    {
        virtual ~fat_events();

        virtual void add_file(std::string name, fat_entry::file_info fi, fat_entry::props_map_t props) = 0;
        virtual void add_file(std::string name, fat_entry::file_info fi,
                              fat_entry::props_map_t props, fat_entry::dependencies_t deps) = 0;
        virtual void add_file(std::string name, fat_entry::file_info fi) = 0;

        virtual void start_dir(std::string name, fat_entry::props_map_t props) = 0;
        virtual void start_dir(std::string name) = 0;
        virtual void end_dir() = 0;
    };

    class GAMES_COMMON_API fat_builder : public fat_events
    {
    public:
        fat_builder();
        ~fat_builder() override;

        fat_builder(const fat_builder&) = delete;
        fat_builder& operator = (const fat_builder&) = delete;

        void add_file(std::string name, fat_entry::file_info fi, fat_entry::props_map_t props) override;
        void add_file(std::string name, fat_entry::file_info fi,
                              fat_entry::props_map_t props, fat_entry::dependencies_t deps) override;
        void add_file(std::string name, fat_entry::file_info fi) override;

        void start_dir(std::string name, fat_entry::props_map_t props) override;
        void start_dir(std::string name) override;
        void end_dir() override;

        // fat array, size of root dir
        [[nodiscard]] std::tuple<std::vector<fat_entry>, std::size_t> build() const;
    private:
        struct impl;
        std::unique_ptr<impl> m_pimpl;
    };
    // ======================================================================================
    // Implementation
    // ======================================================================================
    namespace detail
    {
        template<typename Key>
        props_map<Key>::props_map(init_list ilist)
        : m_props{ilist}
        {

        }
        // --------------------------------------------------------------------------------
        template<typename Key>
        template<typename T>
        void props_map<Key>::put(Key key, const T& value)
        {
            m_props.insert({key, value});
        }
        // --------------------------------------------------------------------------------
        template<typename Key>
        template<typename T>
        T props_map<Key>::get(Key key) const
        {
            auto itr = m_props.find(key);
            if (itr == m_props.end())
            {
                RAISE_EX("Key ", key, " not found");
            }
            auto x = std::any_cast<T>(const_cast<std::any*>(&itr->second));
            if (!x)
            {
                RAISE_EX("Key ", key, " stores ", itr->second.type().name(), " but cast to ", typeid(T).name(), " requested");
            }
            return *x;
        }
        // --------------------------------------------------------------------------------
        template<typename Key>
        bool props_map<Key>::exists(Key key) const
        {
            return (m_props.find(key) != m_props.end());
        }
        // --------------------------------------------------------------------------------
        template<typename Key>
        template<typename T>
        bool props_map<Key>::has_type(Key key) const
        {
            auto itr = m_props.find(key);
            if (itr == m_props.end())
            {
                RAISE_EX("Key ", key, " not found");
            }
            const T* x = std::any_cast<T>(&itr->second);
            return x != nullptr;
        }
    } // ns detail
}

#include <tomb-excavator/msvc/c4251-end.h>

#endif //TOMBEXCAVATOR_FAT_HH
