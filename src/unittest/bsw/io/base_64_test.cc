//
// Created by igor on 04/03/2021.
//

#include <sstream>
#include <stdexcept>
#include <doctest.h>

#include "tomb-excavator/bsw/io/base64.hh"
#define REQUIRE_S(cond)                 \
    do                                  \
    {                                   \
        if (!(cond))                    \
        {                               \
            REQUIRE(false);             \
        }                               \
    } while(false)

TEST_SUITE("Base64 Encoder")
{

    TEST_CASE("testEncoder")
    {
        {
            std::ostringstream str;
            bsw::io::base64_encoder encoder(str);
            encoder << std::string("\00\01\02\03\04\05", 6);
            encoder.close();
            REQUIRE(str.str() == "AAECAwQF");
        }
        {
            std::ostringstream str;
            bsw::io::base64_encoder encoder(str);
            encoder << std::string("\00\01\02\03", 4);
            encoder.close();
            REQUIRE(str.str() == "AAECAw==");
        }
        {
            std::ostringstream str;
            bsw::io::base64_encoder encoder(str);
            encoder << "ABCDEF";
            encoder.close();
            REQUIRE(str.str() == "QUJDREVG");
        }
        {
            std::ostringstream str;
            bsw::io::base64_encoder encoder(str);
            encoder << "!@#$%^&*()_~<>";
            encoder.close();
            REQUIRE(str.str() == "IUAjJCVeJiooKV9+PD4=");
        }
    }

    TEST_CASE("testEncoderURL")
    {
        {
            std::ostringstream str;
            bsw::io::base64_encoder encoder(str, bsw::io::BASE64_URL_ENCODING);
            encoder << std::string("\00\01\02\03\04\05", 6);
            encoder.close();
            REQUIRE(str.str() == "AAECAwQF");
        }
        {
            std::ostringstream str;
            bsw::io::base64_encoder encoder(str, bsw::io::BASE64_URL_ENCODING);
            encoder << std::string("\00\01\02\03", 4);
            encoder.close();
            REQUIRE(str.str() == "AAECAw==");
        }
        {
            std::ostringstream str;
            bsw::io::base64_encoder encoder(str, bsw::io::BASE64_URL_ENCODING);
            encoder << "ABCDEF";
            encoder.close();
            REQUIRE(str.str() == "QUJDREVG");
        }
        {
            std::ostringstream str;
            bsw::io::base64_encoder encoder(str, bsw::io::BASE64_URL_ENCODING);
            encoder << "!@#$%^&*()_~<>";
            encoder.close();
            REQUIRE(str.str() == "IUAjJCVeJiooKV9-PD4=");
        }
    }

    TEST_CASE("testEncoderNoPadding")
    {
        {
            std::ostringstream str;
            bsw::io::base64_encoder encoder(str, bsw::io::BASE64_URL_ENCODING | bsw::io::BASE64_NO_PADDING);
            encoder << std::string("\00\01\02\03\04\05", 6);
            encoder.close();
            REQUIRE(str.str() == "AAECAwQF");
        }
        {
            std::ostringstream str;
            bsw::io::base64_encoder encoder(str, bsw::io::BASE64_URL_ENCODING | bsw::io::BASE64_NO_PADDING);
            encoder << std::string("\00\01\02\03", 4);
            encoder.close();
            REQUIRE(str.str() == "AAECAw");
        }
        {
            std::ostringstream str;
            bsw::io::base64_encoder encoder(str, bsw::io::BASE64_URL_ENCODING | bsw::io::BASE64_NO_PADDING);
            encoder << "ABCDEF";
            encoder.close();
            REQUIRE(str.str() == "QUJDREVG");
        }
        {
            std::ostringstream str;
            bsw::io::base64_encoder encoder(str, bsw::io::BASE64_URL_ENCODING | bsw::io::BASE64_NO_PADDING);
            encoder << "!@#$%^&*()_~<>";
            encoder.close();
            REQUIRE(str.str() == "IUAjJCVeJiooKV9-PD4");
        }
    }
}

