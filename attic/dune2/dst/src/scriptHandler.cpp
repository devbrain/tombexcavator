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
#include "scriptHandler.h"
#include "scriptHandlerDecompiler.h"
#include "scriptHandlerCompiler.h"

namespace script {

	// Constructor
	_scriptHandler::_scriptHandler( const char *fileName ) {
		_fileName = fileName;
		
		_headerPointers	 = 0;
		_pointerCount	 = 0;

		_scriptBuffer	 = 0;
		_scriptPtr		 = 0;
		_scriptPos		 = 0;

		_modePreProcess = true;
	}

	// Destructor
	_scriptHandler::~_scriptHandler() {
		
		if( _scriptBuffer )
			delete _scriptBuffer;

		if( _headerPointers )
			delete _headerPointers;
	}

	// Search the opcode table for a string opcode name
	word _scriptHandler::scriptOpcodeFind( string opcodeStr, const _Opcode *opcodes ) {
		const _Opcode *opcode;
		word  count =0;

		// Search opcode list
		for( opcode = opcodes; opcode->description; opcode++, count++ ) {
			if( opcodeStr.compare(opcode->description) == 0)
				return count;
		}

		// Return invalid opcode
		return 0xFFFF;
	}

	// Setup the opcode name/function table
	void _scriptHandler::opcodesSetup() {
		static const _Opcode scriptOpcodes[0x14] = {
			"Goto",				&_scriptHandler::o_goto,				// 0
			"SetReturn",		&_scriptHandler::o_setreturn,			// 1
			"PushOp",			&_scriptHandler::o_pushOp,				// 2
			"PushWord",			&_scriptHandler::o_pushWord,			// 3
			"PushByte",			&_scriptHandler::o_push,				// 4
			"PushVar",			&_scriptHandler::o_pushreg,				// 5
			"PushLocalVar",		&_scriptHandler::o_pushframeMinArg,		// 6
			"PushParameter",	&_scriptHandler::o_pushframePluArg,		// 7
			"Pop",				&_scriptHandler::o_popret,				// 8
			"PopReg",			&_scriptHandler::o_popreg,				// 9
			"PopLocalVar",		&_scriptHandler::o_popframeMinArg,		// A
			"PopParameter",		&_scriptHandler::o_popframePluArg,		// B
			"AddSP",			&_scriptHandler::o_spadd,				// C
			"SubSP",			&_scriptHandler::o_spsub,				// D
			"Execute",			&_scriptHandler::o_execute,				// E
			"IfNotGoto",		&_scriptHandler::o_ifnotgoto,			// F
			"Negate",			&_scriptHandler::o_negate,				// 10
			"Evaluate",			&_scriptHandler::o_evaluate,			// 11
			"Return",			&_scriptHandler::o_return,				// 12
		};
		
		// The 'Evaluate' opcode functions
		static const _Opcode scriptOpcodesEvaluate[0x12] = {
			"IfAnd",					&_scriptHandler::o_evaluate_IfEither,				// 0
			"IfOr",						&_scriptHandler::o_evaluate_IfEither,				// 1
			"Equal",					&_scriptHandler::o_evaluate_Equal,					// 2
			"NotEqual",					&_scriptHandler::o_evaluate_NotEqual,				// 3
			"CompareLess",		&_scriptHandler::o_evaluate_CompareGreaterEqual,	// 4
			"CompareLessEqual",			&_scriptHandler::o_evaluate_CompareGreater,			// 5
			"CompareRight",			&_scriptHandler::o_evaluate_CompareLessEqual,		// 6
			"CompareRightEqual",				&_scriptHandler::o_evaluate_CompareLess,			// 7
			"Add",						&_scriptHandler::o_evaluate_Add,					// 8
			"Subtract",					&_scriptHandler::o_evaluate_Subtract,				// 9
			"Multiply",					&_scriptHandler::o_evaluate_Multiply,				// A
			"Divide",					&_scriptHandler::o_evaluate_Divide,					// B
			"ShiftRight",				&_scriptHandler::o_evaluate_ShiftRight,				// C
			"ShiftLeft",				&_scriptHandler::o_evaluate_ShiftLeft,				// D
			"And",						&_scriptHandler::o_evaluate_And,					// E
			"Or",						&_scriptHandler::o_evaluate_Or,						// F
			"DivideRemainder",			&_scriptHandler::o_evaluate_DivideRemainder,		// 10
			"XOR",						&_scriptHandler::o_evaluate_XOR,					// 11
		};

		_opcodes		  = scriptOpcodes;
		_opcodesEvaluate  = scriptOpcodesEvaluate;
	}

