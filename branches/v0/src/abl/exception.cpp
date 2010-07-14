#include "abl/exception.hpp"
#include <typeinfo>


namespace abl {


  exception_c::exception_c(int code)
    : _pNested(0), _code(code)
  {
  }


  exception_c::exception_c(const std::string& msg, int code): 
    _msg(msg), _pNested(0), _code(code)
  {
  }


  exception_c::exception_c(const std::string& msg, const std::string& arg, int code): _msg(msg), _pNested(0), _code(code)
  {
    if (!arg.empty())
      {
	_msg.append(": ");
	_msg.append(arg);
      }
  }


  exception_c::exception_c(const std::string& msg, const exception_c& nested, int code): _msg(msg), _pNested(nested.clone()), _code(code)
  {
  }


  exception_c::exception_c(const exception_c& exc):
    std::exception(exc),
    _msg(exc._msg),
    _code(exc._code)
  {
    _pNested = exc._pNested ? exc._pNested->clone() : 0;
  }

	
  exception_c::~exception_c() throw()
  {
    delete _pNested;
  }


  exception_c& exception_c::operator = (const exception_c& exc)
  {
    if (&exc != this)
      {
	delete _pNested;
	_msg     = exc._msg;
	_pNested = exc._pNested ? exc._pNested->clone() : 0;
	_code    = exc._code;
      }
    return *this;
  }


  const char* exception_c::name() const throw()
  {
    return "exception_c";
  }


  const char* exception_c::className() const throw()
  {
    return typeid(*this).name();
  }

	
  const char* exception_c::what() const throw()
  {
    return name();
  }

	
  std::string exception_c::displayText() const
  {
    std::string txt = name();
    if (!_msg.empty())
      {
	txt.append(": ");
	txt.append(_msg);
      }
    return txt;
  }


  void exception_c::extendedMessage(const std::string& arg)
  {
    if (!arg.empty())
      {
	if (!_msg.empty()) _msg.append(": ");
	_msg.append(arg);
      }
  }


  exception_c* exception_c::clone() const
  {
    return new exception_c(*this);
  }


  void exception_c::rethrow() const
  {
    throw *this;
  }



  const exception_c* exception_c::nested() const
  {
    return _pNested;
  }
  
  
  const std::string& exception_c::message() const
  {
    return _msg;
  }
  

  void exception_c::message(const std::string& msg)
  {
    _msg = msg;
  }
  

  int exception_c::code() const
  {
    return _code;
  }
  
  // ===========================================================================================================
    ABL_IMPLEMENT_EXCEPTION (logic_exception_c                , exception_c     , "Logic exception")
    ABL_IMPLEMENT_EXCEPTION (assertion_violation_exception_c  , logic_exception_c, "Assertion violation")
    ABL_IMPLEMENT_EXCEPTION (null_pointer_exception_c         , logic_exception_c, "Null pointer")
    ABL_IMPLEMENT_EXCEPTION (bugcheck_exception_c             , logic_exception_c, "Bugcheck")
    ABL_IMPLEMENT_EXCEPTION (invalid_argument_exception_c     , logic_exception_c, "Invalid argument")
    ABL_IMPLEMENT_EXCEPTION (not_implemented_exception_c      , logic_exception_c, "Not implemented")
    ABL_IMPLEMENT_EXCEPTION (range_exception_c                , logic_exception_c, "Out of range")
    ABL_IMPLEMENT_EXCEPTION (illegal_state_exception_c        , logic_exception_c, "Illegal state")
    ABL_IMPLEMENT_EXCEPTION (invalid_access_exception_c       , logic_exception_c, "Invalid access")
    ABL_IMPLEMENT_EXCEPTION (signal_exception_c               , logic_exception_c, "Signal received")
    ABL_IMPLEMENT_EXCEPTION (unhandled_exception_c            , logic_exception_c, "Unhandled exception")

