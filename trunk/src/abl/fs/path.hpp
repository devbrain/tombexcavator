/**
 * @file   path.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Tue Dec 22 13:54:50 2009
 * 
 * @brief  Definition of the path_c class.
 */

#ifndef __ABL_FS_PATH_HPP__
#define __ABL_FS_PATH_HPP__


#include "abl/abl_export.h"
#include <vector>
#include <string>

namespace abl 
{
  /**
   * This class represents filesystem paths in a 
   * platform-independent manner.
   * Unix and Windows all use a different
   * syntax for filesystem paths.
   * This class can work with all three formats.
   * A path is made up of an optional node name
   * (only Windows), an optional
   * device name (also only Windows),
   * a list of directory names and an optional
   * filename.
   */
  class ABL_API path_c
  {
  public:
    enum style_t
      {
	PATH_UNIX,    /**< Unix-style path */
	PATH_WINDOWS, /**< Windows-style path */
	PATH_NATIVE,  /**< The current platform's native style */
	PATH_GUESS    /**< Guess the style by examining the path */
      };
	
    typedef std::vector <std::string> string_vec_t;
    /**
     * Creates an empty relative path.
     */
    path_c ();
		
    /**
     * Creates an empty absolute or relative path.
     */
     path_c(bool absolute);
		
     /**
     * Creates a path from a string.
     */
    path_c (const char* path);
		
    /**
     * Creates a path from a string.
     */
    path_c (const char* path, style_t style);
		
    /**
     * Creates a path from a string.
     */
    path_c (const std::string& path);
		
    /**
     * Creates a path from a string.
     */
    path_c (const std::string& path, style_t style);
		
    /**
     * Copy constructor
     */
    path_c (const path_c& path);
		
    /**
     * Creates a path from a parent path and a filename.
     * The parent path is expected to reference a directory.
     */
    path_c (const path_c& parent, const std::string& fileName);
		
    /**
     * Creates a path from a parent path and a filename.
     * The parent path is expected to reference a directory.
     */
    path_c (const path_c& parent, const char* fileName);
		
    /**
     * Creates a path from a parent path and a relative path.
     * The parent path is expected to reference a directory.
     * The relative path is appended to the parent path.
     */
    path_c (const path_c& parent, const path_c& relative);
		
    /**
     * Destroys the path_c.
     */
    ~path_c ();
		
    /**
     * Creates a path from a parent path and a filename.
     * The parent path is expected to reference a directory.
     */
    path_c& operator = (const path_c& path);
		
    /**
     * Assigns a string containing a path in native format.
     */
    path_c& operator = (const std::string& path);
		
    /**
     * Assigns a string containing a path in native format.
     */

    path_c& operator = (const char* path);
		
    /**
     * Swaps the path with another one.
     */
    void swap (path_c& path);
		
    /**
     * Assigns a string containing a path in native format.
     */
    path_c& assign (const std::string& path);
		
    /**
     * Assigns a string containing a path.
     */
    path_c& assign (const std::string& path, style_t style);
		
    /**
     * Assigns the given path.
     */
    path_c& assign (const path_c& path);
    /**
     * Assigns a string containing a path.
     */
		
    path_c& assign (const char* path);
    /**
     * Returns a string containing the path in native format.
     */
    std::string to_string () const;
		
    /**
     * Returns a string containing the path in the given format.
     */
    std::string to_string (style_t style) const;
		
    /**
     * Same as assign().
     */
    path_c& parse (const std::string& path);
		
    /**
     * Assigns a string containing a path.
     */
    path_c& parse (const std::string& path, style_t style);
		
    /**
     * Tries to interpret the given string as a path
     * in native format.
     * If the path is syntactically valid, assigns the
     * path and returns true. Otherwise leaves the 
     * object unchanged and returns false.
     */
    bool try_parse (const std::string& path);
		
    /**
     * Tries to interpret the given string as a path,
     * according to the given style.
     * If the path is syntactically valid, assigns the
     * path and returns true. Otherwise leaves the
     * object unchanged and returns false.
     */
    bool try_parse (const std::string& path, style_t style);
		
    /**
     * The resulting path always refers to a directory and
     * the filename part is empty.
     */
    path_c& parse_directory (const std::string& path);
		
    /**
     * The resulting path always refers to a directory and
     * the filename part is empty.
     */
    path_c& parse_directory (const std::string& path, style_t style);
		
    /**
     * If the path contains a filename, the filename is appended
     * to the directory list and cleared. Thus the resulting path
     * always refers to a directory.
     */
    path_c& make_directory ();
		
    /**
     * If the path contains no filename, the last directory
     * becomes the filename.
     */
    path_c& make_file ();
		
    /**
     * Makes the path refer to its parent.
     */
    path_c& make_parent ();
		
    /**
     * Makes the path absolute if it is relative.
     * The current working directory is taken as base directory.
     */
    path_c& make_absolute ();
		
    /**
     * Makes the path absolute if it is relative.
     * The given path is taken as base. 
     */
    path_c& make_absolute (const path_c& base);
		
    /**
     * Appends the given path.
     */
    path_c& append (const path_c& path);
		
    /**
     * Resolves the given path agains the current one.
     *
     * If the given path is absolute, it replaces the current one.
     * Otherwise, the relative path is appended to the current path.
     */
    path_c& resolve (const path_c& path);
		
    /**
     * Returns true iff the path is absolute.
     */
    bool is_absolute () const;
		
    /**
     * Returns true iff the path is relative.
     */
    bool is_relative () const;
		
