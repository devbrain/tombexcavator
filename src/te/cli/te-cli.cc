//
// Created by igor on 22/02/2021.
//

#include <iostream>
#include <tomb-excavator/provider/vfs_registry.hh>

#include "params.hh"


class cmd_handler
{
public:
    void operator()(const show_modules& m);
private:
    static std::unique_ptr<provider::vfs_registry> create_registry (const basic_params& m);
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
void cmd_handler::operator()(const show_modules& m)
{
    auto registry = create_registry(m);

    registry->visit([](const auto& path, auto fs)
    {
        std::cout << path << "\t" << fs->name() << std::endl;
    });
}
// ----------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{

    provider::vfs_registry registry;
    /*
    provider::physfs::directory dir("/home/igor/games/CAVES/");
    auto fs = registry.get(dir);
    if (fs)
    {
        std::cout << fs->name() << std::endl;
        auto root = fs->root(dir);
        auto data = root->open_file(0);
        auto w = std::get<provider::dto::sprite_group>(data);
        provider::sprite_group_to_png_export v("cc.png");
        v(w);

    }
    else
    {
        std::cout << "No supported games found" << std::endl;
    }
     */
    auto cmd = build_from_command_line(argc, argv);
    std::visit(cmd_handler{}, cmd);
    return 0;
}
