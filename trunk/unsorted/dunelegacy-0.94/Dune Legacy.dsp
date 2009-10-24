# Microsoft Developer Studio Project File - Name="Dune Legacy" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Dune Legacy - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Dune Legacy.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Dune Legacy.mak" CFG="Dune Legacy - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Dune Legacy - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Dune Legacy - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Dune Legacy - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zd /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "PLATFORM_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG PLATFORM_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib SDL.lib SDLmain.lib zziplib.lib libz.lib zlib.lib SDL_mixer.lib SDL_net.lib SDL_ttf.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none /incremental:yes /debug /nodefaultlib

!ELSEIF  "$(CFG)" == "Dune Legacy - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "PLATFORM_WINDOWS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib SDL.lib SDLmain.lib libz.lib zziplib.lib zlib.lib SDL_mixer.lib SDL_net.lib SDL_ttf.lib /nologo /subsystem:windows /profile /map /debug /machine:I386 /out:"Release/Dune Legacy.exe"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "Dune Legacy - Win32 Release"
# Name "Dune Legacy - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AiPlayerClass.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildItemClass.cpp
# End Source File
# Begin Source File

SOURCE=.\BulletClass.cpp
# End Source File
# Begin Source File

SOURCE=.\CommViewClass.cpp
# End Source File
# Begin Source File

SOURCE=.\CutScene.cpp
# End Source File
# Begin Source File

SOURCE=.\DataFile.cpp
# End Source File
# Begin Source File

SOURCE=.\editor.cpp
# End Source File
# Begin Source File

SOURCE=.\Link.cpp
# End Source File
# Begin Source File

SOURCE=.\LinkedList.cpp
# End Source File
# Begin Source File

SOURCE=.\ListIterator.cpp
# End Source File
# Begin Source File

SOURCE=.\listStuff.cpp
# End Source File
# Begin Source File

SOURCE=.\MapClass.cpp
# End Source File
# Begin Source File

SOURCE=.\MapGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\MentatClass.cpp
# End Source File
# Begin Source File

SOURCE=.\Menu.cpp
# End Source File
# Begin Source File

SOURCE=.\mmath.cpp
# End Source File
# Begin Source File

SOURCE=.\Net.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectClass.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerClass.cpp
# End Source File
# Begin Source File

SOURCE=.\PriorityQ.cpp
# End Source File
# Begin Source File

SOURCE=.\sand.cpp
# End Source File
# Begin Source File

SOURCE=.\zlib\SDL_rwops_zzip.c
# End Source File
# Begin Source File

SOURCE=.\SoundPlayerClass.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainClass.cpp
# End Source File
# End Group
# Begin Group "units"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\units\AirUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\units\AirUnit.h
# End Source File
# Begin Source File

SOURCE=.\units\Carryall.cpp
# End Source File
# Begin Source File

SOURCE=.\units\Carryall.h
# End Source File
# Begin Source File

SOURCE=.\units\DevastatorClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\DevastatorClass.h
# End Source File
# Begin Source File

SOURCE=.\units\DeviatorClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\DeviatorClass.h
# End Source File
# Begin Source File

SOURCE=.\units\FremenClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\Frigate.cpp
# End Source File
# Begin Source File

SOURCE=.\units\Frigate.h
# End Source File
# Begin Source File

SOURCE=.\units\GroundUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\units\GroundUnit.h
# End Source File
# Begin Source File

SOURCE=.\units\HarvesterClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\HarvesterClass.h
# End Source File
# Begin Source File

SOURCE=.\units\InfantryClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\InfantryClass.h
# End Source File
# Begin Source File

SOURCE=.\units\LauncherClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\LauncherClass.h
# End Source File
# Begin Source File

SOURCE=.\units\MCVClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\MCVClass.h
# End Source File
# Begin Source File

SOURCE=.\units\Ornithopter.cpp
# End Source File
# Begin Source File

SOURCE=.\units\Ornithopter.h
# End Source File
# Begin Source File

SOURCE=.\units\QuadClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\QuadClass.h
# End Source File
# Begin Source File

SOURCE=.\units\RaiderClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\Saboteur.cpp
# End Source File
# Begin Source File

SOURCE=.\units\Saboteur.h
# End Source File
# Begin Source File

SOURCE=.\units\SandWorm.cpp
# End Source File
# Begin Source File

SOURCE=.\units\SandWorm.h
# End Source File
# Begin Source File

SOURCE=.\units\SardukarClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\SiegeTankClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\SiegeTankClass.h
# End Source File
# Begin Source File

SOURCE=.\units\SonicTankClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\SonicTankClass.h
# End Source File
# Begin Source File

SOURCE=.\units\TankClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\TankClass.h
# End Source File
# Begin Source File

