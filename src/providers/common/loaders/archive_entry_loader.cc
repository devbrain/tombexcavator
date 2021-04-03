//
// Created by igor on 02/04/2021.
//

#include <tomb-excavator/games/common/loaders/archive_entry_loader.hh>
#include <tomb-excavator/bsw/exceptions.hh>
#include <istream>
#include <utility>

namespace games::common
{
    archive_entry_loader::archive_entry_loader(name_acceptor_t name_acceptor,
                         provider::file_type_t type,
                         entry_reader_t<provider::file_content_t> loader)
                         :accept_fn(std::move(name_acceptor)),
                         m_type(type),
                         reader_fn(std::move(loader))
    {

    }
    // -----------------------------------------------------------------------------------------------------------
    bool archive_entry_loader::accept(const std::string& name) const
    {
        return accept_fn(name);
    }
    // -----------------------------------------------------------------------------------------------------------
    provider::file_type_t archive_entry_loader::get_file_type() const
    {
        return m_type;
    }
    // -----------------------------------------------------------------------------------------------------------
    provider::file_content_t archive_entry_loader::read(std::istream& is, uint64_t offset,
                                                        std::size_t size, const fat_entry::props_map_t& props) const
    {
        return reader_fn(is, offset, size, props);
    }
    // =================================================================================================================
    archive_entry_loaders_registry::archive_entry_loaders_registry(std::initializer_list<archive_entry_loader> inits)
    : m_loaders{inits}
    {

    }
    // ----------------------------------------------------------------------------------------------------------------
    archive_entry_loaders_registry::~archive_entry_loaders_registry() = default;
    // ----------------------------------------------------------------------------------------------------------------
    void archive_entry_loaders_registry::add(archive_entry_loader loader)
    {
        m_loaders.push_back(std::move(loader));
    }
    // ----------------------------------------------------------------------------------------------------------------
    std::optional<std::tuple<int, provider::file_type_t>>
    archive_entry_loaders_registry::get_file_metadata(const std::string& name) const
    {
        for (std::size_t i=0; i<m_loaders.size(); i++)
        {
            if (m_loaders[i].accept(name))
            {
                return std::make_tuple(static_cast<int>(i), m_loaders[i].get_file_type());
            }
        }
        return std::nullopt;
    }
    // ----------------------------------------------------------------------------------------------------------------
    provider::file_content_t archive_entry_loaders_registry::read(std::istream& is, const fat_entry::file_info& fi,
                                                                  const fat_entry::props_map_t& props) const
    {
        if (fi.internal_type >= m_loaders.size())
        {
            RAISE_EX("Invalid internal type");
        }
        return m_loaders[fi.internal_type].read(is, fi.offset, fi.size, props);
    }
    // ===============================================================================================================
    std::vector<char> simple_loaders_registry::load_as_vector(std::istream& is,
                                     uint64_t offset,
                                     std::size_t size)
    {
        std::vector<char> out(size);

        const auto current = is.tellg();
        is.seekg(offset, std::ios::beg);
        is.read(out.data(), out.size());
        is.seekg(current, std::ios::beg);
        return out;
    }
    // --------------------------------------------------------------------------------------------------------------
    static bool null_acceptor([[maybe_unused]] const std::string& name)
    {
        return false;
    }
    // --------------------------------------------------------------------------------------------------------------
    static std::vector<char> load_as_vector_wrapper(std::istream& is,uint64_t offset,
                                                    std::size_t size,
                                                    const fat_entry::props_map_t& props)
    {
        return simple_loaders_registry::load_as_vector(is, offset, size);
    };
    // --------------------------------------------------------------------------------------------------------------
    simple_loaders_registry::simple_loaders_registry(std::initializer_list<archive_entry_loader> inits)
    : archive_entry_loaders_registry(inits),
      m_fallback (make_entry_loader(null_acceptor,load_as_vector_wrapper))
    {
        m_fallback_id = inits.size();
    }
    // ---------------------------------------------------------------------------------------------------------------
    [[nodiscard]] std::optional<std::tuple<int, provider::file_type_t>>
    simple_loaders_registry::get_file_metadata(const std::string& name) const
    {
        auto ret = archive_entry_loaders_registry::get_file_metadata(name);
        if (ret)
        {
            return ret;
        }
        return std::make_tuple(m_fallback_id, m_fallback.get_file_type());
    }
}