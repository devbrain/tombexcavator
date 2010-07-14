#include "sand.h"

#ifdef _MSC_VER

#include <io.h>

#include <direct.h>

#else

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
//	readdir();

//getcwd();

//opendir();

#endif



#include "zlib/SDL_rwops_zzip.h"



char* fileNames[Unknown+1];

char musicFiles[MAX_MUSICFILES][MAX_LINE];

int musicCount = 0;



int getMusicFileNames(char* dir)

{

#ifdef _MSC_VER

	char lowerCaseName[260];

	char searchString[MAX_LINE];

	int count, count2 = 0;

	long hFile;

	struct _finddata_t fdata;



	strcpy(searchString, dir);

	strcat(searchString, "*");

	if ((hFile = (long)_findfirst(searchString, &fdata)) != -1L)

	{

		while(_findnext(hFile, &fdata) == 0)

		{

			strcpy(lowerCaseName, fdata.name);

			count = 0;

			while(lowerCaseName[count] != '\0')

			{

				lowerCaseName[count] = tolower(lowerCaseName[count]);

				count++;

			}



			if ((musicCount < MAX_MUSICFILES)

				&& ((strstr(lowerCaseName, ".mid")

					|| strstr(lowerCaseName, ".wav")

					|| strstr(lowerCaseName, ".ogg"))))

					//|| strstr(fdata.name, ".mp3"))))

			{

				strcpy(musicFiles[musicCount], dir);

				strcat(musicFiles[musicCount++], fdata.name);

				count2++;

			}

		}

		_findclose(hFile);

	}



	return count2;

#else
	char searchString[MAX_LINE];
	int count, count2 = 0;
	DIR *dirp;
	struct dirent *dp;
	struct stat buf;
	time_t tnow;
	time(&tnow);
	static char fullPath[MAX_LINE];
	
	dirp = opendir(dir);
	while (dp = readdir(dirp)) {
		strcpy(fullPath, dir);
		strcat(fullPath, dp->d_name);
					
		if (stat(fullPath,&buf) == 0)
		{
			strcpy(searchString,dp->d_name);
			count = 0;
			
			while(searchString[count] != '\0')
			{
				searchString[count] = tolower(searchString[count]);
				count++;
			}
												
		}
		if ((musicCount < MAX_MUSICFILES)
				&& ((strstr(searchString, ".mid")
				|| strstr(searchString, ".ogg")
			       	|| strstr(searchString, ".wav")
			       	|| strstr(searchString, ".mp3"))))
		{
			strcpy(musicFiles[musicCount++],fullPath);
		count2++;
		}
	}
	return count2;
#endif

}

	

void loadDataFile()

