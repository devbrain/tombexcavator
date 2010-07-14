#include "abl/unicode/unicode.h"

#include "pcre_config.h"
#include "pcre_internal.h" 

void unicode_properties (ucs4_t ch, character_properties_t* props)
{
  const ucd_record* ucd = GET_UCD(ch);
  props->category = _pcre_ucp_gentype [ucd->chartype];
  props->type     = ucd->chartype;
  props->script   = ucd->script; 
}
/* ============================================================ */
te_bool_t unicode_is_lower (ucs4_t ch)
{
  character_properties_t props;
  unicode_properties (ch, &props);
  return props.category == UCP_LETTER && props.type == UCP_LOWER_CASE_LETTER; 
}

/* ============================================================ */
te_bool_t unicode_is_upper (ucs4_t ch)
{
  character_properties_t props;
  unicode_properties (ch, &props);
  return props.category == UCP_LETTER && props.type == UCP_UPPER_CASE_LETTER; 
}
		
/* ============================================================ */
ucs4_t unicode_to_lower (ucs4_t ch)
{
  if (unicode_is_upper (ch))
    {
      return UCD_OTHERCASE(ch);
    }
  return ch; 
}

/* ============================================================ */
ucs4_t unicode_to_upper (ucs4_t ch)
{
  if (unicode_is_lower (ch))
    {
      return UCD_OTHERCASE (ch);
    }
  return ch; 
}
