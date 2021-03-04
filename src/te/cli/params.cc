//
// Created by igor on 02/03/2021.
//

#include "params.hh"

#include <argagg/argagg.hpp>

constexpr const char* HELP = "help";
constexpr const char* LIST = "list";
constexpr const char* PATH_TO_MODULES = "PATH_TO_MODULES";

static argagg::parser define_parser()
{
    return {{
                    {HELP, {"-h", "--help"},
                            "shows this help message", 0},
                    {LIST, {"-l", "--list"},
                            "list modules", 0},
                    {PATH_TO_MODULES, {"-p", "--path-to-modules"},
                            "path", 1},
            }};
}

params_t build_from_command_line(int argc, char* argv[])
{
    auto parser = define_parser();
    argagg::parser_results args = parser.parse(argc, argv);

    if (args[LIST])
    {
        show_modules res;
        if (args[PATH_TO_MODULES])
        {
            res.modules_dir = args[PATH_TO_MODULES].as<std::string>();
        }
        return res;
    }
    throw std::runtime_error("Should not be here");
}