SOURCE=.\units\TrackedUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\units\TrackedUnit.h
# End Source File
# Begin Source File

SOURCE=.\units\TrikeClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\TrikeClass.h
# End Source File
# Begin Source File

SOURCE=.\units\TrooperClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\TrooperClass.h
# End Source File
# Begin Source File

SOURCE=.\units\UnitClass.cpp
# End Source File
# Begin Source File

SOURCE=.\units\UnitClass.h
# End Source File
# End Group
# Begin Group "structures"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\structures\BarracksClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\BarracksClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\BuilderClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\BuilderClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\ConstructionYardClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\ConstructionYardClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\GunTurretClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\GunTurretClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\HeavyFactoryClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\HeavyFactoryClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\HighTechFactoryClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\HighTechFactoryClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\IXClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\IXClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\LightFactoryClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\LightFactoryClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\PalaceClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\PalaceClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\RadarClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\RadarClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\RefineryClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\RefineryClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\RepairYardClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\RepairYardClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\RocketTurretClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\RocketTurretClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\SiloClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\SiloClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\StarPortClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\StarPortClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\StructureClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\StructureClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\WallClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\WallClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\WindTrapClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\WindTrapClass.h
# End Source File
# Begin Source File

SOURCE=.\structures\WORClass.cpp
# End Source File
# Begin Source File

SOURCE=.\structures\WORClass.h
# End Source File
# End Group
# Begin Group "gui"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gui\Box.cpp
# End Source File
# Begin Source File

SOURCE=.\gui\Box.h
# End Source File
# Begin Source File

SOURCE=.\gui\Button.cpp
# End Source File
# Begin Source File

SOURCE=.\gui\Button.h
# End Source File
# Begin Source File

SOURCE=.\gui\Container.cpp
# End Source File
# Begin Source File

SOURCE=.\gui\Container.h
# End Source File
# Begin Source File

SOURCE=.\gui\Entry.cpp
# End Source File
# Begin Source File

SOURCE=.\gui\Entry.h
# End Source File
# Begin Source File

SOURCE=.\gui\Graphics.cpp
# End Source File
# Begin Source File

SOURCE=.\gui\Graphics.h
# End Source File
# Begin Source File

SOURCE=.\gui\gui.h
# End Source File
# Begin Source File

SOURCE=.\gui\HBox.cpp
# End Source File
# Begin Source File

SOURCE=.\gui\HBox.h
# End Source File
# Begin Source File

SOURCE=.\gui\Label.cpp
# End Source File
# Begin Source File

SOURCE=.\gui\Label.h
# End Source File
# Begin Source File

SOURCE=.\gui\MessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\gui\MessageBox.h
# End Source File
# Begin Source File

SOURCE=.\gui\VBox.cpp
# End Source File
# Begin Source File

SOURCE=.\gui\VBox.h
# End Source File
# Begin Source File

SOURCE=.\gui\Widget.cpp
# End Source File
# Begin Source File

SOURCE=.\gui\Widget.h
# End Source File
# Begin Source File

SOURCE=.\gui\Window.cpp
# End Source File
# Begin Source File

SOURCE=.\gui\Window.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AiPlayerClass.h
# End Source File
# Begin Source File

SOURCE=.\BuildItemClass.h
# End Source File
# Begin Source File

SOURCE=.\BulletClass.h
# End Source File
# Begin Source File

SOURCE=.\CommViewClass.h
# End Source File
# Begin Source File

SOURCE=.\data.h
# End Source File
# Begin Source File

SOURCE=.\Items.h
# End Source File
# Begin Source File

SOURCE=.\Link.h
# End Source File
# Begin Source File

SOURCE=.\LinkedList.h
# End Source File
# Begin Source File

SOURCE=.\ListIterator.h
# End Source File
# Begin Source File

SOURCE=.\MapClass.h
# End Source File
# Begin Source File

SOURCE=.\MentatClass.h
# End Source File
# Begin Source File

SOURCE=.\Menu.h
# End Source File
# Begin Source File

SOURCE=.\mmath.h
# End Source File
# Begin Source File

SOURCE=.\Net.h
# End Source File
# Begin Source File

SOURCE=.\ObjectClass.h
# End Source File
# Begin Source File

SOURCE=.\PlayerClass.h
# End Source File
# Begin Source File

SOURCE=.\PriorityQ.h
# End Source File
# Begin Source File

SOURCE=.\sand.h
# End Source File
# Begin Source File

SOURCE=.\SoundPlayerClass.h
# End Source File
# Begin Source File

SOURCE=.\TerrainClass.h
# End Source File
# Begin Source File

SOURCE=.\terrainData.h
# End Source File
# End Group
# End Target
# End Project
