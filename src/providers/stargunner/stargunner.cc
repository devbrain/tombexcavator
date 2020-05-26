//
// Created by igor on 16/05/2020.
//

#include <bsw/errors.hh>
#include <set>
#include <cstring>
#include <archive/io.hh>
#include <archive/exceptions.hh>
#include <archive/byte_order.hh>

#include "stargunner.hh"

namespace stargunner
{
    fat_entry::fat_entry(std::size_t size, std::size_t out_size, const std::byte* stream) noexcept
    : m_size(size), m_out_size(out_size), m_stream(stream)
    {

    }
    // ----------------------------------------------------------------------------------------------
    fat_entry::fat_entry() noexcept
    : m_size (0), m_out_size(0), m_stream(nullptr)
    {

    }
    // ----------------------------------------------------------------------------------------------
    std::size_t fat_entry::size() const noexcept
    {
        return m_size;
    }
    // ----------------------------------------------------------------------------------------------
    std::size_t fat_entry::out_size() const noexcept
    {
        return m_out_size;
    }
    // ----------------------------------------------------------------------------------------------
    bool fat_entry::is_compressed() const noexcept
    {
        return m_out_size != m_size;
    }
    // ----------------------------------------------------------------------------------------------
    bool fat_entry::is_directory() const noexcept
    {
        return !m_stream;
    }
    // ----------------------------------------------------------------------------------------------
    const std::byte* fat_entry::stream () const noexcept
    {
        return m_stream;
    }
    // ==============================================================================================
    struct dlt_header
    {
        char signature[4];
        archive::le_uint16_t version;
        archive::le_uint16_t count;
    };
    // =========================================================================================================
    dlt_archive::dlt_archive (const std::string& path)
    : m_source(path)
    {
        archive::inmem_input input(reinterpret_cast<const unsigned char*>(m_source.data()), m_source.size());
        dlt_header header;
        input.read(header.signature);
        input >> header.version >> header.count;
        if (strncmp(header.signature, "DAVE", 4) != 0)
        {
            throw archive::decoder_error("Not a DLT file");
        }

        for (uint16_t file = 0; file < header.count.data; file++)
        {
            char name[32];
            input.read_buff(name, sizeof(name));
            for (int i = 1; i < 32; i++)
            {
                name[i] ^= name[i - 1] + i;
            }
            archive::le_uint32_t unknown;
            archive::le_uint32_t size;

            input >> unknown >> size;
            auto offset = input.tell();
        //    std::cout << name << std::endl;
            bool valid = false;
            if (size.data > 4)
            {
                const char* sig = reinterpret_cast<const char*>(input.data());
                 valid = strncmp(sig, "PGBP", 4) == 0;
            }
            if (valid)
            {
                input.seek(offset+4);
                archive::le_uint32_t out_size;
                input >> out_size;
                m_fat.insert(std::make_pair(std::string(name),
                                            fat_entry(static_cast<std::size_t>(size.data),
                                                      out_size.data,
                                                      reinterpret_cast<const std::byte*>(input.data()))));
            }
            if (file < header.count.data - 1)
            {
                input.seek(offset + size.data);
            }
        }
    }
    // ---------------------------------------------------------------------------------------------------------
    static std::string sub_path(const std::string& prefix, const std::string& name)
    {
        if (prefix.empty())
        {
            return name;
        }
        std::string dir_prefix = prefix + "\\";
        auto idx = name.rfind(dir_prefix, 0);
        if (idx == 0)
        {
            return name.substr(dir_prefix.size());
        }
        return "";
    }
    // ---------------------------------------------------------------------------------------------------------
    std::map<std::string, fat_entry> dlt_archive::scan(const std::string& directory) const
    {
        std::map<std::string, fat_entry> res;
        std::set<std::string> subdirs;
        for (const auto& kv : m_fat)
        {
            auto name = sub_path(directory, kv.first);
            if (name.empty())
            {
                continue;
            }
            auto sep_idx = name.find("\\");
            if (sep_idx == std::string::npos)
            {
                res.insert(std::make_pair(name, kv.second));
            }
            else
            {
                std::string dir_name = name.substr(0, sep_idx);
                if (subdirs.find(dir_name) == subdirs.end())
                {
                    subdirs.insert(dir_name);
                    res.insert(std::make_pair(dir_name, fat_entry()));
                }
            }
        }
        return res;
    }
    // ----------------------------------------------------------------------------
    /// Chunk size used during compression.  Each chunk expands to this amount of data.
    static constexpr std::size_t CHUNK_SIZE = 4096;

