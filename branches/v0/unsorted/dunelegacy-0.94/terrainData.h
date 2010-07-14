//definitions for positions in the spice, rock and wall images

typedef enum
{
	Terrain_HiddenFull,
	Terrain_HiddenIsland,
	Terrain_HiddenLeftRight,
	Terrain_HiddenLeft,
	Terrain_HiddenRight,
	Terrain_HiddenUp,
	Terrain_HiddenDown,
	Terrain_HiddenUpDown,
	Terrain_HiddenNotLeft,
	Terrain_HiddenNotRight,
	Terrain_HiddenNotUp,
	Terrain_HiddenNotDown,
	Terrain_HiddenDownRight,
	Terrain_HiddenUpLeft,
	Terrain_HiddenDownLeft,
	Terrain_HiddenUpRight
} HIDDENTYPE;

typedef enum
{
	Terrain_MountainFull,
	Terrain_MountainIsland,
	Terrain_MountainLeftRight,
	Terrain_MountainLeft,
	Terrain_MountainRight,
	Terrain_MountainUp,
	Terrain_MountainDown,
	Terrain_MountainUpDown,
	Terrain_MountainNotLeft,
	Terrain_MountainNotRight,
	Terrain_MountainNotUp,
	Terrain_MountainNotDown,
	Terrain_MountainDownRight,
	Terrain_MountainUpLeft,
	Terrain_MountainDownLeft,
	Terrain_MountainUpRight
} MOUNTAINTYPE;

typedef enum
{
	Terrain_DunesFull,
	Terrain_DunesIsland,
	Terrain_DunesLeftRight,
	Terrain_DunesLeft,
	Terrain_DunesRight,
	Terrain_DunesUp,
	Terrain_DunesDown,
	Terrain_DunesUpDown,
	Terrain_DunesNotLeft,
	Terrain_DunesNotRight,
	Terrain_DunesNotUp,
	Terrain_DunesNotDown,
	Terrain_DunesDownRight,
	Terrain_DunesUpLeft,
	Terrain_DunesDownLeft,
	Terrain_DunesUpRight
} DUNESTYPE;

typedef enum
{
	Terrain_a1,
	Terrain_a2,
	Terrain_a3
} SANDTYPE;

typedef enum
{
	Terrain_sd1,
	Terrain_sd2,
	Terrain_sd3,
} SANDDAMAGETYPE;

typedef enum
{
	Terrain_s1,
	Terrain_s10,
	Terrain_s11,
	Terrain_s12,
	Terrain_s13,
	Terrain_s14,
	Terrain_s15,
	Terrain_s16,
	Terrain_s2,
	Terrain_s3,
	Terrain_s4,
	Terrain_s5,
	Terrain_s6,
	Terrain_s7,
	Terrain_s8,
	Terrain_s9
} SPICETYPE;

typedef enum
{
	Terrain_ThickSpiceFull,
	Terrain_ThickSpiceIsland,
	Terrain_ThickSpiceLeftRight,
	Terrain_ThickSpiceLeft,
	Terrain_ThickSpiceRight,
	Terrain_ThickSpiceUp,
	Terrain_ThickSpiceDown,
	Terrain_ThickSpiceUpDown,
	Terrain_ThickSpiceNotLeft,
	Terrain_ThickSpiceNotRight,
	Terrain_ThickSpiceNotUp,
	Terrain_ThickSpiceNotDown,
	Terrain_ThickSpiceDownRight,
	Terrain_ThickSpiceUpLeft,
	Terrain_ThickSpiceDownLeft,
	Terrain_ThickSpiceUpRight
} THICKSPICETYPE;

typedef enum
{
	Terrain_td1,
	Terrain_td2,
} ROCKDAMAGETYPE;

typedef enum
{
	Terrain_t1,
	Terrain_t10,
	Terrain_t11,
	Terrain_t12,
	Terrain_t13,
	Terrain_t14,
	Terrain_t15,
	Terrain_t16,
	Terrain_t2,
	Terrain_t3,
	Terrain_t4,
	Terrain_t5,
	Terrain_t6,
	Terrain_t7,
	Terrain_t8,
	Terrain_t9,
	Terrain_tc1x1,
	Terrain_tc2x2TopLeft,
	Terrain_tc2x2TopRight,
	Terrain_tc2x2BottomLeft,
	Terrain_tc2x2BottomRight,
	Terrain_tc3x3TopLeft,
	Terrain_tc3x3TopCentre,
	Terrain_tc3x3TopRight,
	Terrain_tc3x3MiddleLeft,
	Terrain_tc3x3MiddleCentre,
	Terrain_tc3x3MiddleRight,
	Terrain_tc4x4BottomLeft,
	Terrain_tc4x4BottomCentre,
	Terrain_tc4x4BottomRight,
} ROCKTYPE;

typedef enum
{
	Structure_w1,
	Structure_w10,
	Structure_w11,
	Structure_w2,
	Structure_w3,
	Structure_w4,
	Structure_w5,
	Structure_w6,
	Structure_w7,
	Structure_w8,
	Structure_w9
} WALLTYPE;

