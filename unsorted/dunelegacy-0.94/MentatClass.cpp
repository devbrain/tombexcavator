#include "sand.h"

#include "MentatClass.h"





#define FRAME_TIME 10

#define MOUTH_TIME 10

#define EYES_TIME 200

#define WAIT_TIME 5



char *mentatFileNames[Mentat_Unknown];//for the mentat character



int *mentatFrames[Mentat_Entry_Unknown];//frames for each entry

char *mentatEntryFileNames[Mentat_Entry_Unknown];//files for mentat entries



char *mentatDescriptionsAtreides[Mentat_Entry_Unknown];//for the mentat texts

//char *mentatDescriptionsOrdos[Mentat_Entry_Unknown];//for the mentat texts

//char *mentatDescriptionsHarkonnen[Mentat_Entry_Unknown];//for the mentat texts

Mentat_Entry mentatEntries[Mentat_Entry_Unknown];//for the mentat entries



int levelpics[22];

char *levelBriefingText[22];





MentatClass::MentatClass()

{

	house = -1;

	currentViewID = -1;

	loadedBefore = 0;

	frameTimer = FRAME_TIME;

	eyesTimer = EYES_TIME;

	mouthTimer = MOUTH_TIME;

	talkLength = 0;



	eyesFrame = 0;

	mouthFrame = 0;



	mentatWindow = new Window();

	houseInfoWindow = new Window();

	houseChoiceWindow = new Window();



	LoadData();

	createList();

	makeTextSurface(1);//Make a temp surface



	inBriefing = false;

	briefingRunning = false;



	inInfoScreen = false;

	inHouseChoice = false;



	textLabel.setVisible(true);

	textLabel.setTransparent(true);

	textLabel.setTextColour(COLOUR_WHITE);

	textLabel.setJustification(1);



	zipPath = "data/mentat.zip";

}





MentatClass::~MentatClass()

{

	mentatButtons.dropChildren();

	listBox.dropChildren();

	mentatWindow->dropChildren();



	UnloadTemporaryData();



	//should get rid of all surfaces too...

	//SDL_FreeSurface(surface);



	for(int i=0;i<Mentat_Entry_Unknown;i++)

	{

		if(mentatEntries[i].loaded)

		{

			for(int j=0;j<mentatEntries[i].frames;j++)

			{

				SDL_FreeSurface(mentatEntries[i].pic[j]);

			}



		}

	}



}



bool MentatClass::doExit()

{

	//mentatWindow->setVisible(false);



	if(!inBriefing)

	{

		if(currentViewID == -1)

		{

			return true;

		}

		else

		{

			currentViewID = -1;

			return false;

		}

	}

	else

	{

		briefingRunning = false;

		return false;

	}

}



void MentatClass::showMentat()

{

	mentatWindow->setVisible(true);

}



void MentatClass::draw()

{

	//Draw pic first

	if(inHouseChoice && !inInfoScreen)

	{

			SDL_BlitSurface(heraldSurf, NULL, screen, &mentatPos);



			//houseButtons.draw();

			houseChoiceWindow->draw();

//			houseAtre.draw();

//			houseOrdos.draw();

//			houseHark.draw();

	}

	else if(loadedBefore)

	{

			graphicPos.x = mentatPos.x + 256;

			graphicPos.y = mentatPos.y + 96;

			graphicPos.w = 368;//change as the pictures are loaded....

			graphicPos.h = 224;



			SDL_BlitSurface(mentatSurface, NULL, screen, &mentatPos);

			SDL_FillRect(screen, &graphicPos, COLOUR_BLACK);



			if(inInfoScreen)

			{

				//houseInfoWindow->draw();

				yesNoButtons.draw();

			}

			else

			{

				//mentatButtons.draw();

				buttonExit.draw();

			}

			



			//eyes always there

			SDL_BlitSurface(eyesFrames[eyesFrame], NULL, screen, &eyesPos);



			if(currentViewID != -1)

			{

			//	fprintf(stdout, "drawing ID:%d currentFrame: %d\n", currentViewID, mentatEntries[currentViewID].curFrame);

			      SDL_Surface *pic;



				if(inBriefing || inInfoScreen)

				pic = levelEntry.pic[levelEntry.curFrame];

				else

					pic = mentatEntries[currentViewID].pic[mentatEntries[currentViewID].curFrame];

					

				graphicPos.x = mentatPos.x + 256;

				graphicPos.y = mentatPos.y + 96;

				graphicPos.w = pic->w;//change as the pictures are loaded....

				graphicPos.h = pic->h;



				

				SDL_BlitSurface(pic, NULL, screen, &graphicPos);



				//draw mouth if he hasnt stopped talking

				if(talkLength > 0)

				{

					talkLength--;

					SDL_BlitSurface(mouthFrames[mouthFrame], NULL, screen, &mouthPos);

				}

				

				//draw the text

//				textPos.x = mentatPos.x + 8;

//				textPos.y = mentatPos.y + 8;

//				textPos.w = textSurface->w;//change as the pictures are loaded....

//				textPos.h = textSurface->h;

//				SDL_BlitSurface(textSurface, NULL, screen, &textPos);



				textLabel.draw();

				

			}

			else 

			{

				listBox.draw();

			}



			//draw shoulder last

			SDL_BlitSurface(shoulderSurface, NULL, screen, &shoulderPos);



			updateTimers();

		

	}

	else

	{

		inBriefing = false;

		briefingRunning = false;

		currentViewID = -1;

		game_mentat_exited();

	}

}



void MentatClass::handlePress(int x, int y)

{

	if(inInfoScreen)

	{

		houseInfoWindow->handlePress(x, y);

	}

	else if(inHouseChoice)

	{

		houseChoiceWindow->handlePress(x, y);

	}

	else

	{

		mentatWindow->handlePress(x, y);

	}

}



void MentatClass::handleCancelPress(int x, int y)

{

	if(inInfoScreen)

	{

		houseInfoWindow->handleCancelPress(x, y);

	}

	else if(inHouseChoice)

	{

		houseChoiceWindow->handleCancelPress(x, y);

	}

	else

	{

		mentatWindow->handleCancelPress(x, y);

	}

}



void MentatClass::handleRelease(int x, int y)

{

	if(inInfoScreen)

	{

		houseInfoWindow->handleRelease(x, y);

	}

	else if(inHouseChoice)

	{

		houseChoiceWindow->handleRelease(x, y);

	}

	else

	{

		mentatWindow->handleRelease(x, y);

	}

}



void MentatClass::handleCancelRelease(int x, int y)

{

	if(inInfoScreen)

	{

		houseInfoWindow->handleCancelRelease(x, y);

	}

	else if(inHouseChoice)

	{

		houseChoiceWindow->handleCancelRelease(x, y);

	}

	else

	{

		mentatWindow->handleCancelRelease(x, y);

	}

}



void MentatClass::LoadData()

