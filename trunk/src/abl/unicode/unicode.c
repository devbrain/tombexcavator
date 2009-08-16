#include <stdlib.h>
#include "abl/unicode/unicode.h"

static const char utf8_skip_data[256] = 
  {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
  };

const char * const s_utf8_skip = utf8_skip_data;

#define s_utf8_next_char(p) (char *)((p) + s_utf8_skip[*(const unsigned char *)(p)]) 

#define SURROGATE_VALUE(h,l) (((h) - 0xd800) * 0x400 + (l) - 0xdc00 + 0x10000)


#define UTF8_LENGTH(Char)			\
  ((Char) < 0x80 ? 1 :				\
   ((Char) < 0x800 ? 2 :			\
    ((Char) < 0x10000 ? 3 :			\
     ((Char) < 0x200000 ? 4 :			\
      ((Char) < 0x4000000 ? 5 : 6)))))

static ucs4_t
s_utf8_get_char_extended (const char *p,
			  unsigned long max_len)  
{
  unsigned int i, len;
  ucs4_t wc = (unsigned char) *p;

  if (wc < 0x80)
    {
      return wc;
    }
  else if (wc < 0xc0)
    {
      return (ucs4_t)-1;
    }
  else if (wc < 0xe0)
    {
      len = 2;
      wc &= 0x1f;
    }
  else if (wc < 0xf0)
    {
      len = 3;
      wc &= 0x0f;
    }
  else if (wc < 0xf8)
    {
      len = 4;
      wc &= 0x07;
    }
  else if (wc < 0xfc)
    {
      len = 5;
      wc &= 0x03;
    }
  else if (wc < 0xfe)
    {
      len = 6;
      wc &= 0x01;
    }
  else
    {
      return (ucs4_t)-1;
    }
  
  if (max_len >= 0 && len > max_len)
    {
      for (i = 1; i < max_len; i++)
	{
	  if ((((unsigned char *)p)[i] & 0xc0) != 0x80)
	    return (ucs4_t)-1;
	}
      return (ucs4_t)-2;
    }

  for (i = 1; i < len; ++i)
    {
      ucs4_t ch = ((unsigned char *)p)[i];
      
      if ((ch & 0xc0) != 0x80)
	{
	  if (ch)
	    return (ucs4_t)-1;
	  else
	    return (ucs4_t)-2;
	}

      wc <<= 6;
      wc |= (ch & 0x3f);
    }

  if (UTF8_LENGTH(wc) != len)
    return (ucs4_t)-1;
  
  return wc;
}

#define UTF8_GET(Result, Chars, Count, Mask, Len)	\
  (Result) = (Chars)[0] & (Mask);			\
  for ((Count) = 1; (Count) < (Len); ++(Count))		\
    {							\
      if (((Chars)[(Count)] & 0xc0) != 0x80)		\
	{						\
	  (Result) = -1;				\
	  break;					\
	}						\
      (Result) <<= 6;					\
      (Result) |= ((Chars)[(Count)] & 0x3f);		\
    }

#define UTF8_COMPUTE(Char, Mask, Len)		\
  if (Char < 128)				\
    {						\
      Len = 1;					\
      Mask = 0x7f;				\
    }						\
  else if ((Char & 0xe0) == 0xc0)		\
    {						\
      Len = 2;					\
      Mask = 0x1f;				\
    }						\
  else if ((Char & 0xf0) == 0xe0)		\
    {						\
      Len = 3;					\
      Mask = 0x0f;				\
    }						\
  else if ((Char & 0xf8) == 0xf0)		\
    {						\
      Len = 4;					\
      Mask = 0x07;				\
    }						\
  else if ((Char & 0xfc) == 0xf8)		\
    {						\
      Len = 5;					\
      Mask = 0x03;				\
    }						\
  else if ((Char & 0xfe) == 0xfc)		\
    {						\
      Len = 6;					\
      Mask = 0x01;				\
    }						\
  else						\
    Len = -1;


/* --------------------------------------------------------------------------- */
static ucs4_t s_utf8_get_char (const char *p)
{
  int i, mask = 0, len;
  ucs4_t result;
  unsigned char c = (unsigned char) *p;

  UTF8_COMPUTE (c, mask, len);
  if (len == -1)
    return (ucs4_t)-1;
  UTF8_GET (result, p, i, mask, len);

  return result;
}

/* --------------------------------------------------------------------------- */
static int s_ucs4_to_utf8 (ucs4_t c, char  *outbuf)
{
  /* If this gets modified, also update the copy in g_string_insert_unichar() */
  unsigned int len = 0;    
  int first;
  int i;

  if (c < 0x80)
    {
      first = 0;
      len = 1;
    }
  else if (c < 0x800)
    {
      first = 0xc0;
      len = 2;
    }
  else if (c < 0x10000)
    {
      first = 0xe0;
      len = 3;
    }
  else if (c < 0x200000)
    {
      first = 0xf0;
      len = 4;
    }
  else if (c < 0x4000000)
    {
      first = 0xf8;
      len = 5;
    }
  else
    {
      first = 0xfc;
      len = 6;
    }

  if (outbuf)
    {
      for (i = len - 1; i > 0; --i)
	{
	  outbuf[i] = (c & 0x3f) | 0x80;
	  c >>= 6;
	}
      outbuf[0] = c | first;
    }

  return len;
}

