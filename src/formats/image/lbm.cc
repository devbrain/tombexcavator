//
// Created by igor on 14/02/2021.
//

#include "tomb-excavator/formats/image/lbm.hh"
#include "formats/image/picture_loader.hh"

#include "tomb-excavator/formats/iff/iff_parser.hh"
#include "tomb-excavator/formats/iff/ea/ea.hh"
#include "tomb-excavator/formats/iff/ea/ea_events.hh"

#include <tomb-excavator/bsw/io/memory_stream_buf.hh>
#include <tomb-excavator/bsw/io/binary_reader.hh>

namespace
{
    using WORD = uint16_t;
    using BYTE = uint8_t;

    struct bmhd
    {
        WORD Width;        /* Width of image in pixels */
        WORD Height;       /* Height of image in pixels */
        WORD Left;         /* X coordinate of image */
        WORD Top;          /* Y coordinate of image */
        BYTE Bitplanes;    /* Number of bitplanes */
        BYTE Masking;      /* Type of masking used */
        BYTE Compress;     /* Compression method use on image data */
        BYTE Padding;      /* Alignment padding (always 0) */
        WORD Transparency; /* Transparent background color */
        BYTE XAspectRatio; /* Horizontal pixel size */
        BYTE YAspectRatio; /* Vertical pixel size */
        WORD PageWidth;    /* Horizontal resolution of display device */
        WORD PageHeight;   /* Vertical resolution of display device */
    };

    struct cmap_entry
    {
        BYTE Red;           /* Red color component (0-255) */
        BYTE Green;         /* Green color component (0-255) */
        BYTE Blue;          /* Blue color component (0-255) */
    };

    constexpr auto BMHD = formats::iff::fourcc("BMHD");
    constexpr auto CMAP = formats::iff::fourcc("CMAP");
    constexpr auto BODY = formats::iff::fourcc("BODY");
    // ===========================================================================================================

    class lbm_parser : public formats::iff::ea_events
    {
    private:
        void on_form_start(formats::iff::chunk_type name) {};
        void on_form_end(formats::iff::chunk_type name) {};
        void on_chunk(std::istream& is, formats::iff::chunk_type type, [[maybe_unused]] uint64_t offset, std::size_t size) override
        {
            if (type == BMHD)
            {
                load_bmhd(is, size);
            } else if (type == CMAP)
            {
                bsw::io::binary_reader reader(is, bsw::io::binary_reader::BIG_ENDIAN_BYTE_ORDER);
                auto n = size / 3;
                m_cmap.reserve(n);
                for (std::size_t i=0; i<n; i++)
                {
                    cmap_entry e;
                    reader >> e.Red >> e.Green >> e.Blue;
                    m_cmap.push_back(e);
                }
            } else if (type == BODY)
            {
                m_body.resize(size);
                is.read((char*)m_body.data(), size);
            }
        }
    private:
        void load_bmhd(std::istream& is, std::size_t size)
        {
            if (size < sizeof(m_bmhd))
            {
                RAISE_EX("IFF BMHD chunk is corrupted");
            }

            bsw::io::binary_reader reader(is, bsw::io::binary_reader::BIG_ENDIAN_BYTE_ORDER);
            reader >> m_bmhd.Width
                    >> m_bmhd.Height
                    >> m_bmhd.Left
                    >> m_bmhd.Top
                    >> m_bmhd.Bitplanes
                    >> m_bmhd.Masking
                    >> m_bmhd.Compress
                    >> m_bmhd.Padding
                    >> m_bmhd.Transparency
                    >> m_bmhd.XAspectRatio
                    >> m_bmhd.YAspectRatio
                    >> m_bmhd.PageWidth
                    >> m_bmhd.PageHeight;
        }
    private:
        bmhd m_bmhd;
        std::vector<cmap_entry> m_cmap;
        std::vector<unsigned char> m_body;
    };
}

namespace formats::image
{
    bool is_lbm(const char* input, std::size_t input_length)
    {
        using namespace formats::iff;
        ea_tester tester({
            fourcc("ILBM"),
            fourcc("PBM "),
            fourcc("ACBM")
        });
        bsw::io::memory_input_stream istream(input, input_length);
        iff_parser<ea_iff>(istream, &tester);
        return tester.valid();
    }
    // ------------------------------------------------------------------------------------------------------
    bool load_lbm(const char* input, std::size_t input_length, picture& out)
    {
        using namespace formats::iff;
        bsw::io::memory_input_stream istream(input, input_length);
        lbm_parser parser;
        iff_parser<ea_iff>(istream, &parser);

        return false;
    }
    // =====================================================================================================
    struct register_lbm
    {
        register_lbm() noexcept
        {
            picture_loader::instance().register_functions(is_lbm, load_lbm);
        }
    };

    static register_lbm register_lbm_inst;
}

