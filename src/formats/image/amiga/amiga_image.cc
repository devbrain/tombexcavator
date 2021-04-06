//
// Created by igor on 05/04/2021.
//

#include "amiga_image.hh"
#include <tomb-excavator/bsw/io/binary_reader.hh>
#include <tomb-excavator/bsw/exceptions.hh>
#include <boost/pfr.hpp>


namespace formats::image::amiga
{
    template <typename T>
    void generic_read(bsw::io::binary_reader& is, std::size_t size, T& e, const iff::chunk_type& nm)
    {
        std::size_t sz = 0;
        boost::pfr::for_each_field(e, [&sz](const auto& v) {sz += sizeof(v);});

        if (size < sz)
        {
            RAISE_EX("Chunk ", nm, " is corrputed");
        }
        boost::pfr::for_each_field(e, [&is](auto& v) {is >> v;});
    }

    template <typename T>
    void generic_read(bsw::io::binary_reader& is, std::size_t size, std::optional<T>& opt, const iff::chunk_type& nm)
    {
        T e{};
        std::size_t sz = 0;
        boost::pfr::for_each_field(e, [&sz](const auto& v) {sz += sizeof(v);});

        if (size < sz)
        {
            RAISE_EX("Chunk ", nm, " is corrputed");
        }
        boost::pfr::for_each_field(e, [&is](auto& v) {is >> v;});
        opt = e;
    }

    constexpr auto BMHD = formats::iff::fourcc("BMHD");
    // ============================================================================================================
    constexpr auto CMAP = formats::iff::fourcc("CMAP");
    void read(bsw::io::binary_reader& is, std::size_t size, std::vector<cmap_entry>& cmap)
    {
        auto current = static_cast<std::size_t>(is.stream().tellg());
        auto start = current;
        while (current + 3 < start + size)
        {
            cmap_entry e {0, 0, 0};
            is >> e.Red >> e.Green >> e.Blue;
            cmap.push_back(e);
            current = static_cast<std::size_t>(is.stream().tellg());
        }
    }
    // ============================================================================================================
    constexpr auto CMYK = formats::iff::fourcc("CMYK");
    void read(bsw::io::binary_reader& is, std::size_t size, std::vector<cmyk_entry>& cmap)
    {
        auto current = static_cast<std::size_t>(is.stream().tellg());
        while (current + 4 < size)
        {
            cmyk_entry e {0, 0, 0, 0};
            is >> e.cyan >> e.magenta >> e.yellow >> e.black;
            cmap.push_back(e);
            current = static_cast<std::size_t>(is.stream().tellg());
        }
    }
    // ============================================================================================================
    constexpr auto CNAM = formats::iff::fourcc("CNAM");
    void read(bsw::io::binary_reader& is, std::size_t size, color_names& e)
    {
        if (size < 2*sizeof(WORD))
        {
            RAISE_EX("Corrupted CNAM");
        }
        is >> e.starting_color >> e.ending_color;
        if (e.ending_color < e.starting_color)
        {
            RAISE_EX("Corrupted CNAM");
        }
        int n = e.ending_color - e.starting_color + 1;
        std::size_t has_bytes = 2*sizeof(WORD);
        for (int i=0; i<n; i++)
        {
            std::string s;
            while (has_bytes < size)
            {
                char ch;
                is >> ch;
                has_bytes++;
                if (ch == 0)
                {
                    break;
                }
                s += ch;
            }
            e.names.push_back(s);
        }
    }
    // ============================================================================================================
    constexpr auto DRNG = formats::iff::fourcc("DRNG");
    void read(bsw::io::binary_reader& is, std::size_t size, drange& e)
    {
        BYTE nregs, ntrue;

        is >> e.min >> e.max >> e.rate >> e.flags >> ntrue >> nregs;
        for(unsigned int i = 0; i < ntrue; i++)
        {
            dcolor dc;
            generic_read(is, size, dc, DRNG);
            e.dcolors.push_back(dc);
        }
        for(unsigned int i = 0; i < nregs; i++)
        {
            dindex dc;
            generic_read(is, size, dc, DRNG);
            e.dindexes.push_back(dc);
        }
        if((e.flags & ILBM_RNG_FADE) == ILBM_RNG_FADE)
        {
            BYTE nfades;
            is >> nfades >> e.pad;

            for(unsigned int i = 0; i < nfades; i++)
            {
                dfade dc;
                generic_read(is, size, dc, DRNG);
                e.dfades.push_back(dc);
            }
        }
    }

    template <typename T>
    void read(bsw::io::binary_reader& is, std::size_t size, std::optional<T>& x)
    {
        T e{};
        read(is, size, e);
        x = e;
    }
    // ============================================================================================================
    constexpr auto DPI = formats::iff::fourcc("DPI ");
    constexpr auto GRAB = formats::iff::fourcc("GRAB");
    constexpr auto DEST = formats::iff::fourcc("DEST");
    constexpr auto CRNG = formats::iff::fourcc("CRNG");
    constexpr auto SPRITE = formats::iff::fourcc("SPRT");
    constexpr auto CAMG = formats::iff::fourcc("CAMG");
    constexpr auto BODY = formats::iff::fourcc("BODY");
    constexpr auto CYCLE = formats::iff::fourcc("CCRT");
    constexpr auto ABIT = formats::iff::fourcc("ABIT");
    // ============================================================================================================
    void image::update(std::istream& is, formats::iff::chunk_type type, std::size_t size)
    {
        bsw::io::binary_reader reader(is, bsw::io::binary_reader::BIG_ENDIAN_BYTE_ORDER);
        if (type == BMHD) {
            generic_read(reader, size, bmhd, BMHD);
            //read(reader, size, bmhd);
        } else if (type == CMAP) {
            read(reader, size, cmap);
        } else if (type == CMYK) {
            read(reader, size, cmyk);
        } else if (type == CNAM) {
            read(reader, size, cnames);
        } else if (type == DPI) {
            generic_read(reader, size, dpi, DPI);
        } else if (type == GRAB) {
            generic_read(reader, size, grab, GRAB);
        } else if (type == DEST) {
            generic_read(reader, size, dm, DEST);
        } else if (type == CRNG) {
            color_range cr{};
            generic_read(reader, size, cr, CRNG);
            this->crng.push_back(cr);
        } else if (type == DRNG) {
            drange dr{};
            read(reader, size, dr);
            drng.push_back(dr);
        } else if (type == SPRITE) {
            generic_read(reader, size, sprt, SPRITE);
        } else if (type == CAMG) {
            generic_read(reader, size, view_port, CAMG);
        } else if (type == BODY) {
            body.resize(size);
            reader.read_raw(body.data(), size);
        } else if (type == CYCLE) {
            cycle_info ci{};
            generic_read(reader, size, ci, CYCLE);
            cycles.push_back(ci);
        } else if (type == ABIT) {
            bitplanes.resize(size);
            reader.read_raw(bitplanes.data(), size);
        }
    }
    // ========================================================================================================


}