#include <iostream>

#include <bsw/filesystem.hh>
#include <bsw/whereami.hh>
#include <vfs/api/system.hh>
#include <thirdparty/docopt/docopt.h>

static void extract_helper(const std::string& src, const std::string& dst)
{
    for (const auto& f : vfs::open_directory(src))
    {
        vfs::stats st;
        std::string name;
        std::tie(name, st) = f;
        std::string in_name = src +"/" +name;
        std::string o_name = dst + "/" +name;
        auto sts = vfs::get_stats(in_name);
        if (sts->type == vfs::stats::eFILE)
        {
            std::cout << "Extracting " << in_name << std::endl;
            vfs::file* in_file = vfs::open(in_name, vfs::creation_disposition::eOPEN_EXISTING, true);
            vfs::file* out_file = vfs::open(o_name, vfs::creation_disposition::eCREATE_ALWAYS, false);
            size_t read = 0;
            while (read < sts->size)
            {

                static constexpr std::size_t sz = 8192;
                char buff[sz];
                std::size_t to_read = sz;
                if (read + to_read > sts->size)
                {
                    to_read = sts->size - read;
                }
                auto has_bytes = vfs::read(in_file, buff, to_read);
                vfs::write(out_file, buff, has_bytes);
                read += has_bytes;
            }
            vfs::close(in_file);
            vfs::close(out_file);

        } else
        {
            vfs::create_directory(o_name);
            extract_helper(in_name, o_name);
        }
    }
}

static void extract(const stdfs::path& modules_path, const stdfs::path& out,
        const std::string& module, const std::string& args)
{
    vfs::load_module(modules_path);
    if (stdfs::exists(out))
    {
        std::cerr << "Phisical directory " << out << "exists";
        return;
    }
    stdfs::create_directories(out);
    vfs::mount("physfs", out.string(), "/");
    vfs::create_directory("/output");
    vfs::create_directory("/input");
    vfs::mount (module, args, "/input");

    extract_helper("/input", "/output");

}


static const char USAGE[] =
R"(extractor

    Usage:
      extractor <destination> <module> --args=<args> [--modules-dir=<path>]
)";
int main(int argc, char* argv[])
{
    std::map<std::string, docopt::value> args
            = docopt::docopt(USAGE,
                             { argv + 1, argv + argc },
                             true,               // show help if requested
                             "extractor 1.0");  // version string
    for(auto const& arg : args)
    {
        std::cout << arg.first << ":" <<  arg.second << std::endl;
    }
    auto my_dir = core::get_exec_path().parent_path();
    stdfs::path modules_dir = my_dir;
    if (args["--modules-dir"])
    {
        modules_dir = stdfs::path(args["--modules-dir"].asString());
    }
    stdfs::path output = stdfs::path(args["<destination>"].asString());
    try
    {
        extract(modules_dir, output, args["<module>"].asString(), args["--args"].asString());
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    vfs::deinitialize();
    return 0;
}
