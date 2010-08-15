#include "archive/c_archive_adaptor.hpp"

namespace archive
{
  class carchive_adaptor_c
  {
  public:
    carchive_adaptor_c (archive_s* ar);
    virtual ~carchive_adaptor_c ();

    virtual bool is_read_only () const;

    virtual void open (const char* path);
    virtual void enum_files (const char* path, 
			     bool omit_links, 
			     abstract_files_visitor_c* visitor) const;

    virtual bool exists  (const std::string& path) const;
    virtual bool is_file (const std::string& path) const;
    virtual bool is_dir  (const std::string& path) const;
    virtual bool is_link (const std::string& path) const;

    virtual bool remove (const std::string& path);
    virtual bool mkdir  (const std::string& path);

    virtual abstract_file_c* open (const std::string& path, 
				   bool for_write);

    virtual abstract_file_c* create (const std::string& path);
  private:
    archive_s* m_ar;
  };
} // ns archive