{

	//Mentat animations

/*	mentatFileNames[Mentat_Atreides] = "data/mentat/mentata.bmp";

	mentatFileNames[Mentat_Ordos] = "data/mentat/mentato.bmp";

	mentatFileNames[Mentat_Harkonnen] = "data/mentat/mentath.bmp";

	//mentatFileNames[Mentat_Harkonnen] = "data/mentat/mentatm.bmp";

	mentatFileNames[Mentat_Atreides_Eyes1] = "data/mentat/menshpa_eyes1.bmp";
	mentatFileNames[Mentat_Atreides_Eyes2] = "data/mentat/menshpa_eyes2.bmp";
	mentatFileNames[Mentat_Atreides_Eyes3] = "data/mentat/menshpa_eyes3.bmp";
	mentatFileNames[Mentat_Atreides_Eyes4] = "data/mentat/menshpa_eyes4.bmp";
	mentatFileNames[Mentat_Atreides_Eyes5] = "data/mentat/menshpa_eyes5.bmp";
	mentatFileNames[Mentat_Atreides_Mouth1] = "data/mentat/menshpa_mouth1.bmp";
	mentatFileNames[Mentat_Atreides_Mouth2] = "data/mentat/menshpa_mouth2.bmp";
	mentatFileNames[Mentat_Atreides_Mouth3] = "data/mentat/menshpa_mouth3.bmp";
	mentatFileNames[Mentat_Atreides_Mouth4] = "data/mentat/menshpa_mouth4.bmp";
	mentatFileNames[Mentat_Atreides_Mouth5] = "data/mentat/menshpa_mouth5.bmp";
	mentatFileNames[Mentat_Atreides_Shoulder] = "data/mentat/menshpa_shoulder.bmp";
	mentatFileNames[Mentat_Ordos_Eyes1] = "data/mentat/menshpo_eyes1.bmp";
	mentatFileNames[Mentat_Ordos_Eyes2] = "data/mentat/menshpo_eyes2.bmp";
	mentatFileNames[Mentat_Ordos_Eyes3] = "data/mentat/menshpo_eyes3.bmp";
	mentatFileNames[Mentat_Ordos_Eyes4] = "data/mentat/menshpo_eyes4.bmp";
	mentatFileNames[Mentat_Ordos_Eyes5] = "data/mentat/menshpo_eyes5.bmp";
	mentatFileNames[Mentat_Ordos_Mouth1] = "data/mentat/menshpo_mouth1.bmp";
	mentatFileNames[Mentat_Ordos_Mouth2] = "data/mentat/menshpo_mouth2.bmp";
	mentatFileNames[Mentat_Ordos_Mouth3] = "data/mentat/menshpo_mouth3.bmp";
	mentatFileNames[Mentat_Ordos_Mouth4] = "data/mentat/menshpo_mouth4.bmp";
	mentatFileNames[Mentat_Ordos_Mouth5] = "data/mentat/menshpo_mouth5.bmp";
	mentatFileNames[Mentat_Ordos_Shoulder] = "data/mentat/menshpo_shoulder.bmp";
	mentatFileNames[Mentat_Harkonnen_Eyes1] = "data/mentat/menshph_eyes1.bmp";
	mentatFileNames[Mentat_Harkonnen_Eyes2] = "data/mentat/menshph_eyes2.bmp";
	mentatFileNames[Mentat_Harkonnen_Eyes3] = "data/mentat/menshph_eyes3.bmp";
	mentatFileNames[Mentat_Harkonnen_Eyes4] = "data/mentat/menshph_eyes4.bmp";
	mentatFileNames[Mentat_Harkonnen_Eyes5] = "data/mentat/menshph_eyes5.bmp";
	mentatFileNames[Mentat_Harkonnen_Mouth1] = "data/mentat/menshph_mouth1.bmp";
	mentatFileNames[Mentat_Harkonnen_Mouth2] = "data/mentat/menshph_mouth2.bmp";
	mentatFileNames[Mentat_Harkonnen_Mouth3] = "data/mentat/menshph_mouth3.bmp";
	mentatFileNames[Mentat_Harkonnen_Mouth4] = "data/mentat/menshph_mouth4.bmp";
	mentatFileNames[Mentat_Harkonnen_Mouth5] = "data/mentat/menshph_mouth5.bmp";

	mentatFileNames[Mentat_Harkonnen_Shoulder] = "data/mentat/menshph_shoulder.bmp";

*/

	mentatFileNames[Mentat_Atreides] = "mentat/mentata.bmp";

	mentatFileNames[Mentat_Ordos] = "mentat/mentato.bmp";

	mentatFileNames[Mentat_Harkonnen] = "mentat/mentath.bmp";

	//mentatFileNames[Mentat_Harkonnen] = "mentat/mentatm.bmp";

	mentatFileNames[Mentat_Atreides_Eyes1] = "mentat/menshpa_eyes1.bmp";
	mentatFileNames[Mentat_Atreides_Eyes2] = "mentat/menshpa_eyes2.bmp";
	mentatFileNames[Mentat_Atreides_Eyes3] = "mentat/menshpa_eyes3.bmp";
	mentatFileNames[Mentat_Atreides_Eyes4] = "mentat/menshpa_eyes4.bmp";
	mentatFileNames[Mentat_Atreides_Eyes5] = "mentat/menshpa_eyes5.bmp";
	mentatFileNames[Mentat_Atreides_Mouth1] = "mentat/menshpa_mouth1.bmp";
	mentatFileNames[Mentat_Atreides_Mouth2] = "mentat/menshpa_mouth2.bmp";
	mentatFileNames[Mentat_Atreides_Mouth3] = "mentat/menshpa_mouth3.bmp";
	mentatFileNames[Mentat_Atreides_Mouth4] = "mentat/menshpa_mouth4.bmp";
	mentatFileNames[Mentat_Atreides_Mouth5] = "mentat/menshpa_mouth5.bmp";
	mentatFileNames[Mentat_Atreides_Shoulder] = "mentat/menshpa_shoulder.bmp";
	mentatFileNames[Mentat_Ordos_Eyes1] = "mentat/menshpo_eyes1.bmp";
	mentatFileNames[Mentat_Ordos_Eyes2] = "mentat/menshpo_eyes2.bmp";
	mentatFileNames[Mentat_Ordos_Eyes3] = "mentat/menshpo_eyes3.bmp";
	mentatFileNames[Mentat_Ordos_Eyes4] = "mentat/menshpo_eyes4.bmp";
	mentatFileNames[Mentat_Ordos_Eyes5] = "mentat/menshpo_eyes5.bmp";
	mentatFileNames[Mentat_Ordos_Mouth1] = "mentat/menshpo_mouth1.bmp";
	mentatFileNames[Mentat_Ordos_Mouth2] = "mentat/menshpo_mouth2.bmp";
	mentatFileNames[Mentat_Ordos_Mouth3] = "mentat/menshpo_mouth3.bmp";
	mentatFileNames[Mentat_Ordos_Mouth4] = "mentat/menshpo_mouth4.bmp";
	mentatFileNames[Mentat_Ordos_Mouth5] = "mentat/menshpo_mouth5.bmp";
	mentatFileNames[Mentat_Ordos_Shoulder] = "mentat/menshpo_shoulder.bmp";
	mentatFileNames[Mentat_Harkonnen_Eyes1] = "mentat/menshph_eyes1.bmp";
	mentatFileNames[Mentat_Harkonnen_Eyes2] = "mentat/menshph_eyes2.bmp";
	mentatFileNames[Mentat_Harkonnen_Eyes3] = "mentat/menshph_eyes3.bmp";
	mentatFileNames[Mentat_Harkonnen_Eyes4] = "mentat/menshph_eyes4.bmp";
	mentatFileNames[Mentat_Harkonnen_Eyes5] = "mentat/menshph_eyes5.bmp";
	mentatFileNames[Mentat_Harkonnen_Mouth1] = "mentat/menshph_mouth1.bmp";
	mentatFileNames[Mentat_Harkonnen_Mouth2] = "mentat/menshph_mouth2.bmp";
	mentatFileNames[Mentat_Harkonnen_Mouth3] = "mentat/menshph_mouth3.bmp";
	mentatFileNames[Mentat_Harkonnen_Mouth4] = "mentat/menshph_mouth4.bmp";
	mentatFileNames[Mentat_Harkonnen_Mouth5] = "mentat/menshph_mouth5.bmp";

	mentatFileNames[Mentat_Harkonnen_Shoulder] = "mentat/menshph_shoulder.bmp";





	//mentat texts

	//this should really be loaded from a file...will do that when i get the full texts i suppose

	mentatDescriptionsAtreides[Mentat_Concrete_Slab] = "A concrete foundation for structures. Use this or buildings will deteriorate.\n";
	mentatDescriptionsAtreides[Mentat_Barracks] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Carryall] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Construction_Yard] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Atreides_World] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Harkonnen_World] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Ordos_World] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Fremen] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Frigate] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Deathhand] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Devastator] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Harvester] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Radar] = "Description\n";
	mentatDescriptionsAtreides[Mentat_HighTech] = "Description\n";
	mentatDescriptionsAtreides[Mentat_SeigeTank] = "Description\n";
	mentatDescriptionsAtreides[Mentat_HeavyFactory] = "Description\n";
	mentatDescriptionsAtreides[Mentat_HeavyInfantry] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Infantry] = "Description\n";
	mentatDescriptionsAtreides[Mentat_IX] = "Description\n";
	mentatDescriptionsAtreides[Mentat_LightFactory] = "Description\n";
	mentatDescriptionsAtreides[Mentat_CombatTank] = "Description\n";
	mentatDescriptionsAtreides[Mentat_MCV] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Deviator] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Ornithopter] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Raider] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Palace] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Quad] = "Description\n";
	mentatDescriptionsAtreides[Mentat_Refinery] = "Description";
	mentatDescriptionsAtreides[Mentat_RepairBay] = "Description";
	mentatDescriptionsAtreides[Mentat_RocketTank] = "Description";
	mentatDescriptionsAtreides[Mentat_RocketTurret] = "Description";
	mentatDescriptionsAtreides[Mentat_Saboteur] = "Description";
	mentatDescriptionsAtreides[Mentat_Sardaukar] = "Description";
	mentatDescriptionsAtreides[Mentat_Slab] = "Description";
	mentatDescriptionsAtreides[Mentat_SonicTank] = "Description";
	mentatDescriptionsAtreides[Mentat_Starport] = "Description";
	mentatDescriptionsAtreides[Mentat_Silo] = "Description";
	mentatDescriptionsAtreides[Mentat_Trike] = "Description";
	mentatDescriptionsAtreides[Mentat_Turret] = "Description";
	mentatDescriptionsAtreides[Mentat_Wall] = "Description";
	mentatDescriptionsAtreides[Mentat_Windtrap] = "Description";
	mentatDescriptionsAtreides[Mentat_WOR] = "Description";
	mentatDescriptionsAtreides[Mentat_Worm] = "Description";



	for(int i=0;i<22;i++)

	{

		levelBriefingText[i] = "You are required to build a refinery and gather 2000 credits";

		levelpics[i] = Mentat_Harvester;

	}

