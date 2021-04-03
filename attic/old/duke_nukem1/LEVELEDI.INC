type
{ Event handling codes for editor }

TEditorEvent=(
    eeNone,
    eeLevelUp,
    eeLevelDown,
    eeLevelLeft,
    eeLevelRight,
    eeLevelUpLeft,
    eeLevelUpRight,
    eeLevelDownLeft,
    eeLevelDownRight,
    eeLevelFastUp,
    eeLevelFastDown,
    eeLevelFastLeft,
    eeLevelFastRight,
    eeLevelHome,
    eeLevelEnd,
    eeLevelPgUp,
    eeLevelPgDn,
    eeTileUp,
    eeTileDown,
    eeTileHome,
    eeTileEnd,
    eeTilePgUp,
    eeTilePgDn,
    eeTileNext,
    eeTilePrev,
    eeTileSelect,
    eeWorkSpaceChange,
    eeZoomIn,
    eeZoomOut,
    eeMenuToggle,
    eeScreenModePrev,
    eeScreenModeNext,
    eeFullScreenToggle,
    eeUnmarkAll,
    eeFillSel,
    eeCopy,
    eeCopyTo,
    eeMove,
    eeMoveTo,
    eeSave,
    eeLoadLevel,
    eeLoadFileName,
    eeReload,
    eeSaveAsLevel,
    eeSaveAsFileName,
    eeCreateNew,
    eeQuit,
    eeAbort,
    eeToggleFlashers,
    eeToggleActives,
    eeToggleBridges,
    eeToggleConveyors,
    eeToggleMultis,
    eeGrab,
    eePlot,
    eePlotErase,
    eePlotToggleSel,
    eePlotAnchor,
    eeDragMark,
    eeDragUnmark,
    eeDragFill,
    eeDragErase);

const

EditMenuSize=16;
MessageDelay=182;

{ Editor command constants }
ecUnmark=2;
ecFill=3;
ecCopy=4;
ecMove=5;
ecSave=7;
ecSaveAsLevel=8;
ecSaveAsFile=9;
ecLoadLevel=10;
ecLoadFile=11;
ecReload=12;
ecCreateNew=13;
ecQuit=15;

{ Edit menu options }
EditCommandMenu:array[0..EditMenuSize-1] of TMenuOption=(
    (Name:'Edit Menu';     OptionType:mtTitle;  Code:0),
    (Name:'';              OptionType:mtBlank;  Code:0),
    (Name:'Clear Sel';     OptionType:mtOption; Code:ecUnmark),
    (Name:'Fill Sel';      OptionType:mtOption; Code:ecFill),
    (Name:'Copy';          OptionType:mtOption; Code:ecCopy),
    (Name:'Move';          OptionType:mtOption; Code:ecMove),
    (Name:'';              OptionType:mtBlank;  Code:0),
    (Name:'Save';          OptionType:mtOption; Code:ecSave),
    (Name:'Save as Level'; OptionType:mtOption; Code:ecSaveAsLevel),
    (Name:'Save as File';  OptionType:mtOption; Code:ecSaveAsFile),
    (Name:'Load Level';    OptionType:mtOption; Code:ecLoadLevel),
    (Name:'Load File';     OptionType:mtOption; Code:ecLoadFile),
    (Name:'Reload File';   OptionType:mtOption; Code:ecReload),
    (Name:'New';           OptionType:mtOption; Code:ecCreateNew),
    (Name:'';              OptionType:mtBlank;  Code:0),
    (Name:'Quit';          OptionType:mtOption; Code:ecQuit));


{ Edit menu help strings }
EditCommandHelp:array[0..EditMenuSize-1] of String[79]=(
    '',
    '',
    'Cancels the current selection (Ctrl-U)',
    'Fills the current selection with the selected sprite (Ctrl-F)',
    'Copies the current selection to another location or workspace (Ctrl-C)',
    'Moves the current selection to another location or workspace (Ctrl-M)',
    '',
    'Saves current workspace (F2)',
    'Saves current workspace as a level (F5)',
    'Saves current workspace as a file (F6)',
    'Loads an existing level into current workspace (F3)',
    'Loads an existing file into current workspace (F4)',
    'Reloads file from disk, undoing changes since last save (F7)',
    'Creates a new blank level in the current workspace (F9)',
    '',
    'Exits to DOS (F10)');

{ Other help messages }
hmDefault='DN1Mod v1.0  (C) 1997, 2000 B Rodgers   Released under GPL';

hmCopy='Copy: Select destination anchor point or hit ESC to cancel';
hmMove='Move: Select destination anchor point or hit ESC to cancel';

hmNotFound='Unable to find file';
hmBadName='Invalid filename (specify a valid filename)';
hmNoSpace='Insufficient space to save file';
hmFileError='Error reading file (possibly corrupted?)';

{ Scroll button numbers }
btLevelLeft=0;
btLevelRight=1;
btLevelUp=2;
btLevelDown=3;
btToolBarLeft=4;
btToolBarUp=5;
btToolBarDown=6;
btToolBarRight=7;
