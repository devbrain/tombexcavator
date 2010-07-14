/**
 * @file   manifest.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Sun Jan 03 14:52:43 2010
 * 
 * @brief  Definition of the manifest_c class.
 * 
 */

#ifndef __ABL_OBJECT_MANIFEST_HPP__
#define __ABL_OBJECT_MANIFEST_HPP__


#include "abl/abl_export.h"
#include "abl/object/meta_object.hpp"
#include <map>
#include <typeinfo>


namespace abl 
{
  /**
   * manifest_base_c is a common base class for
   * all instantiations of manifest_c.
   */
  class ABL_API manifest_base_c

  {
  public:
    manifest_base_c();
    virtual ~manifest_base_c();
    /**
     * Returns the type name of the manifest's class.
     */
    virtual const char* className() const = 0;
		
  };
  /**
   * A manifest_c maintains a list of all classes
   * contained in a dynamically loadable class
   * library.
   * Internally, the information is held
   * in a map. An iterator is provided to
   * iterate over all the classes in a manifest_c.
   */
  template <class B>
  class manifest_c: public manifest_base_c
  {
  public:
    typedef abstract_meta_object_c <B> meta_t;
    typedef std::map<std::string, const meta_t*> meta_map_t;
    /**
     * The manifest_c's very own iterator class.
     */
    class iterator_t
    {
    public:
      iterator_t(const typename meta_map_t::const_iterator& it)
      {
	_it = it;
      }
      iterator_t(const iterator_t& it)
      {
	_it = it._it;
      }
      ~iterator_t()
      {
      }
      iterator_t& operator = (const iterator_t& it)
      {
	_it = it._it;
	return *this;
      }
      inline bool operator == (const iterator_t& it) const
      {
	return _it == it._it;
      }
      inline bool operator != (const iterator_t& it) const
      {
	return _it != it._it;
      }
      iterator_t& operator ++ () // prefix
      {
	++_it;
	return *this;
      }
      iterator_t operator ++ (int) // postfix
      {
	iterator_t result(_it);
	++_it;
	return result;
      }
      inline const meta_t* operator * () const
      {
	return _it->second;
      }
      inline const meta_t* operator -> () const
      {
	return _it->second;
      }
		
    private:
      typename meta_map_t::const_iterator _it;
    };
    /**
     * Creates an empty manifest_c.
     */
    manifest_c()
		
    {
    }
    /**
     * Destroys the manifest_c.
     */
    virtual ~manifest_c()
		
    {
      clear();
    }
    /**
     * Returns an iterator pointing to the MetaObject
     * for the given class. If the MetaObject cannot
     * be found, the iterator points to end().
     */
    iterator_t find(const std::string& className) const

    {
      return iterator_t(m_meta_map.find(className));
    }

    iterator_t begin() const
    {
      return iterator_t(m_meta_map.begin());
    }

    iterator_t end() const
    {
      return iterator_t(m_meta_map.end());
    }
    /**
     * Inserts a MetaObject. Returns true if insertion
     * was successful, false if a class with the same
     * name already exists.
     */
    bool insert(const meta_t* pMeta)
    {
      return m_meta_map.insert(typename meta_map_t::value_type(pMeta->name(), pMeta)).second;
    }
    /**
     * Removes all MetaObjects from the manifest.
     */
    void clear()
    {
      for (typename meta_map_t::iterator it = m_meta_map.begin(); it != m_meta_map.end(); ++it)
	{
	  delete it->second;
	}
      m_meta_map.clear();
    }
    /**
    * Returns the number of MetaObjects in the manifest_c.
    */
    int size() const
    {
      return int(m_meta_map.size());
    }
    /**
     * Returns true iff the manifest_c does not contain any MetaObjects.
     */
    bool empty() const
    {
      return m_meta_map.empty();
    }

    const char* class_name() const
    {
      return typeid(*this).name();
    }

  private:
    meta_map_t m_meta_map;
  };


} 


#endif 

