//
// Created by igor on 21/02/2021.
//

#ifndef TOMBEXCAVATOR_PHYS_FS_DIRECTORY_HH
#define TOMBEXCAVATOR_PHYS_FS_DIRECTORY_HH

#include <tomb-excavator/export-provider.h>
#include <filesystem>
#include <string>
#include <memory>
#include <iosfwd>
#include <vector>

#include <tomb-excavator/msvc/c4251-begin.h>

namespace provider::physfs
{
    class PROVIDER_API directory
    {
    public:
        explicit directory(const std::filesystem::path& path);
        ~directory();

        std::size_t entries() const;
        std::string name(const std::size_t idx) const;

        bool contains(const std::string& name) const;

        bool is_directory(const std::string& name) const;
        bool is_directory(std::size_t idx) const;

        std::unique_ptr<directory> open_directory(const std::string& name) const;
        std::unique_ptr<directory> open_directory(std::size_t idx) const;

        std::unique_ptr<std::istream> open_file(const std::string& name) const;
        std::unique_ptr<std::istream> open_file(std::size_t idx) const;
    private:
        std::size_t _get_real_index(const std::string& name) const;
    private:
        std::filesystem::path m_root;
        std::vector<std::string> m_entries;
        std::vector<std::string> m_entries_real;
    };
} // ns

#include <tomb-excavator/msvc/c4251-end.h>
#endif