/*

	//mentat data files

	mentatEntryFileNames[Mentat_Barracks] = "data/mentat/data/barrac";

	Mentat_Concrete_Slab,

	mentatEntryFileNames[Mentat_Carryall] = "data/mentat/data/carryall";
	mentatEntryFileNames[Mentat_Construction_Yard] = "data/mentat/data/construc";
	mentatEntryFileNames[Mentat_Atreides_World] = "data/mentat/data/fartr";
	mentatEntryFileNames[Mentat_Harkonnen_World] = "data/mentat/data/fhark";
	mentatEntryFileNames[Mentat_Ordos_World] = "data/mentat/data/fordos";
	mentatEntryFileNames[Mentat_Fremen] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Frigate] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Deathhand] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Devastator] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Harvester] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Radar] = "data/mentat/data/
	mentatEntryFileNames[Mentat_HighTech] = "data/mentat/data/
	mentatEntryFileNames[Mentat_SeigeTank] = "data/mentat/data/
	mentatEntryFileNames[Mentat_HeavyFactory] = "data/mentat/data/
	mentatEntryFileNames[Mentat_HeavyInfantry] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Infantry] = "data/mentat/data/
	mentatEntryFileNames[Mentat_IX] = "data/mentat/data/
	mentatEntryFileNames[Mentat_LightFactory] = "data/mentat/data/
	mentatEntryFileNames[Mentat_CombatTank] = "data/mentat/data/
	mentatEntryFileNames[Mentat_MCV] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Deviator] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Ornithopter] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Raider] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Palace] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Quad] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Refinery] = "data/mentat/data/
	mentatEntryFileNames[Mentat_RepairBay] = "data/mentat/data/
	mentatEntryFileNames[Mentat_RocketTank] = "data/mentat/data/
	mentatEntryFileNames[Mentat_RocketTurret] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Saboteur] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Sardaukar] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Slab] = "data/mentat/data/
	mentatEntryFileNames[Mentat_SonicTank] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Starport] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Silo] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Trike] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Turret] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Wall] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Windtrap] = "data/mentat/data/
	mentatEntryFileNames[Mentat_WOR] = "data/mentat/data/
	mentatEntryFileNames[Mentat_Worm] = "data/mentat/data/

*/

/*	LoadMentatEntry(Mentat_Barracks, FRAMES_BARRACKS, "data/mentat/data/barrac", "Barracks");

	LoadMentatEntry(Mentat_Concrete_Slab, FRAMES_CONCRETE_SLAB, "data/mentat/data/4slab", "4x4 Concrete Slab");
	LoadMentatEntry(Mentat_Carryall, FRAMES_CARRYALL, "data/mentat/data/carryall", "Carryall");
	LoadMentatEntry(Mentat_Construction_Yard, FRAMES_CONST_YARD, "data/mentat/data/construc", "Construction Yard");
	LoadMentatEntry(Mentat_Atreides_World, FRAMES_ATRE_WORLD, "data/mentat/data/fartr", "Atreides");
	LoadMentatEntry(Mentat_Harkonnen_World, FRAMES_HARK_WORLD, "data/mentat/data/fhark", "Harkonnen");
	LoadMentatEntry(Mentat_Ordos_World, FRAMES_ORDOS_WORLD, "data/mentat/data/fordos", "Ordos");

	LoadMentatEntry(Mentat_Fremen, FRAMES_FREMEN, "data/mentat/data/fremen", "Fremen");
	LoadMentatEntry(Mentat_Frigate, FRAMES_FRIGATE, "data/mentat/data/frigate", "Frigate");
	LoadMentatEntry(Mentat_Deathhand, FRAMES_DEATHHAND, "data/mentat/data/gold-bb", "Death Hand Missile");
	LoadMentatEntry(Mentat_Devastator, FRAMES_DEVASTATOR, "data/mentat/data/harktank", "Devastator");
	LoadMentatEntry(Mentat_Harvester, FRAMES_HARVESTER, "data/mentat/data/harvest", "Harvester");
	LoadMentatEntry(Mentat_Radar, FRAMES_RADAR, "data/mentat/data/headqrts", "Radar Outpost");
	LoadMentatEntry(Mentat_HighTech, FRAMES_HIGHTECH, "data/mentat/data/hitcftry", "High-tech Factory");
	LoadMentatEntry(Mentat_SeigeTank, FRAMES_SEIGETANK, "data/mentat/data/htank", "Seige Tank");
	LoadMentatEntry(Mentat_HeavyFactory, FRAMES_HEAVYFACT, "data/mentat/data/hvyftry", "Heavy Factory");
	LoadMentatEntry(Mentat_HeavyInfantry, FRAMES_HEAVYINF, "data/mentat/data/hyinfy", "Heavy Infantry");
	LoadMentatEntry(Mentat_Infantry, FRAMES_INFANTRY, "data/mentat/data/infantry", "Infantry");
	LoadMentatEntry(Mentat_IX, FRAMES_IX, "data/mentat/data/ix", "House of IX");
	LoadMentatEntry(Mentat_LightFactory, FRAMES_LIGHTFACT, "data/mentat/data/liteftry", "Light Factory");
	LoadMentatEntry(Mentat_CombatTank, FRAMES_COMBATTANK, "data/mentat/data/ltank", "Combat Tank");
	LoadMentatEntry(Mentat_MCV, FRAMES_MCV, "data/mentat/data/mcv", "Mobile Construction Vehicle");
	LoadMentatEntry(Mentat_Deviator, FRAMES_DEVIATOR, "data/mentat/data/ordrtank", "Deviator");
	LoadMentatEntry(Mentat_Ornithopter, FRAMES_ORNI, "data/mentat/data/orni", "Ornithopter");
	LoadMentatEntry(Mentat_Raider, FRAMES_RADAR, "data/mentat/data/otrike", "Raider");
	LoadMentatEntry(Mentat_Palace, FRAMES_PALACE, "data/mentat/data/palace", "Palace");
	LoadMentatEntry(Mentat_Quad, FRAMES_QUAD, "data/mentat/data/quad", "Quad");
	LoadMentatEntry(Mentat_Refinery, FRAMES_REFINERY, "data/mentat/data/refinery", "Refinery");
	LoadMentatEntry(Mentat_RepairBay, FRAMES_REPAIRBAY, "data/mentat/data/repair", "Repair Bay");
	LoadMentatEntry(Mentat_RocketTank, FRAMES_ROCKETTANK, "data/mentat/data/rtank", "Rocket Tank");
	LoadMentatEntry(Mentat_RocketTurret, FRAMES_ROCKETTURRET, "data/mentat/data/rturret", "Rocket Turret");
	LoadMentatEntry(Mentat_Saboteur,FRAMES_SABOTEUR, "data/mentat/data/saboture", "Saboteur");
	LoadMentatEntry(Mentat_Sardaukar, FRAMES_SARDAUKAR, "data/mentat/data/sardaukar", "Sardaukar");
	LoadMentatEntry(Mentat_Slab, FRAMES_SLAB, "data/mentat/data/slab", "1x1 Concrete Slab");
	LoadMentatEntry(Mentat_SonicTank, FRAMES_SONICTANK, "data/mentat/data/stank", "Sonic Tank");
	LoadMentatEntry(Mentat_Starport, FRAMES_STARPORT, "data/mentat/data/starport", "Starport");
	LoadMentatEntry(Mentat_Silo, FRAMES_SILO, "data/mentat/data/silo", "Silo");
	LoadMentatEntry(Mentat_Trike, FRAMES_TRIKE, "data/mentat/data/trike", "Trike");
	LoadMentatEntry(Mentat_Turret, FRAMES_TURRET, "data/mentat/data/turret", "Gun Turret");
	LoadMentatEntry(Mentat_Wall, FRAMES_WALL, "data/mentat/data/wall", "Wall");
	LoadMentatEntry(Mentat_Windtrap, FRAMES_WINDTRAP, "data/mentat/data/windtrap", "Windtrap");
	LoadMentatEntry(Mentat_WOR, FRAMES_WOR, "data/mentat/data/wor", "WOR");
	LoadMentatEntry(Mentat_Worm, FRAMES_WORM, "data/mentat/data/worm", "Sandworm");*/

		LoadMentatEntry(Mentat_Barracks, FRAMES_BARRACKS, "mentat/data/barrac", "Barracks");

	LoadMentatEntry(Mentat_Concrete_Slab, FRAMES_CONCRETE_SLAB, "mentat/data/4slab", "4x4 Concrete Slab");
	LoadMentatEntry(Mentat_Carryall, FRAMES_CARRYALL, "mentat/data/carryall", "Carryall");
	LoadMentatEntry(Mentat_Construction_Yard, FRAMES_CONST_YARD, "mentat/data/construc", "Construction Yard");
	LoadMentatEntry(Mentat_Atreides_World, FRAMES_ATRE_WORLD, "mentat/data/fartr", "Atreides");
	LoadMentatEntry(Mentat_Harkonnen_World, FRAMES_HARK_WORLD, "mentat/data/fhark", "Harkonnen");
	LoadMentatEntry(Mentat_Ordos_World, FRAMES_ORDOS_WORLD, "mentat/data/fordos", "Ordos");

	LoadMentatEntry(Mentat_Fremen, FRAMES_FREMEN, "mentat/data/fremen", "Fremen");
	LoadMentatEntry(Mentat_Frigate, FRAMES_FRIGATE, "mentat/data/frigate", "Frigate");
	LoadMentatEntry(Mentat_Deathhand, FRAMES_DEATHHAND, "mentat/data/gold-bb", "Death Hand Missile");
	LoadMentatEntry(Mentat_Devastator, FRAMES_DEVASTATOR, "mentat/data/harktank", "Devastator");
	LoadMentatEntry(Mentat_Harvester, FRAMES_HARVESTER, "mentat/data/harvest", "Harvester");
	LoadMentatEntry(Mentat_Radar, FRAMES_RADAR, "mentat/data/headqrts", "Radar Outpost");
	LoadMentatEntry(Mentat_HighTech, FRAMES_HIGHTECH, "mentat/data/hitcftry", "High-tech Factory");
	LoadMentatEntry(Mentat_SeigeTank, FRAMES_SEIGETANK, "mentat/data/htank", "Seige Tank");
	LoadMentatEntry(Mentat_HeavyFactory, FRAMES_HEAVYFACT, "mentat/data/hvyftry", "Heavy Factory");
	LoadMentatEntry(Mentat_HeavyInfantry, FRAMES_HEAVYINF, "mentat/data/hyinfy", "Heavy Infantry");
	LoadMentatEntry(Mentat_Infantry, FRAMES_INFANTRY, "mentat/data/infantry", "Infantry");
	LoadMentatEntry(Mentat_IX, FRAMES_IX, "mentat/data/ix", "House of IX");
	LoadMentatEntry(Mentat_LightFactory, FRAMES_LIGHTFACT, "mentat/data/liteftry", "Light Factory");
	LoadMentatEntry(Mentat_CombatTank, FRAMES_COMBATTANK, "mentat/data/ltank", "Combat Tank");
	LoadMentatEntry(Mentat_MCV, FRAMES_MCV, "mentat/data/mcv", "Mobile Construction Vehicle");
	LoadMentatEntry(Mentat_Deviator, FRAMES_DEVIATOR, "mentat/data/ordrtank", "Deviator");
	LoadMentatEntry(Mentat_Ornithopter, FRAMES_ORNI, "mentat/data/orni", "Ornithopter");
	LoadMentatEntry(Mentat_Raider, FRAMES_RADAR, "mentat/data/otrike", "Raider");
	LoadMentatEntry(Mentat_Palace, FRAMES_PALACE, "mentat/data/palace", "Palace");
	LoadMentatEntry(Mentat_Quad, FRAMES_QUAD, "mentat/data/quad", "Quad");
	LoadMentatEntry(Mentat_Refinery, FRAMES_REFINERY, "mentat/data/refinery", "Refinery");
	LoadMentatEntry(Mentat_RepairBay, FRAMES_REPAIRBAY, "mentat/data/repair", "Repair Bay");
	LoadMentatEntry(Mentat_RocketTank, FRAMES_ROCKETTANK, "mentat/data/rtank", "Rocket Tank");
	LoadMentatEntry(Mentat_RocketTurret, FRAMES_ROCKETTURRET, "mentat/data/rturret", "Rocket Turret");
	LoadMentatEntry(Mentat_Saboteur,FRAMES_SABOTEUR, "mentat/data/saboture", "Saboteur");
	LoadMentatEntry(Mentat_Sardaukar, FRAMES_SARDAUKAR, "mentat/data/sardaukar", "Sardaukar");
	LoadMentatEntry(Mentat_Slab, FRAMES_SLAB, "mentat/data/slab", "1x1 Concrete Slab");
	LoadMentatEntry(Mentat_SonicTank, FRAMES_SONICTANK, "mentat/data/stank", "Sonic Tank");
	LoadMentatEntry(Mentat_Starport, FRAMES_STARPORT, "mentat/data/starport", "Starport");
	LoadMentatEntry(Mentat_Silo, FRAMES_SILO, "mentat/data/silo", "Silo");
	LoadMentatEntry(Mentat_Trike, FRAMES_TRIKE, "mentat/data/trike", "Trike");
	LoadMentatEntry(Mentat_Turret, FRAMES_TURRET, "mentat/data/turret", "Gun Turret");
	LoadMentatEntry(Mentat_Wall, FRAMES_WALL, "mentat/data/wall", "Wall");
	LoadMentatEntry(Mentat_Windtrap, FRAMES_WINDTRAP, "mentat/data/windtrap", "Windtrap");
	LoadMentatEntry(Mentat_WOR, FRAMES_WOR, "mentat/data/wor", "WOR");
	LoadMentatEntry(Mentat_Worm, FRAMES_WORM, "mentat/data/worm", "Sandworm");