/* --------------------------------------------------------------------------- */

ucs16_t* utf8_to_utf16 (const char* str , long len,            
			long* items_read, long* items_written)
{
  ucs16_t *result = NULL;
  int n16;
  const char *in;
  int i;

  if (!str)
    {
      return 0;
    }

  in = str;
  n16 = 0;
  while ((len < 0 || str + len - in > 0) && *in)
    {
      ucs4_t wc = s_utf8_get_char_extended (in, len < 0 ? 6 : str + len - in);
      if (wc & 0x80000000)
	{
	  if (wc == (ucs4_t)-2)
	    {
	      if (items_read)
		break;
	    }
	  else
	    goto err_out;
	}

      if (wc < 0xd800)
	n16 += 1;
      else if (wc < 0xe000)
	{
	  goto err_out;
	}
      else if (wc < 0x10000)
	n16 += 1;
      else if (wc < 0x110000)
	n16 += 2;
      else
	{
	  goto err_out;
	}
      
      in = s_utf8_next_char (in);
    }

  result = malloc (sizeof (ucs16_t) * (n16 + 1)) ;
  
  in = str;
  for (i = 0; i < n16;)
    {
      ucs4_t wc = s_utf8_get_char (in);

      if (wc < 0x10000)
	{
	  result[i++] = wc;
	}
      else
	{
	  result[i++] = (wc - 0x10000) / 0x400 + 0xd800;
	  result[i++] = (wc - 0x10000) % 0x400 + 0xdc00;
	}
      
      in = s_utf8_next_char (in);
    }

  result[i] = 0;

  if (items_written)
    *items_written = n16;

 err_out:
  if (items_read)
    *items_read = in - str;
  
  return result;
}
/* ------------------------------------------------------------------------ */
ucs4_t* utf8_to_ucs4  (const char* str , long len,            
		       long* items_read, long* items_written)
{
  int j, charlen;
  ucs4_t* result;
  int n_chars, i;
  const char *p;

  if (!str)
    {
      return 0;
    }

  p = str;
  n_chars = 0;
  if (len < 0)
    {
      while (*p)
	{
	  p = s_utf8_next_char (p);
	  ++n_chars;
	}
    }
  else
    {
      while (p < str + len && *p)
	{
	  p = s_utf8_next_char (p);
	  ++n_chars;
	}
    }
  
  result = malloc (sizeof (ucs4_t) *( n_chars + 1));
  
  p = str;
  for (i=0; i < n_chars; i++)
    {
      ucs4_t wc = ((unsigned char *)p)[0];

      if (wc < 0x80)
	{
	  result[i] = wc;
	  p++;
	}
      else
	{ 
	  if (wc < 0xe0)
	    {
	      charlen = 2;
	      wc &= 0x1f;
	    }
	  else if (wc < 0xf0)
	    {
	      charlen = 3;
	      wc &= 0x0f;
	    }
	  else if (wc < 0xf8)
	    {
	      charlen = 4;
	      wc &= 0x07;
	    }
	  else if (wc < 0xfc)
	    {
	      charlen = 5;
	      wc &= 0x03;
	    }
	  else
	    {
	      charlen = 6;
	      wc &= 0x01;
	    }

	  for (j = 1; j < charlen; j++)
	    {
	      wc <<= 6;
	      wc |= ((unsigned char *)p)[j] & 0x3f;
	    }

	  result[i] = wc;
	  p += charlen;
	}
    }
  result[i] = 0;

  if (items_written)
    *items_written = i;

  return result;
}
/* ------------------------------------------------------------------------ */
ucs4_t* utf16_to_ucs4 (const ucs16_t* str, long len,            
		       long* items_read     , long* items_written)
{
  const ucs16_t *in;
  char *out;
  char *result = NULL;
  int n_bytes;
  ucs4_t high_surrogate;

  if (!str)
    {
      return 0;
    }

  n_bytes = 0;
  in = str;
  high_surrogate = 0;
  while ((len < 0 || in - str < len) && *in)
    {
      ucs16_t c = *in;
      ucs4_t wc;

      if (c >= 0xdc00 && c < 0xe000) /* low surrogate */
	{
	  if (high_surrogate)
	    {
	      wc = SURROGATE_VALUE (high_surrogate, c);
	      high_surrogate = 0;
	    }
	  else
	    {
	      goto err_out;
	    }
	}
      else
	{
	  if (high_surrogate)
	    {
	      goto err_out;
	    }

	  if (c >= 0xd800 && c < 0xdc00) /* high surrogate */
	    {
	      high_surrogate = c;
	      goto next1;
	    }
	  else
	    wc = c;
	}

      n_bytes += sizeof (ucs4_t);

    next1:
      in++;
    }

  if (high_surrogate && !items_read)
    {
      goto err_out;
    }
  
  /* At this point, everything is valid, and we just need to convert
   */
  result = malloc (n_bytes + 4);
  
  high_surrogate = 0;
  out = result;
  in = str;
  while (out < result + n_bytes)
    {
      ucs16_t c = *in;
      ucs4_t wc;

      if (c >= 0xdc00 && c < 0xe000) /* low surrogate */
	{
	  wc = SURROGATE_VALUE (high_surrogate, c);
	  high_surrogate = 0;
	}
      else if (c >= 0xd800 && c < 0xdc00) /* high surrogate */
	{
	  high_surrogate = c;
	  goto next2;
	}
      else
	wc = c;

      *(ucs4_t *)out = wc;
      out += sizeof (ucs4_t);

    next2:
      in++;
    }

  *(ucs4_t *)out = 0;

  if (items_written)
    *items_written = (out - result) / sizeof (ucs4_t);

 err_out:
  if (items_read)
    *items_read = in - str;

  return (ucs4_t *)result;
}
/* ------------------------------------------------------------------------ */
char* utf16_to_utf8 (const ucs16_t* str, long len,
		     long* items_read     , long* items_written)
{
  const ucs16_t *in;
  char *out;
  char *result = NULL;
  int n_bytes;
  ucs4_t high_surrogate;

  if (!str)
    {
      return 0;
    }

  n_bytes = 0;
  in = str;
  high_surrogate = 0;
  while ((len < 0 || in - str < len) && *in)
    {
      ucs16_t c = *in;
      ucs4_t wc;

      if (c >= 0xdc00 && c < 0xe000) /* low surrogate */
	{
	  if (high_surrogate)
	    {
	      wc = SURROGATE_VALUE (high_surrogate, c);
	      high_surrogate = 0;
	    }
	  else
	    {
	      goto err_out;
	    }
	}
      else
	{
	  if (high_surrogate)
	    {
	      goto err_out;
	    }

	  if (c >= 0xd800 && c < 0xdc00) /* high surrogate */
	    {
	      high_surrogate = c;
	      goto next1;
	    }
	  else
	    wc = c;
	}

      n_bytes += UTF8_LENGTH (wc);

    next1:
      in++;
    }

  if (high_surrogate && !items_read)
    {
      goto err_out;
    }
  
  /* At this point, everything is valid, and we just need to convert
   */
  result = malloc (n_bytes + 1);
  
  high_surrogate = 0;
  out = result;
  in = str;
  while (out < result + n_bytes)
    {
      ucs16_t c = *in;
      ucs4_t wc;

      if (c >= 0xdc00 && c < 0xe000) /* low surrogate */
	{
	  wc = SURROGATE_VALUE (high_surrogate, c);
	  high_surrogate = 0;
	}
      else if (c >= 0xd800 && c < 0xdc00) /* high surrogate */
	{
	  high_surrogate = c;
	  goto next2;
	}
      else
	wc = c;

      out += s_ucs4_to_utf8 (wc, out);

    next2:
      in++;
    }
  
  *out = '\0';

  if (items_written)
    *items_written = out - result;

 err_out:
  if (items_read)
    *items_read = in - str;

  return result;
}
/* ------------------------------------------------------------------------ */
ucs16_t* ucs4_to_utf16 (const ucs4_t* str, long len,            
			long* items_read    , long* items_written)
{
  ucs16_t* result = NULL;
  int n16;
  int i, j;

  n16 = 0;
  i = 0;
  while ((len < 0 || i < len) && str[i])
    {
      ucs4_t wc = str[i];

      if (wc < 0xd800)
	n16 += 1;
      else if (wc < 0xe000)
	{
	  goto err_out;
	}
      else if (wc < 0x10000)
	n16 += 1;
      else if (wc < 0x110000)
	n16 += 2;
      else
	{
	  goto err_out;
	}

      i++;
    }
  
  result = malloc (sizeof (ucs16_t) * (n16 + 1));
  
  for (i = 0, j = 0; j < n16; i++)
    {
      ucs4_t wc = str[i];

      if (wc < 0x10000)
	{
	  result[j++] = wc;
	}
      else
	{
	  result[j++] = (wc - 0x10000) / 0x400 + 0xd800;
	  result[j++] = (wc - 0x10000) % 0x400 + 0xdc00;
	}
    }
  result[j] = 0;

  if (items_written)
    *items_written = n16;
  
 err_out:
  if (items_read)
    *items_read = i;
  
  return result;

}
/* ------------------------------------------------------------------------ */
char* ucs4_to_utf8 (const ucs4_t* str,  long len,            
		    long* items_read    ,  long *items_written)
{
  int result_length;
  char *result = NULL;
  char *p;
  int i;

  if (!str)
    {
      return 0;
    }

  result_length = 0;
  for (i = 0; len < 0 || i < len ; i++)
    {
      if (!str[i])
	break;

      if (str[i] >= 0x80000000)
	{
	  goto err_out;
	}
      
      result_length += UTF8_LENGTH (str[i]);
    }

  result = malloc (result_length + 1);
  p = result;

  i = 0;
  while (p < result + result_length)
    p += s_ucs4_to_utf8 (str[i++], p);
  
  *p = '\0';

  if (items_written)
    *items_written = p - result;

 err_out:
  if (items_read)
    *items_read = i;

  return result;
}
