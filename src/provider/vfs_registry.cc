//
// Created by igor on 21/02/2021.
//

#include <dlfcn.h>
#include <algorithm>
#include <tomb-excavator/provider/vfs_registry.hh>
#include <tomb-excavator/bsw/whereami.hh>


namespace provider
{
    vfs_registry::vfs_registry()
    {
        _load(bsw::get_executable_path().parent_path());
    }
    // --------------------------------------------------------------------------------------------
    vfs_registry::vfs_registry(std::initializer_list<std::filesystem::path> places)
    {
        _load(bsw::get_executable_path().parent_path());
        for (const auto& dir : places)
        {
            _load(dir);
        }
    }
    // --------------------------------------------------------------------------------------------
    vfs_registry::~vfs_registry()
    {
        for (auto& module : m_modules)
        {
            module.m_fs = nullptr;
            dlclose(module.m_so);
        }
    }
    // --------------------------------------------------------------------------------------------
    std::shared_ptr<vfs::file_system> vfs_registry::get(const physfs::directory& dir)
    {
        for (auto& module : m_modules)
        {
            if (module.m_fs->accept(dir))
            {
                return module.m_fs;
            }
        }
        return nullptr;
    }
    // --------------------------------------------------------------------------------------------
    void vfs_registry::_load(const std::filesystem::path& path)
    {
        if (!std::filesystem::is_directory(path))
        {
            return;
        }

        for (const auto& p : std::filesystem::directory_iterator(path))
        {
            const auto& file = p.path();
            if (std::filesystem::is_regular_file(p.path()))
            {
                 void* library = dlopen(file.c_str(), RTLD_LAZY);
                 if (library)
                 {
                     #define xstr(s) str(s)
                     #define str(s) #s
                     void* addr = dlsym(library, xstr(TE_PROVIDER_LOADER_NAME));
                     if (!addr)
                     {
                         dlclose(library);
                     }
                     else
                     {
                         if (std::find_if (m_modules.begin(), m_modules.end(),
                                       [&file](const auto& module){
                             return module.m_so_name == file;
                         }) == m_modules.end())
                         {
                             using loader_t = void* (*)();
                             loader_t loader = (loader_t)addr;
                             m_modules.emplace_back((vfs::file_system*)loader(), library, file);
                         }
                     }
                 }
            }
        }
    }
} // ns