/*	

	switch (this->house)
	{
			case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
				mentatSurface = SDL_LoadBMP(mentatFileNames[Mentat_Atreides]);
				//load eyes
				eyesFrames[0] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Eyes1]);
				eyesFrames[1] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Eyes2]);
				eyesFrames[2] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Eyes3]);
				eyesFrames[3] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Eyes4]);
				eyesFrames[4] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Eyes5]);
				//load mouth
				mouthFrames[0] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Mouth1]);
				mouthFrames[1] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Mouth2]);
				mouthFrames[2] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Mouth3]);
				mouthFrames[3] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Mouth4]);
				mouthFrames[4] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Mouth5]);

				for(int i=0;i<5;i++)
					SDL_SetColorKey(mouthFrames[i], SDL_SRCCOLORKEY | SDL_RLEACCEL, 255);
				for(int i=0;i<5;i++)
					SDL_SetColorKey(eyesFrames[i], SDL_SRCCOLORKEY | SDL_RLEACCEL, 255);

				break;
			case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
				mentatSurface = SDL_LoadBMP(mentatFileNames[Mentat_Ordos]);

				eyesFrames[0] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Eyes1]);
				eyesFrames[1] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Eyes2]);
				eyesFrames[2] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Eyes3]);
				eyesFrames[3] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Eyes4]);
				eyesFrames[4] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Eyes5]);

				mouthFrames[0] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Mouth1]);
				mouthFrames[1] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Mouth2]);
				mouthFrames[2] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Mouth3]);
				mouthFrames[3] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Mouth4]);
				mouthFrames[4] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Mouth5]);

				for(int i=0;i<5;i++)
					SDL_SetColorKey(mouthFrames[i], SDL_SRCCOLORKEY | SDL_RLEACCEL, 255);
				for(int i=0;i<5;i++)
					SDL_SetColorKey(eyesFrames[i], SDL_SRCCOLORKEY | SDL_RLEACCEL, 255);

				break;
			case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
				mentatSurface = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen]);

				eyesFrames[0] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Eyes1]);
				eyesFrames[1] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Eyes2]);
				eyesFrames[2] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Eyes3]);
				eyesFrames[3] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Eyes4]);
				eyesFrames[4] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Eyes5]);

				mouthFrames[0] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Mouth1]);
				mouthFrames[1] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Mouth2]);
				mouthFrames[2] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Mouth3]);
				mouthFrames[3] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Mouth4]);
				mouthFrames[4] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Mouth5]);

				for(int i=0;i<5;i++)
					SDL_SetColorKey(mouthFrames[i], SDL_SRCCOLORKEY | SDL_RLEACCEL, 255);
				
				for(int i=0;i<5;i++)
					SDL_SetColorKey(eyesFrames[i], SDL_SRCCOLORKEY | SDL_RLEACCEL, 255);
				
				break;
			default:
				break;
	}

*/

	





}



void MentatClass::LoadMentatEntry(int entry, int frames, char *basePath, char *text)

