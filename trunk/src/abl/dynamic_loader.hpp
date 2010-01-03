#ifndef __ABL_DYNAMIC_LOADER_HPP__
#define __ABL_DYNAMIC_LOADER_HPP__

#include <string>
#include "abl/abl_export.h"

namespace abl
{
  class path_c;

  class dynamic_loader_c;
  
  struct dynamic_library_impl_s;
  /**
   * This class represents a dynamic library
   */
  class ABL_API dynamic_library_c
  {
    friend class dynamic_loader_c;
  public:
    dynamic_library_c ();

    ~dynamic_library_c ();

    bool load (const path_c& path, const std::string& name);
    /** 
     * returns the name of the library. For example, for /a/libX.so it returns X
     * 
     * 
     * @return base name of the shared object
     */
    std::string name () const;
    /** 
     * returns the full name of the library. For example, for /a/libX.so it returns libX.so
     * 
     * 
     * @return system dependent name of the library
     */
    std::string qualified_name () const;
    /** 
     * returns path to the library. For example, for /a/libX.so it returns /a
     * 
     * 
     * @return path where the library resides
     */
    std::string path () const;

    /** 
     * loads the symbol from the library
     * 
     * @param symbol - name of the symbol to load
     * 
     * @return pointer to the loaded symbol or NULL in case of failure
     */
    void* load_symbol (const std::string& symbol);
  private:
    dynamic_library_c ();
    ~dynamic_library_c ();
  private:
    dynamic_library_impl_s* m_pimpl;
  };
}

#endif

