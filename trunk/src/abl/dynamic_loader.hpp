#ifndef __ABL_DYNAMIC_LOADER_HPP__
#define __ABL_DYNAMIC_LOADER_HPP__

#include <string>

namespace abl
{
  class dynamic_loader_c;
  
  struct dynamic_library_impl_s;
  /**
   * This class represents a dynamic library
   */
  class ABL_API dynamic_library_c
  {
    friend class dynamic_loader_c;
  public:
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
    void* load (const std::string& symbol);
    
    unsigned int ref_count () const;
    bool release ();
  private:
    dynamic_library_c ();
    ~dynamic_library_c ();
  private:
    dynamic_library_impl_s* m_pimpl;
  };
}


#endif