	// The 'Building' Execute functions
	void _scriptHandler::opcodesBuildingsSetup() {
		static const _Opcode scriptOpcodesExecuteBuildings[0x19] = {
			"Delay",					&_scriptHandler::o_execute_Building_Null,					// 0
			"Null",						&_scriptHandler::o_execute_Building_Null,					// 1
			"Unknown0A81",				&_scriptHandler::o_execute_Building_Null,					// 2	
			"FindUnitByType",			&_scriptHandler::o_execute_Building_Null,					// 3
			"SetState",					&_scriptHandler::o_execute_Building_Null,					// 4
			"DisplayText",				&_scriptHandler::o_execute_Building_Null,					// 5
			"Unknown11B9",				&_scriptHandler::o_execute_Building_Null,					// 6
			"Unknown0C5A",				&_scriptHandler::o_execute_Building_Null,					// 7
			"FindTargetUnit",			&_scriptHandler::o_execute_Building_Null,					// 8
			"RotateTurret",				&_scriptHandler::o_execute_Building_Null,					// 9
			"GetDirection",				&_scriptHandler::o_execute_Building_Null,					// A
			"Fire",						&_scriptHandler::o_execute_Building_Null,					// B
			"Null",						&_scriptHandler::o_execute_Building_Null,					// C
			"GetState",					&_scriptHandler::o_execute_Building_Null,					// D
			"VoicePlay",				&_scriptHandler::o_execute_Building_Null,					// E
			"RemoveFogAroundTile",		&_scriptHandler::o_execute_Building_Null,					// F
			"Null",						&_scriptHandler::o_execute_Building_Null,					// 10
			"Null",						&_scriptHandler::o_execute_Building_Null,					// 11
			"Null",						&_scriptHandler::o_execute_Building_Null,					// 12
			"Null",						&_scriptHandler::o_execute_Building_Null,					// 13
			"Null",						&_scriptHandler::o_execute_Building_Null,					// 14
			"RefineSpice",				&_scriptHandler::o_execute_Building_Null,					// 15
			"Explode",					&_scriptHandler::o_execute_Building_Null,					// 16
			"Destroy",					&_scriptHandler::o_execute_Building_Null,					// 17
			"Null",						&_scriptHandler::o_execute_Building_Null,					// 18
		};

		_opcodesExecute = scriptOpcodesExecuteBuildings;

	}

