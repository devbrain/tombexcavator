#include "archive.h"
namespace
{
    template <typename T>
    size_t ifs_read (std::ifstream& ifs, T& data)
    {
        ifs.read (reinterpret_cast <char*> (&data), sizeof (T));
        return sizeof (T);
    }
}
// ===========================================================================
off_t chaos_archive_c::fat_size () const
{
    return m_fat_size;
}
// ----------------------------------------------------------------------------
unsigned int chaos_archive_c::size () const
{
    return m_entries.size ();
}
// ----------------------------------------------------------------------------
const std::string& chaos_archive_c::entry_name (unsigned int id) const
{
    const entry_descriptor_s& e = m_entries [id];
    return e.m_name;
}
// ----------------------------------------------------------------------------
unsigned int chaos_archive_c::entry_id (const std::string& name) const
{
    const_name_itr_t i = m_names_map.find (name);
    if (i == m_names_map.end ())
    {
        return size ();
    }
    return i->second;
}
// ----------------------------------------------------------------------------
off_t chaos_archive_c::entry_offset (unsigned int id) const
{
    const entry_descriptor_s& e = m_entries [id];
    return e.m_offset;
}
// ----------------------------------------------------------------------------
off_t chaos_archive_c::entry_size (unsigned int id) const
{
    const entry_descriptor_s& e = m_entries [id];
    return e.m_size;
}
// ----------------------------------------------------------------------------
void chaos_archive_c::extract (const std::string& path,
                               unsigned int id) const
{
    std::string fname = path + "/" + entry_name (id);
    std::ofstream ofs (fname.c_str (), std::ios::out   |
                                       std::ios::trunc |
                                       std::ios::binary);
    if (!ofs.good ())
    {
        throw std::runtime_error (std::string ("Cant open output file ") + fname);
    }
    off_t size   = entry_size   (id);
    off_t offset = entry_offset (id);

    char* data = new char [size];
    m_chaos_ifs.seekg (offset);
    m_chaos_ifs.read (data, size);
    ofs.write (data, size);

    delete [] data;
    ofs.close ();
}
// ----------------------------------------------------------------------------
chaos_archive_c::chaos_archive_c (const std::string& path)
: c_chunk_size (0x400),
  m_arr_A (m_table, 1081, 626),
  m_arr_Z (m_table, 2333, 315),
  m_arr_D (m_table, 2963, 627),
  m_arr_X (m_table, 4217, 628)
{
    m_chunk_buff = new char [c_chunk_size];
    m_chunk_ptr = m_chunk_buff;
    m_has_bytes = 0;
    m_chaos_ifs.open (path.c_str (), std::ios::binary | std::ios::in);
    if (!m_chaos_ifs.good ())
    {
        throw std::runtime_error ("Failed to open input file");
    }

    types::dword fat_size;
    size_t bytes_processed = ifs_read (m_chaos_ifs, fat_size);
    m_fat_size = fat_size;

    unsigned int id = 0;

    while (bytes_processed < m_fat_size)
    {
        types::byte len;
        char name [256];
        types::dword x,y;
        bytes_processed += ifs_read (m_chaos_ifs, len);
        m_chaos_ifs.read (name, len);
        name [len] = '\0';
        bytes_processed += len;
        bytes_processed += ifs_read (m_chaos_ifs, x);
        bytes_processed += ifs_read (m_chaos_ifs, y);

        entry_descriptor_s e;
        e.m_name   = name;
        e.m_offset = x;
        e.m_size   = y;
        m_entries.push_back (e);
        m_names_map.insert (name_val_t (e.m_name, id++));
    }
}
// -------------------------------------------------------------------------------
types::reg16_t AX;
// -------------------------------------------------------------------------------
void chaos_archive_c::simul ()
{
    std::cout << "=============================================" << std::endl;
    const unsigned int id = entry_id ("ADLIB.DAT");
    const entry_descriptor_s& e = m_entries [id];
    std::cout << "SEEK: 0x" << std::hex << e.m_offset << std::endl;

    m_chaos_ifs.seekg (e.m_offset);

    types::reg16_t w1, w2;


    R_LOW  (w1) = _read_1024 ();
    R_HIGH (w1) = _read_1024 ();

    R_LOW  (w2) = _read_1024 ();
    R_HIGH (w2) = _read_1024 ();

    std::cout << "w1 = " << w1 << std::endl
              << "w2 = " << w2 << std::endl;


    if (w1 | w2)
    {
        std::cout << "Case 1" << std::endl;
        _build_table ();

        memarea_c di_1561;
        di_1561.fill_byte (0, 4036, 32);
        char di_1561h [4036];
        memset (di_1561h, 32, 4036);


        short int sf = w2 | w2;
        if (sf >= 0)
        {
                std::cout << "Case 1.2 : " << (sf | w1.data) << std::endl;
                if (sf | w1.data)
                {
                        std::cout << "Case 1.2.1" << std::endl;
                        AX.data = sf | w1.data;
                        _decode ();
                        m_table.dump ("simul",  0x38d3, 0x439, 0x2000);
                }
        }
    }
    else
    {
        std::cout << "Case 2" << std::endl;
    }
}
// -----------------------------------------------------------------------------
void chaos_archive_c::_decode ()
{
    types::byte ds_42bh = 0;

    types::reg16_t dx, bp;
    R_LOW (dx) = ds_42bh;
    unsigned short si = m_table.get_word (4215);
    unsigned short cx;
    // =======================================================================
    while (si < 627)
    {
        if (R_LOW (dx))
            {

              R_LOW (dx)--;
            }
            else
            {
                R_HIGH (bp) = _read_1024 ();
                R_LOW  (bp) = _read_1024 ();
                R_LOW  (dx) = 15;
            }
            if (bp.data & 32768) // get msb of bp
            {
                si ++; // adc si, 0
            }
            bp.data = bp.data << 1;

            si = si << 1;
            si = m_table.get_word (si + 2963);
    }

    ds_42bh = R_LOW (dx);
    si = si - 627;

    unsigned short stack_si = si;
    unsigned short stack_bp = bp.data;
    if (m_table.get_word (0x155d) == 32768)
    {
        _decode2 ();
    }

    si = si << 1;
    si = m_table.get_word ( si + 2335);

    cx = si;
    si = si << 1;
    do
    {
       unsigned short w = m_table.get_word (si + 4217);
       w++;
       m_table.set_word (si+4217, w);
       bp.data = w;
       unsigned short di = si;
       dx.data = cx;
       di++;
       di++;
       dx.data++;
       if (bp.data > m_table.get_word (di + 4217))
        {
            do
            {
              di++;
              di++;
            }
            while (bp.data > m_table.get_word (di + 4217) );

            di--;
            di--;
            dx.data = di;
            dx.data = dx.data >> 1;
            w = m_table.get_word (di + 4217);
            m_table.set_word (di + 4217, bp.data);
            bp.data = w;
            m_table.set_word (si + 4217, bp.data);
            unsigned short bx = m_table.get_word (si + 2963);
            bp.data = bx;
            bx = bx << 1;
            m_table.set_word (bx + 1081, dx.data);
            if (bp.data < 627)
            {
               m_table.set_word (bx + 1083, dx.data);
            }
            w = m_table.get_word (di + 2963);
            m_table.set_word (di + 2963, bp.data);
            bp.data = w;
            bx = bp.data;
            bx = bx << 1;
            m_table.set_word (bx + 1081, cx);
            if (bp.data < 627)
            {
               m_table.set_word (bx + 1083, cx);
            }
            m_table.set_word (si + 2963, bp.data);
            si = di;
            cx = dx.data;
        }

      si = m_table.get_word (si + 1081);
      cx = si;
      si = si << 1;
     }
     while (cx);
 }
