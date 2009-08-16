/* 
 * File:   xtet.hpp
 * Author: igorgu
 *
 * Created on 19 ������ 2009, 12:29
 */

#ifndef _XTET_HPP
#define	_XTET_HPP

#include "header.hpp"
#include "u0.hpp"

class xtet_c
{
public:
    xtet_c  ();
    ~xtet_c ();
    bool check_check_sum () const;

    friend std::ostream& operator << (std::ostream& os, const xtet_c& xtet);
private:
    std::ifstream m_ifs;
    header_c*     m_header;
private:
    /* 0Ch : pointer to filename */
    unsigned int m_chunks_num;  /* 14h */
    u0_c*        m_u0;          /* 18h : pointer to u0 */
    unsigned int dword_1ch;     /* 1Ch */
    unsigned int m_unk2;        /* 20h : unk2*/
    unsigned int dword_24h;     /* 24h */
    unsigned int dword_0ch;     /* 0Ch */
};


#endif	/* _XTET_HPP */