{

	int count = 0, size = 0, sizeMusic = 0;



	fileNames[Bullet_DRocket] = "data/graphics/Bullet_Rocket.png";

	fileNames[Bullet_GasHit] = "data/graphics/Object_Gas.png";

	fileNames[Bullet_Gun] = "data/graphics/Bullet_Small.png";

	fileNames[Bullet_Inf_Gun] = "data/graphics/Bullet_Small.png";

	fileNames[Bullet_LargeRocket] = "data/graphics/Bullet_LargeRocket.png";

	fileNames[Bullet_LargeShot] = "data/graphics/Bullet_LargeShot.png";

	fileNames[Bullet_Rocket] = "data/graphics/Bullet_Rocket.png";

	fileNames[Bullet_RocketHit] = "data/graphics/Object_Explosion1.png";

	fileNames[Bullet_Shell] = "data/graphics/Bullet_Medium.png";

	fileNames[Bullet_ShellHit] = "data/graphics/Bullet_ShellHit.png";

	fileNames[Bullet_SmallRocket] = "data/graphics/Bullet_SmallRocket.png";

	fileNames[Bullet_SmallRocketHeavy] = "data/graphics/Bullet_SmallRocket.png";

	fileNames[Bullet_SmallShot] = "data/graphics/Bullet_SmallShot.png";

	fileNames[Bullet_Sonic] = "data/graphics/Object_ShimmerMask.png";//Bullet_Sonic.png";



	fileNames[Object_DeadInfantry] = "data/graphics/Object_DeadInfantry.png";

	fileNames[Object_AirUnitExplosion] = "data/graphics/Object_Explosion5.png";

	fileNames[Object_LargeExplosion1] = "data/graphics/Object_Explosion2.png";

	fileNames[Object_LargeExplosion2] = "data/graphics/Object_Explosion3.png";

	fileNames[Object_LargeUnitExplosion] = "data/graphics/Object_Explosion4.png";

	fileNames[Object_SmallUnitExplosion] = "data/graphics/Object_SmallUnitExplosion.png";

	fileNames[Object_Smoke] = "data/graphics/Object_Smoke.png";

	fileNames[Object_ShimmerMask] = "data/graphics/Object_ShimmerMask.png";



	fileNames[Picture_Barracks] = "data/graphics/Picture_Barracks.png";

	fileNames[Picture_Carryall] = "data/graphics/Picture_Carryall.png";

	fileNames[Picture_ConstructionYard] = "data/graphics/Picture_ConstructionYard.png";

	fileNames[Picture_Devastator] = "data/graphics/Picture_Devastator.png";

	fileNames[Picture_Deviator] = "data/graphics/Picture_Deviator.png";

	fileNames[Picture_DeathHand] = "data/graphics/new/Picture_DeathHand.png";//altered

	fileNames[Picture_Fremen] = "data/graphics/new/Picture_Fremen.png";//altered

	fileNames[Picture_GunTurret] = "data/graphics/Picture_GunTurret.png";

	fileNames[Picture_Harvester] = "data/graphics/Picture_Harvester.png";

	fileNames[Picture_HeavyFactory] = "data/graphics/Picture_HeavyFactory.png";

	fileNames[Picture_HighTechFactory] = "data/graphics/Picture_HighTechFactory.png";

	fileNames[Picture_Infantry] = "data/graphics/Picture_Infantry.png";

	fileNames[Picture_IX] = "data/graphics/Picture_IX.png";

	fileNames[Picture_Launcher] = "data/graphics/Picture_Launcher.png";

	fileNames[Picture_LightFactory] = "data/graphics/Picture_LightFactory.png";

	fileNames[Picture_MCV] = "data/graphics/Picture_MCV.png";

	fileNames[Picture_Ornithopter] = "data/graphics/Picture_Ornithopter.png";

	fileNames[Picture_Palace] = "data/graphics/Picture_Palace.png";

	fileNames[Picture_Quad] = "data/graphics/Picture_Quad.png";

	fileNames[Picture_Radar] = "data/graphics/Picture_Radar.png";

	fileNames[Picture_RadarStatic] = "data/graphics/uigraphics/UI_RadarStatic.png";

	fileNames[Picture_Raider] = "data/graphics/Picture_Raider.png";

	fileNames[Picture_Refinery] = "data/graphics/Picture_Refinery.png";

	fileNames[Picture_RepairYard] = "data/graphics/Picture_RepairYard.png";

	fileNames[Picture_RocketTurret] = "data/graphics/Picture_RocketTurret.png";

	fileNames[Picture_Sardaukar] = "data/graphics/Picture_Sardaukar.png";//altered

	fileNames[Picture_Saboteur] = "data/graphics/new/Picture_Saboteur.png";//altered

	fileNames[Picture_SiegeTank] = "data/graphics/Picture_SiegeTank.png";

	fileNames[Picture_SonicTank] = "data/graphics/Picture_SonicTank.png";

	fileNames[Picture_Silo] = "data/graphics/Picture_Silo.png";

	fileNames[Picture_Slab1] = "data/graphics/Picture_Slab1.png";

	fileNames[Picture_Slab4] = "data/graphics/Picture_Slab4.png";

	fileNames[Picture_StarPort] = "data/graphics/Picture_StarPort.png";

	fileNames[Picture_Tank] = "data/graphics/Picture_Tank.png";

	fileNames[Picture_Trike] = "data/graphics/Picture_Trike.png";

	fileNames[Picture_Trooper] = "data/graphics/Picture_HeavyInfantry.png";

	fileNames[Picture_Wall] = "data/graphics/Picture_Wall.png";

	fileNames[Picture_WindTrap] = "data/graphics/Picture_WindTrap.png";

	fileNames[Picture_WOR] = "data/graphics/Picture_WOR.png";



	fileNames[Sound_acknowledged] = "data/sound/Sound_acknowledged.ogg";

	fileNames[Sound_affirmative] = "data/sound/Sound_affirmative.ogg";

	fileNames[Sound_bloom] = "data/sound/Sound_bloom.ogg";

	fileNames[Sound_buttonClick] = "data/sound/Sound_buttonClick.ogg";

	fileNames[Sound_cannot] = "data/sound/Sound_cannot.ogg";

	fileNames[Sound_credit] = "data/sound/Sound_credit.ogg";

	fileNames[Sound_crush] = "data/sound/Sound_crush.ogg";

	fileNames[Sound_gas] = "data/sound/Sound_gas.ogg";

	fileNames[Sound_gun] = "data/sound/Sound_gun.ogg";

	fileNames[Sound_gunTurret] = "data/sound/Sound_gunTurret.ogg";

	fileNames[Sound_infantryOut] = "data/sound/Sound_infantryOut.ogg";

	fileNames[Sound_largeExplosion] = "data/sound/Sound_largeExplosion.ogg";

	fileNames[Sound_machineGun] = "data/sound/Sound_machineGun.ogg";

	fileNames[Sound_movingOut] = "data/sound/Sound_movingOut.ogg";

	fileNames[Sound_placeStructure] = "data/sound/Sound_placeStructure.ogg";

	fileNames[Sound_radar] = "data/sound/Sound_radar.ogg";

	fileNames[Sound_reporting] = "data/sound/Sound_reporting.ogg";

	fileNames[Sound_rocket] = "data/sound/Sound_rocket.ogg";

	fileNames[Sound_scream1] = "data/sound/Sound_scream1.ogg";

	fileNames[Sound_scream2] = "data/sound/Sound_scream2.ogg";

	fileNames[Sound_scream3] = "data/sound/Sound_scream3.ogg";

	fileNames[Sound_scream4] = "data/sound/Sound_scream4.ogg";

	fileNames[Sound_scream5] = "data/sound/Sound_scream5.ogg";

	fileNames[Sound_smallExplosion] = "data/sound/Sound_smallExplosion.ogg";

	fileNames[Sound_smallRocket] = "data/sound/Sound_smallRocket.ogg";

	fileNames[Sound_something] = "data/sound/Sound_something.ogg";

	fileNames[Sound_sonic] = "data/sound/Sound_sonic.ogg";

	fileNames[Sound_structureExplosion] = "data/sound/Sound_structureExplosion.ogg";

	fileNames[Sound_whatsHappening] = "data/sound/Sound_whatsHappening.ogg";

	fileNames[Sound_wormAttack] = "data/sound/Sound_wormAttack.ogg";

	fileNames[Sound_yesSir] = "data/sound/Sound_yesSir.ogg";



	//Atreides voice over

	    fileNames[SoundVO_Aapprch] = "data/sound/atre/aapprch.ogg";

        fileNames[SoundVO_Aarrive] = "data/sound/atre/aarrive.ogg";

        fileNames[SoundVO_Aatre] = "data/sound/atre/aatre.ogg";

        fileNames[SoundVO_Aattack] = "data/sound/atre/aattack.ogg";

        fileNames[SoundVO_Abloom] = "data/sound/atre/abloom.ogg";

		fileNames[SoundVO_Acapture] = "data/sound/atre/acapture.ogg";

        fileNames[SoundVO_Adeploy] = "data/sound/atre/adeploy.ogg";

        fileNames[SoundVO_Adestroy] = "data/sound/atre/adestroy.ogg";

        fileNames[SoundVO_Aeast] = "data/sound/atre/aeast.ogg";

        fileNames[SoundVO_Aenemy] = "data/sound/atre/aenemy.ogg";

        fileNames[SoundVO_Afield] = "data/sound/atre/afield.ogg";

        fileNames[SoundVO_Afive] = "data/sound/atre/afive.ogg";

        fileNames[SoundVO_Afour] = "data/sound/atre/afour.ogg";

        fileNames[SoundVO_Afremen] = "data/sound/atre/afremen.ogg";

        fileNames[SoundVO_Afrigate] = "data/sound/atre/afrigate.ogg";

        fileNames[SoundVO_Ahark] = "data/sound/atre/ahark.ogg";

        fileNames[SoundVO_Aharvest] = "data/sound/atre/aharvest.ogg";

        fileNames[SoundVO_Alaunch] = "data/sound/atre/alaunch.ogg";

        fileNames[SoundVO_Alocated] = "data/sound/atre/alocated.ogg";

        fileNames[SoundVO_Amerc] = "data/sound/atre/amerc.ogg";

        fileNames[SoundVO_Amissile] = "data/sound/atre/amissile.ogg";

        fileNames[SoundVO_Anext] = "data/sound/atre/anext.ogg";

        fileNames[SoundVO_Anext2] = "data/sound/atre/anext2.ogg";

        fileNames[SoundVO_Anorth] = "data/sound/atre/anorth.ogg";

        fileNames[SoundVO_Aoff] = "data/sound/atre/aoff.ogg";

        fileNames[SoundVO_Aon] = "data/sound/atre/aon.ogg";

		fileNames[SoundVO_Aone] = "data/sound/atre/aone.ogg";

        fileNames[SoundVO_Aordos] = "data/sound/atre/aordos.ogg";

        fileNames[SoundVO_Aradar] = "data/sound/atre/aradar.ogg";

        fileNames[SoundVO_Arepair] = "data/sound/atre/arepair.ogg";

        fileNames[SoundVO_Asabot] = "data/sound/atre/asabot.ogg";

        fileNames[SoundVO_Asard] = "data/sound/atre/asard.ogg";

        fileNames[SoundVO_Asouth] = "data/sound/atre/asouth.ogg";

        fileNames[SoundVO_Astruct] = "data/sound/atre/astruct.ogg";

        fileNames[SoundVO_Athree] = "data/sound/atre/athree.ogg";

        fileNames[SoundVO_Atwo] = "data/sound/atre/atwo.ogg";

        fileNames[SoundVO_Aunit] = "data/sound/atre/aunit.ogg";

        fileNames[SoundVO_Avehicle] = "data/sound/atre/avehicle.ogg";

        fileNames[SoundVO_Awarning] = "data/sound/atre/awarning.ogg";

        fileNames[SoundVO_Awest] = "data/sound/atre/awest.ogg";

        fileNames[SoundVO_Awormy] = "data/sound/atre/awormy.ogg";

	fileNames[SoundVO_AConst] = "data/sound/SoundVO_AConst.ogg";

	fileNames[SoundVO_ALose] = "data/sound/SoundVO_ALose.ogg";

	fileNames[SoundVO_AWin] = "data/sound/SoundVO_AWin.ogg";



	//Harkonnen voice over

	    fileNames[SoundVO_Happrch] = "data/sound/hark/happrch.ogg";

        fileNames[SoundVO_Harrive] = "data/sound/hark/harrive.ogg";

        fileNames[SoundVO_Hatre] = "data/sound/hark/hatre.ogg";

        fileNames[SoundVO_Hattack] = "data/sound/hark/hattack.ogg";

        fileNames[SoundVO_Hbloom] = "data/sound/hark/hbloom.ogg";

		fileNames[SoundVO_Hcapture] = "data/sound/hark/hcapture.ogg";

        fileNames[SoundVO_Hdeploy] = "data/sound/hark/hdeploy.ogg";

        fileNames[SoundVO_Hdestroy] = "data/sound/hark/hdestroy.ogg";

        fileNames[SoundVO_Heast] = "data/sound/hark/heast.ogg";

        fileNames[SoundVO_Henemy] = "data/sound/hark/henemy.ogg";

        fileNames[SoundVO_Hfield] = "data/sound/hark/hfield.ogg";

        fileNames[SoundVO_Hfive] = "data/sound/hark/hfive.ogg";

        fileNames[SoundVO_Hfour] = "data/sound/hark/hfour.ogg";

        fileNames[SoundVO_Hfremen] = "data/sound/hark/hfremen.ogg";

        fileNames[SoundVO_Hfrigate] = "data/sound/hark/hfrigate.ogg";

        fileNames[SoundVO_Hhark] = "data/sound/hark/hhark.ogg";

        fileNames[SoundVO_Hharvest] = "data/sound/hark/hharvest.ogg";

        fileNames[SoundVO_Hlaunch] = "data/sound/hark/hlaunch.ogg";

        fileNames[SoundVO_Hlocated] = "data/sound/hark/hlocated.ogg";

        fileNames[SoundVO_Hmerc] = "data/sound/hark/hmerc.ogg";

        fileNames[SoundVO_Hmissile] = "data/sound/hark/hmissile.ogg";

        fileNames[SoundVO_Hnext] = "data/sound/hark/hnext.ogg";

        fileNames[SoundVO_Hnext2] = "data/sound/hark/hnext2.ogg";

        fileNames[SoundVO_Hnorth] = "data/sound/hark/hnorth.ogg";

        fileNames[SoundVO_Hoff] = "data/sound/hark/hoff.ogg";

        fileNames[SoundVO_Hon] = "data/sound/hark/hon.ogg";

		fileNames[SoundVO_Hone] = "data/sound/hark/hone.ogg";

        fileNames[SoundVO_Hordos] = "data/sound/hark/hordos.ogg";

        fileNames[SoundVO_Hradar] = "data/sound/hark/hradar.ogg";

        fileNames[SoundVO_Hrepair] = "data/sound/hark/hrepair.ogg";

        fileNames[SoundVO_Hsabot] = "data/sound/hark/hsabot.ogg";

        fileNames[SoundVO_Hsard] = "data/sound/hark/hsard.ogg";

        fileNames[SoundVO_Hsouth] = "data/sound/hark/hsouth.ogg";

        fileNames[SoundVO_Hstruct] = "data/sound/hark/hstruct.ogg";

        fileNames[SoundVO_Hthree] = "data/sound/hark/hthree.ogg";

        fileNames[SoundVO_Htwo] = "data/sound/hark/htwo.ogg";

        fileNames[SoundVO_Hunit] = "data/sound/hark/hunit.ogg";

        fileNames[SoundVO_Hvehicle] = "data/sound/hark/hvehicle.ogg";

        fileNames[SoundVO_Hwarning] = "data/sound/hark/hwarning.ogg";

        fileNames[SoundVO_Hwest] = "data/sound/hark/hwest.ogg";

        fileNames[SoundVO_Hwormy] = "data/sound/hark/hwormy.ogg";

	fileNames[SoundVO_HConst] = "data/sound/SoundVO_HConst.ogg";

	fileNames[SoundVO_HLose] = "data/sound/SoundVO_HLose.ogg";

	fileNames[SoundVO_HWin] = "data/sound/SoundVO_HWin.ogg";



	//Ordos voice over

	    fileNames[SoundVO_Oapprch] = "data/sound/ordos/oapprch.ogg";

        fileNames[SoundVO_Oarrive] = "data/sound/ordos/oarrive.ogg";

        fileNames[SoundVO_Oatre] = "data/sound/ordos/oatre.ogg";

        fileNames[SoundVO_Oattack] = "data/sound/ordos/oattack.ogg";

        fileNames[SoundVO_Obloom] = "data/sound/ordos/obloom.ogg";

		fileNames[SoundVO_Ocapture] = "data/sound/ordos/ocapture.ogg";

        fileNames[SoundVO_Odeploy] = "data/sound/ordos/odeploy.ogg";

        fileNames[SoundVO_Odestroy] = "data/sound/ordos/odestroy.ogg";

        fileNames[SoundVO_Oeast] = "data/sound/ordos/oeast.ogg";

        fileNames[SoundVO_Oenemy] = "data/sound/ordos/oenemy.ogg";

        fileNames[SoundVO_Ofield] = "data/sound/ordos/ofield.ogg";

        fileNames[SoundVO_Ofive] = "data/sound/ordos/ofive.ogg";

        fileNames[SoundVO_Ofour] = "data/sound/ordos/ofour.ogg";

        fileNames[SoundVO_Ofremen] = "data/sound/ordos/ofremen.ogg";

        fileNames[SoundVO_Ofrigate] = "data/sound/ordos/ofrigate.ogg";

        fileNames[SoundVO_Ohark] = "data/sound/ordos/ohark.ogg";

        fileNames[SoundVO_Oharvest] = "data/sound/ordos/oharvest.ogg";

        fileNames[SoundVO_Olaunch] = "data/sound/ordos/olaunch.ogg";

        fileNames[SoundVO_Olocated] = "data/sound/ordos/olocated.ogg";

        fileNames[SoundVO_Omerc] = "data/sound/ordos/omerc.ogg";

        fileNames[SoundVO_Omissile] = "data/sound/ordos/omissile.ogg";

        fileNames[SoundVO_Onext] = "data/sound/ordos/onext.ogg";

        fileNames[SoundVO_Onext2] = "data/sound/ordos/onext2.ogg";

        fileNames[SoundVO_Onorth] = "data/sound/ordos/onorth.ogg";

        fileNames[SoundVO_Ooff] = "data/sound/ordos/ooff.ogg";

        fileNames[SoundVO_Oon] = "data/sound/ordos/oon.ogg";

		fileNames[SoundVO_Oone] = "data/sound/ordos/oone.ogg";

        fileNames[SoundVO_Oordos] = "data/sound/ordos/oordos.ogg";

        fileNames[SoundVO_Oradar] = "data/sound/ordos/oradar.ogg";

        fileNames[SoundVO_Orepair] = "data/sound/ordos/orepair.ogg";

        fileNames[SoundVO_Osabot] = "data/sound/ordos/osabot.ogg";

        fileNames[SoundVO_Osard] = "data/sound/ordos/osard.ogg";

        fileNames[SoundVO_Osouth] = "data/sound/ordos/osouth.ogg";

        fileNames[SoundVO_Ostruct] = "data/sound/ordos/ostruct.ogg";

        fileNames[SoundVO_Othree] = "data/sound/ordos/othree.ogg";

        fileNames[SoundVO_Otwo] = "data/sound/ordos/otwo.ogg";

        fileNames[SoundVO_Ounit] = "data/sound/ordos/ounit.ogg";

        fileNames[SoundVO_Ovehicle] = "data/sound/ordos/ovehicle.ogg";

        fileNames[SoundVO_Owarning] = "data/sound/ordos/owarning.ogg";

        fileNames[SoundVO_Owest] = "data/sound/ordos/owest.ogg";

        fileNames[SoundVO_Owormy] = "data/sound/ordos/owormy.ogg";

	fileNames[SoundVO_OConst] = "data/sound/SoundVO_OConst.ogg";

	fileNames[SoundVO_OLose] = "data/sound/SoundVO_OLose.ogg";

	fileNames[SoundVO_OWin] = "data/sound/SoundVO_OWin.ogg";





	fileNames[SoundVO_Matre] = "data/sound/merc/matre.ogg";

	fileNames[SoundVO_Mhark] = "data/sound/merc/mhark.ogg";

	fileNames[SoundVO_Mordos] = "data/sound/merc/mordos.ogg";



	fileNames[Structure_Building22] = "data/graphics/Structure_Building22.png";

	fileNames[Structure_Building23] = "data/graphics/Structure_Building23.png";

	fileNames[Structure_Building33] = "data/graphics/Structure_Building33.png";

	fileNames[Structure_BuildingTurret] = "data/graphics/Structure_BuildingTurret.png";



	fileNames[Structure_Barracks] = "data/graphics/Structure_Barracks.png";

	fileNames[Structure_ConstructionYard] = "data/graphics/Structure_ConstructionYard.png";

	fileNames[Structure_ConstructionYardPlace] = "data/graphics/new/Structure_BuildingConyard.png";

	fileNames[Structure_GunTurret] = "data/graphics/Structure_GunTurret.png";

	fileNames[Structure_HeavyFactory] = "data/graphics/Structure_HeavyFactory.png";

	fileNames[Structure_HighTechFactory] = "data/graphics/Structure_HighTechFactory.png";

	fileNames[Structure_IX] = "data/graphics/Structure_IX.png";

	fileNames[Structure_LightFactory] = "data/graphics/Structure_LightFactory.png";

	fileNames[Structure_Palace] = "data/graphics/Structure_Palace.png";

	fileNames[Structure_Radar] = "data/graphics/Structure_Radar.png";

	fileNames[Structure_Refinery] = "data/graphics/new/Structure_Refinery.png";//altered

	fileNames[Structure_RepairYard] = "data/graphics/Structure_RepairYard.png";

	fileNames[Structure_RocketTurret] = "data/graphics/Structure_RocketTurret.png";

	fileNames[Structure_Silo] = "data/graphics/Structure_Silo.png";

	fileNames[Structure_Slab1] = "data/graphics/Structure_Slab1.png";

	fileNames[Structure_Slab4] = "data/graphics/Structure_Slab4.png";

	fileNames[Structure_StarPort] = "data/graphics/new/Structure_StarPort.png";//altered

	fileNames[Structure_Wall] = "data/graphics/Structure_Wall.png";

	fileNames[Structure_WindTrap] = "data/graphics/Structure_WindTrap.png";

	fileNames[Structure_WOR] = "data/graphics/Structure_WOR.png";



	fileNames[Terrain_Dunes] = "data/graphics/Terrain_Dunes.png";

	fileNames[Terrain_Hidden] = "data/graphics/Terrain_Hidden.png";

	fileNames[Terrain_Mountain] = "data/graphics/Terrain_Mountain.png";

	fileNames[Terrain_Rock] = "data/graphics/Terrain_Rock.png";

	fileNames[Terrain_RockDamage] = "data/graphics/Terrain_RockDamage.png";

	fileNames[Terrain_Sand] = "data/graphics/Terrain_Sand.png";

	fileNames[Terrain_SandDamage] = "data/graphics/Terrain_SandDamage.png";

	fileNames[Terrain_Spice] = "data/graphics/Terrain_Spice.png";

	fileNames[Terrain_ThickSpice] = "data/graphics/Terrain_ThickSpice.png";



	///////load ui

	fileNames[UI_Cursor] = "data/graphics/UI_Cursor.png";

	fileNames[UI_CreditsDigits] = "data/graphics/UI_CreditsDigits.png";

	fileNames[UI_GameBar] = "data/graphics/UI_GameBar.png";

	//fileNames[UI_GameBar2] = "data/graphics/UI_GameBar2.png";

	//fileNames[UI_GameBar2] = "data/graphics/uigraphics/UI_GameBar3.png";

	fileNames[UI_GameBar2] = "data/graphics/uigraphics/UI_GameBar4.png";

	fileNames[UI_Indicator] = "data/graphics/UI_Indicator.png";

	fileNames[UI_InvalidPlace] = "data/graphics/UI_InvalidPlace.png";

	//fileNames[UI_Menu] = "data/graphics/UI_Menu.png";

	//fileNames[UI_Menu] = "data/title.png";

	fileNames[UI_Menu] = "data/graphics/uigraphics/UI_Menu2.png";



	fileNames[UI_PlaceIt] = "data/graphics/UI_PlaceIt.png";

	fileNames[UI_SelectionBox] = "data/graphics/UI_SelectionBox.png";

	fileNames[UI_TopBar] = "data/graphics/uigraphics/UI_TopBar.png";//altered

	fileNames[UI_ValidPlace] = "data/graphics/UI_ValidPlace.png";

	//New gui stuff

	fileNames[UI_Down] = "data/graphics/uigraphics/UI_Down.png";

	fileNames[UI_Down_Pressed] = "data/graphics/uigraphics/UI_Down_Pressed.png";

	fileNames[UI_Up] = "data/graphics/uigraphics/UI_Up.png";

	fileNames[UI_Up_Pressed] = "data/graphics/uigraphics/UI_Up_Pressed.png";

	fileNames[UI_MessageBox] = "data/graphics/uigraphics/UI_MessageBox.png";

	fileNames[UI_Mentat] = "data/graphics/uigraphics/UI_Mentat.png";

	fileNames[UI_Mentat_Pressed] = "data/graphics/uigraphics/UI_Mentat_Pressed.png";

	fileNames[UI_Options] = "data/graphics/uigraphics/UI_Options.png";

	fileNames[UI_Options_Pressed] = "data/graphics/uigraphics/UI_Options_Pressed.png";

	fileNames[UI_Upgrade] = "data/graphics/uigraphics/UI_Upgrade.png";

	fileNames[UI_Upgrade_Pressed] = "data/graphics/uigraphics/UI_Upgrade_Pressed.png";



	//more new gui stuff

	fileNames[UI_Cancel] = "data/graphics/uigraphics/Button_Cancel.png";

	fileNames[UI_Cancel_Pressed] = "data/graphics/uigraphics/Button_CancelPushed.png";

	fileNames[UI_Load] = "data/graphics/uigraphics/Button_Load.png";

	fileNames[UI_Load_Pressed] = "data/graphics/uigraphics/Button_LoadPushed.png";

	fileNames[UI_Minus] = "data/graphics/uigraphics/Button_Minus.png";

	fileNames[UI_Minus_Pressed] = "data/graphics/uigraphics/Button_MinusPushed.png";

	fileNames[UI_Plus] = "data/graphics/uigraphics/Button_Plus.png";

	fileNames[UI_Plus_Pressed] = "data/graphics/uigraphics/Button_PlusPushed.png";

	fileNames[UI_OK] = "data/graphics/uigraphics/Button_OK.png";

	fileNames[UI_OK_Pressed] = "data/graphics/uigraphics/Button_OKPushed.png";

	fileNames[UI_Save] = "data/graphics/uigraphics/Button_Save.png";

	fileNames[UI_Save_Pressed] = "data/graphics/uigraphics/Button_SavePushed.png";

	fileNames[UI_Dif_Easy] = "data/graphics/uigraphics/Difficulty_Easy.png";

	fileNames[UI_Dif_Hard] = "data/graphics/uigraphics/Difficulty_Hard.png";

	fileNames[UI_Dif_Medium] = "data/graphics/uigraphics/Difficulty_Medium.png";

	fileNames[UI_ED_Structures] = "data/graphics/uigraphics/Editor_Structures.png";

	fileNames[UI_ED_Structures_Pressed] = "data/graphics/uigraphics/Editor_StructuresPushed.png";

	fileNames[UI_ED_Terrain] = "data/graphics/uigraphics/Editor_Terrain.png";

	fileNames[UI_ED_Terrain_Pressed] = "data/graphics/uigraphics/Editor_TerrainPushed.png";

	fileNames[UI_ED_Units] = "data/graphics/uigraphics/Editor_Units.png";

	fileNames[UI_ED_Units_Pressed] = "data/graphics/uigraphics/Editor_UnitsPushed.png";

	fileNames[UI_Loadgame] = "data/graphics/uigraphics/Ingame_Load.png";

	fileNames[UI_Loadgame_Pressed] = "data/graphics/uigraphics/Ingame_LoadPushed.png";

	fileNames[UI_Game_Quit] = "data/graphics/uigraphics/Ingame_Quit.png";

	fileNames[UI_Game_Quit_Pressed] = "data/graphics/uigraphics/Ingame_QuitPushed.png";

	fileNames[UI_Game_Restart] = "data/graphics/uigraphics/Ingame_Restart.png";

	fileNames[UI_Game_Restart_Pressed] = "data/graphics/uigraphics/Ingame_RestartPushed.png";

	fileNames[UI_Game_Resume] = "data/graphics/uigraphics/Ingame_Resume.png";

	fileNames[UI_Game_Resume_Pressed] = "data/graphics/uigraphics/Ingame_ResumePushed.png";

	fileNames[UI_Game_Save] = "data/graphics/uigraphics/Ingame_Save.png";

	fileNames[UI_Game_Save_Pressed] = "data/graphics/uigraphics/Ingame_SavePushed.png";

	fileNames[UI_Game_Settings] = "data/graphics/uigraphics/Ingame_Settings.png";

	fileNames[UI_Game_Settings_Pressed] = "data/graphics/uigraphics/Ingame_SettingsPushed.png";



	fileNames[UI_About] = "data/graphics/uigraphics/Main_About.png";

	fileNames[UI_About_Pressed] = "data/graphics/uigraphics/Main_AboutPushed.png";

	fileNames[UI_MapEdit] = "data/graphics/uigraphics/Main_MapEdit.png";

	fileNames[UI_MapEdit_Pressed] = "data/graphics/uigraphics/Main_MapEditPushed.png";

	fileNames[UI_Multi] = "data/graphics/uigraphics/Main_Multi.png";

	fileNames[UI_Multi_Pressed] = "data/graphics/uigraphics/Main_MultiPushed.png";

	fileNames[UI_OptionsMM] = "data/graphics/uigraphics/Main_Options.png";

	fileNames[UI_OptionsMM_Pressed] = "data/graphics/uigraphics/Main_OptionsPushed.png";

	fileNames[UI_Quit] = "data/graphics/uigraphics/Main_Quit.png";

	fileNames[UI_Quit_Pressed] = "data/graphics/uigraphics/Main_QuitPushed.png";

	fileNames[UI_Single] = "data/graphics/uigraphics/Main_Single.png";

	fileNames[UI_Single_Pressed] = "data/graphics/uigraphics/Main_SinglePushed.png";

	fileNames[UI_Menu_Background] = "data/graphics/uigraphics/Menu_Background.png";

	fileNames[UI_MM_CornerNE] = "data/graphics/uigraphics/Menu_CornerNE.png";

	fileNames[UI_MM_CornerNW] = "data/graphics/uigraphics/Menu_CornerNW.png";

	fileNames[UI_MM_CornerSE] = "data/graphics/uigraphics/Menu_CornerSE.png";

	fileNames[UI_MM_CornerSW] = "data/graphics/uigraphics/Menu_CornerSW.png";

	fileNames[UI_Difficulty] = "data/graphics/uigraphics/Menu_Difficulty.png";

	fileNames[UI_HeraldAtre] = "data/graphics/uigraphics/Menu_HeraldAtre.png";

	fileNames[UI_HeraldHark] = "data/graphics/uigraphics/Menu_HeraldHark.png";

	fileNames[UI_HeraldOrd] = "data/graphics/uigraphics/Menu_HeraldOrd.png";

	fileNames[UI_HeraldAtre_Coloured] = "data/graphics/uigraphics/House_Atreides.png";

	fileNames[UI_HeraldHark_Coloured] = "data/graphics/uigraphics/House_Harkonnen.png";

	fileNames[UI_HeraldOrd_Coloured] = "data/graphics/uigraphics/House_Ordos.png";

	fileNames[UI_HorizBorder] = "data/graphics/uigraphics/Menu_HorizontalBorder.png";

	fileNames[UI_HouseSelect] = "data/graphics/uigraphics/Menu_HouseSelect.png";

	fileNames[UI_MissionSelect] = "data/graphics/uigraphics/Menu_MissionSelect.png";

	fileNames[UI_Planet] = "data/graphics/uigraphics/Menu_Planet.png";

	fileNames[UI_VersionBox] = "data/graphics/uigraphics/Menu_VersionBox.png";

	fileNames[UI_VertBorder] = "data/graphics/uigraphics/Menu_VerticalBorder.png";



	fileNames[UI_Single_Campaign] = "data/graphics/uigraphics/Single_Campaign.png";

	fileNames[UI_Single_Campaign_Pressed] = "data/graphics/uigraphics/Single_CampaignPushed.png";

	fileNames[UI_Single_Cancel] = "data/graphics/uigraphics/Single_Cancel.png";

	fileNames[UI_Single_Cancel_Pressed] = "data/graphics/uigraphics/Single_CancelPushed.png";

	fileNames[UI_Single_Custom] = "data/graphics/uigraphics/Single_Custom.png";

	fileNames[UI_Single_Custom_Pressed] = "data/graphics/uigraphics/Single_CustomPushed.png";

	fileNames[UI_Single_Random] = "data/graphics/uigraphics/Single_Random.png";

	fileNames[UI_Single_Random_Pressed] = "data/graphics/uigraphics/Single_RandomPushed.png";

	fileNames[UI_GameMenu] = "data/graphics/uigraphics/UI_GameMenu.png";

	fileNames[UI_LoadSaveGameHead] = "data/graphics/uigraphics/UI_LoadGame.png";

	fileNames[UI_OptionsMenu] = "data/graphics/uigraphics/UI_OptionsMenu.png";

	fileNames[UI_MultiPlayerSplash] = "data/graphics/uigraphics/UI_Multiplayer.png";

	fileNames[UI_RepairIcon] = "data/graphics/uigraphics/UI_RepairSign.png";



	fileNames[UI_Unit_Aggr] = "data/graphics/uigraphics/Unit_Aggr.png";

	fileNames[UI_Unit_Aggr_Active] = "data/graphics/uigraphics/Unit_AggrActive.png";

	fileNames[UI_Unit_Def] = "data/graphics/uigraphics/Unit_Def.png";

	fileNames[UI_Unit_Def_Active] = "data/graphics/uigraphics/Unit_DefActive.png";

	fileNames[UI_Unit_Deploy] = "data/graphics/uigraphics/Unit_Deploy.png";

	fileNames[UI_Unit_Deploy_Pressed] = "data/graphics/uigraphics/Unit_DeployPushed.png";

	fileNames[UI_Unit_Scout] = "data/graphics/uigraphics/Unit_Scout.png";

	fileNames[UI_Unit_Scout_Active] = "data/graphics/uigraphics/Unit_ScoutActive.png";

	fileNames[UI_Unit_Destruct] = "data/graphics/uigraphics/Unit_Destruct.png";

	fileNames[UI_Unit_Destruct_Pressed] = "data/graphics/uigraphics/Unit_DestructPushed.png";

	fileNames[UI_Unit_StGnd] = "data/graphics/uigraphics/Unit_StGr.png";

	fileNames[UI_Unit_StGnd_Active] = "data/graphics/uigraphics/Unit_StGrActive.png";



	fileNames[UI_LoadSaveWindow] = "data/graphics/uigraphics/UI_SavegameSlots2.png";

	fileNames[UI_GameSlot] = "data/graphics/uigraphics/UI_GameSlot.png";



	fileNames[UI_Single_Skirmish] = "data/graphics/uigraphics/Button_Skirmish.png";

	fileNames[UI_Single_Skirmish_Pressed] = "data/graphics/uigraphics/Button_SkirmishPushed.png";

	fileNames[UI_Single_Start] = "data/graphics/uigraphics/Button_Start.png";

	fileNames[UI_Single_Start_Pressed] = "data/graphics/uigraphics/Button_StartPushed.png";



	fileNames[UI_Order] = "data/graphics/uigraphics/Button_Order.png";

	fileNames[UI_Order_Pressed] = "data/graphics/uigraphics/Button_OrderPushed.png";

	fileNames[UI_Repair] = "data/graphics/uigraphics/Button_Repair.png";
	fileNames[UI_Repair_Pressed] = "data/graphics/uigraphics/Button_RepairPushed.png";


	///load units////////////////

	fileNames[Unit_Carryall] = "data/graphics/Unit_Carryall.png";

	fileNames[Unit_Devastator] = "data/graphics/Unit_Devastator.png";

	fileNames[Unit_DevastatorBase] = "data/graphics/Unit_DevastatorBase.png";

	fileNames[Unit_Deviator] = "data/graphics/Unit_Launcher.png";

	fileNames[Unit_Frigate] = "data/graphics/Unit_Frigate.png";

	fileNames[Unit_Harvester] = "data/graphics/Unit_Harvester.png";

	fileNames[Unit_HarvesterSand] = "data/graphics/Unit_HarvesterSand.png";

	fileNames[Unit_Infantry] = "data/graphics/Unit_Infantry.png";

	fileNames[Unit_Launcher] = "data/graphics/Unit_Launcher.png";

	fileNames[Unit_MCV] = "data/graphics/Unit_MCV.png";

	fileNames[Unit_Ornithopter] = "data/graphics/Unit_Ornithopter.png";

	fileNames[Unit_Quad] = "data/graphics/Unit_Quad.png";

	fileNames[Unit_Saboteur] = "data/graphics/Unit_Saboteur.png";

	fileNames[Unit_Sandworm] = "data/graphics/Unit_Sandworm.png";

	fileNames[Unit_SiegeTank] = "data/graphics/Unit_SiegeTank.png";

	fileNames[Unit_SiegeTankBase] = "data/graphics/Unit_SiegeTankBase.png";

	fileNames[Unit_SonicTank] = "data/graphics/Unit_SonicTank.png";

	fileNames[Unit_Tank] = "data/graphics/Unit_Tank.png";

	fileNames[Unit_TankBase] = "data/graphics/Unit_TankBase.png";

	fileNames[Unit_Trike] = "data/graphics/Unit_Trike.png";

	fileNames[Unit_Raider] = "data/graphics/Unit_Trike.png";

	fileNames[Unit_Trooper] = "data/graphics/Unit_Trooper.png";



	fileNames[Unit_Sardaukar] = "data/graphics/Unit_Trooper.png";

	fileNames[Unit_Fremen] = "data/graphics/Unit_Trooper.png";



	fileNames[Unknown] = "data/graphics/Unknown.png";



	///////////////// work out how much needs loading, find music names to load from music dir

	//get attack music names

	sizeMusic = getMusicFileNames("./data/sound/music/attack/");



	Music_AttackFirst = Music_First;

	Music_AttackLast = Music_AttackFirst + sizeMusic - 1;



	//get intro music names

	sizeMusic = getMusicFileNames("./data/sound/music/intro/");



	Music_IntroFirst = Music_AttackLast + 1;

	Music_IntroLast = Music_IntroFirst + sizeMusic - 1;



	//get lose music names

	sizeMusic = getMusicFileNames("./data/sound/music/lose/");



	Music_LoseFirst = Music_IntroLast + 1;

	Music_LoseLast = Music_LoseFirst + sizeMusic - 1;



	//get peace music names

	sizeMusic = getMusicFileNames("./data/sound/music/peace/");



	Music_PeaceFirst = Music_LoseLast + 1;

	Music_PeaceLast = Music_PeaceFirst + sizeMusic - 1;



	//get win music names

	sizeMusic = getMusicFileNames("./data/sound/music/win/");



	Music_WinFirst = Music_PeaceLast + 1;

	Music_WinLast = Music_WinFirst + sizeMusic - 1;



	Music_Last = Music_First + musicCount - 1;

	size = Unknown + musicCount + 1;



	//load in all data

	dataFile = new DATAFILE[size];	//the array of datafiles, Unknown is the last thing with the highest number

	for (count = 0; count < size; count++)

		dataFile[count].dat = (char*)0;	//set everything invalid, so after it loads can check if its not loaded



//	

//	FILE *f;

	//ZZIP_DIR* zzipdir = zzip_dir_open( "data.zip", NULL ); 

    



	for (count = 0; count <= Unknown; count++)

	{

		//ZZIP_FILE* zfile = zzip_file_open(zzipdir, fileNames[count], ZZIP_CASELESS);



		if (strstr(fileNames[count], ".png"))

		{

			//dataFile[count].dat = (char*)SDL_LoadBMP(fileNames[count]);

			dataFile[count].dat = (char*)IMG_Load_RW(SDL_RWFromFile(fileNames[count], "rb"), 0);

			//SDL_RWops *rw = SDL_RWFromZZIP(fileNames[count], "rb");

			//dataFile[count].dat = (char*)SDL_LoadBMP_RW(rw, 0);

			

			//SDL_RWops *rw = SDL_RWFromZZIP_FILE(zfile);

			//dataFile[count].dat = (char*)SDL_LoadBMP_RW(rw, 0);





			if ((dataFile[count].dat != NULL)

				&& (count != UI_GameBar)

				&& (count != UI_GameBar2)

				&& (count != UI_TopBar)

				&& (count != UI_Menu)

				&& (count != Unknown)

				&& ((count < Picture_Barracks) || (count > Picture_WOR)))

				SDL_SetColorKey((SDL_Surface*)dataFile[count].dat, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);



		}

		else if (strstr(fileNames[count], ".ogg"))

		{

			//dataFile[count].dat = (char*)Mix_LoadWAV(fileNames[count]);

			//dataFile[count].dat = (char*)Mix_LoadWAV_RW(SDL_RWFromZZIP(fileNames[count], "rb"), 1);

			dataFile[count].dat = (char*)Mix_LoadWAV_RW(SDL_RWFromFile(fileNames[count], "rb"), 1);

			if(!dataFile[count].dat)

			{

				fprintf(stderr, "Mix_LoadWAV_RW: %s\n", Mix_GetError());

			}

		}



		if (dataFile[count].dat == NULL)

			fprintf(stderr, "file %s not found.\n", fileNames[count]);



	//	if(zfile)

	//		zzip_file_close( zfile );

	}



	if (musicCount > 0)

	{

		int count2 = 0;

		for(count=Music_First; count <= Music_Last; count++)

			dataFile[count].dat =  (char*)(Mix_LoadMUS(musicFiles[count2++]));

	}

	

	int unloadedItems = 0;

	for (count=0; count < size; count++)

	{

		if (!dataFile[count].dat)

		{

			//fprintf(stderr, "ERROR: unable to load %d.\n", count);

			unloadedItems++;

		}

	}



	if (unloadedItems)

	{

		fprintf(stderr, "ERROR: unable to load all data.\n");

		exit(1);

	}

}



