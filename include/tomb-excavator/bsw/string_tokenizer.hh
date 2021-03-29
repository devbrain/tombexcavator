//
// Created by igor on 20/03/2021.
//

#ifndef TOMBEXCAVATOR_STRING_TOKENIZER_HH
#define TOMBEXCAVATOR_STRING_TOKENIZER_HH

#include <tomb-excavator/export-bsw.h>
#include <tomb-excavator/bsw/exceptions.hh>

#include <string>
#include <vector>

#include <tomb-excavator/msvc/c4251-begin.h>
namespace bsw
{
    class BSW_API string_tokenizer
        /// A simple tokenizer that splits a string into
        /// tokens, which are separated by separator characters.
        /// An iterator is used to iterate over all tokens.
    {
    public:
        enum options
        {
            TOK_IGNORE_EMPTY = 1u, /// ignore empty tokens
            TOK_TRIM = 2u  /// remove leading and trailing whitespace from tokens
        };

        typedef std::vector<std::string> token_vec_t;
        typedef token_vec_t::const_iterator iterator;

        string_tokenizer(const std::string& str, const std::string& separators, unsigned int options = 0);
        /// Splits the given string into tokens. The tokens are expected to be
        /// separated by one of the separator characters given in separators.
        /// Additionally, options can be specified:
        ///   * TOK_IGNORE_EMPTY: empty tokens are ignored
        ///   * TOK_TRIM: trailing and leading whitespace is removed from tokens.


        [[nodiscard]] iterator begin() const;
        [[nodiscard]] iterator end() const;

        const std::string& operator[](std::size_t index) const;
        /// Returns const reference the index'th token.
        /// Throws a RangeException if the index is out of range.

        std::string& operator[](std::size_t index);
        /// Returns reference to the index'th token.
        /// Throws a RangeException if the index is out of range.

        [[nodiscard]] bool has(const std::string& token) const;
        /// Returns true if token exists, false otherwise.

        std::string::size_type find(const std::string& token, std::string::size_type pos = 0) const;
        /// Returns the index of the first occurrence of the token
        /// starting at position pos.
        /// Throws a NotFoundException if the token is not found.

        std::size_t replace(const std::string& oldToken, const std::string& newToken, std::string::size_type pos = 0);
        /// Starting at position pos, replaces all subsequent tokens having value
        /// equal to oldToken with newToken.
        /// Returns the number of modified tokens.

        [[nodiscard]] std::size_t count() const;
        /// Returns the total number of tokens.

        [[nodiscard]] std::size_t count(const std::string& token) const;
        /// Returns the number of tokens equal to the specified token.

    private:
        static void trim(std::string& token);

        token_vec_t m_tokens;
    };


//
// inlines
//
    inline string_tokenizer::iterator string_tokenizer::begin() const
    {
        return m_tokens.begin();
    }
    // ----------------------------------------------------------------------------------------
    inline string_tokenizer::iterator string_tokenizer::end() const
    {
        return m_tokens.end();
    }
    // ----------------------------------------------------------------------------------------
    inline std::string& string_tokenizer::operator[](std::size_t index)
    {
        if (index >= m_tokens.size())
        {
            RAISE_EX("Out of range");
        }

        return m_tokens[index];
    }
    // ----------------------------------------------------------------------------------------
    inline const std::string& string_tokenizer::operator[](std::size_t index) const
    {
        if (index >= m_tokens.size())
        {
            RAISE_EX("Out of range");
        }

        return m_tokens[index];
    }
    // ----------------------------------------------------------------------------------------
    inline std::size_t string_tokenizer::count() const
    {
        return m_tokens.size();
    }
}

#include <tomb-excavator/msvc/c4251-end.h>
#endif //TOMBEXCAVATOR_STRING_TOKENIZER_HH
