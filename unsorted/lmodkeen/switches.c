/* SWITCHES.C - Switch-handling routines.
**
** Original code Copyright (c)2002-2004 Andrew Durdin. (andy@durdin.net)
** Linux port code by Ignacio R. Morelle "Shadow Master". (shadowm2006@gmail.com)
** March 11th 2007
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software.
** Permission is granted to anyone to use this software for any purpose, including
** commercial applications, and to alter it and redistribute it freely, subject
** to the following restrictions:
**    1. The origin of this software must not be misrepresented; you must not
**       claim that you wrote the original software. If you use this software in
**       a product, an acknowledgment in the product documentation would be
**       appreciated but is not required.
**    2. Altered source versions must be plainly marked as such, and must not be
**       misrepresented as being the original software.
**    3. This notice may not be removed or altered from any source distribution.
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "utils.h"
#include "switches.h"

#include "linconio.h"

// GNU C compatibility:
#define stricmp strcasecmp

static SwitchStruct switches;

static void showswitches();
static void defaultswitches();
static int getswitch(char *string, char **option, char **value);

SwitchStruct *getswitches(int argc, char *argv[])
{
	char *option, *value;
	int i;
	
	defaultswitches();
	
	if(argc <= 1)
	{
		showswitches();
	}

	for(i = 1; i < argc; i++)
	{
		if(!getswitch(argv[i], &option, &value))
			quit("Invalid switch '%s'!", argv[i]);
		if(stricmp(option, "nowait") == 0)
		{
			NoWait = 1;
		}
			
		else if(stricmp(option, "export") == 0)
		{
			if(switches.Import)
				quit("Cannot both import and export!");
			switches.Export = 1;
		}
		else if(stricmp(option, "import") == 0)
		{
			if(switches.Export)
				quit("Cannot both import and export!");
			switches.Import = 1;
		}
		else if(stricmp(option, "episode") == 0)
		{
			if(!value)
				quit("No episode number given!");
			
			switches.Episode = atoi(value);
			if(switches.Episode < 1 || switches.Episode > 6)
				quit("Episode must be between 1 and 6!");
		}
		else if(stricmp(option, "keendir") == 0)
		{
			if(!value)
				quit("No directory for Keen files given!");

			strncpy(switches.InputPath, value, PATH_MAX);
		}
		else if(stricmp(option, "bmpdir") == 0)
		{
			if(!value)
				quit("No directory for BMP files given!");

			strncpy(switches.OutputPath, value, PATH_MAX);
		}
		else if(stricmp(option, "backup") == 0)
		{
			switches.Backup = 1;
		}
		else if(stricmp(option, "help") == 0 || stricmp(option, "?") == 0)
		{
			showswitches();
			exit(0);
		}
		else
		{
			showswitches();
			quit("Unknown switch '%s'!", option);
		}
	}
	
	if(!switches.Import && !switches.Export)
		quit("Either -import or -export must be given!");
	if(switches.Episode == 0)
		quit("The episode number must be given!");
	
	return &switches;
}

static void defaultswitches()
{
	strncpy(switches.InputPath, ".", PATH_MAX);
	strncpy(switches.OutputPath, ".", PATH_MAX);
	
	switches.Backup = 0;
	switches.Episode = 0;
	switches.Export = 0;
	switches.Import = 0;
}

/* Switch format: -option="value string" -option -option=value */
static int getswitch(char *string, char **option, char **value)
{
	unsigned int len = strlen(string);
	char *p;
	
	/* An empty string is not a valid switch */
	if(len == 0)
		return 0;
		
	/* A switch must begin with '-' or '/' */
	p = string;
	if(*p != '-' && *p != '/')
		return 0;
		
	p++;
	*option = p;
	*value = NULL;
	while(*p)
	{
		if(*p == '=' || *p == ':')
		{
			*p = 0;
			p++;
			
			if(*p != '"')
			{
				*value = p;
			}
			else
			{
				*value = p++;
				while(*p && *p != '"')
					p++;
				*p = 0;
			}
			
			/* We've found a valid switch */
			return 1;
		}
		p++;
	}
	/* We've found a valid switch -- but it has no value*/
	return 1;
}

static void showswitches()
{
	printf("  Valid options for MODKEEN are:\n"
			"    -nowait             [Doesn't ask for a keypress at exit; useful for scripts]\n"
			"    -episode=N          [Episode number; n = 1, 2, 3, 4, 5, or 6]\n"
			"    -export             [Export data from Keen to BMP files]\n"
			"    -import             [Import data from BMP files to Keen]\n"
			"    -keendir=DIRECTORY  [Keen files are in DIRECTORY (defaults to .)]\n"
			"    -bmpdir=DIRECTORY   [BMP files will be stored in DIRECTORY (defaults to .)]\n"
			"    -backup             [Create backups of changed files]\n"
			"    -help               [Shows the valid options for MODKEEN]\n"
			"\n"
			"  EXAMPLES:\n"
			"    modkeen -episode=4 -export -bmpdir=\"/home/joe/my_mods/keen4\"\n"
			"    modkeen -import -episode=1 -backup -nowait\n"
			"    modkeen -help\n"
			"\n");
}

