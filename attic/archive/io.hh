#ifndef ARCHIVE_IO_HH__
#define ARCHIVE_IO_HH__

#include <cstdio>
#include <cstddef>
#include <cstdint>
#include <vector>

#include <sys/types.h>
#include "byte_order.hh"

#include "archive/archive_api.h"

namespace archive
{
  typedef off_t offset_type;

  class ARCHIVE_API input
  {
  public:
    input ();
    virtual ~input ();
    virtual void read_buff (char* buffer, std::size_t size) = 0;
    virtual offset_type tell () const = 0;
    virtual offset_type bytes_remains () const = 0;
    virtual void seek (offset_type offset) = 0;

    template <typename T>
    void read (T& x)
    {
      union 
      {
	char* bytes;
	T*    words;
      } u;
      u.words = &x;
      this->read_buff (u.bytes, sizeof (T));
    }
  

  private:
    input (const input&);
    input& operator = (const input&);
  };

  // ============================================================

  class ARCHIVE_API output
  {
  public:
    output ();
    virtual ~output ();
    virtual void write_buff (const char* buffer, std::size_t size) = 0;
    virtual offset_type tell () = 0;
    virtual void seek (offset_type offset) = 0;

    template <typename T>
    void write (const T& x)
    {
      union 
      {
		const char* bytes;
		const T*    words;
      } u;
      u.words = &x;
      write_buff (u.bytes, sizeof (T));
    }


  private:
    output (const output&);
    output& operator = (const output&);
  };

  // ============================================================

  class ARCHIVE_API file_input : public input
  {
  public:
    explicit file_input (const char* path);
    explicit file_input (FILE* file);

    ~file_input ();

    virtual void read_buff (char* buffer, std::size_t size);
    virtual offset_type tell () const;
    virtual offset_type bytes_remains () const;
    virtual void seek (offset_type offset);

  private:
    bool m_owner;
    FILE* m_file;
  };
  // ============================================================
  class ARCHIVE_API inmem_input : public input
  {
  public:
	  inmem_input(const unsigned char* data, std::size_t size);
	  inmem_input(inmem_input&& other);
	  
	  virtual void read_buff (char* buffer, std::size_t size);
	  virtual offset_type tell() const;
	  virtual offset_type bytes_remains() const;
	  virtual void seek(offset_type offset);
	  const unsigned char* data() const;
	  std::size_t size() const noexcept ;
  private:
	  const unsigned char* m_data;
	  const std::size_t    m_size;
	  std::size_t          m_ptr;
  };

  // ============================================================
  class ARCHIVE_API file_output : public output
  {
  public:
    explicit file_output (const char* path);
    explicit file_output (FILE* file);

    ~file_output ();

    virtual void write_buff (const char* buffer, std::size_t size);
    virtual offset_type tell ();
    virtual void seek (offset_type offset);

  private:
    bool m_owner;
    FILE* m_file;
  };
  // ============================================================
  class ARCHIVE_API inmem_output : public output
  {
  public:
	  explicit inmem_output(std::vector <char>& out_buff);

	  virtual void write_buff (const char* buffer, std::size_t size);
	  virtual offset_type tell();
	  virtual void seek(offset_type offset);

  private:
	  std::vector <char>& m_buff;
	  std::size_t m_ptr;
  };
  // =============================================================================
  enum class endianity
  {
      BIG,
      LITTLE
  };

  template <typename T, endianity e>
  struct endian_data
  {
      static constexpr bool is_big_endian = e == endianity::BIG;
      using value_type = T;

      T data;
  };

  using be_uint16_t = endian_data<uint16_t, endianity::BIG>;
  using be_uint32_t = endian_data<uint32_t, endianity::BIG>;
  using be_uint64_t = endian_data<uint64_t, endianity::BIG>;
  using be_int16_t = endian_data<int16_t, endianity::BIG>;
  using be_int32_t = endian_data<int32_t, endianity::BIG>;
  using be_int64_t = endian_data<int64_t, endianity::BIG>;

  using le_uint16_t = endian_data<uint16_t, endianity::LITTLE>;
  using le_uint32_t = endian_data<uint32_t, endianity::LITTLE>;
  using le_uint64_t = endian_data<uint64_t, endianity::LITTLE>;
  using le_int16_t = endian_data<int16_t, endianity::LITTLE>;
  using le_int32_t = endian_data<int32_t, endianity::LITTLE>;
  using le_int64_t = endian_data<int64_t, endianity::LITTLE>;

  template <typename T>
  input& operator >> (input& inp, T& val)
  {
     inp.read(val);
     return inp;
  }

    template <typename T>
    input& operator >> (input& inp, endian_data<T, endianity::BIG>& val)
    {
        inp.read(val.data);
        val.data = byte_order::from_big_endian(val.data);
        return inp;
    }

    template <typename T>
    input& operator >> (input& inp, endian_data<T, endianity::LITTLE>& val)
    {
        inp.read(val.data);
        val.data = byte_order::from_little_endian(val.data);
        return inp;
    }

} // ns archive



#endif