    ABL_IMPLEMENT_EXCEPTION (runtime_exception_c               , exception_c       , "runtime exception")
    ABL_IMPLEMENT_EXCEPTION (not_found_exception_c             , runtime_exception_c, "Not found")
    ABL_IMPLEMENT_EXCEPTION (exists_exception_c                , runtime_exception_c, "Exists")
    ABL_IMPLEMENT_EXCEPTION (timeout_exception_c               , runtime_exception_c, "Timeout")
    ABL_IMPLEMENT_EXCEPTION (system_exception_c                , runtime_exception_c, "System exception")
    ABL_IMPLEMENT_EXCEPTION (regular_expression_exception_c    , runtime_exception_c, "Error in regular expression")
    ABL_IMPLEMENT_EXCEPTION (library_load_exception_c          , runtime_exception_c, "Cannot load library")
    ABL_IMPLEMENT_EXCEPTION (library_already_loaded_exception_c, runtime_exception_c, "Library already loaded")
    ABL_IMPLEMENT_EXCEPTION (no_thread_available_exception_c   , runtime_exception_c, "No thread available")
    ABL_IMPLEMENT_EXCEPTION (property_not_supported_exception_c, runtime_exception_c, "Property not supported")
    ABL_IMPLEMENT_EXCEPTION (pool_overflow_exception_c         , runtime_exception_c, "Pool overflow")
    ABL_IMPLEMENT_EXCEPTION (no_permission_exception_c         , runtime_exception_c, "No permission")
    ABL_IMPLEMENT_EXCEPTION (out_of_memory_exception_c         , runtime_exception_c, "Out of memory")

    ABL_IMPLEMENT_EXCEPTION (data_exception_c                  , runtime_exception_c, "Data error")
    ABL_IMPLEMENT_EXCEPTION (data_format_exception_c           , data_exception_c   , "Bad data format")
    ABL_IMPLEMENT_EXCEPTION (circular_reference_exception_c    , data_exception_c   , "Circular reference")

    ABL_IMPLEMENT_EXCEPTION (syntax_exception_c                , data_exception_c  , "Syntax error")
    ABL_IMPLEMENT_EXCEPTION (path_syntax_exception_c           , syntax_exception_c, "Bad path syntax")

    ABL_IMPLEMENT_EXCEPTION (io_exception_c                    , runtime_exception_c, "I/O error")
    ABL_IMPLEMENT_EXCEPTION (Protocol_exception_c              , io_exception_c     , "Protocol error")

    ABL_IMPLEMENT_EXCEPTION (file_exception_c                  , io_exception_c     , "File access error")
    ABL_IMPLEMENT_EXCEPTION (file_exists_exception_c           , file_exception_c, "File exists")
    ABL_IMPLEMENT_EXCEPTION (file_not_found_exception_c        , file_exception_c, "File not found")
    ABL_IMPLEMENT_EXCEPTION (path_not_found_exception_c        , file_exception_c, "Path not found")
    ABL_IMPLEMENT_EXCEPTION (file_read_only_exception_c        , file_exception_c, "File is read-only")
    ABL_IMPLEMENT_EXCEPTION (file_access_denied_exception_c    , file_exception_c, "Access to file denied")
    ABL_IMPLEMENT_EXCEPTION (create_file_exception_c           , file_exception_c, "Cannot create file")
    ABL_IMPLEMENT_EXCEPTION (open_file_exception_c             , file_exception_c, "Cannot open file")
    ABL_IMPLEMENT_EXCEPTION (write_file_exception_c            , file_exception_c, "Cannot write file")
    ABL_IMPLEMENT_EXCEPTION (read_file_exception_c             , file_exception_c, "Cannot read file")

    ABL_IMPLEMENT_EXCEPTION (unknown_uri_scheme_exception_c    , runtime_exception_c, "Unknown URI scheme")


    ABL_IMPLEMENT_EXCEPTION (application_exception_c          , exception_c       , "Application exception")
    ABL_IMPLEMENT_EXCEPTION (bad_cast_exception_c             , runtime_exception_c, "Bad cast exception")

    } // namespace abl

