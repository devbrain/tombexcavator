/**
 * @file   file.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Sun Dec 27 11:06:27 2009
 * 
 * @brief  Definition of the file_impl_c class.
 * 
 */

#ifndef __ABL_FS_FILE_IMPL_HPP__
#define __ABL_FS_FILE_IMPL_HPP__


#include "abl/abl_export.h"
#include "abl/types/types.hpp"
#include "abl/date_time/time_stamp.hpp"
#include <string>

namespace abl
{
#if defined(_WIN32)
  class ABL_API file_impl_c
  {
  protected:
    typedef uint64_t FileSizeImpl;

    file_impl_c ();
    file_impl_c (const std::string& path);
    virtual ~file_impl_c ();
    void swapImpl (file_impl_c& file);
    void setPathImpl (const std::string& path);
    const std::string& getPathImpl () const;
    bool existsImpl () const;
    bool canReadImpl () const;
    bool canWriteImpl () const;
    bool canExecuteImpl () const;
    bool isFileImpl () const;
    bool isDirectoryImpl () const;
    bool isLinkImpl () const;
    bool isDeviceImpl () const;
    bool isHiddenImpl () const;
    time_stamp_c createdImpl () const;
    time_stamp_c getLastModifiedImpl () const;
    void setLastModifiedImpl (const time_stamp_c& ts);
    FileSizeImpl getSizeImpl () const;
    void setSizeImpl (FileSizeImpl size);
    void setWriteableImpl (bool flag = true);		
    void setExecutableImpl (bool flag = true);		
    void copyToImpl (const std::string& path) const;
    void renameToImpl (const std::string& path);
    void removeImpl ();
    bool createFileImpl ();
    bool createDirectoryImpl ();
    static void handleLastErrorImpl (const std::string& path);
	
  private:
    std::string  _path;
    std::wstring _upath;
	
    friend class FileHandle;
    friend class DirectoryIteratorImpl;
  }; 
#else
  class file_impl_c
  {
  protected:
    typedef uint64_t FileSizeImpl;

    file_impl_c ();
    file_impl_c (const std::string& path);
    virtual ~file_impl_c ();
    void swapImpl (file_impl_c& file);
    void setPathImpl (const std::string& path);
    const std::string& getPathImpl () const;
    bool existsImpl () const;
    bool canReadImpl () const;
    bool canWriteImpl () const;
    bool canExecuteImpl () const;
    bool isFileImpl () const;
    bool isDirectoryImpl () const;
    bool isLinkImpl () const;
    bool isDeviceImpl () const;
    bool isHiddenImpl () const;
    time_stamp_c createdImpl () const;
    time_stamp_c getLastModifiedImpl () const;
    void setLastModifiedImpl (const time_stamp_c& ts);
    FileSizeImpl getSizeImpl () const;
    void setSizeImpl (FileSizeImpl size);
    void setWriteableImpl (bool flag = true);		
    void setExecutableImpl (bool flag = true);		
    void copyToImpl (const std::string& path) const;
    void renameToImpl (const std::string& path);
    void removeImpl ();
    bool createFileImpl ();
    bool createDirectoryImpl ();
    static void handleLastErrorImpl (const std::string& path);
	
  private:
    std::string _path;
	
    friend class DirectoryIteratorImpl;
  }; 
#endif
  inline 
  const std::string& file_impl_c::getPathImpl () const
  {
    return _path;
  }
 
}

#endif
