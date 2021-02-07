//
// Created by igor on 07/02/2021.
//

#ifndef TOMBEXCAVATOR_OUTPUT_HH
#define TOMBEXCAVATOR_OUTPUT_HH

#include <tombexcavator-formats_export.h>
#include <endian.hpp>
#include <cstdio>
#include <type_traits>
#include <filesystem>
#include <vector>
#include "formats/io/io_base.hh"

namespace formats
{
    class FORMATS_API output : public io_base
    {
    public:
        virtual void write_buff(const char* buffer, std::size_t size) = 0;

        template<typename T>
        void write(const T& x)
        {
            char temp[sizeof(T)];
            if (big_endian())
            {
                endian::write_be<T>(x, temp);
            } else
            {
                endian::write_le<T>(x, temp);
            }
            write_buff(temp, sizeof(T));
        }
    };

    template<typename T>
    output& operator<<(output& out, T& x)
    {
        out.write(x);
        return out;
    }

    // =========================================================================================

    class FORMATS_API file_output : public output
    {
    public:
        explicit file_output(const std::filesystem::path& path);
        explicit file_output(FILE* file);

        ~file_output();

        void write_buff(const char* buffer, std::size_t size) override;
        std::size_t tell() const override;
        void seek(std::size_t offset) override;

    private:
        bool m_owner;
        FILE* m_file;
    };

    // ============================================================

    class FORMATS_API memory_output : public output
    {
    public:
        explicit memory_output(std::vector<char>& out_buff);

        void write_buff(const char* buffer, std::size_t size) override;
        std::size_t tell() const override;
        void seek(std::size_t offset) override;
    private:
        std::vector<char>& m_buff;
        std::size_t m_ptr;
    };

} // ns formats

#endif //TOMBEXCAVATOR_OUTPUT_HH
