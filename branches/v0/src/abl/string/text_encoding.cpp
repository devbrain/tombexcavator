#include "abl/string/text_encoding.hpp"
#include "abl/exception.hpp"
#include "abl/string/string.hpp"
#include "abl/string/encoding/ascii.hpp"
#include "abl/string/encoding/latin1.hpp"
#include "abl/string/encoding/latin9.hpp"
#include "abl/string/encoding/utf16.hpp"
#include "abl/string/encoding/utf8.hpp"
#include "abl/string/encoding/win1252.hpp"
#include "abl/mt/rw_lock.hpp"
#include "abl/mt/fast_mutex.hpp"
#include "abl/mt/guard.hpp"
#include "abl/singleton_holder.hpp"

#include <map>


namespace abl 
{
  //
  // text_encodig_manager_c
  //
  class text_encoding_manager_c
  {
  public:
    text_encoding_manager_c()
    {
      text_encoding_c::ptr_t pUtf8Encoding (new utf8_encoding_c);
      add(pUtf8Encoding, text_encoding_c::GLOBAL); 

      add(new ascii_encoding_c);
      add(new latin1_encoding_c);
      add(new latin9_encoding_c);
      add(pUtf8Encoding);
      add(new utf16_encoding_c);
      add(new win1252_encoding_c);
    }
	
    ~text_encoding_manager_c()
    {
    }
	
    void add(text_encoding_c::ptr_t pEncoding)
    {
      add(pEncoding, pEncoding->canonicalName());
    }
	
    void add(text_encoding_c::ptr_t pEncoding, const std::string& name)
    {
      writers_guard_c <rw_lock_c> lock (_lock);
	
      _encodings[name] = pEncoding;
    }
	
    void remove(const std::string& name)
    {
      writers_guard_c <rw_lock_c> lock (_lock);
	
      _encodings.erase(name);
    }
	
    text_encoding_c::ptr_t find(const std::string& name) const
    {
      readers_guard_c <rw_lock_c> lock (_lock);
		
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
    text_encoding_manager_c(const text_encoding_manager_c&);
    text_encoding_manager_c& operator = (const text_encoding_manager_c&);
	
    struct ILT
    {
      bool operator() (const std::string& s1, const std::string& s2) const
      {
	return abl::icompare(s1, s2) < 0;
      }
    };
	
    typedef std::map<std::string, text_encoding_c::ptr_t, ILT> EncodingMap;
	
    EncodingMap    _encodings;
    mutable rw_lock_c _lock;
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
      throw not_found_exception_c (encodingName);
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


  text_encoding_manager_c& text_encoding_c::manager()
  {
    static singleton_holder_c <text_encoding_manager_c, fast_mutex_c> sh;
    return *sh.get();
  }


} 