{

	

	char *path = (char *)malloc(1024);

	mentatEntries[entry].curFrame = 0;

	mentatEntries[entry].frames = frames;

	mentatEntries[entry].basePath = basePath;

	mentatEntries[entry].text = text;

	mentatEntries[entry].pic = (SDL_Surface **)malloc(sizeof(SDL_Surface *) * frames);

	mentatEntries[entry].picID = entry;



	/*

	for(int i=1;i<=frames;i++)

	{

		fprintf(stdout, "loading %s\n", getFramePath(basePath, i));

		sprintf(path, "%s", getFramePath(basePath, i));

		//mentatEntries[entry].pic[i-1] = (SDL_Surface *)malloc(sizeof(SDL_Surface));

		mentatEntries[entry].pic[i-1] = SDL_LoadBMP(path);

	}

	*/



	//mentatFrames[entry] = frames;

	//mentatEntryFileNames[entry] = basePath;



}



void MentatClass::UnLoadMentatEntry(int entry)

{

	for(int i=0;i<mentatEntries[entry].frames;i++)

	{

		SDL_FreeSurface(mentatEntries[entry].pic[i]);

	}

}



char *MentatClass::getFramePath(char *basePath, int frame)

{



	char *framePath = (char *)malloc(strlen(basePath) + 9);

	char *end = (char *)malloc(10);



	if(frame-1 >= 10)

	{

		sprintf(end, " 00%d.bmp", frame -1);

	}

	else

	{

		sprintf(end, " 000%d.bmp", frame -1);

	}



	framePath = strcpy(framePath, basePath);

	framePath = strcat(framePath, end);



	return framePath;



}



void MentatClass::setMentatView(int entry)

{

	currentViewID = entry;



	if(!mentatEntries[entry].loaded)

	{

		char *path = (char *)malloc(1024);

		

		ZZIP_DIR* zzipdir = zzip_dir_open( zipPath, NULL );



		for(int i=1;i<=mentatEntries[entry].frames;i++)

		{

			//fprintf(stdout, "loading %s\n", getFramePath(basePath, i));

			sprintf(path, "%s", getFramePath(mentatEntries[entry].basePath, i));

			//mentatEntries[entry].pic[i-1] = (SDL_Surface *)malloc(sizeof(SDL_Surface));

			//mentatEntries[entry].pic[i-1] = SDL_LoadBMP(path);



			//mentatEntries[entry].pic[i-1] = SDL_LoadBMP_RW(SDL_RWFromZZIP(path, "rb"), 0);

			mentatEntries[entry].pic[i-1] = loadImageFromZip(zzipdir, path);

		}



		zzip_dir_close(zzipdir);



		mentatEntries[entry].loaded = true;

		free(path);

	}

}



void MentatClass::listButtonCallBack(void *val)

{

	BUILD_ITEM_DATA*	buildData = (BUILD_ITEM_DATA*)val;



	((MentatClass*)buildData->object)->makeTextSurface(buildData->buildNum);

	((MentatClass*)buildData->object)->setMentatView(buildData->buildNum);

	//currentViewID = Mentat_Palace;

}



void MentatClass::createList()

{

	selectionList = new ListIterator();



	for(int i=0;i<Mentat_Entry_Unknown;i++)

	{

		selectionList->insertLast(&mentatEntries[i]);

	}



	maxSelection = 6;

	minSelection = 0;



	checkMinMaxSelection();

}



void MentatClass::checkMinMaxSelection()

{

	Mentat_Entry *temp;



	temp = (Mentat_Entry *)selectionList->getElement(minSelection);

	data1.buildNum = temp->picID;

	data1.object = this;

	listBt1.setText(temp->text);

	listBt1.setCallBack(listButtonCallBack, &data1);



	temp = (Mentat_Entry *)selectionList->getElement(minSelection + 1);

	data2.buildNum = temp->picID;

	data2.object = this;

	listBt2.setText(temp->text);

	listBt2.setCallBack(listButtonCallBack, &data2);



	temp = (Mentat_Entry *)selectionList->getElement(minSelection + 2);

	data3.buildNum = temp->picID;

	data3.object = this;

	listBt3.setText(temp->text);

	listBt3.setCallBack(listButtonCallBack, &data3);



	temp = (Mentat_Entry *)selectionList->getElement(minSelection + 3);

	data4.buildNum = temp->picID;

	data4.object = this;

	listBt4.setText(temp->text);

	listBt4.setCallBack(listButtonCallBack, &data4);



	temp = (Mentat_Entry *)selectionList->getElement(minSelection + 4);

	data5.buildNum = temp->picID;

	data5.object = this;

	listBt5.setText(temp->text);

	listBt5.setCallBack(listButtonCallBack, &data5);



	temp = (Mentat_Entry *)selectionList->getElement(minSelection + 5);

	data6.buildNum = temp->picID;

	data6.object = this;

	listBt6.setText(temp->text);

	listBt6.setCallBack(listButtonCallBack, &data6);





}



void MentatClass::handleUp(void *val)

{

	((MentatClass *)val)->doUp();

}



void MentatClass::handleDown(void *val)

{

	((MentatClass *)val)->doDown();

}



void MentatClass::doUp()

{

	if(minSelection > 0)

	{

		minSelection--;

		maxSelection--;



		checkMinMaxSelection();

	}

}



void MentatClass::doDown()

{

	if(maxSelection < Mentat_Entry_Unknown)

	{

		minSelection++;

		maxSelection++;



		checkMinMaxSelection();

	}

}



void MentatClass::makeTextSurface(int num)

{

	//remember to change it for different houses

	talkLength = (int)((strlen(mentatDescriptionsAtreides[num]) * MOUTH_TIME) / 2);//variable talk time depending on length

//	textSurface = TTF_RenderText_Solid(font[14], mentatDescriptionsAtreides[num], palette->colors[COLOUR_WHITE]);

	textLabel.setText(mentatDescriptionsAtreides[num]);

	textLabel.setX(mentatPos.x);

	textLabel.setY(mentatPos.y);

}



void MentatClass::updateTimers()

{

	frameTimer--;

	mouthTimer--;

	eyesTimer--;



	if(frameTimer < 0 && currentViewID != -1)

	{

		if(inBriefing || inInfoScreen)

		{

			levelEntry.curFrame++;

			if(levelEntry.curFrame >= levelEntry.frames)

			{

				levelEntry.curFrame = 0;

			}



			

		}

		else

		{

			mentatEntries[currentViewID].curFrame++;

			if(mentatEntries[currentViewID].curFrame >= mentatEntries[currentViewID].frames)

			{

				mentatEntries[currentViewID].curFrame = 0;

			}

			

		}

		frameTimer = FRAME_TIME;

	}



	if(mouthTimer < 0)

	{

		mouthFrame++;

		if(mouthFrame >= 5)

		{

			mouthFrame = 0;

		}

		mouthTimer = MOUTH_TIME;

	}



	if(eyesTimer < 0)

	{

		eyesFrame++;

		if(eyesFrame >= 5)

		{

			eyesFrame = 0;

		}

		eyesTimer = EYES_TIME;

	}



}



void MentatClass::UnloadTemporaryData()

{

	mentatButtons.dropChildren();

	mentatWindow->dropChildren();

	listBox.dropChildren();



	//should get rid of all surfaces too...

	//SDL_FreeSurface(surface);



	//maybe put a check in to see if loading a different mentat or not? keep existing data?

	for(int i=0;i<5;i++)

	{

		SDL_FreeSurface(eyesFrames[i]);

		SDL_FreeSurface(mouthFrames[i]);

	}

}



SDL_Surface *MentatClass::loadImageFromZip(ZZIP_DIR *zzipdir, char *path)

{

	ZZIP_FILE* zfile = zzip_file_open(zzipdir, path, ZZIP_CASELESS);

	


	if(zfile)
	  {
	    SDL_RWops* rw = SDL_RWFromZZIP_FILE(zfile);

	    

	    if(rw)

	      {
		SDL_Surface *surf = SDL_LoadBMP_RW(rw, 0);
		SDL_FreeRW(rw);


		if(surf) 
		  return surf;
		else 
		  {
		    printf("Problem while loading surface from %s \n",*path);

		    exit(1);
		  };
	      };
	    zzip_file_close(zfile);
	  }
	else 
	  {
	    printf("File not found %s \n",*path);

	    return 0;
	  };	
}



void MentatClass::setHouse(int h)

