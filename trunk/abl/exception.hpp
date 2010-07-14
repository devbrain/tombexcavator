#ifndef __ABL_EXCEPTION_HPP__
#define __ABL_EXCEPTION_HPP__

#include "abl/abl_export.h"

#include <string>
#include <stdexcept>


namespace abl
{


  class ABL_API exception_c: public std::exception
			     /// This is the base class for all exceptions defined
			     /// in the Poco class library.
  {
  public:
    exception_c(const std::string& msg, int code = 0);
    /// Creates an exception.

    exception_c(const std::string& msg, const std::string& arg, int code = 0);
    /// Creates an exception.

    exception_c(const std::string& msg, const exception_c& nested, int code = 0);
    /// Creates an exception and stores a clone
    /// of the nested exception.

    exception_c(const exception_c& exc);
    /// Copy constructor.
		
    ~exception_c() throw();
    /// Destroys the exception and deletes the nested exception.

    exception_c& operator = (const exception_c& exc);
    /// Assignment operator.

    virtual const char* name() const throw();
    /// Returns a static string describing the exception.
		
    virtual const char* className() const throw();
    /// Returns the name of the exception class.
		
    virtual const char* what() const throw();
    /// Returns a static string describing the exception.
    ///
    /// Same as name(), but for compatibility with std::exception.
		
    const exception_c* nested() const;
    /// Returns a pointer to the nested exception, or
    /// null if no nested exception exists.
			
    const std::string& message() const;
    /// Returns the message text.
			
    int code() const;
    /// Returns the exception code if defined.
		
    std::string displayText() const;
    /// Returns a string consisting of the
    /// message name and the message text.

    virtual exception_c* clone() const;
    /// Creates an exact copy of the exception.
    ///
    /// The copy can later be thrown again by
    /// invoking rethrow() on it.
		
    virtual void rethrow() const;
    /// (Re)Throws the exception.
    ///
    /// This is useful for temporarily storing a
    /// copy of an exception (see clone()), then
    /// throwing it again.

  protected:
    exception_c(int code = 0);
    /// Standard constructor.

    void message(const std::string& msg);
    /// Sets the message for the exception.

    void extendedMessage(const std::string& arg);
    /// Sets the extended message for the exception.
		
  private:
    std::string   _msg;
    exception_c*  _pNested;
    int		  _code;
  };




  //
  // Macros for quickly declaring and implementing exception classes.
  // Unfortunately, we cannot use a template here because character
  // pointers (which we need for specifying the exception name)
  // are not allowed as template arguments.
  //
#define ABL_DECLARE_EXCEPTION(API, CLS, BASE)				\
  class API CLS: public BASE						\
  {									\
  public:								\
    CLS(int code = 0);							\
    CLS(const std::string& msg, int code = 0);				\
    CLS(const std::string& msg, const std::string& arg, int code = 0);	\
    CLS(const std::string& msg, const abl::exception_c& exc, int code = 0); \
    CLS(const CLS& exc);						\
    ~CLS() throw();							\
    CLS& operator = (const CLS& exc);					\
    const char* name() const throw();					\
    const char* className() const throw();				\
    abl::exception_c* clone() const;					\
    void rethrow() const;						\
  }


#define ABL_IMPLEMENT_EXCEPTION(CLS, BASE, NAME)			\
  CLS::CLS(int code): BASE(code)					\
  {									\
  }									\
    CLS::CLS(const std::string& msg, int code): BASE(msg, code)		\
    {									\
    }									\
    CLS::CLS(const std::string& msg, const std::string& arg, int code): BASE(msg, arg, code) \
    {									\
    }									\
    CLS::CLS(const std::string& msg, const abl::exception_c& exc, int code): BASE(msg, exc, code) \
    {									\
    }									\
    CLS::CLS(const CLS& exc): BASE(exc)					\
    {									\
    }									\
    CLS::~CLS() throw()							\
    {									\
    }									\
    CLS& CLS::operator = (const CLS& exc)				\
    {									\
      BASE::operator = (exc);						\
      return *this;							\
    }									\
    const char* CLS::name() const throw()				\
    {									\
      return NAME;							\
    }									\
    const char* CLS::className() const throw()				\
    {									\
      return typeid(*this).name();					\
    }									\
    abl::exception_c* CLS::clone() const					\
    {									\
      return new CLS(*this);						\
    }									\
    void CLS::rethrow() const						\
    {									\
      throw *this;							\
    }


  ABL_DECLARE_EXCEPTION (ABL_API, logic_exception_c                , exception_c     );
  ABL_DECLARE_EXCEPTION (ABL_API, assertion_violation_exception_c  , logic_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, null_pointer_exception_c         , logic_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, bugcheck_exception_c             , logic_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, invalid_argument_exception_c     , logic_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, not_implemented_exception_c      , logic_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, range_exception_c                , logic_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, illegal_state_exception_c        , logic_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, invalid_access_exception_c       , logic_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, signal_exception_c               , logic_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, unhandled_exception_c            , logic_exception_c);

  ABL_DECLARE_EXCEPTION (ABL_API, runtime_exception_c               , exception_c       );
  ABL_DECLARE_EXCEPTION (ABL_API, not_found_exception_c             , runtime_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, exists_exception_c                , runtime_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, timeout_exception_c               , runtime_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, system_exception_c                , runtime_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, regular_expression_exception_c    , runtime_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, library_load_exception_c          , runtime_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, library_already_loaded_exception_c, runtime_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, no_thread_available_exception_c   , runtime_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, property_not_supported_exception_c, runtime_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, pool_overflow_exception_c         , runtime_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, no_permission_exception_c         , runtime_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, out_of_memory_exception_c         , runtime_exception_c);

  ABL_DECLARE_EXCEPTION (ABL_API, data_exception_c                  , runtime_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, data_format_exception_c           , data_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, circular_reference_exception_c    , data_exception_c   );

  ABL_DECLARE_EXCEPTION (ABL_API, syntax_exception_c                , data_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, path_syntax_exception_c           , syntax_exception_c);

  ABL_DECLARE_EXCEPTION (ABL_API, io_exception_c                    , runtime_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, Protocol_exception_c              , io_exception_c);

  ABL_DECLARE_EXCEPTION (ABL_API, file_exception_c                  , io_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, file_exists_exception_c           , file_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, file_not_found_exception_c        , file_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, path_not_found_exception_c        , file_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, file_read_only_exception_c        , file_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, file_access_denied_exception_c    , file_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, create_file_exception_c           , file_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, open_file_exception_c             , file_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, write_file_exception_c            , file_exception_c);
  ABL_DECLARE_EXCEPTION (ABL_API, read_file_exception_c             , file_exception_c);

  ABL_DECLARE_EXCEPTION (ABL_API, unknown_uri_scheme_exception_c    , runtime_exception_c);


  ABL_DECLARE_EXCEPTION (ABL_API, application_exception_c          , exception_c       );
  ABL_DECLARE_EXCEPTION (ABL_API, bad_cast_exception_c             , runtime_exception_c);

} 


#endif 
