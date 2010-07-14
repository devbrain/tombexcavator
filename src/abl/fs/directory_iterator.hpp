/**
 * @file   directory_iterator.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Sun Dec 27 12:23:14 2009
 * 
 * @brief  Definition of the directory_iterator_c class.
 * 
 */

#ifndef __ABL_FS_DIRECTORY_ITERATOR_HPP__
#define __ABL_FS_DIRECTORY_ITERATOR_HPP__


#include "abl/abl_export.h"
#include "abl/fs/file.hpp"
#include "abl/fs/path.hpp"


namespace abl 
{


  class directory_iterator_impl_c;
  /**
   * The directory_iterator_c class is used to enumerate
   * all files in a directory.
   *
   * directory_iterator_c has some limitations:
   *   * only forward iteration (++) is supported
   *   * an iterator copied from another one will always
   *     point to the same file as the original iterator,
   *     even is the original iterator has been advanced
   *     (all copies of an iterator share their state with
   *     the original iterator)
   *   * because of this you should only use the prefix 
   *     increment operator
   */
  class ABL_API directory_iterator_c
  {
  public:
    /**
     * Creates the end iterator.
     */
    directory_iterator_c();
    /**
     * Creates a directory iterator for the given path.		
     */
    directory_iterator_c(const std::string& path);
    /**
     * Creates a directory iterator for the given path.
     */
    directory_iterator_c(const directory_iterator_c& iterator);
    /**
     * Creates a directory iterator for the given file.		
     */
    directory_iterator_c(const file_c& file);
    /**
     * Creates a directory iterator for the given path.
     */
    directory_iterator_c(const path_c& path);
    /**
     * Destroys the directory_iterator_c.
     */
    ~directory_iterator_c();
    /**
     * Returns the current filename.
     */
    const std::string& name() const;
    /**
     * Returns the current path.		
     */
    const path_c& path() const;


    directory_iterator_c& operator = (const directory_iterator_c& it);
    directory_iterator_c& operator = (const file_c& file);
    directory_iterator_c& operator = (const path_c& path);
    directory_iterator_c& operator = (const std::string& path);
	
    directory_iterator_c& operator ++ ();   // prefix
	
	
    const file_c& operator * () const;
    file_c& operator * ();
    const file_c* operator -> () const;
    file_c* operator -> ();
	
    bool operator == (const directory_iterator_c& iterator) const;
    bool operator != (const directory_iterator_c& iterator) const;

  private:
    path_c m_path;
    file_c m_file;
    directory_iterator_impl_c* m_pimpl;
  };
} 


#endif 
