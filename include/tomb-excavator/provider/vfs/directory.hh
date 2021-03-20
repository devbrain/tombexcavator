//
// Created by igor on 21/02/2021.
//

#ifndef TOMBEXCAVATOR_DIRECTORY_HH
#define TOMBEXCAVATOR_DIRECTORY_HH

#include <tomb-excavator/export-provider.h>
#include <tomb-excavator/provider/file_types.hh>

#include <string>
#include <memory>


namespace provider::vfs
{
    class PROVIDER_API file_system;
    class PROVIDER_API directory
    {
        friend class PROVIDER_API file_system;
    public:
        explicit directory(file_system* owner);
        virtual ~directory();

        virtual std::size_t entries() const = 0;
        virtual std::string name(std::size_t entry_idx) const = 0;
        virtual bool is_directory(std::size_t entry_idx) const = 0;

        virtual std::unique_ptr<directory> load_directory(std::size_t entry_idx) const = 0;
        virtual file_type_t open_file(std::size_t entry_idx) const = 0;
    protected:
        file_system* owner();
    private:
        file_system* m_owner;
    };
} // ns

#endif //TOMBEXCAVATOR_DIRECTORY_HH
