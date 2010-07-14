/**
 * @file   meta_object.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Sun Jan 03 14:39:19 2010
 * 
 * @brief  Definition of meta_object_c class
 * 
 * 
 */


#ifndef __ABL_OBJECT_META_OBJECT_HPP__
#define __ABL_OBJECT_META_OBJECT_HPP__

#include "abl/exception.hpp"
#include "abl/singleton_holder.hpp"
#include <set>


namespace abl 
{
  /**
   * A meta_object_c stores some information
   * about a C++ class. The meta_object_c class
   * is used by the Manifest class.
   * abstract_meta_object_c is a common base class
   * for all meta_object_c in a rooted class hierarchy.
   * A meta_object_c can also be used as an object
   * factory for its class.
   */
  template  <class B>
  class abstract_meta_object_c
  {
  public:
    abstract_meta_object_c (const char* name)
      : m_name (name)
    {
    }
    // --------------------------------------------------------
    virtual ~abstract_meta_object_c ()
    {
      for  (typename object_set_t::iterator it = m_delete_set.begin (); it != m_delete_set.end (); ++it)
	{
	  delete *it;
	}
    }
    // --------------------------------------------------------
    const char* name () const
    {
      return m_name;
    }
    /**
     * Create a new instance of a class.
     * Cannot be used for singletons.
     */
    virtual B* create () const = 0;
    /**
     * Returns a reference to the only instance
     * of the class. Used for singletons only.		
     */
    virtual B& instance () const = 0;
    /**
     * Returns true iff the create method can be used
     * to create instances of the class.
     * Returns false if the class is a singleton.
     */
    virtual bool can_create () const = 0;
    /**
     * If pObject was owned by meta object, the
     * ownership of the deleted object is removed
     * and the object is deleted.
     */
    virtual void destroy (B* pObject) const
    {
      typename object_set_t::iterator it = m_delete_set.find (pObject);
		
      if  (it != m_delete_set.end ())
	{
	  m_delete_set.erase (pObject);
	  delete pObject;
	}
    }
    /**
     * Give ownership of pObject to the meta object.
     * The meta object will delete all objects it owns
     * when it is destroyed.
     *
     * Returns pObject.
     */
    B* auto_delete (B* pObject) const
    {
      if  (this->can_create ()) // guard against singleton
	{
	  m_delete_set.insert (pObject);
	}
      else throw invalid_access_exception_c ("Cannot take ownership of", this->name ());

      return pObject;
    }
    /**
     * Returns true if the object is owned
     * by meta object.
     *
     * Overloaded in MetaSingleton - returns true
     * if the class is a singleton.
     */
    virtual bool is_auto_delete (B* pObject) const
    {
      return m_delete_set.find (pObject) != m_delete_set.end ();
    }

  private:
    abstract_meta_object_c ();
    abstract_meta_object_c (const abstract_meta_object_c&);
    abstract_meta_object_c& operator =  (const abstract_meta_object_c&);
  private:
    typedef std::set <B*> object_set_t;
	
    const char*          m_name;
    mutable object_set_t m_delete_set;
  };

  /**
   * A meta_object_c stores some information
   * about a C++ class. The meta_object_c class
   * is used by the Manifest class.
   * A meta_object_c can also be used as an object
   * factory for its class.
   */
  template  <class C, class B>
  class meta_object_c: public abstract_meta_object_c <B>

  {
  public:
    meta_object_c (const char* name)
      : abstract_meta_object_c <B>  (name)
    {
    }

    ~meta_object_c ()
    {
    }

    B* create () const
    {
      return new C;
    }
	
    B& instance () const
    {
      throw invalid_access_exception_c  ("Not a singleton. Use create () to create instances of", this->name ());
    }
	
    bool can_create () const
    {
      return true;
    }
  };
  /**
   * A Singletonmeta_object_c disables the create () method
   * and instead offers an instance () method to access
   * the single instance of its class. 
   */
  template  <class C, class B, class Lock> 
  class meta_singleton_c: public abstract_meta_object_c <B> 

  { 
  public: 
    meta_singleton_c (const char* name)
      : abstract_meta_object_c <B> (name) 
    {
    }
	
    ~meta_singleton_c () 
    {
    }
	
    B* create () const
    {
      throw invalid_access_exception_c ("Cannot create instances of a singleton class. Use instance () to obtain a", 
				       this->name ());
    }
	
    bool can_create () const
    {
      return false;
    }

    B& instance () const
    {
      return *_object.get ();
    }
      
    bool is_auto_delete (B* pObject) const
    {
      return true;
    }

  private: 
    mutable singleton_holder_c <C, Lock> _object; 
   }; 


} 


#endif 