void destroyDataFile()

{

	int count;

	

	for (count = 0; count <= Unknown; count++)

	if (dataFile[count].dat != NULL)

	{

		if (strstr(fileNames[count], ".png"))

			SDL_FreeSurface((SDL_Surface*)dataFile[count].dat);

		else if (strstr(fileNames[count], ".ogg"))

			Mix_FreeChunk((Mix_Chunk*)dataFile[count].dat);

		else

			Mix_FreeMusic((Mix_Music*)dataFile[count].dat);

	}

}



void fixDisplayFormat()

{

	int count;

	for (count = 0; count <= Unknown; count++)

	{

		if (strstr(fileNames[count], ".png"))

		{

			SDL_FreeSurface(((SDL_Surface*)dataFile[count].dat));

			dataFile[count].dat = (char*)IMG_Load(fileNames[count]);

			if (dataFile[count].dat != NULL)

			{

				if ((count != UI_GameBar)

				&& (count != UI_GameBar2)

				&& (count != UI_Menu)

				&& (count != Unknown)

				&& ((count < Picture_Barracks) || (count > Picture_WOR)))

					SDL_SetColorKey((SDL_Surface*)dataFile[count].dat, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);

				dataFile[count].dat = (char*)SDL_DisplayFormat((SDL_Surface*)dataFile[count].dat);

			}

		}

	}

}