// -----------------------------------------------------------------------------
types::byte chaos_archive_c::_read_1024 ()
{
    if (m_has_bytes == 0)
    {
        m_chaos_ifs.read  (m_chunk_buff, c_chunk_size);
        m_has_bytes = c_chunk_size;
    }

    types::byte al = m_chunk_ptr [0];
    m_chunk_ptr++;
    m_has_bytes--;
    return al;
}
// -----------------------------------------------------------------------------
void chaos_archive_c::_decode2 ()
{
    types::word cx = 0;
    types::word dx = 0;
    while (dx < 627)
    {
        types::word bx = dx << 1;
        if (m_table.get_word (bx + 2963) >= 627)
        {
            types::word ax = m_table.get_word (bx + 4217);
            ax++;
            ax = ax >> 1;
            bx = cx << 1;
            m_table.set_word (bx + 4217, ax);
            bx = dx << 1;
            ax = m_table.get_word (bx + 2963);
            bx = cx << 1;
            m_table.set_word (bx + 2963, ax);
            cx++;
        }
        else
        {
            dx++;
        }
    }
    dx = 0;
    cx = 314;
    while (cx < 627)
    {
        types::word bp = dx;
        bp++;
        types::word bx = dx;
        bx = bx << 1;
        types::word si = m_table.get_word (bx + 4217) + m_table.get_word (bx + 4219);
        bx = cx << 1;
        m_table.set_word (bx + 4217, si);
        bp = cx;
        bp--;
        types::word ax;
        do
        {
            bx = bp << 1;
            ax = m_table.get_word (bx + 4217);
            if (ax > si)
            {
                bp--;
            }
        } while (ax > si);
        bp++;
        types::word di = cx - bp;
        // push si
        // push cx
        types::word old_si = si;
        types::word old_cx = cx;

        si = bp + di;
        si = si << 1;
        bx = si;
        bx--;
        bx --;
        cx = 0;
        while (cx < di)
        {
            ax = m_table.get_word (bx + 4217);
            m_table.set_word (si + 4217, ax);

            ax = m_table.get_word (bx + 2963);
            m_table.set_word (si + 2963, ax);
            si--;
            si--;
            bx--;
            bx--;
            cx++;
        }
        si = old_si;
        cx = old_cx;
        bx = bp << 1;
        m_table.set_word (bx + 4217, si);
        m_table.set_word (bx + 2963, dx);
        dx = dx + 2;
        cx++;
    }
    dx = 0;
    while (dx < 627)
    {
        types::word bx = dx << 1;
        types::word bp = m_table.get_word (bx + 2963);
        if (bp < 627)
        {
            bx = bp << 1;
            m_table.set_word (bx + 1083, dx);
            m_table.set_word (bx + 1081, dx);
        }
        bx = bp << 1;
        m_table.set_word (bx + 1081, dx);
        dx++;
    }
}
// -----------------------------------------------------------------------------
void chaos_archive_c::_build_table ()
{

#if 0

    unsigned int t = 0;
    unsigned short x1 = 4217,x2;
    unsigned short y1 = 2963,y2;
    unsigned short z1 = 2335,z2;
    unsigned short cx = 0;
    while (cx < 314)
    {
        types::word word_pos = 2 * cx;

        x2 = x1 + word_pos;
        y2 = y1 + word_pos;
        z2 = z1 + word_pos;

        m_table.set_word (word_pos + 4217, 1);
        m_table.set_word (word_pos + 2963, cx + 627);
        m_table.set_word (word_pos + 2335, cx);
        cx++;
    }
    printf ("x: %d - %d\n", x1, x2);
    printf ("y: %d - %d\n", y1, y2);
    printf ("z: %d - %d\n", z1, z2);
    cx = 0;
    unsigned short a1 = 1083, a2;
    unsigned short b1 = 1081, b2;
    unsigned short c1 = 4217, c2;
    unsigned short d1 = 2963, d2;

    unsigned short dx = 314;
    while (dx <= 626)
    {
        types::word word_pos = 2 * cx;
        a2 = a1 + word_pos;
        b2 = b1 + word_pos;

        m_table.set_word (word_pos + 1083, dx);
        m_table.set_word (word_pos + 1081, dx);

        unsigned short a = word_pos + 4217;
        unsigned short b = word_pos + 4219;
        types::word ax = m_table.get_word (a);
        ax += m_table.get_word (b);

        t++;

        c2 = c1 + 2*dx;
        d2 = d1 + 2*dx;
        m_table.set_word ((2* dx) + 4217, ax);
        printf ("%d = %d | ", dx, ax);
        if (t%8 == 0)
        {
            printf ("\n");
        }
        m_table.set_word ((2* dx) + 2963, cx);

        cx = cx + 2;
        dx ++;
    }

    printf ("a: %d - %d\n", a1, a2);
    printf ("b: %d - %d\n", b1, b2);
    printf ("c: %d - %d\n", c1, c2);
    printf ("d: %d - %d\n", d1, d2);

    m_table.set_word (5471, 0xFFFF);
    m_table.set_word (2333, 0);
    m_table.dump ("justbuild",  0x38d3, 0x439, 0x2000);
    //m_table.dump ("justbuild",  0x38d3, 0x439, 0x1128);
#else
    // build array A
    for (unsigned short apos = 0; apos < 625; apos+=2)
    {
        m_arr_A.set (apos  , 314 + apos/2);
        m_arr_A.set (apos+1, 314 + apos/2);
    }
    // build array Z
    m_arr_Z.set (0,0);
    for (unsigned short zpos = 0; zpos < 314; zpos++)
    {
        m_arr_Z.set (zpos+1, zpos);
    }

    // build array D
    for (unsigned short ypos = 0; ypos < 314; ypos++)
    {
        m_arr_D.set (ypos, ypos + 627);
    }

    for (unsigned short dpos = 314; dpos <= 626; dpos++)
    {
        m_arr_D.set (dpos, 2*(dpos-314));
    }
    // build array X
    for (unsigned short xpos = 0; xpos < 314; xpos++)
    {
        m_arr_X.set (xpos, 1);
    }

    int t = 0;
    for (unsigned short cpos = 314; cpos <= 626; cpos++)
    {
        unsigned short a  = m_arr_X.get (t);
        a += m_arr_X.get (t + 1);
        m_arr_X.set (cpos , a);
        t+=2;
    }
    m_arr_X.set (627, 0xFFFF);
    m_table.dump ("arr",  0x38d3, 0x439, 0x2000);

#endif

}
