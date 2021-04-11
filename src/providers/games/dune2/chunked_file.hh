//
// Created by igor on 11/04/2021.
//

#ifndef TOMBEXCAVATOR_CHUNKED_FILE_HH
#define TOMBEXCAVATOR_CHUNKED_FILE_HH


#include <optional>
#include <cstdint>
#include <optional>
#include <tuple>
#include <tomb-excavator/formats/iff/fourcc.hh>
#include <tomb-excavator/bsw/io/binary_reader.hh>

class chunked_file
{
public:
    using offset_and_size_t = std::tuple <uint64_t, std::size_t>;
public:
    chunked_file(std::istream& is, std::size_t entry_size);
    ~chunked_file();
    std::optional<offset_and_size_t> find_section(const formats::iff::chunk_type& type);
private:
    bsw::io::binary_reader m_is;
    uint64_t               m_offset;
    uint64_t               m_data_offset;
    std::size_t            m_file_size;
    const std::size_t      m_entry_size;
};


#endif //TOMBEXCAVATOR_CHUNKED_FILE_HH
