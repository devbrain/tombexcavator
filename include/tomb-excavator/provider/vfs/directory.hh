//
// Created by igor on 21/02/2021.
//

#ifndef TOMBEXCAVATOR_DIRECTORY_HH
#define TOMBEXCAVATOR_DIRECTORY_HH

#include <tombexcavator-provider_export.h>

#include <cstddef>
#include <string>
#include <memory>
#include <iosfwd>

namespace provider::vfs
{
    class PROVIDER_API directory
    {
    public:
        directory();
        virtual ~directory();

        virtual std::size_t entries() const = 0;
        virtual std::string name(std::size_t entry_idx) const = 0;
        virtual bool is_directory(std::size_t entry_idx) const = 0;
        virtual std::string type(std::size_t entry_idx) const = 0;

        virtual std::unique_ptr<directory> load_directory(std::size_t entry_idx) const = 0;
        virtual std::unique_ptr<std::istream> open_file(std::size_t entry_idx) const = 0;
    };
} // ns

#endif //TOMBEXCAVATOR_DIRECTORY_HH
