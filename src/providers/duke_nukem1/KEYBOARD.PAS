unit Keyboard;

{ Keyboard interface routines }

interface

const
    kbF1=$3B00;
    kbF2=$3C00;
    kbF3=$3D00;
    kbF4=$3E00;
    kbF5=$3F00;
    kbF6=$4000;
    kbF7=$4100;
    kbF8=$4200;
    kbF9=$4300;
    kbF10=$4400;
    kbF11=$8500;
    kbF12=$8600;
    kbShiftF1=$5400;
    kbShiftF2=$5500;
    kbShiftF3=$5600;
    kbShiftF4=$5700;
    kbShiftF5=$5800;
    kbShiftF6=$5900;
    kbShiftF7=$5A00;
    kbShiftF8=$5B00;
    kbShiftF9=$5C00;
    kbShiftF10=$5D00;
    kbShiftF11=$8700;
    kbShiftF12=$8800;
    kbCtrlF1=$5E00;
    kbCtrlF2=$5F00;
    kbCtrlF3=$6000;
    kbCtrlF4=$6100;
    kbCtrlF5=$6200;
    kbCtrlF6=$6300;
    kbCtrlF7=$6400;
    kbCtrlF8=$6500;
    kbCtrlF9=$6600;
    kbCtrlF10=$6700;
    kbCtrlF11=$8900;
    kbCtrlF12=$8A00;
    kbAltF1=$6800;
    kbAltF2=$6900;
    kbAltF3=$6A00;
    kbAltF4=$6B00;
    kbAltF5=$6C00;
    kbAltF6=$6D00;
    kbAltF7=$6E00;
    kbAltF8=$6F00;
    kbAltF9=$7000;
    kbAltF10=$7100;
    kbAltF11=$8B00;
    kbAltF12=$8C00;

    kbDel=$5300;
    kbDown=$5000;
    kbEnd=$4F00;
    kbHome =$4700;
    kbIns=$5200;
    kbLeft=$4B00;
    kbPgDn=$5100;
    kbPgUp=$4900;
    kbRight=$4D00;
    kbUp=$4800;

    kbGreyDel=$53E0;
    kbGreyDown=$50E0;
    kbGreyEnd=$4FE0;
    kbGreyHome =$47E0;
    kbGreyIns=$52E0;
    kbGreyLeft=$4BE0;
    kbGreyPgDn=$51E0;
    kbGreyPgUp=$49E0;
    kbGreyRight=$4DE0;
    kbGreyUp=$48E0;

    kbCtrlDel=$9300;
    kbCtrlDown=$9100;
    kbCtrlEnd=$7500;
    kbCtrlHome=$7700;
    kbCtrlIns=$9200;
    kbCtrlLeft=$7300;
    kbCtrlPgDn=$7600;
    kbCtrlPgUp=$8400;
    kbCtrlRight=$7400;
    kbCtrlUp=$8D00;

    kbCtrlGreyDel=$93E0;
    kbCtrlGreyDown=$91E0;
    kbCtrlGreyEnd=$75E0;
    kbCtrlGreyHome=$77E0;
    kbCtrlGreyIns=$92E0;
    kbCtrlGreyLeft=$73E0;
    kbCtrlGreyPgDn=$76E0;
    kbCtrlGreyPgUp=$84E0;
    kbCtrlGreyRight=$74E0;
    kbCtrlGreyUp=$8DE0;

    kbEnter=$1C0D;
    kbEsc=$011B;
    kbBkSpc=$0E08;
    kbSpace=$3920;
    kbTab=$0F09;

    kbShiftBkSpc=$0E08;
    kbShiftDel=$532E;
    kbShiftDown=$5032;
    kbShiftEnd=$4F31;
    kbShiftEnter=$1C0D;
    kbShiftEsc=$011B;
    kbShiftHome=$4737;
    kbShiftIns=$5230;
    kbShiftLeft=$4B34;
    kbShiftPgDn=$5133;
    kbShiftPgUp=$4939;
    kbShiftRight=$4D36;
    kbShiftSpace=$3920;
    kbShiftTab=$0F00;
    kbShiftUp=$4838;

    kbCtrlBkSpc=$0E7F;
    kbCtrlEnter=$1C0A;
    kbCtrlEsc=$011B;
    kbCtrlSpace=$3920;
    kbCtrlTab=$9400;

    kbAltBkSpc=$0E00;
    kbAltDel=$A300;
    kbAltDown=$A000;
    kbAltEnd=$9F00;
    kbAltEnter=$A600;
    kbAltEsc=$0100;
    kbAltHome=$9700;
    kbAltIns=$A200;
    kbAltLeft=$9B00;
    kbAltPgDn=$A100;
    kbAltPgUp=$9900;
    kbAltRight=$9D00;
    kbAltSpace=$3920;
    kbAltTab=$A500;
    kbAltUp=$9800;
    kbAlt1=$7800;
    kbAlt2=$7900;
    kbAlt3=$7A00;
    kbAlt4=$7B00;
    kbAlt5=$7C00;
    kbAlt6=$7D00;
    kbAlt7=$7E00;
    kbAlt8=$7F00;
    kbAlt9=$8000;
    kbAlt0=$8100;
    kbAltA=$1E00;
    kbAltB=$3000;
    kbAltC=$2E00;
    kbCtrlU=$1615;
    kbCtrlF=$2106;
    kbCtrlE=$1205;
    kbCtrlC=$2E03;
    kbCtrlM=$320D;
    kbGreyPlus=$4E2B;
    kbGreyMinus=$4A2D;
    kbCtrlLSQBracket=$1A1B;
    kbCtrlRSQBracket=$1B1D;

    kfRShift=$01;
    kfLShift=$02;
    kfShift=$03;
    kfCtrl=$04;
    kfAlt=$08;

var
{ BIOS shift key state variabe }
    KeyFlags:Byte absolute $0040:$0017;

{ GetKey returns the scancode/ASCII combination of the first key waiting in }
{ the keyboard buffer, or returns zero if no keys are pending. Any keys }
{ returned are removed from the keyboard buffer. }
function GetKey:Word;

{ WaitKey blocks program execution until a key is pressed, and returns the }
{ scancode/ASCII combination of that key, removing it from the keyboard }
{ buffer. }
function WaitKey:Word;

implementation

function GetKey:Word; assembler;
asm
    MOV AH,$11
    INT $16
    JZ @NoKey
    CALL WaitKey
    JMP @Done
@NoKey:
    MOV AX,$0000
@Done:
end;

function WaitKey:Word; assembler;
asm
    MOV AH,$10
    INT $16
end;

end.
