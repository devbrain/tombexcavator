#include <vector>
#include <sstream>

#include "formats/explode/mz/unexepack.hh"

#include "formats/explode/mz/struct_reader.hh"
#include "bsw/exceptions.hh"



namespace formats::explode::mz
{
	static const int unpackerDataLen = 0x12;
	static const int unpackerLen = 0x105; /* size of unpacker code */
	static const int errLen = 0x16;
	// ----------------------------------------------------------------------------------------
	unexepack::unexepack(input_exe_file& inp)
		: m_file(inp.file()),
		m_exe_file(inp)
	{
		const uint16_t ip = inp[exe_file::INITIAL_IP];
		if (ip != 0x10 && ip != 0x12 && ip != 0x14)
		{
			throw bsw::decoder_error ("unsupported version");
		}
		m_exe_data_start = inp[exe_file::HEADER_SIZE_PARA] * 16L;
		uint32_t var_2c = inp[exe_file::INITIAL_CS] * 16L;
		var_2c += m_exe_data_start;
		m_file.seek(var_2c);
		union
		{
			char* bytes;
			uint16_t* words;
		} u;
		u.words = m_header;
		m_file.read_buff (u.bytes, sizeof(m_header));
		for (int i = 0; i < eMAX_HEADER_VAL; i++)
		{
			m_header[i] = bsw::byte_order::from_little_endian(m_header[i]);
		}

		uint16_t sig_ptr = inp[exe_file::INITIAL_IP];
		sig_ptr += 0x0FFFE;
		sig_ptr = sig_ptr >> 1;

		uint16_t signature = u.words[sig_ptr];

		if (signature == 0x4252)
		{
			uint32_t len = 0;
			uint32_t var_36 = 0;
			// 10729
			if (inp[exe_file::INITIAL_IP] == 0x10)
			{
				// 10733
				uint16_t p = inp[exe_file::INITIAL_IP] >> 1;
				p = p << 1;
				uint16_t w = u.words[p];
				if (w == 0xe88b)
				{
					// 10750
					// second generation
					len = 0x132;
				}
				else
				{
					len = 0x125;
				}
			}
			if (inp[exe_file::INITIAL_IP] == 0x12)
			{
				len = 0x12d;
			}
			
			if (inp[exe_file::INITIAL_IP] == 0x14)
			{
				var_36 = 1;
				len = 0x131;
			}
			uint16_t lo = var_2c & 0xFFFF;
			uint16_t hi = (var_2c & 0xFFFF0000)>> 16;
			lo += (uint16_t)len;
			if (lo > 7)
			{
				hi++;
			}
			lo = lo + (uint16_t)0xFFF9;
			hi = hi + (uint16_t)0xFFFF;

			uint32_t p = ((uint32_t)hi << 16) | lo;
			m_file.seek(p);
			char ch;
			m_file.read_buff(&ch, 1);

			if (ch != 0x63)
			{
				throw bsw::decoder_error("not an exepack");
			}

			uint16_t ax = var_36;
			ax += 6;
			
			uint32_t uncompressed_size = ((uint32_t)u.words[ax] & 0xFFFF) << 4;
			uint32_t var_8 = ((uint32_t)inp[exe_file::INITIAL_CS] & 0xFFFF) << 4;
			uint32_t offs = ((uint32_t)inp[exe_file::HEADER_SIZE_PARA] & 0xFFFF) << 4;
			
			m_file.seek(offs);
			std::vector <uint8_t> ibuff(var_8);
			m_file.read_buff((char*)&ibuff[0], var_8);
			
			uint16_t bytes_in = var_8;

			


		}
		// goto 107f5
		
		return;
	}

	void unexepack::unpack(output_exe_file& oexe)
	{
	}

	uint32_t unexepack::decomp_size() const
	{
		return 16 * (uint32_t)m_header[eDEST_LEN];
	}

}
