//
// Created by igor on 14/02/2021.
//

#include "tomb-excavator/formats/image/lbm.hh"
#include "formats/image/picture_loader.hh"
#include "formats/image/amiga/amiga_image.hh"

#include "tomb-excavator/formats/iff/iff_parser.hh"
#include "tomb-excavator/formats/iff/ea/ea.hh"
#include "tomb-excavator/formats/iff/ea/ea_events.hh"

#include <tomb-excavator/bsw/io/memory_stream_buf.hh>

namespace
{
    constexpr auto ILBM = formats::iff::fourcc("ILBM");
    constexpr auto PBM  = formats::iff::fourcc("PBM ");
    constexpr auto ACBM = formats::iff::fourcc("ACBM");
    // ===========================================================================================================

    class lbm_parser : public formats::iff::ea_events
    {
    public:
        std::vector<formats::image::amiga::image> m_images;
    private:
        void on_form_start(formats::iff::chunk_type name) override
        {
            using namespace formats::image::amiga;
            image_type type = image_type::ILBM;
            if (name == PBM) {
                type = image_type::PBM;
            } else if (name == ACBM) {
                type = image_type::ACBM;
            }  else if (name == ILBM) {
                type = image_type::ILBM;
            } else {
                RAISE_EX("Unknown IFF image type ", name);
            }
            m_images.emplace_back(type);
        };
        void on_form_end([[maybe_unused]] formats::iff::chunk_type name) override {};
        void on_chunk(std::istream& is, formats::iff::chunk_type type, [[maybe_unused]] uint64_t offset, std::size_t size) override
        {
            m_images.back().update(type, is, size);
        }
    private:

    };
}

namespace formats::image
{
    bool is_lbm(const char* input, std::size_t input_length)
    {
        using namespace formats::iff;
        ea_tester tester({
            ILBM,
            PBM,
            ACBM
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
        if (parser.m_images.empty())
        {
            return false;
        }
        return parser.m_images.front().convert(out);
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