    static unsigned int decompress_chunk(const unsigned char* in, size_t expanded_size, unsigned char* out)
    {
        uint8_t tableA[256], tableB[256];
        unsigned int inpos = 0;
        unsigned int outpos = 0;

        while (outpos < expanded_size) {
            // Initialise the dictionary so that no bytes are codewords (or if you
            // prefer, each byte expands to itself only.)
            for (int i = 0; i < 256; i++) tableA[i] = i;

            //
            // Read in the dictionary
            //

            uint8_t code;
            unsigned int tablepos = 0;
            do {
                code = in[inpos++];

                // If the code has the high bit set, the lower 7 bits plus one is the
                // number of codewords that will be skipped from the dictionary.  (Those
                // codewords were initialised to expand to themselves in the loop above.)
                if (code > 127) {
                    tablepos += code - 127;
                    code = 0;
                }
                if (tablepos == 256) break;

                // Read in the indicated number of codewords.
                for (int i = 0; i <= code; i++) {
                    ENFORCE(tablepos < 256);
                    uint8_t data = in[inpos++];
                    tableA[tablepos] = data;
                    if (tablepos != data) {
                        // If this codeword didn't expand to itself, store the second byte
                        // of the expansion pair.
                        tableB[tablepos] = in[inpos++];
                    }
                    tablepos++;
                }
            } while (tablepos < 256);

            // Read the length of the data encoded with this dictionary
            int len = in[inpos++];
            len |= in[inpos++] << 8;

            //
            // Decompress the data
            //

            int expbufpos = 0;
            // This is the maximum number of bytes a single codeword can expand to.
            uint8_t expbuf[32];
            while (1) {
                if (expbufpos) {
                    // There is data in the expansion buffer, use that
                    code = expbuf[--expbufpos];
                }
                else {
                    // There is no data in the expansion buffer, use the input data
                    if (--len == -1) break; // no more input data
                    code = in[inpos++];
                }

                if (code == tableA[code]) {
                    // This byte is itself, write this to the output
                    out[outpos++] = code;
                }
                else {
                    // This byte is actually a codeword, expand it into the expansion buffer
                    ENFORCE(expbufpos < (signed)sizeof(expbuf) - 2);
                    expbuf[expbufpos++] = tableB[code];
                    expbuf[expbufpos++] = tableA[code];
                }
            }
        }
        return outpos - expanded_size;
    }

    void dlt_archive::decompress(const fat_entry& fe, std::vector<std::byte>& out)
    {
        out.resize(fe.out_size());
        if (!fe.is_compressed())
        {
            std::memcpy(out.data(), fe.stream(), fe.size());
        }
        else
        {
            archive::inmem_input input(reinterpret_cast<const unsigned char*>(fe.stream()), fe.size());
            std::size_t has_bytes = 0;
            std::size_t remains = fe.out_size();
            while (remains > 0)
            {
                archive::le_uint16_t chunk_size;
                input >> chunk_size;
                auto offset = input.tell();
                std::size_t chunk_out_size = remains;
                if (remains > CHUNK_SIZE)
                {
                    chunk_out_size = CHUNK_SIZE;
                } else{
                    chunk_out_size = remains;
                }

                auto undecoded = decompress_chunk(input.data(), chunk_out_size, reinterpret_cast<unsigned char*>(out.data()) + has_bytes);
                ENFORCE(undecoded == 0);
                input.seek(offset+chunk_size.data);
                has_bytes += chunk_out_size;
                remains -= chunk_out_size;
            }
        }
    }
    // --------------------------------------------------------------------------------------
    fat_entry dlt_archive::get_entry(const std::string& name)
    {
        return m_fat.find(name)->second;
    }
    // ======================================================================================
    std::vector<std::vector<int16_t >> load_animation_sequences(archive::input& input)
    {
        archive::le_uint32_t sprites, data_size;
        input >> sprites >> data_size;
        std::vector<uint32_t> offsets(sprites.data);
        size_t sz = 0;
        for (std::size_t i=0; i<offsets.size(); i++)
        {
            archive::le_uint32_t size;
            input >> size;
            offsets[i] = size.data;
        }
        offsets.push_back(data_size.data);
        std::vector<std::vector<int16_t >> res;
        size_t start = input.tell();
        for (uint32_t i=0; i<sprites.data; i++)
        {
            uint32_t start_offset = offsets[i];
            input.seek(start + start_offset);
            std::vector<int16_t> group;
            while (true)
            {
                archive::le_int16_t idx;
                input >> idx;
                int16_t value = idx.data;
                if (value == -1)
                {
                    break;
                }
                group.push_back(value);
            }
            res.push_back(group);
        }
        return res;
    }
} // ns stargunner