TEST_SUITE("Base64 Decoder")
{
    TEST_CASE("testDecoder")
    {
        {
            std::istringstream istr("AAECAwQF");
            bsw::io::base64_decoder decoder(istr);
            REQUIRE_S(decoder.good() && decoder.get() == 0);
            REQUIRE_S(decoder.good() && decoder.get() == 1);
            REQUIRE_S(decoder.good() && decoder.get() == 2);
            REQUIRE_S(decoder.good() && decoder.get() == 3);
            REQUIRE_S(decoder.good() && decoder.get() == 4);
            REQUIRE_S(decoder.good() && decoder.get() == 5);
            REQUIRE_S(decoder.good() && decoder.get() == -1);
        }
        {
            std::istringstream istr("AAECAwQ=");
            bsw::io::base64_decoder decoder(istr);
            REQUIRE_S(decoder.good() && decoder.get() == 0);
            REQUIRE_S(decoder.good() && decoder.get() == 1);
            REQUIRE_S(decoder.good() && decoder.get() == 2);
            REQUIRE_S(decoder.good() && decoder.get() == 3);
            REQUIRE_S(decoder.good() && decoder.get() == 4);
            REQUIRE_S(decoder.good() && decoder.get() == -1);
        }
        {
            std::istringstream istr("AAECAw==");
            bsw::io::base64_decoder decoder(istr);
            REQUIRE_S(decoder.good() && decoder.get() == 0);
            REQUIRE_S(decoder.good() && decoder.get() == 1);
            REQUIRE_S(decoder.good() && decoder.get() == 2);
            REQUIRE_S(decoder.good() && decoder.get() == 3);
            REQUIRE_S(decoder.good() && decoder.get() == -1);
        }
        {
            std::istringstream istr("QUJDREVG");
            bsw::io::base64_decoder decoder(istr);
            std::string s;
            decoder >> s;
            REQUIRE(s == "ABCDEF");
            REQUIRE(decoder.eof());
            REQUIRE(!decoder.fail());
        }
        {
            std::istringstream istr("QUJ\r\nDRE\r\nVG");
            bsw::io::base64_decoder decoder(istr);
            std::string s;
            decoder >> s;
            REQUIRE(s == "ABCDEF");
            REQUIRE(decoder.eof());
            REQUIRE(!decoder.fail());
        }
        {
            std::istringstream istr("QUJD#REVG");
            bsw::io::base64_decoder decoder(istr);
            std::string s;
            try
            {
                decoder >> s;
                REQUIRE(decoder.bad());
            }
            catch (std::runtime_error&)
            {
            }
            REQUIRE(!decoder.eof());
        }
    }

    TEST_CASE("testDecoderURL")
    {
        {
            std::istringstream istr("AAECAwQF");
            bsw::io::base64_decoder decoder(istr, bsw::io::BASE64_URL_ENCODING);
            REQUIRE_S(decoder.good() && decoder.get() == 0);
          //  auto x = decoder.good();
          //  auto y = decoder.get();
            REQUIRE_S(decoder.good() && decoder.get() == 1);
            REQUIRE_S(decoder.good() && decoder.get() == 2);
            REQUIRE_S(decoder.good() && decoder.get() == 3);
            REQUIRE_S(decoder.good() && decoder.get() == 4);
            REQUIRE_S(decoder.good() && decoder.get() == 5);
            REQUIRE_S(decoder.good() && decoder.get() == -1);
        }
        {
            std::istringstream istr("AAECAwQ=");
            bsw::io::base64_decoder decoder(istr, bsw::io::BASE64_URL_ENCODING);
            REQUIRE_S(decoder.good() && decoder.get() == 0);
            REQUIRE_S(decoder.good() && decoder.get() == 1);
            REQUIRE_S(decoder.good() && decoder.get() == 2);
            REQUIRE_S(decoder.good() && decoder.get() == 3);
            REQUIRE_S(decoder.good() && decoder.get() == 4);
            REQUIRE_S(decoder.good() && decoder.get() == -1);
        }
        {
            std::istringstream istr("AAECAw==");
            bsw::io::base64_decoder decoder(istr, bsw::io::BASE64_URL_ENCODING);
            REQUIRE_S(decoder.good() && decoder.get() == 0);
            REQUIRE_S(decoder.good() && decoder.get() == 1);
            REQUIRE_S(decoder.good() && decoder.get() == 2);
            REQUIRE_S(decoder.good() && decoder.get() == 3);
            REQUIRE_S(decoder.good() && decoder.get() == -1);
        }
        {
            std::istringstream istr("QUJDREVG");
            bsw::io::base64_decoder decoder(istr, bsw::io::BASE64_URL_ENCODING);
            std::string s;
            decoder >> s;
            REQUIRE(s == "ABCDEF");
            REQUIRE(decoder.eof());
            REQUIRE(!decoder.fail());
        }
        {
            std::istringstream istr("QUJ\r\nDRE\r\nVG");
            bsw::io::base64_decoder decoder(istr, bsw::io::BASE64_URL_ENCODING);
            try
            {
                std::string s;
                decoder >> s;
                REQUIRE(decoder.bad());
            }
            catch (std::runtime_error&)
            {
            }
        }
        {
            std::istringstream istr("QUJD#REVG");
            bsw::io::base64_decoder decoder(istr, bsw::io::BASE64_URL_ENCODING);
            std::string s;
            try
            {
                decoder >> s;
                REQUIRE(decoder.bad());
            }
            catch (std::runtime_error&)
            {
            }
            REQUIRE(!decoder.eof());
        }
        {
            std::istringstream istr("IUAjJCVeJiooKV9-PD4=");
            bsw::io::base64_decoder decoder(istr, bsw::io::BASE64_URL_ENCODING);
            std::string s;
            decoder >> s;
            REQUIRE(s == "!@#$%^&*()_~<>");
            REQUIRE(decoder.eof());
            REQUIRE(!decoder.fail());
        }
    }

    TEST_CASE("testDecoderNoPadding")
    {
        {
            std::istringstream istr("AAECAwQF");
            bsw::io::base64_decoder decoder(istr, bsw::io::BASE64_NO_PADDING);
            REQUIRE_S(decoder.good() && decoder.get() == 0);
            REQUIRE_S(decoder.good() && decoder.get() == 1);
            REQUIRE_S(decoder.good() && decoder.get() == 2);
            REQUIRE_S(decoder.good() && decoder.get() == 3);
            REQUIRE_S(decoder.good() && decoder.get() == 4);
            REQUIRE_S(decoder.good() && decoder.get() == 5);
            REQUIRE_S(decoder.good() && decoder.get() == -1);
        }
        {
            std::istringstream istr("AAECAwQ");
            bsw::io::base64_decoder decoder(istr, bsw::io::BASE64_NO_PADDING);
            REQUIRE_S(decoder.good() && decoder.get() == 0);
            REQUIRE_S(decoder.good() && decoder.get() == 1);
            REQUIRE_S(decoder.good() && decoder.get() == 2);
            REQUIRE_S(decoder.good() && decoder.get() == 3);
            REQUIRE_S(decoder.good() && decoder.get() == 4);
            REQUIRE_S(decoder.good() && decoder.get() == -1);
        }
        {
            std::istringstream istr("AAECAw");
            bsw::io::base64_decoder decoder(istr, bsw::io::BASE64_NO_PADDING);
            REQUIRE_S(decoder.good() && decoder.get() == 0);
            REQUIRE_S(decoder.good() && decoder.get() == 1);
            REQUIRE_S(decoder.good() && decoder.get() == 2);
            REQUIRE_S(decoder.good() && decoder.get() == 3);
            REQUIRE_S(decoder.good() && decoder.get() == -1);
        }
    }

    TEST_CASE("testEncodeDecode")
    {
        {
            std::stringstream str;
            bsw::io::base64_encoder encoder(str);
            encoder << "The quick brown fox ";
            encoder << "jumped over the lazy dog.";
            encoder.close();
            bsw::io::base64_decoder decoder(str);
            std::string s;
            int c = decoder.get();
            while (c != -1)
            {
                s += char(c);
                c = decoder.get();
            }
            REQUIRE(s == "The quick brown fox jumped over the lazy dog.");
        }
        {
            std::string src;
            for (int i = 0; i < 255; ++i) src += char(i);
            std::stringstream str;
            bsw::io::base64_encoder encoder(str);
            encoder.write(src.data(), (std::streamsize) src.size());
            encoder.close();
            bsw::io::base64_decoder decoder(str);
            std::string s;
            int c = decoder.get();
            while (c != -1)
            {
                s += char(c);
                c = decoder.get();
            }
            REQUIRE(s == src);
        }
    }

}
