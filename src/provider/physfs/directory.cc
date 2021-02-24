//
// Created by igor on 21/02/2021.
//

#include <cctype>
#include <algorithm>
#include <fstream>
#include <tomb-excavator/provider/physfs/directory.hh>
#include <tomb-excavator/bsw/exceptions.hh>

static std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                    [](unsigned char c){ return (unsigned char)std::tolower(c); }
                  );
    return s;
}
// =====================================================================================
namespace provider::physfs
{
    directory::~directory() = default;
    // ---------------------------------------------------------------------------------
    directory::directory(const std::filesystem::path& path)
    : m_root(path)
    {
        for(auto& p: std::filesystem::directory_iterator(m_root))
        {
            m_entries.emplace_back(str_tolower(p.path().filename().u8string()));
            m_entries_real.emplace_back(p.path().filename().u8string());
        }
    }
    // ---------------------------------------------------------------------------------
    std::size_t directory::entries() const
    {
        return m_entries.size();
    }
    // ---------------------------------------------------------------------------------
    std::string directory::name(const std::size_t idx) const
    {
        if (idx < entries())
        {
            return m_entries[idx];
        }
        RAISE_EX("Entry index ", idx, " > total entries ", entries());
    }
    // ---------------------------------------------------------------------------------
    bool directory::contains(const std::string& name) const
    {
        return std::find(m_entries.begin(), m_entries.end(), str_tolower(name)) != m_entries.end();
    }
    // ---------------------------------------------------------------------------------
    bool directory::is_directory(const std::string& name) const
    {
        return is_directory(_get_real_index(name));
    }
    // ---------------------------------------------------------------------------------
    bool directory::is_directory(std::size_t idx) const
    {
        auto path = m_root / m_entries_real[idx];
        return std::filesystem::is_directory(path);
    }
    // ---------------------------------------------------------------------------------
    std::unique_ptr<directory> directory::open_directory(const std::string& name) const
    {
        return open_directory(_get_real_index(name));
    }
    // ---------------------------------------------------------------------------------
    std::unique_ptr<directory> directory::open_directory(std::size_t idx) const
    {
        auto path = m_root / m_entries_real[idx];
        return std::make_unique<directory>(path);
    }
    // ---------------------------------------------------------------------------------
    std::unique_ptr<std::istream> directory::open_file(const std::string& name) const
    {
        return open_file(_get_real_index(name));
    }
    // ---------------------------------------------------------------------------------
    std::unique_ptr<std::istream> directory::open_file(std::size_t idx) const
    {
        auto path = m_root / m_entries_real[idx];
        std::unique_ptr<std::istream> ret (new std::ifstream(path, std::ios::binary | std::ios::in));
        if (!*ret)
        {
            RAISE_EX("Can not open file ", path);
        }
        return ret;
    }
    // ---------------------------------------------------------------------------------
    std::size_t directory::_get_real_index(const std::string& name) const
    {
        auto itr = std::find(m_entries.begin(), m_entries.end(), str_tolower(name));
        if (itr == m_entries.end())
        {
            RAISE_EX("Can not find ", name, " in ", m_root);
        }
        return itr - m_entries.begin();
    }
}
