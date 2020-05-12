#include <linenoise/linenoise.hpp>
#include <vfs/api/system.hh>
#include <bsw/logger/logger.hh>

const auto path = "history.txt";


int main(int argc, char* argv[])
{
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
