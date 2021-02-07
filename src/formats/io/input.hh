//
// Created by igor on 06/02/2021.
//

#ifndef TOMBEXCAVATOR_INPUT_HH
#define TOMBEXCAVATOR_INPUT_HH

#include <tombexcavator-formats_export.h>
#include <endian.hpp>
#include <mio/mmap.hpp>
#include <type_traits>
#include <filesystem>
#include <vector>
#include "formats/io/io_base.hh"

namespace formats
{
    class FORMATS_API input : public io_base
    {
    public:
        virtual void read_buff (char* buffer, std::size_t size) = 0;
        [[nodiscard]] virtual std::size_t bytes_remains () const = 0;

        template <typename T>
        void read (T& x)
        {
            static_assert(std::is_integral_v<T>, "Only integral types are supported");
            T temp;
            union
            {
                char* bytes;
                T*    words;
            } u;
            u.words = &temp;
            this->read_buff (u.bytes, sizeof (T));
            if (big_endian())
            {
                x = endian::read_be<T>(u.bytes);
            }
            else
            {
                x = endian::read_le<T>(u.bytes);
            }
        }
    };

    template <typename T>
    input& operator >> (input& inp, T& x)
    {
        inp.read(x);
        return inp;
    }

    // ==========================================================================================
    class FORMATS_API base_memory_input : public input
    {
    public:
        void read_buff (char* buffer, std::size_t size) override;
        std::size_t tell () const override;
        std::size_t bytes_remains () const override;
        void seek (std::size_t offset) override;
    protected:
        base_memory_input();

        void setup(const char* buff, std::size_t size);
    private:
        const char* m_buffer;
        std::size_t m_size;
        std::size_t m_offset;
    };
    // =============================================================================
    class FORMATS_API file_input : public base_memory_input
    {
    public:
        explicit file_input(const std::filesystem::path& path);
    private:
        mio::mmap_source m_source;
    };
    // =============================================================================
    class FORMATS_API memory_input : public base_memory_input
    {
    public:
        memory_input(const char* buff, std::size_t size);
        memory_input(const std::vector<char>& buff);
    };

} // ns formats

#endif //TOMBEXCAVATOR_INPUT_HH