{

	//fiishes the loading of the mentat



	ZZIP_DIR* zzipdir = zzip_dir_open(zipPath, NULL );

	



	if(zzipdir)

	{

		this->house = h;



		if(loadedBefore)

		{

			UnloadTemporaryData();

		}

/*

		switch (this->house)
		{
				case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
					mentatSurface = SDL_LoadBMP(mentatFileNames[Mentat_Atreides]);
					//load eyes
					eyesFrames[0] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Eyes1]);
					eyesFrames[1] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Eyes2]);
					eyesFrames[2] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Eyes3]);
					eyesFrames[3] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Eyes4]);
					eyesFrames[4] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Eyes5]);
					//load mouth
					mouthFrames[0] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Mouth1]);
					mouthFrames[1] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Mouth2]);
					mouthFrames[2] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Mouth3]);
					mouthFrames[3] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Mouth4]);
					mouthFrames[4] = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Mouth5]);

					shoulderSurface = SDL_LoadBMP(mentatFileNames[Mentat_Atreides_Shoulder]);

					break;
				case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
					mentatSurface = SDL_LoadBMP(mentatFileNames[Mentat_Ordos]);

					eyesFrames[0] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Eyes1]);
					eyesFrames[1] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Eyes2]);
					eyesFrames[2] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Eyes3]);
					eyesFrames[3] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Eyes4]);
					eyesFrames[4] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Eyes5]);

					mouthFrames[0] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Mouth1]);
					mouthFrames[1] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Mouth2]);
					mouthFrames[2] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Mouth3]);
					mouthFrames[3] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Mouth4]);
					mouthFrames[4] = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Mouth5]);

					shoulderSurface = SDL_LoadBMP(mentatFileNames[Mentat_Ordos_Shoulder]);

					break;
				case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
					mentatSurface = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen]);

					eyesFrames[0] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Eyes1]);
					eyesFrames[1] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Eyes2]);
					eyesFrames[2] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Eyes3]);
					eyesFrames[3] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Eyes4]);
					eyesFrames[4] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Eyes5]);

					mouthFrames[0] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Mouth1]);
					mouthFrames[1] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Mouth2]);
					mouthFrames[2] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Mouth3]);
					mouthFrames[3] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Mouth4]);
					mouthFrames[4] = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Mouth5]);

					shoulderSurface = SDL_LoadBMP(mentatFileNames[Mentat_Harkonnen_Shoulder]);

					break;
				default:
					break;
		}

*/

		switch (this->house)
		{
				case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
					mentatSurface = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Atreides]);
					//load eyes
					eyesFrames[0] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Atreides_Eyes1]);
					eyesFrames[1] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Atreides_Eyes2]);
					eyesFrames[2] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Atreides_Eyes3]);
					eyesFrames[3] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Atreides_Eyes4]);
					eyesFrames[4] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Atreides_Eyes5]);
					//load mouth
					mouthFrames[0] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Atreides_Mouth1]);
					mouthFrames[1] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Atreides_Mouth2]);
					mouthFrames[2] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Atreides_Mouth3]);
					mouthFrames[3] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Atreides_Mouth4]);
					mouthFrames[4] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Atreides_Mouth5]);

					shoulderSurface = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Atreides_Shoulder]);

					break;
				case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
					mentatSurface = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Ordos]);

					eyesFrames[0] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Ordos_Eyes1]);
					eyesFrames[1] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Ordos_Eyes2]);
					eyesFrames[2] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Ordos_Eyes3]);
					eyesFrames[3] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Ordos_Eyes4]);
					eyesFrames[4] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Ordos_Eyes5]);

					mouthFrames[0] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Ordos_Mouth1]);
					mouthFrames[1] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Ordos_Mouth2]);
					mouthFrames[2] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Ordos_Mouth3]);
					mouthFrames[3] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Ordos_Mouth4]);
					mouthFrames[4] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Ordos_Mouth5]);

					shoulderSurface = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Ordos_Shoulder]);

					break;
				case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
					mentatSurface = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Harkonnen]);

					eyesFrames[0] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Harkonnen_Eyes1]);
					eyesFrames[1] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Harkonnen_Eyes2]);
					eyesFrames[2] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Harkonnen_Eyes3]);
					eyesFrames[3] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Harkonnen_Eyes4]);
					eyesFrames[4] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Harkonnen_Eyes5]);

					mouthFrames[0] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Harkonnen_Mouth1]);
					mouthFrames[1] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Harkonnen_Mouth2]);
					mouthFrames[2] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Harkonnen_Mouth3]);
					mouthFrames[3] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Harkonnen_Mouth4]);
					mouthFrames[4] = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Harkonnen_Mouth5]);

					shoulderSurface = loadImageFromZip(zzipdir, mentatFileNames[Mentat_Harkonnen_Shoulder]);

					break;
				default:
					break;
		}



		//mentatExitSurf = SDL_LoadBMP("data/mentat/mentat_exit.bmp");

		//mentatExitSurfPressed = SDL_LoadBMP("data/mentat/mentat_exit_pressed.bmp");

		mentatExitSurf = loadImageFromZip(zzipdir, "mentat/mentat_exit.bmp");

		mentatExitSurfPressed = loadImageFromZip(zzipdir, "mentat/mentat_exit_pressed.bmp");



		zzip_dir_close(zzipdir);



		//set the transparent colour

		SDL_SetColorKey(shoulderSurface, SDL_SRCCOLORKEY | SDL_RLEACCEL, 255);

		for(int i=0;i<5;i++)
		{
			SDL_SetColorKey(mouthFrames[i], SDL_SRCCOLORKEY | SDL_RLEACCEL, 255);
			SDL_SetColorKey(eyesFrames[i], SDL_SRCCOLORKEY | SDL_RLEACCEL, 255);
		}			
		
			



		if(screen->w > mentatSurface->w)

			mentatPos.x = (screen->w - mentatSurface->w)/2;

		else

			mentatPos.x = 0;



		if(screen->h > mentatSurface->h)

			mentatPos.y = (screen->h - mentatSurface->h)/2;

		else

			mentatPos.y = 0;



		mentatPos.w = mentatSurface->w;

		mentatPos.h = mentatSurface->h;



		switch (this->house)
		{
				case(HOUSE_ATREIDES): case(HOUSE_FREMEN):

					//Atreides specific eye/mouth positions
					eyesPos.x = mentatPos.x + 80;

					eyesPos.y = mentatPos.y + 160;

					eyesPos.w = eyesFrames[0]->w;

					eyesPos.h = eyesFrames[0]->h;



					mouthPos.x = eyesPos.x;

					mouthPos.y = eyesPos.y + 32;

					mouthPos.w = mouthFrames[0]->w;

					mouthPos.h = mouthFrames[0]->h;

					break;

				case(HOUSE_ORDOS): case(HOUSE_MERCENARY):

					//HOUSE_ORDOS specific eye/mouth positions
					eyesPos.x = mentatPos.x + 32;

					eyesPos.y = mentatPos.y + 160;

					eyesPos.w = eyesFrames[0]->w;

					eyesPos.h = eyesFrames[0]->h;



					mouthPos.x = eyesPos.x;

					mouthPos.y = eyesPos.y + 32;

					mouthPos.w = mouthFrames[0]->w;

					mouthPos.h = mouthFrames[0]->h;			

					break;

				case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):

					//HOUSE_HARKONNEN specific eye/mouth positions
					eyesPos.x = mentatPos.x + 64;

					eyesPos.y = mentatPos.y + 176;

					eyesPos.w = eyesFrames[0]->w;

					eyesPos.h = eyesFrames[0]->h;



					mouthPos.x = eyesPos.x;

					mouthPos.y = eyesPos.y + 32;

					mouthPos.w = mouthFrames[0]->w;

					mouthPos.h = mouthFrames[0]->h;

					break;

		}



		graphicPos.x = mentatPos.x + 256;

		graphicPos.y = mentatPos.y + 96;

		graphicPos.w = 368;//change as the pictures are loaded....

		graphicPos.h = 224;



		shoulderPos.x = graphicPos.x;

		shoulderPos.y = graphicPos.y + graphicPos.h - shoulderSurface->h;

		shoulderPos.h = shoulderSurface->h;

		shoulderPos.w = shoulderSurface->w;



		mentatWindow->setWidth(mentatPos.w );

		mentatWindow->setHeight(mentatPos.h);

		mentatWindow->setY(mentatPos.y);

		mentatWindow->setX(mentatPos.x);



	//	mentatButtons.setHeight(mentatExitSurf->h + 15);

	//	mentatButtons.setWidth(mentatExitSurf->w + 15);

		//mentatButtons.setX(mentatPos.x + 300);

		//mentatButtons.setY(mentatPos.y + 350);

	//	mentatButtons.setX(mentatPos.x + 300);

	//	mentatButtons.setY(mentatPos.y + 350);

	//	mentatButtons.setVisible(true);



		buttonExit.setCallBack(game_mentat_exited);

		buttonExit.setSurface(mentatExitSurf, NULL);

		buttonExit.setPressedSurface(mentatExitSurfPressed, NULL);

		buttonExit.setHeight(mentatExitSurf->h);

		buttonExit.setWidth(mentatExitSurf->w);

		buttonExit.setX(mentatPos.x + 300);

		buttonExit.setY(mentatPos.y + 350);

		buttonExit.setVisible(true);



		//mentatButtons.addChild(&buttonExit);



		//Make the list



		data1.object = data2.object = data3.object = data4.object = data5.object = data6.object = this;



		listBox.setX(mentatPos.x + 256);

		listBox.setY(mentatPos.y + 96);

		listBox.setWidth(368);

		listBox.setHeight(224);

		listBox.setVisible(true);



		//Temp entries for now

		listUp.setVisible(true);

		listUp.setText("Up");

		listUp.setEnabled(true);

		listUp.setCallBack(handleUp, this);



		listBt1.setEnabled(true);

		listBt1.setVisible(true);

		listBt2.setEnabled(true);

		listBt2.setVisible(true);

		listBt3.setEnabled(true);

		listBt3.setVisible(true);

		listBt4.setEnabled(true);

		listBt4.setVisible(true);

		listBt5.setEnabled(true);

		listBt5.setVisible(true);

		listBt6.setEnabled(true);

		listBt6.setVisible(true);



		//probably want a better colour

		listUp.setBackgroundColour(COLOUR_LIGHTGREY);

		listBt1.setBackgroundColour(COLOUR_LIGHTGREY);

		listBt2.setBackgroundColour(COLOUR_LIGHTGREY);

		listBt3.setBackgroundColour(COLOUR_LIGHTGREY);

		listBt4.setBackgroundColour(COLOUR_LIGHTGREY);

		listBt5.setBackgroundColour(COLOUR_LIGHTGREY);

		listBt6.setBackgroundColour(COLOUR_LIGHTGREY);

		listDown.setBackgroundColour(COLOUR_LIGHTGREY);

		

		listDown.setVisible(true);

		listDown.setText("Down");

		listDown.setEnabled(true);

		listDown.setCallBack(handleDown, this);



		listBox.addChild(&listUp);

		listBox.addChild(&listBt1);

		listBox.addChild(&listBt2);

		listBox.addChild(&listBt3);

		listBox.addChild(&listBt4);

		listBox.addChild(&listBt5);

		listBox.addChild(&listBt6);

		listBox.addChild(&listDown);



		textLabel.setVisible(true);

		textLabel.setTransparent(true);



		mentatWindow->addChild(&textLabel);

		//mentatWindow->addChild(&mentatButtons);

		mentatWindow->addChild(&buttonExit);

		mentatWindow->addChild(&listBox);



		loadedBefore = 1;



		

	}



}