    /**
     * Returns true iff the path references a directory
     * (the filename part is empty).
     */
    bool is_directory () const;
		
    /**
     * Returns true iff the path references a file
     * (the filename part is not empty).
     */
    bool is_file () const;
		
    /**
     * Sets the node name.
     * Setting a non-empty node automatically makes
     * the path an absolute one.
     */
    void set_node (const std::string& node);
		
    /**
     * Returns the node name.
     */
    const std::string& get_node () const;
		
    /**
     * Sets the device name.
     * Setting a non-empty device automatically makes
     * the path an absolute one.
     */
    void set_device (const std::string& device);
		
    /**
     * Returns the device name.
     */
    const std::string& get_device () const;
		
    /**
     * Returns the number of directories in the directory list.
     */
    int depth() const;
    /**
     * Returns the n'th directory in the directory list.
     * If n == depth(), returns the filename.
     */		
    const std::string& directory (int n) const;
		
    /**
     * Returns the n'th directory in the directory list.
     * If n == depth(), returns the filename.
     */
    const std::string& operator [] (int n) const;
		
    /**
     * Adds a directory to the directory list.
     */
    void push_directory (const std::string& dir);
		
    /**
     * Removes the last directory from the directory list.
     */
    void pop_directory ();
		
    /**
     * Sets the filename.
     */
    void set_file_name (const std::string& name);
		
    /**
     * Returns the filename.
     */
    const std::string& get_file_name () const;
		
    /**
     * Sets the basename part of the filename and
     * does not change the extension.
     */
    void set_base_name (const std::string& name);
		
    /**
     * Returns the basename (the filename sans
     * extension) of the path.
     */
    std::string get_base_name () const;
		
    /**
     * Sets the filename extension.
     */
    void set_extension (const std::string& extension);
		
    /**
     * Returns the filename extension.
     */		
    std::string get_extension () const;
		
    /**
     * Clears all components.
     */
    void clear ();
		
    /**
     * Returns a path referring to the path's
     * directory.
     */
    path_c parent () const;
		
    /**
     * Returns an absolute variant of the path,
     * taking the current working directory as base.
     */
    path_c absolute () const;
		
    /**
     * Returns an absolute variant of the path,
     * taking the given path as base.
     */
    path_c absolute (const path_c& base) const;
		
    /**
     * Creates a path referring to a directory.
     */
    static path_c for_directory (const std::string& path);
		
    /**
     * Creates a path referring to a directory.
     */
    static path_c for_directory (const std::string& path, style_t style);
		
    /**
     * Returns the platform's path name separator, which separates
     * the components (names) in a path. 
     *
     * On Unix systems, this is the slash '/'. On Windows systems, 
     * this is the backslash '\'. 
     */
    static char separator ();
		
    /**
     * Returns the platform's path separator, which separates
     * single paths in a list of paths.
     *
     * On Unix systems, this is the colon ':'. On Windows systems,
     * this is the semicolon ';'. 
     */
    static char path_separator ();
		
    /**
     * Returns the current working directory.
     */
    static std::string current ();
		
    /**
     * Returns the user's home directory.
     */
    static std::string home ();
		
    /**
     * Returns the temporary directory.
     */
    static std::string temp ();
		
    /**
     * Returns the name of the null device.
     */
    static std::string null ();
		
    /**
     * Expands all environment variables contained in the path.
     *
     * On Unix, a tilde as first character in the path is
     * replaced with the path to user's home directory.
     */
    static std::string expand (const std::string& path);
    /**
     * Fills the vector with all filesystem roots available on the
     * system. On Unix, there is exactly one root, "/".
     * On Windows, the roots are the drive letters.
     */
    static void list_roots (std::vector<std::string>& roots);
		
    /**
     * Searches the file with the given name in the locations (paths) specified
     * by it and end. A relative path may be given in name.
     *
     * If the file is found in one of the locations, the complete
     * path of the file is stored in the path given as argument and true is returned. 
     * Otherwise false is returned and the path argument remains unchanged.
     */
    static bool find (string_vec_t::const_iterator it, 
		      string_vec_t::const_iterator end, 
		      const std::string& name, 
		      path_c& path);
		
    /**
     * Searches the file with the given name in the locations (paths) specified
     * in pathList. The paths in pathList must be delimited by the platform's
     * path separator (see pathSeparator()). A relative path may be given in name.
     *
     * If the file is found in one of the locations, the complete
     * path of the file is stored in the path given as argument and true is returned. 
     * Otherwise false is returned and the path argument remains unchanged.
     */
    static bool find (const std::string& pathList, const std::string& name, path_c& path);
		
    /**
     * On Windows, if POCO has been compiled with Windows UTF-8 support 
     * (POCO_WIN32_UTF8), this function converts a string (usually containing a path) 
     * encoded in UTF-8 into a string encoded in the current Windows code page.
     * 
     * This function should be used for every string passed as a file name to
     * a string stream or fopen().
     *
     * On all other platforms, or if POCO has not been compiled with Windows UTF-8
     * support, this function returns the string unchanged.
     */
    static std::string transcode (const std::string& path);

  protected:
    void _parse_unix    (const std::string& path);
    void _parse_windows (const std::string& path);
    void _parse_guess   (const std::string& path);

    std::string _build_unix    () const;
    std::string _build_windows () const;

  private:
    std::string  m_node;
    std::string  m_device;
    std::string  m_name;
    std::string  m_version;
    string_vec_t m_dirs;
    bool         m_absolute;
  };

  void swap(path_c& p1, path_c& p2);
  
} 


#endif 

