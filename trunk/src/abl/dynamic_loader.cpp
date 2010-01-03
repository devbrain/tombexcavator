
namespace abl
{
  dynamic_library_c ();

    ~dynamic_library_c ();

    bool load (const path_c& path, const std::string& name);
    std::string name () const;
    std::string qualified_name () const;
    std::string path () const;
    void* load_symbol (const std::string& symbol);
}
