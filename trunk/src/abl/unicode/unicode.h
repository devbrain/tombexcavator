#ifndef __ABL_UNICODE_UNICODE_H__
#define __ABL_UNICODE_UNICODE_H__

#include "abl/abl_export.h"
#include "abl/unicode/unicode_types.h"

CPP_VISIBLE_BEGIN

/// Unicode 5.0 character categories.
enum character_category_
{
  UCP_OTHER,
  UCP_LETTER,
  UCP_MARK,
  UCP_NUMBER,
  UCP_PUNCTUATION,
  UCP_SYMBOL,
  UCP_SEPARATOR
};

typedef enum character_category_ character_category_t;

/// Unicode 5.0 character types.
enum character_type_
{
  UCP_CONTROL,
  UCP_FORMAT,
  UCP_UNASSIGNED,
  UCP_PRIVATE_USE,
  UCP_SURROGATE,
  UCP_LOWER_CASE_LETTER,
  UCP_MODIFIER_LETTER,
  UCP_OTHER_LETTER,
  UCP_TITLE_CASE_LETTER,
  UCP_UPPER_CASE_LETTER,
  UCP_SPACING_MARK,
  UCP_ENCLOSING_MARK,
  UCP_NON_SPACING_MARK,
  UCP_DECIMAL_NUMBER,
  UCP_LETTER_NUMBER,
  UCP_OTHER_NUMBER,
  UCP_CONNECTOR_PUNCTUATION,
  UCP_DASH_PUNCTUATION,
  UCP_CLOSE_PUNCTUATION,
  UCP_FINAL_PUNCTUATION,
  UCP_INITIAL_PUNCTUATION,
  UCP_OTHER_PUNCTUATION,
  UCP_OPEN_PUNCTUATION,
  UCP_CURRENCY_SYMBOL,
  UCP_MODIFIER_SYMBOL,
  UCP_MATHEMATICAL_SYMBOL,
  UCP_OTHER_SYMBOL,
  UCP_LINE_SEPARATOR,
  UCP_PARAGRAPH_SEPARATOR,
  UCP_SPACE_SEPARATOR
};

typedef enum character_type_ character_type_t;

/// Unicode 5.0 scripts.
enum script_
{
  UCP_ARABIC,
  UCP_ARMENIAN,
  UCP_BENGALI,
  UCP_BOPOMOFO,
  UCP_BRAILLE,
  UCP_BUGINESE,
  UCP_BUHID,
  UCP_CANADIAN_ABORIGINAL,
  UCP_CHEROKEE,
  UCP_COMMON,
  UCP_COPTIC,
  UCP_CYPRIOT,
  UCP_CYRILLIC,
  UCP_DESERET,
  UCP_DEVANAGARI,
  UCP_ETHIOPIC,
  UCP_GEORGIAN,
  UCP_GLAGOLITIC,
  UCP_GOTHIC,
  UCP_GREEK,
  UCP_GUJARATI,
  UCP_GURMUKHI,
  UCP_HAN,
  UCP_HANGUL,
  UCP_HANUNOO,
  UCP_HEBREW,
  UCP_HIRAGANA,
  UCP_INHERITED,
  UCP_KANNADA,
  UCP_KATAKANA,
  UCP_KHAROSHTHI,
  UCP_KHMER,
  UCP_LAO,
  UCP_LATIN,
  UCP_LIMBU,
  UCP_LINEAR_B,
  UCP_MALAYALAM,
  UCP_MONGOLIAN,
  UCP_MYANMAR,
  UCP_NEW_TAI_LUE,
  UCP_OGHAM,
  UCP_OLD_ITALIC,
  UCP_OLD_PERSIAN,
  UCP_ORIYA,
  UCP_OSMANYA,
  UCP_RUNIC,
  UCP_SHAVIAN,
  UCP_SINHALA,
  UCP_SYLOTI_NAGRI,
  UCP_SYRIAC,
  UCP_TAGALOG,
  UCP_TAGBANWA,
  UCP_TAI_LE,
  UCP_TAMIL,
  UCP_TELUGU,
  UCP_THAANA,
  UCP_THAI,
  UCP_TIBETAN,
  UCP_TIFINAGH,
  UCP_UGARITIC,
  UCP_YI,
  UCP_BALINESE,
  UCP_CUNEIFORM,
  UCP_NKO,
  UCP_PHAGS_PA,
  UCP_PHOENICIAN,
  UCP_CARIAN,
  UCP_CHAM,
  UCP_KAYAH_LI,
  UCP_LEPCHA,
  UCP_LYCIAN,
  UCP_LYDIAN,
  UCP_OL_CHIKI,
  UCP_REJANG,
  UCP_SAURASHTRA,
  UCP_SUNDANESE,
  UCP_VAI
};

typedef enum script_ script_t;

/// This structure holds the character properties
/// of an Unicode character.
struct character_properties_
{
  character_category_t category;
  character_type_t     type;
  script_t             script;
};

typedef struct character_properties_ character_properties_t;

/// Return the Unicode character properties for the
/// character with the given Unicode value.
ABL_API void properties (ucs4_t ch, character_properties_t*);

/// Returns true iff the given character is a lowercase
/// character.
ABL_API te_bool_t is_lower (ucs4_t ch);

/// Returns true iff the given character is an uppercase
/// character.
ABL_API te_bool_t is_upper (ucs4_t ch);
		
/// If the given character is an uppercase character,
/// return its lowercase counterpart, otherwise return
/// the character.		
ABL_API ucs4_t to_lower (ucs4_t ch);

/// If the given character is a lowercase character,
/// return its uppercase counterpart, otherwise return
/// the character.
ABL_API ucs4_t to_upper (ucs4_t ch);
		
CPP_VISIBLE_END
#endif 
