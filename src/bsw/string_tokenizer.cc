//
// Created by igor on 20/03/2021.
//

#include <tomb-excavator/bsw/string_tokenizer.hh>
#include <tomb-excavator/bsw/ascii.hh>
#include <algorithm>

namespace bsw
{
    string_tokenizer::string_tokenizer(const std::string& str, const std::string& separators, unsigned int options)
    {
        std::string::const_iterator it = str.begin();
        std::string::const_iterator end = str.end();
        std::string token;
        bool doTrim = ((options & TOK_TRIM) != 0);
        bool ignoreEmpty = ((options & TOK_IGNORE_EMPTY) != 0);
        bool lastToken = false;

        for (; it != end; ++it)
        {
            if (separators.find(*it) != std::string::npos)
            {
                if (doTrim)
                { trim(token); }
                if (!token.empty() || !ignoreEmpty)
                { m_tokens.push_back(token); }
                if (!ignoreEmpty)
                { lastToken = true; }
                token.clear();
            } else
            {
                token += *it;
                lastToken = false;
            }
        }

        if (!token.empty())
        {
            if (doTrim)
            { trim(token); }
            if (!token.empty() || !ignoreEmpty)
            { m_tokens.push_back(token); }
        } else
        {
            if (lastToken)
            {
                m_tokens.push_back(std::string());
            }
        }
    }
    // ----------------------------------------------------------------------------------------
    void string_tokenizer::trim(std::string& token)
    {
        std::string::size_type front = 0;
        std::string::size_type back = 0;
        std::string::size_type length = token.length();
        std::string::const_iterator tIt = token.begin();
        std::string::const_iterator tEnd = token.end();
        for (; tIt != tEnd; ++tIt, ++front)
        {
            if (!ascii::is_space(*tIt))
            { break; }
        }
        if (tIt != tEnd)
        {
            std::string::const_reverse_iterator tRit = token.rbegin();
            std::string::const_reverse_iterator tRend = token.rend();
            for (; tRit != tRend; ++tRit, ++back)
            {
                if (!ascii::is_space(*tRit))
                { break; }
            }
        }
        token = token.substr(front, length - back - front);
    }
    // ----------------------------------------------------------------------------------------
    std::size_t string_tokenizer::count(const std::string& token) const
    {
        std::size_t result = 0;
        auto it = std::find(m_tokens.begin(), m_tokens.end(), token);
        while (it != m_tokens.end())
        {
            result++;
            it = std::find(++it, m_tokens.end(), token);
        }
        return result;
    }
    // ----------------------------------------------------------------------------------------
    std::string::size_type string_tokenizer::find(const std::string& token, std::string::size_type pos) const
    {
        auto it = std::find(m_tokens.begin() + pos, m_tokens.end(), token);
        if (it != m_tokens.end())
        {
            return it - m_tokens.begin();
        }
        RAISE_EX("Token ", token, " not found");
    }
    // ----------------------------------------------------------------------------------------
    bool string_tokenizer::has(const std::string& token) const
    {
        auto it = std::find(m_tokens.begin(), m_tokens.end(), token);
        return it != m_tokens.end();
    }
    // ----------------------------------------------------------------------------------------
    std::size_t
    string_tokenizer::replace(const std::string& oldToken, const std::string& newToken, std::string::size_type pos)
    {
        std::size_t result = 0;
        auto it = std::find(m_tokens.begin() + pos, m_tokens.end(), oldToken);
        while (it != m_tokens.end())
        {
            result++;
            *it = newToken;
            it = std::find(++it, m_tokens.end(), oldToken);
        }
        return result;
    }

}