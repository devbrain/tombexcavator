#include "archive/exceptions.hh"

namespace archive
{
  input_error::input_error ()
    : std::runtime_error ("Input error")
  {
    
  }
  input_error::~input_error () noexcept
  {
  }
  // --------------------------------------------------------------
  exefile_error::exefile_error ()
    : std::runtime_error ("Bad EXE file")
  {
    
  }
  exefile_error::~exefile_error () noexcept
  {
  }
  // --------------------------------------------------------------
  decoder_error::decoder_error (const char* msg)
    : std::runtime_error (msg)
  {
  }
  decoder_error::~decoder_error () noexcept
  {
  }
} // ns archive
