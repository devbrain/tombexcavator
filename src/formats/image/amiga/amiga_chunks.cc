//
// Created by igor on 07/04/2021.
//

#include "amiga_chunks.hh"
#include <tomb-excavator/bsw/io/binary_reader.hh>
#include <tomb-excavator/bsw/exceptions.hh>
#include <boost/pfr.hpp>

namespace formats::amiga
{
    template <typename T>
    T generic_read(bsw::io::binary_reader& is, std::size_t size)
    {
        T e;
        std::size_t sz = 0;
        boost::pfr::for_each_field(e, [&sz](const auto& v) {sz += sizeof(v);});

        if (size < sz)
        {
            RAISE_EX("Chunk ", T::CHUNK_ID, " is corrputed");
        }
        boost::pfr::for_each_field(e, [&is](auto& v) {is >> v;});
        return e;
    }
    // ==========================================================================================
    bmhd bmhd::read(std::istream& is, std::size_t size)
    {
        bsw::io::binary_reader reader(is, bsw::io::binary_reader::BIG_ENDIAN_BYTE_ORDER);
        return generic_read<bmhd>(reader, size);
    }
    // ==========================================================================================
    cmap cmap::read(std::istream& is, std::size_t size)
    {
        bsw::io::binary_reader reader(is, bsw::io::binary_reader::BIG_ENDIAN_BYTE_ORDER);
        auto current = static_cast<std::size_t>(is.tellg());

        cmap r;
        std::size_t has_bytes = 0;
        while (has_bytes + 3 <= size)
        {

            cmap_entry e {0, 0, 0};
            reader >> e.Red >> e.Green >> e.Blue;
            has_bytes += 3;

            r.colors.push_back(e);

        }
        return r;
    }
    // -----------------------------------------------------------------------------------------------
    vport vport::read(std::istream& is, std::size_t size)
    {
        bsw::io::binary_reader reader(is, bsw::io::binary_reader::BIG_ENDIAN_BYTE_ORDER);
        return generic_read<vport>(reader, size);
    }
    // -----------------------------------------------------------------------------------------------
    body body::read(std::istream& is, std::size_t size)
    {
        body x;
        x.data.resize(size);
        is.read((char*)x.data.data(), size);
        return x;
    }
    // -----------------------------------------------------------------------------------------------
    bitplanes bitplanes::read(std::istream& is, std::size_t size)
    {
        bitplanes x;
        x.data.resize(size);
        is.read((char*)x.data.data(), size);
        return x;
    }
}