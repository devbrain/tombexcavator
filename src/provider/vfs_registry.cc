//
// Created by igor on 21/02/2021.
//

#include <dlfcn.h>
#include <algorithm>
#include <tomb-excavator/provider/vfs_registry.hh>
#include <tomb-excavator/bsw/whereami.hh>
#include <tomb-excavator/bsw/macros.hh>

namespace
{
    std::filesystem::path get_self_path()
    {
        try
        {
            return  bsw::get_executable_path();
        }
        catch (std::exception&)
        {
            try
            {
                return bsw::get_module_path();
            }
            catch (std::exception& e)
            {
            }
        }
        return {};
    }
}


namespace provider
{
    vfs_registry::vfs_registry()
    {
        const auto self = get_self_path();
        if (self.empty())
        {
            RAISE_EX("Can not determine self path");
        }
        _load(self.parent_path(), self);
    }
    // --------------------------------------------------------------------------------------------
    vfs_registry::vfs_registry(std::initializer_list<std::filesystem::path> places)
    {
        const auto self = get_self_path();
        if (!self.empty())
        {
            _load(self.parent_path(), self);
        }
        for (const auto& dir : places)
        {
            _load(dir, self);
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
    void vfs_registry::visit(std::function<void(const std::filesystem::path&,
                             std::shared_ptr<vfs::file_system>)> visitor) const
    {
        for (auto& module : m_modules)
        {
            visitor(module.m_so_name, module.m_fs);
        }
    }
    // --------------------------------------------------------------------------------------------
    void vfs_registry::_load(const std::filesystem::path& path, const std::filesystem::path& self)
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
                if (file == self)
                {
                    continue;
                }
                 void* library = dlopen(file.u8string().c_str(), RTLD_LAZY);
                 if (library)
                 {
                     void* addr = dlsym(library, STRINGIZE(TE_PROVIDER_LOADER_NAME));
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
