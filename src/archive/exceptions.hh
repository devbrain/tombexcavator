#ifndef ARCHIVE_EXCEPTIONS_HH__
#define ARCHIVE_EXCEPTIONS_HH__

#include <stdexcept>

#include <archive/archive_api.h>

#if defined(_MSC_VER)
#pragma warning (push)
#pragma warning (disable:4275)
#endif

namespace archive
{
  class ARCHIVE_API input_error : public std::runtime_error
  {
  public:
    input_error ();
    ~input_error () noexcept ;
  };

  class ARCHIVE_API exefile_error : public std::runtime_error
  {
  public:
    exefile_error ();
    ~exefile_error () noexcept ;
  };

  class ARCHIVE_API decoder_error : public std::runtime_error
  {
  public:
    explicit decoder_error (const char* msg);
    ~decoder_error () noexcept ;
  };
} // ns explode

#if defined(_MSC_VER)
#pragma warning (pop)
#endif

#endif
