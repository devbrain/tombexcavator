/**
 * @file   file.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Sun Dec 27 11:06:27 2009
 * 
 * @brief  Definition of the file_c class.
 * 
 */

#ifndef __ABL_FS_FILE_HPP__
#define __ABL_FS_FILE_HPP__


#include "abl/abl_export.h"
#include "abl/date_time/time_stamp.hpp"
#include "abl/fs/file_impl.hpp"

#include <vector>
#include <string>


namespace abl 
{


  class path_c;
  /**
   * The file_c class provides methods for working with a file.
   */
  class ABL_API file_c: private file_impl_c
  {
  public:
    typedef FileSizeImpl file_size_t;
    /**
     * Creates the file.
     */
    file_c ();
    /**	
     * Creates the file.
     */
    file_c (const std::string& path);
    /**
     * Creates the file.
     */
    file_c (const char* path);
    /**	
     * Creates the file.
     */
    file_c (const path_c& path);
    /**	
     * Copy constructor.
     */
    file_c (const file_c& file);
    /**	
     * Destroys the file.
     */
    virtual ~file_c ();
    /**		
     * Assignment operator.	
     *
     file_c& operator =  (const file_c& file);
     /**
     * Assignment operator.
     */
    file_c& operator =  (const std::string& path);
    /**
     * Assignment operator.
     */
    file_c& operator =  (const char* path);
    /**
     * Assignment operator.
     */
    file_c& operator =  (const file_c& file);
    /**
     * Assignment operator.
     */
    file_c& operator =  (const path_c& path);
    /**
     * Swaps the file with another one.
     */
    void swap (file_c& file);
    /**	
     * Returns the path.
     */
    const std::string& path () const;
    /**	
     * Returns true iff the file exists.
     */
    bool exists () const;
    /**	
     * Returns true iff the file is readable.
     */
    bool can_read () const;
    /**	
     * Returns true iff the file is writeable.
     */
    bool can_write () const;
    /**	
     * Returns true iff the file is executable.
     *
     * On Windows and OpenVMS, the file must have
     * the extension ".EXE" to be executable.
     * On Unix platforms, the executable permission
     * bit must be set.
     */
    bool can_execute () const;
    /**	
     * Returns true iff the file is a regular file.
     */
    bool is_file () const;
    /**	
     * Returns true iff the file is a symbolic link.
     */
    bool is_link () const;
    /**		
     * Returns true iff the file is a directory.
     */
    bool is_directory () const;
    /**	
     * Returns true iff the file is a device.
     */
    bool is_device () const;
    /**	
     * Returns true if the file is hidden.
     *
     * On Windows platforms, the file's hidden
     * attribute is set for this to be true.
     *
     * On Unix platforms, the file name must
     * begin with a period for this to be true.
     */
    bool is_hidden () const;
    /**	
     * Returns the creation date of the file.
     *
     * Not all platforms or filesystems  (e.g. Linux and most Unix
     * platforms with the exception of FreeBSD and Mac OS X)
     * maintain the creation date of a file.
     * On such platforms, created () returns
     * the time of the last inode modification.
     */
    time_stamp_c created () const;
    /**	
     * Returns the modification date of the file.
     */
    time_stamp_c get_last_modified () const;
    /**	
     * Sets the modification date of the file.
     */
    void set_last_modified (const time_stamp_c& ts);
    /**	
     * Returns the size of the file in bytes.
     */
    file_size_t get_size () const;
    /**	
     * Sets the size of the file in bytes. Can be used
     * to truncate a file.
     */
    void set_size (file_size_t size);
    /**	
     * Makes the file writeable  (if flag is true), or
     * non-writeable  (if flag is false) by setting the
     * file's flags in the filesystem accordingly.
     */
    void set_writeable (bool flag = true);
    /**	
     * Makes the file non-writeable  (if flag is true), or
     * writeable  (if flag is false) by setting the
     * file's flags in the filesystem accordingly.
     */
    void set_read_only (bool flag = true);
    /**	
     * Makes the file executable  (if flag is true), or
     * non-executable  (if flag is false) by setting
     * the file's permission bits accordingly.
     *
     * Does nothing on Windows and OpenVMS.	
     */
    void set_executable (bool flag = true);
    /**	
     * Copies the file  (or directory) to the given path. 
     * The target path can be a directory.
     *
     * A directory is copied recursively.
     */
    void copy_to (const std::string& path) const;
    /**	
     * Copies the file  (or directory) to the given path and 
     * removes the original file. The target path can be a directory.
     */

    void move_to (const std::string& path);
    /**	
     * Renames the file to the new name.
     */
    void rename_to (const std::string& path);
    /**		
     * Deletes the file. If recursive is true and the
     * file is a directory, recursively deletes all
     * files in the directory.
     */
    void remove (bool recursive = false);
    /**	
     * Creates a new, empty file in an atomic operation.
     * Returns true if the file has been created and false
     * if the file already exists. Throws an exception if
     * an error occurs.
     */
    bool create_file ();
    /**	
     * Creates a directory. Returns true if the directory
     * has been created and false if it already exists.
     * Throws an exception if an error occurs.
     */
    bool create_directory ();
    /**	
     * Creates a directory  (and all parent directories
     * if necessary).
     */
    void create_directories ();
    /**	
     * Fills the vector with the names of all
     * files in the directory.
     */
    void list (std::vector<std::string>& files) const;
    /**	
     * Fills the vector with the names of all
     * files in the directory.
     */
    void list (std::vector<file_c>& files) const;


    bool operator ==  (const file_c& file) const;
    bool operator !=  (const file_c& file) const;
    bool operator <   (const file_c& file) const;
    bool operator <=  (const file_c& file) const;
    bool operator >   (const file_c& file) const;
    bool operator >=  (const file_c& file) const;
    /**	
     * For internal use only. Throws an appropriate
     * exception for the last file-related error.
     */
    static void handle_last_error (const std::string& path);


  protected:
    /**	
     * Copies a directory. Used internally by copyTo ().
     */
    void _copy_directory (const std::string& path) const;

  };
  void swap (file_c& f1, file_c& f2);


} 


#endif 
