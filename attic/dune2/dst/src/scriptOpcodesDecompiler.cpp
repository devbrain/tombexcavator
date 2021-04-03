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

namespace script {

	void	_scriptHandlerDecompiler::o_goto(   ) {
		int labelPos = scriptLabel( _scriptData );

		if( !_modePreProcess ) {

			if( labelPos == -1)
				dataPrint( _scriptData );
			else
				_destinationFile << "l" << _scriptLabels[labelPos]._scriptPos;

		} else {
			if(labelPos == -1)
				scriptLabelAdd("", _scriptData);
		}
	}

	void	_scriptHandlerDecompiler::o_setreturn(   ) {
		dataPrint( _scriptData );
	}

	void	_scriptHandlerDecompiler::o_pushOp(   ) {
		if(_scriptData == 0)
			_stackCount--;

		if(_scriptData == 1) {
			_stackCount--;
			_stackCount--;
		}

		dataPrint(_scriptData);
	}

	void	_scriptHandlerDecompiler::o_push(  ) {
		_stackCount--;

		_scriptLastPush = _scriptData;
		dataPrint( _scriptData );
	}

	void	_scriptHandlerDecompiler::o_pushWord() {

		o_push();
	}

	void	_scriptHandlerDecompiler::o_pushreg(   ) {
		_stackCount--;
		
		dataPrint( _scriptData );
	}

	void	_scriptHandlerDecompiler::o_pushframeMinArg(   ) {
		_stackCount--;
		_stackCount--;

		dataPrint( _scriptData );
	}

	void	_scriptHandlerDecompiler::o_pushframePluArg(   ) {
		_stackCount--;
		_stackCount--;

		dataPrint( _scriptData );
	}

	void	_scriptHandlerDecompiler::o_popret(   ) {
		if( _scriptData == 1) {
			if( !_modePreProcess )
				_destinationFile << " (Return)";
			return;

		} else 
			_stackCount++;

		_stackCount++;
	}

	void	_scriptHandlerDecompiler::o_popreg(   ) {
		_stackCount++;

		dataPrint( _scriptData );
	}

	void	_scriptHandlerDecompiler::o_popframeMinArg(   ) {
		_stackCount++;
		_stackCount++;

		dataPrint( _scriptData );
	}

	void	_scriptHandlerDecompiler::o_popframePluArg(   ) {
		_stackCount++;
		_stackCount++;

		dataPrint( _scriptData );
	}

	void	_scriptHandlerDecompiler::o_spadd(   ) {
		dataPrint( _scriptData );
		_stackCount += (_scriptData & 0xF);
	}

	void	_scriptHandlerDecompiler::o_spsub(   ) {
		dataPrint( _scriptData );
		_stackCount -= (_scriptData & 0xF);
	}

	void	_scriptHandlerDecompiler::o_execute(   ) {

		if( !_modePreProcess )
			_destinationFile << left << _opcodesExecute[ _scriptData ].description << " ";
		
		(this->*_opcodesExecute[ _scriptData ].function)( );
	}

	void	_scriptHandlerDecompiler::o_ifnotgoto(   ) {
		int labelPos;

		labelPos = scriptLabel( _scriptData & 0x7FFF );

		if( _modePreProcess ) {
			if( labelPos == -1 )
				scriptLabelAdd("", _scriptData  & 0x7FFF );
		} else {
			if( labelPos == -1)
				dataPrint( _scriptData  & 0x7FFF );
			else
				_destinationFile << "l" << _scriptLabels[labelPos]._scriptPos;
		}
	}

	void	_scriptHandlerDecompiler::o_negate(   ) {
		dataPrint( _scriptData );
	}

	void	_scriptHandlerDecompiler::o_evaluate(   ) {

		if( !_modePreProcess )
			_destinationFile << _opcodesEvaluate[ _scriptData ].description;

		(this->*_opcodesEvaluate[ _scriptData ].function)( );
	}

	void	_scriptHandlerDecompiler::o_return(   ) {

	}


	// Evaluate Operators
	void	_scriptHandlerDecompiler::o_evaluate_IfEither( 	) {
		
	}

	void	_scriptHandlerDecompiler::o_evaluate_Equal(   ) {

	}

	void	_scriptHandlerDecompiler::o_evaluate_NotEqual(   ) {

	}

	void	_scriptHandlerDecompiler::o_evaluate_CompareGreaterEqual(   ) {

	}

	void	_scriptHandlerDecompiler::o_evaluate_CompareGreater(   ) {

	}

	void	_scriptHandlerDecompiler::o_evaluate_CompareLessEqual(   ) {

	}
	void	_scriptHandlerDecompiler::o_evaluate_CompareLess(   ) {

	}
	void	_scriptHandlerDecompiler::o_evaluate_Add(   ) {

	}
	void	_scriptHandlerDecompiler::o_evaluate_Subtract(   ) {

	}
	void	_scriptHandlerDecompiler::o_evaluate_Multiply(   ) {

	}

	void	_scriptHandlerDecompiler::o_evaluate_Divide(   ) {

	}
	void	_scriptHandlerDecompiler::o_evaluate_ShiftRight(   ) {

	}

	void	_scriptHandlerDecompiler::o_evaluate_ShiftLeft(   ) {

	}

	void	_scriptHandlerDecompiler::o_evaluate_And(   ) {

	}

	void	_scriptHandlerDecompiler::o_evaluate_Or(   ) {

	}

	void	_scriptHandlerDecompiler::o_evaluate_DivideRemainder(   ) {

	}

	void	_scriptHandlerDecompiler::o_evaluate_XOR(   ) {

	}

	void	_scriptHandlerDecompiler::o_execute_Building_Null(  ) {

	}

	void	_scriptHandlerDecompiler::o_execute_Unit_Null(  ) {

	}

	void	_scriptHandlerDecompiler::o_execute_Unit_GetInfo(  ) {
		if( !_modePreProcess )
			_destinationFile << "(" << nameUnitDetails[ _scriptLastPush ] << ")";
	}

	void	_scriptHandlerDecompiler::o_execute_House_Null(  ) {

	}

}
