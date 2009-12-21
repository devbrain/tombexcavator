#include "abl/unicode/unicode.h"

#include "pcre_config.h"
#include "pcre_internal.h" 

void properties (ucs4_t ch, character_properties_t* props)
{
  const ucd_record* ucd = GET_UCD(ch);
  props->category = _pcre_ucp_gentype [ucd->chartype];
  props->type     = ucd->chartype;
  props->script   = ucd->script; 
}
/* ============================================================ */
te_bool_t is_lower (ucs4_t ch)
{
  character_properties_t props;
  properties (ch, &props);
  return props.category == UCP_LETTER && props.type == UCP_LOWER_CASE_LETTER; 
}

/* ============================================================ */
te_bool_t is_upper (ucs4_t ch)
{
  character_properties_t props;
  properties (ch, &props);
  return props.category == UCP_LETTER && props.type == UCP_UPPER_CASE_LETTER; 
}
		
/* ============================================================ */
ucs4_t to_lower (ucs4_t ch)
{
  if (is_upper (ch))
    {
      return UCD_OTHERCASE(ch);
    }
  return ch; 
}

/* ============================================================ */
ucs4_t to_upper (ucs4_t ch)
{
  if (is_lower (ch))
    {
      return UCD_OTHERCASE (ch);
    }
  return ch; 
}

