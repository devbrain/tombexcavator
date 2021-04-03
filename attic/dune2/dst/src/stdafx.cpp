/* DST  -  Dune 2 Script Tools
 *  
 * Copyright (C) 2009-2014 Robert Crossfield <robcrossfield @ gmail.com>
 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *  
 */

#include "stdafx.h"

const char *version = "1.3";

// Object Names
const char *nameTeams[0x06] = {
	"Normal",
	"Staging",
	"Flee",
	"Kamikaze",
	"Guard",
	"Unknown"
};

const char *nameStructures[0x13] = {
	"Concrete",
	"Concrete4",
	"Palace",
	"LightFactory",
	"HeavyFactory",
	"Hi-Tech",
	"IX",
	"WOR",
	"ConstructionYard",
	"Windtrap",
	"Barracks",
	"Starport",
	"Refinery",
	"Repair",
	"Wall",
	"Turret",
	"R-Turret",
	"SpiceSilo",
	"Outpost"
};

const char *nameUnits[0x1B] = {
	"Carryall",
	"Ornithopter",
	"Infantry",
	"Troopers",
	"Soldier",
	"Trooper",
	"Saboteur",
	"Launcher",
	"Deviator",
	"Tank",
	"SiegeTank",
	"Devastator",
	"SonicTank",
	"Trike",
	"RaiderTrike",	
	"Quad",
	"Harvester",
	"MCV",
	"DeathHand",
	"Rocket",
	"ARocket",
	"GRocket",
	"MiniRocket",
	"Bullet",
	"SonicBlast",
	"Sandworm",
	"Frigate"
};

// Unit GetDetail Function DetailNames
const char *nameUnitDetails[0x14] = {
	"HitPoints",   
	"TargetMove",     
	"FireDistance",           
	"Index",              
	"Orientation",            
	"TargetAttack",                   
	"Return",
	"Type",
	"IndexEncoded",
	"MovingSpeed",                 
	"AngleToMove",          
	"IsMoving",                 
	"FireDelay",                 
	"ExplodeOnDeath",               
	"HouseID",            
	"ByScenario",                 
	"Orientation_Current",     
	"Orientation_TargetToCurrent",                 
	"TurretRotates",          
	"isHuman"              
};