void MentatClass::doBriefing(int level)

{

	//FRAME_TIME = 10

	//get the picture thats supposed to display



//	levelEntry.curFrame = 0;

//	levelEntry.frames = 1;

//	levelEntry.basePath = "";

//	levelEntry.text = levelBriefingText[level];

//	levelEntry.pic = (SDL_Surface **)malloc(sizeof(SDL_Surface *) * levelEntry.frames);

//	levelEntry.picID = levelpics[level];



//	levelEntry.pic[0] = (SDL_Surface *)dataFile[levelpics[level]].dat;



//	levelEntry.frames = 0;

//	levelEntry.curFrame = 0;

//	levelEntry.pic = (SDL_Surface **)malloc(sizeof(SDL_Surface *) * 




  //martin:
  //memcpy does not work properly (linux) causing segmentation faults
  //
  //memcpy(&levelEntry, &mentatEntries[levelpics[level]], sizeof(Mentat_Entry));

  //using assignment;
  //levelEntry = mentatEntries[levelpics[level]];
  levelEntry.text = levelBriefingText[level];



	char *path = (char *)malloc(1024);



	ZZIP_DIR* zzipdir = zzip_dir_open( zipPath, NULL ); 



	for(int i=1;i<=levelEntry.frames;i++)

	{

		//fprintf(stdout, "loading %s\n", getFramePath(basePath, i));

		sprintf(path, "%s", getFramePath(levelEntry.basePath, i));

		//mentatEntries[entry].pic[i-1] = (SDL_Surface *)malloc(sizeof(SDL_Surface));

		//levelEntry.pic[i-1] = SDL_LoadBMP(path);

		if(loadImageFromZip(zzipdir, path))
		  levelEntry.pic[i-1] = loadImageFromZip(zzipdir, path);
		else 
		  fprintf(stderr, "Problem loading MentatBG");

	}



	zzip_dir_close(zzipdir);



	levelEntry.loaded = true;

	free(path);



	inBriefing = true;

	briefingRunning = true;

	currentViewID = 1;



	buttonExit.setCallBack(briefingButtonCallback, this);

	

	talkLength = (int)((strlen(levelEntry.text) * MOUTH_TIME) / 2);//variable talk time depending on length

//	textSurface = TTF_RenderText_Solid(font[14], levelEntry.text, palette->colors[COLOUR_WHITE]);

	textLabel.setText(levelEntry.text);

	textLabel.setX(mentatPos.x);

	textLabel.setY(mentatPos.y);

	textLabel.setVisible(true);



//	mentatWindow->addChild(textLabel);



	while(briefingRunning)

	{

		SDL_FillRect(screen, NULL, COLOUR_BLACK);

		draw();

		drawCursor();

		SDL_Flip(screen);

		//handleInput();



		int startTicks, curTicks;

		startTicks = SDL_GetTicks();

		while((curTicks = SDL_GetTicks()) - startTicks < WAIT_TIME)

		{

			handleInput();

		}

	}



	inBriefing = false;

	briefingRunning = false;

	buttonExit.setCallBack(game_mentat_exited);//change button back to normal

	currentViewID = -1;



	for(int i=0;i<levelEntry.frames;i++)

	{

	  SDL_FreeSurface(levelEntry.pic[i]);

	}

}



void MentatClass::handleInput()

{

	SDL_Event event;
	extern int drawnMouseX, drawnMouseY;
	while(SDL_PollEvent(&event))	//check for a key press
	{
		/* Look for a keypress */
		switch (event.type)
		{
			case (SDL_KEYDOWN):

				switch( event.key.keysym.sym )
				{

					case SDLK_ESCAPE:

						//showMessage("pressed escape");

						if(inHouseChoice)

							houseChoiceValue = -2;

					break;



					case SDLK_SPACE:

						//skipIndividScene = true;

						break;

				}

			break;

			case SDL_MOUSEBUTTONDOWN:
			{
				SDL_MouseButtonEvent* mouse = &event.button;

				//TEMP TEMP TEMP
				this->handlePress(mouse->x, mouse->y);

			}

			break;



			case SDL_MOUSEBUTTONUP:
			{

				SDL_MouseButtonEvent* mouse = &event.button;

				//TEMP TEMP TEMP
				this->handleRelease(mouse->x, mouse->y);

			}

			break;



			case SDL_MOUSEMOTION:
			{

				SDL_MouseMotionEvent* mouse = &event.motion;

				drawnMouseX = mouse->x;
				drawnMouseY = mouse->y;

			}

			break;

		}

	}

}



void MentatClass::briefingButtonCallback(void *val)

{

	((MentatClass *)val)->setBriefingRunning(false);

}



int MentatClass::doHouseChoice()

{

	ZZIP_DIR* zzipdir = zzip_dir_open(zipPath, NULL );



	//heraldSurf = SDL_LoadBMP("data/mentat/herald.bmp");

	heraldSurf = loadImageFromZip(zzipdir, "mentat/herald.bmp");



	zzip_dir_close(zzipdir);



	SDL_SetColorKey(heraldSurf, SDL_SRCCOLORKEY | SDL_RLEACCEL, 255);





	houseButtons.dropChildren();

	houseChoiceWindow->dropChildren();



	mentatPos.x = (screen->w - heraldSurf->w)/2;

	mentatPos.y = (screen->h - heraldSurf->h)/2;

	mentatPos.w = heraldSurf->w;

	mentatPos.h = heraldSurf->h;

	

	houseChoiceWindow->setWidth(heraldSurf->w);

	houseChoiceWindow->setHeight(heraldSurf->h);

	houseChoiceWindow->setX(mentatPos.x);

	houseChoiceWindow->setY(mentatPos.y);



	houseButtons.setHeight(heraldSurf->w);

	houseButtons.setWidth(heraldSurf->h);

	houseButtons.setX(mentatPos.x);

	houseButtons.setY(mentatPos.y);

	houseButtons.setVisible(true);



	houseAtre.setCallBack(houseChoiceCallbackA, this);

	houseAtre.setHeight(182);

	houseAtre.setWidth(168);

	houseAtre.setX(mentatPos.x + 40);

	houseAtre.setY(mentatPos.y + 108);

	houseAtre.setVisible(true);

	houseAtre.setEnabled(true);



	houseOrdos.setCallBack(houseChoiceCallbackO, this);

	houseOrdos.setHeight(182);

	houseOrdos.setWidth(168);

	houseOrdos.setX(mentatPos.x + 235);

	houseOrdos.setY(mentatPos.y + 108);

	houseOrdos.setVisible(true);

	houseOrdos.setEnabled(true);



	houseHark.setCallBack(houseChoiceCallbackH, this);

	houseHark.setHeight(182);

	houseHark.setWidth(168);

	houseHark.setX(mentatPos.x + 430);

	houseHark.setY(mentatPos.y + 108);

	houseHark.setVisible(true);

	houseHark.setEnabled(true);



//	houseButtons.addChild(&houseAtre);

//	houseButtons.addChild(&houseOrdos);

//	houseButtons.addChild(&houseHark);



//	houseChoiceWindow->addChild(&houseButtons);



	houseChoiceWindow->addChild(&houseAtre);

	houseChoiceWindow->addChild(&houseOrdos);

	houseChoiceWindow->addChild(&houseHark);



	houseChoiceValue = -1;



	inHouseChoice = true;

	

	while(houseChoiceValue == -1)

	{

		SDL_FillRect(screen, NULL, COLOUR_BLACK);

		draw();

		drawCursor();

		SDL_Flip(screen);

		handleInput();

	}



	inHouseChoice = false;



	SDL_FreeSurface(heraldSurf);



	return houseChoiceValue;

}



