//
// Created by igor on 22/02/2021.
//

#include <iostream>
#include <iterator>

#include <tomb-excavator/provider/vfs_registry.hh>
#include <tomb-excavator/export/file_type_to_string.hh>
#include <tomb-excavator/bsw/exceptions.hh>

#include "params.hh"


class cmd_handler
{
public:
    void operator()(const show_modules& m);
    void operator()(const list_files& m);
private:
    static std::unique_ptr<provider::vfs_registry> create_registry (const basic_params& m);
    static provider::physfs::directory open_fs_dir(const basic_module_op& m);
    static std::tuple<std::unique_ptr<provider::vfs::directory>, std::optional<std::size_t>>
    get_fs_entry(std::unique_ptr<provider::vfs::directory>&& root, const std::filesystem::path& vfs_path);
};
// ----------------------------------------------------------------------------------------------
std::unique_ptr<provider::vfs_registry> cmd_handler::create_registry (const basic_params& m)
{
    if (m.modules_dir)
    {
        return std::make_unique<provider::vfs_registry>(std::initializer_list<std::filesystem::path>{*m.modules_dir});
    }

    return std::make_unique<provider::vfs_registry>();
}
// ----------------------------------------------------------------------------------------------
provider::physfs::directory cmd_handler::open_fs_dir(const basic_module_op& m)
{
    if (!std::filesystem::is_directory(m.input_dir))
    {
        RAISE_EX(m.input_dir, "is not directory");
    }
    return provider::physfs::directory (m.input_dir);
}
// ----------------------------------------------------------------------------------------------
std::tuple<std::unique_ptr<provider::vfs::directory>, std::optional<std::size_t>>
cmd_handler::get_fs_entry(std::unique_ptr<provider::vfs::directory>&& root, const std::filesystem::path& vfs_path)
{
    std::unique_ptr<provider::vfs::directory> vfs_dir = std::move(root);
    std::vector <std::string> components;
    std::copy(vfs_path.begin(), vfs_path.end(), std::back_inserter(components));
    std::size_t start = 0;

    std::string sep;
    sep += std::filesystem::path::preferred_separator;

    if (!components.empty() && components[0] == sep)
    {
        start = 1;
    }
    std::optional<std::size_t> entry_idx;

    for (auto i = start; i < components.size(); i++)
    {
        auto vfs_name = components[i];
        bool component_found = false;
        for (std::size_t j = 0; j<vfs_dir->entries(); j++)
        {
            if (vfs_dir->name(j) == vfs_name)
            {
                if (vfs_dir->is_directory(j))
                {
                    vfs_dir = vfs_dir->load_directory(j);
                    component_found = true;
                    break;
                }
                else
                {
                    if (i + 1 == components.size())
                    {
                        component_found = true;
                        entry_idx = i;
                    }
                    else
                    {
                        return {nullptr, std::nullopt};
                    }
                }
            }
        }
        if (!component_found)
        {
            return {nullptr, std::nullopt};
        }
    }
    return {std::move(vfs_dir), entry_idx};
}
// ----------------------------------------------------------------------------------------------
void cmd_handler::operator()(const show_modules& m)
{
    auto registry = create_registry(m);

    registry->visit([](const auto& path, auto fs)
    {
        std::cout << path << "\t" << fs->name() << std::endl;
    });
}
// ----------------------------------------------------------------------------------------------
void cmd_handler::operator()(const list_files& m)
{
    auto registry = create_registry(m);
    auto dir = open_fs_dir(m);
    auto fs = registry->get(dir);
    if (!fs)
    {
        RAISE_EX("No registered provider can handle ", m.input_dir);
    }
    auto [vfs_dir, entry_index] = get_fs_entry(fs->root(dir), m.vfs_path);
    if (!vfs_dir)
    {
        RAISE_EX("vfs path ", m.vfs_path, " not found in provider ", fs->name());
    }
    if (!entry_index)
    {
        for (std::size_t eidx = 0; eidx < vfs_dir->entries(); eidx++)
        {
            std::cout << vfs_dir->name(eidx);
            if (vfs_dir->is_directory(eidx))
            {
                std::cout << "\t" << "[DIR]";
            }
            std::cout << std::endl;
        }
    }
    else
    {
        std::cout << vfs_dir->name(*entry_index) << std::endl;
    }
}
// ----------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    try
    {
        auto cmd = build_from_command_line(argc, argv);
        std::visit(cmd_handler{}, cmd);
    }
    catch (std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
