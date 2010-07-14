/* 
 * File:   u0.hpp
 * Author: igorgu
 *
 * Created on 19 ������ 2009, 12:24
 */

#ifndef _U0_HPP
#define	_U0_HPP

#include "header.hpp"

class u0_c
{
public:
    u0_c (std::ifstream& ifs, const header_c& hdr);
    ~u0_c ();
private:
    char* m_buff;
    unsigned int m_num_items;
};



#endif	/* _U0_HPP */