	// The 'Units' Execute functions
	void _scriptHandler::opcodesUnitsSetup() {
		static const _Opcode scriptOpcodesExecuteUnits[0x64] = {
			"GetInfo",				&_scriptHandler::o_execute_Unit_GetInfo,					// 0 	
			"SetAction",				&_scriptHandler::o_execute_Unit_Null,					// 1
			"DisplayText",   			&_scriptHandler::o_execute_Unit_Null,					// 2
			"GetDistanceToTile",		&_scriptHandler::o_execute_Unit_Null,					// 3
			"StartAnimation",			&_scriptHandler::o_execute_Unit_Null,					// 4
			"SetDestination",			&_scriptHandler::o_execute_Unit_Null,					// 5
			"GetOrientation",			&_scriptHandler::o_execute_Unit_Null,					// 6
			"SetOrientation",			&_scriptHandler::o_execute_Unit_Null,					// 7
			"Fire",						&_scriptHandler::o_execute_Unit_Null,					// 8
			"MCVDeploy",				&_scriptHandler::o_execute_Unit_Null,					// 9
			"SetActionDefault",			&_scriptHandler::o_execute_Unit_Null,					// A
			"Blink",					&_scriptHandler::o_execute_Unit_Null,					// B
			"CalculateRoute",			&_scriptHandler::o_execute_Unit_Null,					// C
			"IsEnemy",					&_scriptHandler::o_execute_Unit_Null,					// D
			"ExplosionSingle",			&_scriptHandler::o_execute_Unit_Null,					// E
			"Die",						&_scriptHandler::o_execute_Unit_Null,					// F
			"Delay",					&_scriptHandler::o_execute_Unit_Null,					// 10
			"IsFriendly",				&_scriptHandler::o_execute_Unit_Null,					// 11
			"ExplosionMultiple",		&_scriptHandler::o_execute_Unit_Null,					// 12
			"SetSprite",				&_scriptHandler::o_execute_Unit_Null,					// 13
			"TransportDeliver",			&_scriptHandler::o_execute_Unit_Null,					// 14
			"Null",						&_scriptHandler::o_execute_Unit_Null,					// 15
			"MoveToTarget",				&_scriptHandler::o_execute_Unit_Null,					// 16
			"RandomRange",				&_scriptHandler::o_execute_Unit_Null,					// 17
			"FindIdle",					&_scriptHandler::o_execute_Unit_Null,					// 18
			"SetDestinationDirect",		&_scriptHandler::o_execute_Unit_Null,					// 19
			"Stop",						&_scriptHandler::o_execute_Unit_Null,					// 1A
			"SetSpeed",					&_scriptHandler::o_execute_Unit_Null,					// 1B
			"FindBestTarget",			&_scriptHandler::o_execute_Unit_Null,					// 1C
			"GetTargetPriority",		&_scriptHandler::o_execute_Unit_Null,					// 1D
			"MoveToStructure",			&_scriptHandler::o_execute_Unit_Null,					// 1E
			"IsInTransport",			&_scriptHandler::o_execute_Unit_Null,					// 1F
			"GetAmount",				&_scriptHandler::o_execute_Unit_Null,					// 20
			"RandomSoldier",			&_scriptHandler::o_execute_Unit_Null,					// 21
			"Pickup",					&_scriptHandler::o_execute_Unit_Null,					// 22
			"CallUnitByType",			&_scriptHandler::o_execute_Unit_Null,					// 23
			"Unknown2552",				&_scriptHandler::o_execute_Unit_Null,					// 24
			"FindStructure",			&_scriptHandler::o_execute_Unit_Null,					// 25
			"VoicePlay",				&_scriptHandler::o_execute_Unit_Null,					// 26
			"DisplayDestroyedText",		&_scriptHandler::o_execute_Unit_Null,					// 27
			"RemoveFog",				&_scriptHandler::o_execute_Unit_Null,					// 28
			"SearchSpice",				&_scriptHandler::o_execute_Unit_Null,					// 29
			"Harvest",					&_scriptHandler::o_execute_Unit_Null,					// 2A
			"Null",						&_scriptHandler::o_execute_Unit_Null,					// 2B
			"GetLinkedUnitType",		&_scriptHandler::o_execute_Unit_Null,					// 2C
			"GetIndexType",				&_scriptHandler::o_execute_Unit_Null,					// 2D
			"DecodeIndex",				&_scriptHandler::o_execute_Unit_Null,					// 2E
			"IsValidDestination",		&_scriptHandler::o_execute_Unit_Null,					// 2F
			"GetRandomTile",			&_scriptHandler::o_execute_Unit_Null,					// 30
			"IdleAction",				&_scriptHandler::o_execute_Unit_Null,					// 31
			"UnitCount",				&_scriptHandler::o_execute_Unit_Null,					// 32
			"GoToClosestStructure",		&_scriptHandler::o_execute_Unit_Null,					// 33
			"Null",						&_scriptHandler::o_execute_Unit_Null,					// 34
			"Null",						&_scriptHandler::o_execute_Unit_Null,					// 35
			"GetBestTarget",			&_scriptHandler::o_execute_Unit_Null,					// 36
			"Unknown2BD5",				&_scriptHandler::o_execute_Unit_Null,					// 37
			"General_GetOrientation",	&_scriptHandler::o_execute_Unit_Null,					// 38
			"Null",						&_scriptHandler::o_execute_Unit_Null,					// 39
			"SetTarget",				&_scriptHandler::o_execute_Unit_Null,					// 3A
			"Unknown0288",				&_scriptHandler::o_execute_Unit_Null,					// 3B
			"DelayRandom",				&_scriptHandler::o_execute_Unit_Null,					// 3C
			"Rotate",					&_scriptHandler::o_execute_Unit_Null,					// 3D
			"GetDistanceToObject",		&_scriptHandler::o_execute_Unit_Null,					// 3E
			"Null",						&_scriptHandler::o_execute_Unit_Null,					// 3F
		};

		_opcodesExecute = scriptOpcodesExecuteUnits;
	}

	// The 'Team' Execute functions
	void _scriptHandler::opcodesHousesSetup() {
		static const _Opcode scriptOpcodesExecuteTeams[0xF] = {
			"Delay",					&_scriptHandler::o_execute_House_Null,		// 0 	
			"DisplayText",				&_scriptHandler::o_execute_House_Null,		// 1
			"GetMembers",				&_scriptHandler::o_execute_House_Null,		// 2
			"AddCloserUnit",			&_scriptHandler::o_execute_House_Null,		// 3
			"GetAverageDistance",		&_scriptHandler::o_execute_House_Null,		// 4
			"Unknown0543",				&_scriptHandler::o_execute_House_Null,		// 5
			"FindBestTarget",			&_scriptHandler::o_execute_House_Null,		// 6
			"Unknown0788",				&_scriptHandler::o_execute_House_Null,		// 7
			"Load",						&_scriptHandler::o_execute_House_Null,		// 8
			"Load2",					&_scriptHandler::o_execute_House_Null,		// 9
			"DelayRandom",				&_scriptHandler::o_execute_House_Null,		// A
			"DisplayModalMessage",		&_scriptHandler::o_execute_House_Null,		// B
			"GetVariable6",				&_scriptHandler::o_execute_House_Null,		// C
			"GetTarget",				&_scriptHandler::o_execute_House_Null,		// D
			"Null",						&_scriptHandler::o_execute_House_Null,		// E
		};

		_opcodesExecute = scriptOpcodesExecuteTeams;
	}

}
