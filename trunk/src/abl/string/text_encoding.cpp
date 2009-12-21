#include "abl/string/text_encoding.hpp"
#include "abl/exception.hpp"
#include "abl/string/string.hpp"
#include "abl/string/encoding/ascii.hpp"
#include "abl/string/encoding/latin1.hpp"
#include "abl/string/encoding/latin9.hpp"
#include "abl/string/encoding/utf16.hpp"
#include "abl/string/encoding/utf8.hpp"
#include "abl/string/encoding/win1252.hpp"
#include "abl/mt/rw_lock.h"
#include "abl/SingletonHolder.h"
#include <map>


namespace abl 
{
  //
  // text_encodig_manager_c
  //
  class text_encodig_manager_c
  {
  public:
    text_encodig_manager_c()
    {
      text_encoding_c::ptr_t pUtf8Encoding(new UTF8Encoding);
      add(pUtf8Encoding, text_encoding_c::GLOBAL); 

      add(new ASCIIEncoding);
      add(new Latin1Encoding);
      add(new Latin9Encoding);
      add(pUtf8Encoding);
      add(new UTF16Encoding);
      add(new Windows1252Encoding);
    }
	
    ~text_encodig_manager_c()
    {
    }
	
    void add(text_encoding_c::ptr_t pEncoding)
    {
      add(pEncoding, pEncoding->canonicalName());
    }
	
    void add(text_encoding_c::ptr_t pEncoding, const std::string& name)
    {
      RWLock::ScopedLock lock(_lock, true);
	
      _encodings[name] = pEncoding;
    }
	
    void remove(const std::string& name)
    {
      RWLock::ScopedLock lock(_lock, true);
	
      _encodings.erase(name);
    }
	
    text_encoding_c::ptr_t find(const std::string& name) const
    {
      RWLock::ScopedLock lock(_lock);
		
      EncodingMap::const_iterator it = _encodings.find(name);
      if (it != _encodings.end())
	return it->second;
		
      for (it = _encodings.begin(); it != _encodings.end(); ++it)
	{
	  if (it->second->isA(name))
	    return it->second;
	}
      return text_encoding_c::ptr_t();
    }

  private:
    text_encodig_manager_c(const text_encodig_manager_c&);
    text_encodig_manager_c& operator = (const text_encodig_manager_c&);
	
    struct ILT
    {
      bool operator() (const std::string& s1, const std::string& s2) const
      {
	return abl::icompare(s1, s2) < 0;
      }
    };
	
    typedef std::map<std::string, text_encoding_c::ptr_t, ILT> EncodingMap;
	
    EncodingMap    _encodings;
    mutable RWLock _lock;
  };


  //
  // text_encoding_c
  //


  const std::string text_encoding_c::GLOBAL;


  text_encoding_c::~text_encoding_c()
  {
  }


  int text_encoding_c::convert(const unsigned char* bytes) const
  {
    return static_cast<int>(*bytes);
  }


  int text_encoding_c::convert(int ch, unsigned char* bytes, int length) const
  {
    return 0;
  }


  int text_encoding_c::queryConvert(const unsigned char* bytes, int length) const
  {
    return (int) *bytes;
  }


  int text_encoding_c::sequenceLength(const unsigned char* bytes, int length) const
  {
    return 1;
  }


  text_encoding_c& text_encoding_c::byName(const std::string& encodingName)
  {
    text_encoding_c* pEncoding = manager().find(encodingName);
    if (pEncoding)
      return *pEncoding;
    else
      throw NotFoundException(encodingName);
  }

	
  text_encoding_c::ptr_t text_encoding_c::find(const std::string& encodingName)
  {
    return manager().find(encodingName);
  }


  void text_encoding_c::add(text_encoding_c::ptr_t pEncoding)
  {
    manager().add(pEncoding, pEncoding->canonicalName());
  }


  void text_encoding_c::add(text_encoding_c::ptr_t pEncoding, const std::string& name)
  {
    manager().add(pEncoding, name);
  }


  void text_encoding_c::remove(const std::string& encodingName)
  {
    manager().remove(encodingName);
  }


  text_encoding_c::ptr_t text_encoding_c::global(text_encoding_c::ptr_t encoding)
  {
    text_encoding_c::ptr_t prev = find(GLOBAL);
    add(encoding, GLOBAL);
    return prev;
  }


  text_encoding_c& text_encoding_c::global()
  {
    return byName(GLOBAL);
  }


  text_encodig_manager_c& text_encoding_c::manager()
  {
    static SingletonHolder<text_encodig_manager_c> sh;
    return *sh.get();
  }


} 

