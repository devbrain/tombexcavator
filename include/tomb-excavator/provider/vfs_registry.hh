//
// Created by igor on 21/02/2021.
//

#ifndef TOMBEXCAVATOR_VFS_REGISTRY_HH
#define TOMBEXCAVATOR_VFS_REGISTRY_HH

#include <memory>
#include <list>
#include <initializer_list>
#include <filesystem>

#include <tomb-excavator/export-provider.h>
#include <tomb-excavator/provider/vfs/file_system.hh>
#include <tomb-excavator/msvc/c4251-begin.h>
namespace provider
{
    class PROVIDER_API vfs_registry
    {
    public:
        vfs_registry();
        explicit vfs_registry(std::initializer_list<std::filesystem::path> places);
        ~vfs_registry();

        std::shared_ptr<vfs::file_system> get(const physfs::directory& dir);
    private:
        void _load(const std::filesystem::path& path);
    private:
        struct fs_holder
        {
            fs_holder(vfs::file_system* fs, void* so, const std::filesystem::path& so_name)
            : m_fs(fs),
            m_so(so),
            m_so_name(so_name)
            {}

            std::shared_ptr<vfs::file_system> m_fs;
            void* m_so;
            std::filesystem::path m_so_name;
        };
        std::list<fs_holder> m_modules;
    };
}
#include <tomb-excavator/msvc/c4251-end.h>
#endif //TOMBEXCAVATOR_VFS_REGISTRY_HH
