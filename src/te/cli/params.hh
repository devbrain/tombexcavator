//
// Created by igor on 02/03/2021.
//

#ifndef TOMBEXCAVATOR_PARAMS_HH
#define TOMBEXCAVATOR_PARAMS_HH

#include <variant>
#include <optional>
#include <filesystem>

struct basic_params
{
    std::optional<std::filesystem::path> modules_dir;
};

struct show_modules : public basic_params
{
};

struct basic_module_op : public basic_params
{
    std::filesystem::path input_dir;
};

struct list_files : public basic_module_op
{
    std::string vfs_path;
};

using params_t = std::variant<show_modules, list_files>;

params_t build_from_command_line(int argc, char* argv[]);



#endif //TOMBEXCAVATOR_PARAMS_HH
