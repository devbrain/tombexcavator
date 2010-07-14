// data definitions

typedef enum

{

	Bullet_DRocket,

	Bullet_GasHit,

	Bullet_Gun,

	Bullet_Inf_Gun,

	Bullet_LargeRocket,

	Bullet_LargeShot,

	Bullet_Rocket,

	Bullet_RocketHit,

	Bullet_Shell,

	Bullet_ShellHit,

	Bullet_SmallRocket,

	Bullet_SmallRocketHeavy,

	Bullet_SmallShot,

	Bullet_Sonic,

	Object_DeadInfantry,

	Object_AirUnitExplosion,

	Object_LargeExplosion1,

	Object_LargeExplosion2,

	Object_LargeUnitExplosion,

	Object_ShimmerMask,

	Object_SmallUnitExplosion,

	Object_Smoke,

	Picture_Barracks,

	Picture_ConstructionYard,

	Picture_Carryall,

	Picture_Devastator,

	Picture_Deviator,

	Picture_DeathHand,

	Picture_Fremen,

	Picture_GunTurret,

	Picture_Harvester,

	Picture_HeavyFactory,

	Picture_HighTechFactory,

	Picture_Infantry,

	Picture_IX,

	Picture_Launcher,

	Picture_LightFactory,

	Picture_MCV,

	Picture_Ornithopter,

	Picture_Palace,

	Picture_Quad,

	Picture_Radar,
	
	Picture_RadarStatic,

	Picture_Raider,

	Picture_Refinery,

	Picture_RepairYard,

	Picture_RocketTurret,

	Picture_Saboteur,

	Picture_Sardaukar,

	Picture_SiegeTank,

	Picture_Silo,

	Picture_Slab1,

	Picture_Slab4,

	Picture_SonicTank,

	Picture_StarPort,

	Picture_Tank,

	Picture_Trike,

	Picture_Trooper,

	Picture_Wall,

	Picture_WindTrap,

	Picture_WOR,

	Sound_acknowledged,

	Sound_affirmative,

	Sound_bloom,

	Sound_buttonClick,

	Sound_cannot,

	Sound_credit,

	Sound_crush,

	Sound_gas,

	Sound_gun,

	Sound_gunTurret,

	Sound_infantryOut,

	Sound_largeExplosion,

	Sound_machineGun,

	Sound_movingOut,

	Sound_placeStructure,

	Sound_radar,

	Sound_reporting,

	Sound_rocket,

	Sound_scream1,

	Sound_scream2,

	Sound_scream3,

	Sound_scream4,

	Sound_scream5,

	Sound_smallExplosion,

	Sound_smallRocket,

	Sound_something,

	Sound_sonic,

	Sound_structureExplosion,

	Sound_whatsHappening,

	Sound_wormAttack,

	Sound_yesSir,

	//Atreides Voice over

	SoundVO_Aapprch,

	SoundVO_Aarrive,

	SoundVO_Aatre,

	SoundVO_Aattack,

	SoundVO_Abloom,

	SoundVO_Acapture,

	SoundVO_AConst,

	SoundVO_Adeploy,

	SoundVO_Adestroy,

	SoundVO_Aeast,

	SoundVO_Aenemy,

	SoundVO_Afield,

	SoundVO_Afive,

	SoundVO_Afour,

	SoundVO_Afremen,

	SoundVO_Afrigate,

	SoundVO_Ahark,

	SoundVO_Aharvest,

	SoundVO_Alaunch,

	SoundVO_Alocated,

	SoundVO_Amerc,

	SoundVO_Amissile,

	SoundVO_Anext,

	SoundVO_Anext2,

	SoundVO_Anorth,

	SoundVO_Aoff,

	SoundVO_Aon,

	SoundVO_Aone,

	SoundVO_Aordos,

	SoundVO_Aradar,

	SoundVO_Arepair,

	SoundVO_Asabot,

	SoundVO_Asard,

	SoundVO_Asouth,

	SoundVO_Astruct,

	SoundVO_Athree,

	SoundVO_Atwo,

	SoundVO_Aunit,

	SoundVO_Avehicle,

	SoundVO_Awarning,

	SoundVO_Awest,

	SoundVO_Awormy,

	//SoundVO_ADeployed,

	SoundVO_ALose,

	SoundVO_AWin,



	//Hark voice over

	SoundVO_Happrch,

	SoundVO_Harrive,

	SoundVO_Hatre,

	SoundVO_Hattack,

	SoundVO_Hbloom,

	SoundVO_Hcapture,

	SoundVO_HConst,

	SoundVO_Hdeploy,

	SoundVO_Hdestroy,

	SoundVO_Heast,

	SoundVO_Henemy,

	SoundVO_Hfield,

	SoundVO_Hfive,

	SoundVO_Hfour,

	SoundVO_Hfremen,

	SoundVO_Hfrigate,

	SoundVO_Hhark,

	SoundVO_Hharvest,

	SoundVO_Hlaunch,

	SoundVO_Hlocated,

	SoundVO_Hmerc,

	SoundVO_Hmissile,

	SoundVO_Hnext,

	SoundVO_Hnext2,

	SoundVO_Hnorth,

	SoundVO_Hoff,

	SoundVO_Hon,

	SoundVO_Hone,

	SoundVO_Hordos,

	SoundVO_Hradar,

	SoundVO_Hrepair,

	SoundVO_Hsabot,

	SoundVO_Hsard,

	SoundVO_Hsouth,

	SoundVO_Hstruct,

	SoundVO_Hthree,

	SoundVO_Htwo,

	SoundVO_Hunit,

	SoundVO_Hvehicle,

	SoundVO_Hwarning,

	SoundVO_Hwest,

	SoundVO_Hwormy,

	SoundVO_HLose,

	SoundVO_HWin,



	//Ordos voice over

	SoundVO_Oapprch,

    SoundVO_Oarrive,

    SoundVO_Oatre,

    SoundVO_Oattack,

    SoundVO_Obloom,

    SoundVO_Ocapture,

    SoundVO_OConst,

    SoundVO_Odeploy,

    SoundVO_Odestroy,

    SoundVO_Oeast,

    SoundVO_Oenemy,

    SoundVO_Ofield,

    SoundVO_Ofive,

    SoundVO_Ofour,

    SoundVO_Ofremen,

    SoundVO_Ofrigate,

    SoundVO_Ohark,

    SoundVO_Oharvest,

    SoundVO_Olaunch,

    SoundVO_Olocated,

    SoundVO_Omerc,

    SoundVO_Omissile,

    SoundVO_Onext,

    SoundVO_Onext2,

    SoundVO_Onorth,

    SoundVO_Ooff,

    SoundVO_Oon,

    SoundVO_Oone,

    SoundVO_Oordos,

    SoundVO_Oradar,

    SoundVO_Orepair,

    SoundVO_Osabot,

    SoundVO_Osard,

    SoundVO_Osouth,

    SoundVO_Ostruct,

    SoundVO_Othree,

    SoundVO_Otwo,

    SoundVO_Ounit,

    SoundVO_Ovehicle,

    SoundVO_Owarning,

    SoundVO_Owest,

    SoundVO_Owormy,

	SoundVO_OLose,

	SoundVO_OWin,



	SoundVO_Matre,

	SoundVO_Mhark,

	SoundVO_Mordos,



	//Structures

	Structure_Building22,

	Structure_Building23,

	Structure_Building33,

	Structure_BuildingTurret,

	Structure_Barracks,

	Structure_ConstructionYard,

	Structure_ConstructionYardPlace,

	Structure_GunTurret,

	Structure_HeavyFactory,

	Structure_HighTechFactory,

	Structure_IX,

	Structure_LightFactory,

	Structure_Palace,

	Structure_Radar,

	Structure_Refinery,

	Structure_RepairYard,

	Structure_RocketTurret,

	Structure_Silo,

	Structure_Slab1,

	Structure_Slab4,

	Structure_StarPort,

	Structure_Wall,

	Structure_WindTrap,

	Structure_WOR,

	Terrain_Dunes,

	Terrain_Hidden,

	Terrain_Mountain,

	Terrain_Rock,

	Terrain_RockDamage,

	Terrain_Sand,

	Terrain_SandDamage,

	Terrain_Spice,

	Terrain_ThickSpice,

	UI_Cursor,

	UI_CreditsDigits,

	UI_Down,

	UI_Down_Pressed,

	UI_GameBar,

	UI_GameBar2,

	UI_Indicator,

	UI_InvalidPlace,

	UI_MessageBox,

	UI_Mentat,

	UI_Mentat_Pressed,

	UI_Menu,

	UI_Options,

	UI_Options_Pressed,

	UI_PlaceIt,

	UI_SelectionBox,

	UI_TopBar,



	UI_Up,

	UI_Up_Pressed,

	UI_Upgrade,

	UI_Upgrade_Pressed,

	UI_ValidPlace,

	UI_Cancel,

	UI_Cancel_Pressed,

	UI_Load,

	UI_Load_Pressed,

	UI_Minus,

	UI_Minus_Pressed,

	UI_Plus,

	UI_Plus_Pressed,

	UI_OK,

	UI_OK_Pressed,

	UI_Save,

	UI_Save_Pressed,

	UI_Dif_Easy,

	UI_Dif_Hard,

	UI_Dif_Medium,

	UI_ED_Structures,

	UI_ED_Structures_Pressed,

	UI_ED_Terrain,

	UI_ED_Terrain_Pressed,

	UI_ED_Units,

	UI_ED_Units_Pressed,

	UI_Loadgame,

	UI_Loadgame_Pressed,

	UI_Game_Quit,

	UI_Game_Quit_Pressed,

	UI_Game_Restart,

	UI_Game_Restart_Pressed,

	UI_Game_Resume,

	UI_Game_Resume_Pressed,

	UI_Game_Save,

	UI_Game_Save_Pressed,

	UI_Game_Settings,

	UI_Game_Settings_Pressed,

	UI_About,

	UI_About_Pressed,

	UI_MapEdit,

	UI_MapEdit_Pressed,

	UI_Multi,

	UI_Multi_Pressed,

	UI_OptionsMM,

	UI_OptionsMM_Pressed,

	UI_Quit,

	UI_Quit_Pressed,

	UI_Single,

	UI_Single_Pressed,

	UI_Menu_Background,

	UI_MM_CornerNE,

	UI_MM_CornerNW,

	UI_MM_CornerSE,

	UI_MM_CornerSW,

	UI_Difficulty,

	UI_HeraldAtre,

	UI_HeraldHark,

	UI_HeraldOrd,

	UI_HeraldAtre_Coloured,

	UI_HeraldHark_Coloured,

	UI_HeraldOrd_Coloured,

	UI_HorizBorder,

	UI_HouseSelect,

	UI_MissionSelect,

	UI_Planet,

	UI_VersionBox,

	UI_VertBorder,

	UI_Single_Campaign,

	UI_Single_Campaign_Pressed,

	UI_Single_Cancel,

	UI_Single_Cancel_Pressed,

	UI_Single_Custom,

	UI_Single_Custom_Pressed,

	UI_Single_Random,

	UI_Single_Random_Pressed,

	UI_GameMenu,

	UI_LoadSaveGameHead,

	UI_OptionsMenu,

	UI_MultiPlayerSplash,

	UI_RepairIcon,

	UI_Unit_Aggr,

	UI_Unit_Aggr_Active,

	UI_Unit_Def,

	UI_Unit_Def_Active,

	UI_Unit_Deploy,

	UI_Unit_Deploy_Pressed,

	UI_Unit_Scout,

	UI_Unit_Scout_Active,

	UI_Unit_Destruct,

	UI_Unit_Destruct_Pressed,

	UI_Unit_StGnd,

	UI_Unit_StGnd_Active,

	UI_LoadSaveWindow,

	UI_GameSlot,

	UI_Single_Skirmish,

	UI_Single_Skirmish_Pressed,

	UI_Single_Start,

	UI_Single_Start_Pressed,

	UI_Order,

	UI_Order_Pressed,

	UI_Repair,
	
	UI_Repair_Pressed,








	Unit_Carryall,

	Unit_Devastator,

	Unit_DevastatorBase,

	Unit_Deviator,

	Unit_Fremen,

	Unit_Frigate,

	Unit_Harvester,

	Unit_HarvesterSand,

	Unit_Infantry,

	Unit_Launcher,

	Unit_MCV,

	Unit_Ornithopter,

	Unit_Quad,

	Unit_Saboteur,

	Unit_Sandworm,

	Unit_Sardaukar,

	Unit_SiegeTank,

	Unit_SiegeTankBase,

	Unit_SonicTank,

	Unit_Tank,

	Unit_TankBase,

	Unit_Trike,

	Unit_Raider,

	Unit_Trooper,

	Unknown,

	Music_First

} DATA;