bool MentatClass::doHouseInfo(int h)

{



	SDL_Surface *yesSurf, *noSurf;

	SDL_Surface *yesSurfPressed, *noSurfPressed;



	this->setHouse(h);



	yesNoButtons.dropChildren();

	houseInfoWindow->dropChildren();



	switch(h)

	{

		case HOUSE_ATREIDES:

			memcpy(&levelEntry, &mentatEntries[Mentat_Atreides_World], sizeof(Mentat_Entry));

			levelEntry.text = "House Atreides:\nCaladan, home planet of the Atreides, has a warm, calm climate and the lands are lush and \ngreen. The rich soils and mild weather supports an extensive variety of agricultural \nactivities. In recent centuries, industrial and technological development has added to the\n prosperity of the Caladanian peoples.\n                                                                  Do you wish to join House Atreides?\n";

			break;

		case HOUSE_ORDOS:

			memcpy(&levelEntry, &mentatEntries[Mentat_Ordos_World], sizeof(Mentat_Entry));

			levelEntry.text = "House Ordos:\nThe home planet of the Ordos is a frigid and ice-covered world. We presume the Ordos \nimport their agricultural and technological goods from nearby star systems. Acting as\n traders and brokers, the Ordos produce no physical products of their own and rely on their \nmerchandising skills to survive\n                                                                  Do you wish to join House Ordos?\n";

			break;

		case HOUSE_HARKONNEN:

			memcpy(&levelEntry, &mentatEntries[Mentat_Harkonnen_World], sizeof(Mentat_Entry));

			levelEntry.text = "House Harkonnen:\nFrom the dark world of Giedi Prime, the savage House Harkonnen has spread across the \nuniverse. A cruel people, the Harkonnen are ruthless towards both friend and foe in their \nfanatical pursuit of power\n \n                                                                  Do you wish to join House Harkonnen?\n";

			break;

		default:

			memcpy(&levelEntry, &mentatEntries[Mentat_Atreides_World], sizeof(Mentat_Entry));

			levelEntry.text = "Atre";

			break;

	}

	

	char *path = (char *)malloc(1024);



	ZZIP_DIR* zzipdir = zzip_dir_open(zipPath, NULL );

	

	for(int i=1;i<=levelEntry.frames;i++)

	{

		//fprintf(stdout, "loading %s\n", getFramePath(basePath, i));

		sprintf(path, "%s", getFramePath(levelEntry.basePath, i));

		//mentatEntries[entry].pic[i-1] = (SDL_Surface *)malloc(sizeof(SDL_Surface));

		//levelEntry.pic[i-1] = SDL_LoadBMP(path);

		levelEntry.pic[i-1] = loadImageFromZip(zzipdir, path);

	}



//	yesSurf = SDL_LoadBMP("data/mentat/mentat_yes.bmp");

//	noSurf = SDL_LoadBMP("data/mentat/mentat_no.bmp");

//	yesSurfPressed = SDL_LoadBMP("data/mentat/mentat_yes_pressed.bmp");

//	noSurfPressed = SDL_LoadBMP("data/mentat/mentat_no_pressed.bmp");	

	

	yesSurf = loadImageFromZip(zzipdir, "mentat/mentat_yes.bmp");

	noSurf = loadImageFromZip(zzipdir, "mentat/mentat_no.bmp");

	yesSurfPressed = loadImageFromZip(zzipdir, "mentat/mentat_yes_pressed.bmp");

	noSurfPressed = loadImageFromZip(zzipdir, "mentat/mentat_no_pressed.bmp");



	zzip_dir_close(zzipdir);



	levelEntry.loaded = true;

	free(path);



//	if(textSurface)

//		SDL_FreeSurface(textSurface);



	talkLength = (int)((strlen(levelEntry.text) * MOUTH_TIME) / 2);//variable talk time depending on length

//	textSurface = TTF_RenderText_Solid(font[14], levelEntry.text, palette->colors[COLOUR_WHITE]);

 	textLabel.setText(levelEntry.text);

	textLabel.setX(mentatPos.x);

	textLabel.setY(mentatPos.y);





	houseInfoWindow->addChild(&textLabel);

	



	inInfoScreen = true;

	currentViewID = 1;











	houseInfoWindow->setWidth(mentatPos.w);

	houseInfoWindow->setHeight(mentatPos.h);

	houseInfoWindow->setX(mentatPos.x);

	houseInfoWindow->setY(mentatPos.y);



	yesNoButtons.setHeight(yesSurf->h + 15);

	yesNoButtons.setWidth(yesSurf->w + noSurf->w + 15);

	yesNoButtons.setX(mentatPos.x + 300);

	yesNoButtons.setY(mentatPos.y + 350);

	yesNoButtons.setVisible(true);



	yesButton.setCallBack(yesCallback, this);

	yesButton.setSurface(yesSurf, NULL);

	yesButton.setPressedSurface(yesSurfPressed, NULL);

	yesButton.setHeight(yesSurf->h);

	yesButton.setWidth(yesSurf->w);

	yesButton.setX(15);

	yesButton.setY(15);

	yesButton.setVisible(true);

	yesButton.setEnabled(true);



	noButton.setCallBack(noCallback, this);

	noButton.setSurface(noSurf, NULL);

	noButton.setPressedSurface(noSurfPressed, NULL);

	noButton.setHeight(noSurf->h);

	noButton.setWidth(noSurf->w);

	noButton.setX(15);

	noButton.setY(15);

	noButton.setVisible(true);

	noButton.setEnabled(true);



	yesNoButtons.addChild(&noButton);

	yesNoButtons.addChild(&yesButton);



	houseInfoWindow->addChild(&yesNoButtons);



	inInfoScreen = true;

	houseChoiceExitValue = 1;

	while(houseChoiceExitValue == 1)

	{

		SDL_FillRect(screen, NULL, COLOUR_BLACK);

		draw();

		drawCursor();

		SDL_Flip(screen);

			//wait the allocated frame time

		int startTicks, curTicks;



		startTicks = SDL_GetTicks();

		while((curTicks = SDL_GetTicks()) - startTicks < WAIT_TIME)

		{

			handleInput();

		}

		

		//handleInput();

	}



	inInfoScreen = false;



	SDL_FreeSurface(yesSurf);

	SDL_FreeSurface(noSurf);

	SDL_FreeSurface(yesSurfPressed);

	SDL_FreeSurface(noSurfPressed);



	if(houseChoiceExitValue == 2)

		return true;

	else

		return false;



}



void MentatClass::yesCallback(void *val)
{
	((MentatClass *)val)->setHouseChoiceExitValue(2);
}

void MentatClass::noCallback(void *val)

{

	((MentatClass *)val)->setHouseChoiceExitValue(3);

}



void MentatClass::houseChoiceCallbackA(void *val)

{

	soundPlayer->playVOSound(SoundVO_Matre);

	if(((MentatClass *)val)->doHouseInfo(HOUSE_ATREIDES))

		((MentatClass *)val)->setHouseChoiceValue(HOUSE_ATREIDES);

}



void MentatClass::houseChoiceCallbackO(void *val)

{

	soundPlayer->playVOSound(SoundVO_Mordos);

	if(((MentatClass *)val)->doHouseInfo(HOUSE_ORDOS))

		((MentatClass *)val)->setHouseChoiceValue(HOUSE_ORDOS);

}



void MentatClass::houseChoiceCallbackH(void *val)

{

	soundPlayer->playVOSound(SoundVO_Mhark);	

	if(((MentatClass *)val)->doHouseInfo(HOUSE_HARKONNEN))

		((MentatClass *)val)->setHouseChoiceValue(HOUSE_HARKONNEN);

}

