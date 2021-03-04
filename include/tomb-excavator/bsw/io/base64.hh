//
// Created by igor on 04/03/2021.
//

#ifndef TOMBEXCAVATOR_BASE64_HH
#define TOMBEXCAVATOR_BASE64_HH

#include <tomb-excavator/export-bsw.h>
#include <tomb-excavator/bsw/io/unbuffered_stream_buf.hh>
#include <istream>
#include <ostream>

namespace bsw::io
{
    enum base64_encoding_options
    {
        BASE64_URL_ENCODING = 0x01,
        /// Use the URL and filename-safe alphabet,
        /// replacing '+' with '-' and '/' with '_'.
        ///
        /// Will also set line length to unlimited.

        BASE64_NO_PADDING = 0x02
        /// Do not append padding characters ('=') at end.
    };

    class BSW_API base64_encoder_buf : public unbuffered_stream_buf
        /// This streambuf base64-encodes all data written
        /// to it and forwards it to a connected
        /// ostream.
        ///
        /// Note: The characters are directly written
        /// to the ostream's streambuf, thus bypassing
        /// the ostream. The ostream's state is therefore
        /// not updated to match the buffer's state.
    {
    public:
        explicit base64_encoder_buf(std::ostream& ostr, int options = 0);
        ~base64_encoder_buf() override;

        int close();
        /// Closes the stream buffer.

        void set_line_length(int lineLength);
        /// Specify the line length.
        ///
        /// After the given number of characters have been written,
        /// a newline character will be written.
        ///
        /// Specify 0 for an unlimited line length.

        [[nodiscard]] int get_line_length() const;
        /// Returns the currently set line length.

    private:
        int write_to_device(char c) override;

        int m_options;
        unsigned char m_group[3];
        int m_group_length;
        int m_pos;
        int m_line_length;
        std::streambuf& m_buf;
        const unsigned char* m_out_enc;

        base64_encoder_buf(const base64_encoder_buf&);
        base64_encoder_buf& operator=(const base64_encoder_buf&);
    };

    class BSW_API base64_encoder_ios : public virtual std::ios
/// The base class for Base64Encoder.
///
/// This class is needed to ensure the correct initialization
/// order of the stream buffer and base classes.
    {
    public:
        explicit base64_encoder_ios(std::ostream& ostr, int options = 0);
        ~base64_encoder_ios() override;
        int close();
        base64_encoder_buf* rdbuf();

    protected:
        base64_encoder_buf m_buf;

    private:
        base64_encoder_ios(const base64_encoder_ios&);
        base64_encoder_ios& operator=(const base64_encoder_ios&);
    };

    class BSW_API base64_encoder : public base64_encoder_ios, public std::ostream
/// This ostream base64-encodes all data
/// written to it and forwards it to
/// a connected ostream.
/// Always call close() when done
/// writing data, to ensure proper
/// completion of the encoding operation.
///
/// The class implements RFC 4648 - https://tools.ietf.org/html/rfc4648
///
/// Note: The characters are directly written
/// to the ostream's streambuf, thus bypassing
/// the ostream. The ostream's state is therefore
/// not updated to match the buffer's state.
    {
    public:
        explicit base64_encoder(std::ostream& ostr, int options = 0);
        ~base64_encoder() override;

    private:
        base64_encoder(const base64_encoder&);
        base64_encoder& operator=(const base64_encoder&);
    };

    class BSW_API base64_decoder_buf : public unbuffered_stream_buf
        /// This streambuf base64-decodes all data read
        /// from the istream connected to it.
        ///
        /// Note: For performance reasons, the characters
        /// are read directly from the given istream's
        /// underlying streambuf, so the state
        /// of the istream will not reflect that of
        /// its streambuf.
    {
    public:
        explicit base64_decoder_buf(std::istream& istr, int options = 0);
        ~base64_decoder_buf() override;

    private:
        int read_from_device() override;
        int read_one();

        int m_options;
        unsigned char m_group[3];
        int m_group_length;
        int m_group_index;
        std::streambuf& m_buf;
        const unsigned char* m_in_enc;

    private:
        base64_decoder_buf(const base64_decoder_buf&);
        base64_decoder_buf& operator=(const base64_decoder_buf&);
    };

    class BSW_API base64_decoder_ios : public virtual std::ios
/// The base class for Base64Decoder.
///
/// This class is needed to ensure the correct initialization
/// order of the stream buffer and base classes.
    {
    public:
        explicit base64_decoder_ios(std::istream& istr, int options = 0);
        ~base64_decoder_ios() override;
        base64_decoder_buf* rdbuf();

    protected:
        base64_decoder_buf m_buf;

    private:
        base64_decoder_ios(const base64_decoder_ios&);
        base64_decoder_ios& operator=(const base64_decoder_ios&);
    };

    class BSW_API base64_decoder : public base64_decoder_ios, public std::istream
/// This istream base64-decodes all data
/// read from the istream connected to it.
///
/// The class implements RFC 4648 - https://tools.ietf.org/html/rfc4648
///
/// Note: For performance reasons, the characters
/// are read directly from the given istream's
/// underlying streambuf, so the state
/// of the istream will not reflect that of
/// its streambuf.
    {
    public:
        explicit base64_decoder(std::istream& istr, int options = 0);
        ~base64_decoder() override;
    private:
        base64_decoder(const base64_decoder&);
        base64_decoder& operator=(const base64_decoder&);
    };


}

#endif //TOMBEXCAVATOR_BASE64_HH
