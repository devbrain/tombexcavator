#include <iostream>

#include <bsw/warn/push.hh>
#if defined(_MSC_VER)
#pragma warning(disable : 4244 4456 4100)
#endif
#include <linenoise/linenoise.hpp>
#include <bsw/warn/pop.hh>
#include <vfs/api/system.hh>
#include <bsw/whereami.hh>
#include <bsw/logger/logger.hh>

#include "thirdparty/docopt/docopt.h"
#include "eval.hh"

const auto path = "history.txt";

static const char USAGE[] =
        R"(Naval Fate.

    Usage:
      naval_fate ship new <name>...
      naval_fate ship <name> move <x> <y> [--speed=<kn>]
      naval_fate ship shoot <x> <y>
      naval_fate mine (set|remove) <x> <y> [--moored | --drifting]
      naval_fate (-h | --help)
      naval_fate --version

    Options:
      -h --help     Show this screen.
      --version     Show version.
      --speed=<kn>  Speed in knots [default: 10].
      --moored      Moored (anchored) mine.
      --drifting    Drifting mine.
)";
int main(int argc, char* argv[])
{

    auto my_bin_dir = core::get_exec_path().parent_path();
    std::cout << my_bin_dir << std::endl;
    vfs::load_module(my_bin_dir);

    evaluator::instance().eval("list_modules()");


    // Setup completion words every time when a user types
    linenoise::SetCompletionCallback([](const char* editBuffer, std::vector<std::string>& completions) {
        if (editBuffer[0] == 'h') {
            completions.push_back("hello");
            completions.push_back("hello there");
        }
    });

    // Enable the multi-line mode
    linenoise::SetMultiLine(true);

// Set max length of the history
    linenoise::SetHistoryMaxLen(4);

// Load history
    linenoise::LoadHistory(path);

    while (true) {
        // Read line
        std::string line;
        auto quit = linenoise::Readline("hello> ", line);

        if (quit) {
            break;
        }

        std::cout <<  "echo: '" << line << "'" << std::endl;

        // Add text to history
        linenoise::AddHistory(line.c_str());
    }

// Save history
    linenoise::SaveHistory(path);

    EVLOG_TRACE(EVLOG_ERROR, "Zopa", 1);


    vfs::deinitialize();
    return 0;
}
