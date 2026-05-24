#include <Editor/Menubar.h>

#include <Core/WideString.h>
#include <Core/Utils.h>
#include <Core/StringUtils.h>

#include <Managers/NoteskinMan.h>
#include <Managers/SimfileMan.h>
#include <Managers/StyleMan.h>
#include <Managers/LocaleMan.h>

#include <Editor/Action.h>
#include <Editor/Shortcuts.h>
#include <Editor/Editor.h>
#include <Editor/Statusbar.h>
#include <Editor/Notefield.h>
#include <Editor/View.h>
#include <Editor/Waveform.h>
#include <Editor/Editing.h>
#include <Editor/Minimap.h>
#include <Editor/TempoBoxes.h>

#include <System/System.h>
#include <System/Debug.h>

namespace Vortex {

namespace {

#define MENU ((MenuBarImpl*)gMenubar)

// ================================================================================================
// MenuBarImpl :: member data.

struct MenuBarImpl : public Menubar {

typedef void(*UpdateFunction)();
typedef System::MenuItem Item;

Item* myFileMenu;
Item* myVisualSyncMenu;
Item* myViewMenu;
Item* myMinimapMenu;
Item* myBgStyleMenu;
Item* myStatusMenu;
Item* myEditMenu;

UpdateFunction myUpdateFunctions[NUM_PROPERTIES];

// ================================================================================================
// MenuBarImpl :: constructor and destructor.

~MenuBarImpl()
{
}

MenuBarImpl()
{
	registerUpdateFunctions();
}

// ================================================================================================
// MenuBarImpl :: menu construction functions.

static Item* newMenu()
{
	return System::MenuItem::create();
}

static void sep(Item* menu)
{
	menu->addSeperator();
}

static void add(Item* menu, Action::Type action, const char* str)
{
	String notation = gShortcuts->getNotation(action);
	if(notation.len())
	{
		String combined(str);
		Str::append(combined, '\t');
		Str::append(combined, notation);
		menu->addItem(action, combined);
	}
	else
	{
		menu->addItem(action, str);
	}
}

static void add(Item* menu, Action::Type action, StringRef str)
{
	add(menu, action, str.str());
}

static void add(Item* menu, Action::Type action, const wchar_t* str)
{
	add(menu, action, Narrow(str).str());
}

static void add(Item* menu, int item, const wchar_t* str)
{
	menu->addItem(item, Narrow(str).str());
}

static void add(Item* menu, int item, const char* str)
{
	menu->addItem(item, str);
}

static void add(Item* menu, int item, StringRef str)
{
	menu->addItem(item, str.str());
}

static void sub(Item* menu, Item* sub, const char* str)
{
	menu->addSubmenu(sub, str);
}

static void sub(Item* menu, Item* sub, StringRef str)
{
	menu->addSubmenu(sub, str.str());
}

void init(Item* menu)
{
	using namespace Action;

	// File menu.
	Item* hFile = newMenu();
	add(hFile, FILE_OPEN, _TR("Open..."));
	add(hFile, 0 /*dummy*/, _TR("Recent files"));
	add(hFile, FILE_CLOSE, _TR("Close"));
	sep(hFile);
	add(hFile, FILE_SAVE, _TR("Save"));
	add(hFile, FILE_SAVE_AS, _TR("Save as..."));
	sep(hFile);
	add(hFile, OPEN_DIALOG_SONG_PROPERTIES, _TR("Properties..."));
	sep(hFile);
	add(hFile, EXIT_PROGRAM, _TR("Exit"));
	myFileMenu = hFile;

	// Edit menu.
	Item* hEdit = myEditMenu = newMenu();
	add(hEdit, EDIT_UNDO, _TR("Undo"));
	add(hEdit, EDIT_REDO, _TR("Redo"));
	sep(hEdit);
	add(hEdit, EDIT_CUT, _TR("Cut"));
	add(hEdit, EDIT_COPY, _TR("Copy"));
	add(hEdit, EDIT_PASTE, _TR("Paste"));
	add(hEdit, EDIT_DELETE, _TR("Delete"));
	sep(hEdit);
	add(hEdit, SELECT_ALL, _TR("Select all"));
	add(hEdit, SELECT_REGION, _TR("Select region"));
	sep(hEdit);
	add(hEdit, TOGGLE_JUMP_TO_NEXT_NOTE, _TR("Enable jump to next note"));
	add(hEdit, TOGGLE_UNDO_REDO_JUMP, _TR("Enable undo/redo jump"));
	add(hEdit, TOGGLE_TIME_BASED_COPY, _TR("Enable time-based copy"));

	// Chart > Convert menu.
	Item* hChartConvert = newMenu();
	add(hChartConvert, CHART_CONVERT_ROUTINE_TO_COUPLES, L"Routine \x2192 ITG Couple");
	add(hChartConvert, CHART_CONVERT_COUPLES_TO_ROUTINE, L"ITG Couple \x2192 Routine");

	// Chart menu.
	Item* hChart = newMenu();
	add(hChart, OPEN_DIALOG_CHART_LIST, _TR("Chart list..."));
	add(hChart, OPEN_DIALOG_CHART_PROPERTIES, _TR("Properties..."));
	add(hChart, OPEN_DIALOG_DANCING_BOT, _TR("Dancing bot..."));
	sep(hChart);
	add(hChart, OPEN_DIALOG_NEW_CHART, _TR("New chart..."));
	sep(hChart);
	add(hChart, CHART_PREVIOUS, _TR("Previous chart"));
	add(hChart, CHART_NEXT, _TR("Next chart"));
	sep(hChart);
	sub(hChart, hChartConvert, _TR("Convert"));
	sep(hChart);
	add(hChart, CHART_DELETE, _TR("Delete chart"));

	// Notes > Select > Quantization menu.
	Item* hSelectQuant = newMenu();
	add(hSelectQuant, SELECT_QUANT_4, _TR("4th"));
	add(hSelectQuant, SELECT_QUANT_8, _TR("8th"));
	add(hSelectQuant, SELECT_QUANT_12, _TR("12th"));
	add(hSelectQuant, SELECT_QUANT_16, _TR("16th"));
	add(hSelectQuant, SELECT_QUANT_24, _TR("24th"));
	add(hSelectQuant, SELECT_QUANT_32, _TR("32nd"));
	add(hSelectQuant, SELECT_QUANT_48, _TR("48th"));
	add(hSelectQuant, SELECT_QUANT_64, _TR("64th"));
	add(hSelectQuant, SELECT_QUANT_192, _TR("192nd"));

	// Notes > Select menu.
	Item* hSelection = newMenu();
	sub(hSelection, hSelectQuant, _TR("Quantization"));
	sep(hSelection);
	add(hSelection, SELECT_ALL_STEPS, _TR("Steps"));
	add(hSelection, SELECT_ALL_MINES, _TR("Mines"));
	add(hSelection, SELECT_ALL_HOLDS, _TR("Holds"));
	add(hSelection, SELECT_ALL_ROLLS, _TR("Rolls"));
	add(hSelection, SELECT_ALL_FAKES, _TR("Fakes"));
	add(hSelection, SELECT_ALL_LIFTS, _TR("Lifts"));
	sep(hSelection);
	add(hSelection, SELECT_REGION_BEFORE_CURSOR, _TR("Before cursor"));
	add(hSelection, SELECT_REGION_AFTER_CURSOR, _TR("After cursor"));

	// Notes > Convert menu.
	Item* hNoteConvert = newMenu();
	add(hNoteConvert, CHANGE_NOTES_TO_MINES, L"Notes \x2192 Mines");
	add(hNoteConvert, CHANGE_NOTES_TO_FAKES, L"Notes \x2192 Fakes");
	add(hNoteConvert, CHANGE_NOTES_TO_LIFTS, L"Notes \x2192 Lifts");
	sep(hNoteConvert);
	add(hNoteConvert, CHANGE_MINES_TO_NOTES, L"Mines \x2192 Notes");
	add(hNoteConvert, CHANGE_MINES_TO_FAKES, L"Mines \x2192 Fakes");
	add(hNoteConvert, CHANGE_MINES_TO_LIFTS, L"Mines \x2192 Lifts");
	sep(hNoteConvert);
	add(hNoteConvert, CHANGE_FAKES_TO_NOTES, L"Fakes \x2192 Notes");
	add(hNoteConvert, CHANGE_LIFTS_TO_NOTES, L"Lifts \x2192 Notes");
	sep(hNoteConvert);
	add(hNoteConvert, CHANGE_BETWEEN_HOLDS_AND_ROLLS, L"Holds \x2194 Rolls");
	add(hNoteConvert, CHANGE_HOLDS_TO_STEPS, L"Holds \x2192 Steps");
	add(hNoteConvert, CHANGE_HOLDS_TO_MINES, L"Holds \x2192 Mines");
	sep(hNoteConvert);
	add(hNoteConvert, CHANGE_BETWEEN_PLAYER_NUMBERS, L"Switch Player");

	// Notes > Mirror menu.
	Item* hNoteMirror = newMenu();
	add(hNoteMirror, MIRROR_NOTES_HORIZONTALLY, _TR("Horizontally"));
	add(hNoteMirror, MIRROR_NOTES_VERTICALLY, _TR("Vertically"));
	add(hNoteMirror, MIRROR_NOTES_FULL, _TR("Both"));

	// Notes > Expand menu.
	Item* hNoteExpand = newMenu();
	add(hNoteExpand, SCALE_NOTES_2_TO_1, _TR("2:1 (8th to 4th)"));
	add(hNoteExpand, SCALE_NOTES_3_TO_2, _TR("3:2 (12th to 8th)"));
	add(hNoteExpand, SCALE_NOTES_4_TO_3, _TR("4:3 (16th to 12th)"));

	// Notes > Compress menu.
	Item* hNoteCompress = newMenu();
	add(hNoteCompress, SCALE_NOTES_1_TO_2, _TR("1:2 (4th to 8th)"));
	add(hNoteCompress, SCALE_NOTES_2_TO_3, _TR("2:3 (8th to 12th)"));
	add(hNoteCompress, SCALE_NOTES_3_TO_4, _TR("3:4 (12th to 16th)"));

	// Notes menu.
	Item* hNotes = newMenu();
	sub(hNotes, hSelection, _TR("Select"));
	sub(hNotes, hNoteConvert, _TR("Convert"));
	sub(hNotes, hNoteMirror, _TR("Mirror"));
	sub(hNotes, hNoteExpand, _TR("Expand"));
	sub(hNotes, hNoteCompress, _TR("Compress"));
	add(hNotes, OPEN_DIALOG_GENERATE_NOTES, _TR("Generate..."));

	// Tempo > Select menu.
	Item* hSelectTempo = newMenu();
	add(hSelectTempo, SELECT_TEMPO_BPM, _TR("BPM"));
	add(hSelectTempo, SELECT_TEMPO_STOP, _TR("Stop"));
	add(hSelectTempo, SELECT_TEMPO_DELAY, _TR("Delay"));
	add(hSelectTempo, SELECT_TEMPO_WARP, _TR("Warp"));
	add(hSelectTempo, SELECT_TEMPO_TIME_SIG, _TR("Time Sig."));
	add(hSelectTempo, SELECT_TEMPO_TICK_COUNT, _TR("Tick Count"));
	add(hSelectTempo, SELECT_TEMPO_COMBO, _TR("Combo"));
	add(hSelectTempo, SELECT_TEMPO_SPEED, _TR("Speed"));
	add(hSelectTempo, SELECT_TEMPO_SCROLL, _TR("Scroll"));
	add(hSelectTempo, SELECT_TEMPO_FAKE, _TR("Fake"));
	add(hSelectTempo, SELECT_TEMPO_LABEL, _TR("Label"));

	// Tempo > Visual sync menu
	myVisualSyncMenu = newMenu();
	add(myVisualSyncMenu, SET_VISUAL_SYNC_CURSOR_ANCHOR, _TR("Cursor row"));
	add(myVisualSyncMenu, SET_VISUAL_SYNC_RECEPTOR_ANCHOR, _TR("Receptors row"));
  
	// Tempo menu.
	Item* hTempo = newMenu();
	sub(hTempo, hSelectTempo, _TR("Select"));
	sep(hTempo);
	add(hTempo, OPEN_DIALOG_ADJUST_SYNC, _TR("Adjust sync..."));
	add(hTempo, OPEN_DIALOG_ADJUST_TEMPO, _TR("Adjust tempo..."));
	add(hTempo, OPEN_DIALOG_ADJUST_TEMPO_SM5, _TR("Adjust tempo SM5..."));
	sep(hTempo);
	add(hTempo, SWITCH_TO_SYNC_MODE, _TR("Sync mode"));
	add(hTempo, OPEN_DIALOG_TEMPO_BREAKDOWN, _TR("Breakdown..."));
	sub(hTempo, myVisualSyncMenu, _TR("Visual sync anchor"));

	// Audio > Volume menu.
	Item* hAudioVol = newMenu();
	add(hAudioVol, VOLUME_RESET, _TR("Default"));
	sep(hAudioVol);
	add(hAudioVol, VOLUME_INCREASE, _TR("Louder"));
	add(hAudioVol, VOLUME_DECREASE, _TR("Softer"));
	sep(hAudioVol);
	add(hAudioVol, VOLUME_MUTE, _TR("Mute"));

	// Audio > Speed menu.
	Item* hAudioSpeed = newMenu();
	add(hAudioSpeed, SPEED_RESET, _TR("Default"));
	sep(hAudioSpeed);
	add(hAudioSpeed, SPEED_INCREASE, _TR("Faster"));
	add(hAudioSpeed, SPEED_DECREASE, _TR("Slower"));

	// Audio menu.
	Item* hAudio = newMenu();
	sub(hAudio, hAudioVol, _TR("Volume"));
	sub(hAudio, hAudioSpeed, _TR("Speed"));
	sep(hAudio);
	add(hAudio, TOGGLE_BEAT_TICK, _TR("Beat tick"));
	add(hAudio, TOGGLE_NOTE_TICK, _TR("Note tick"));
	sep(hAudio);
	add(hAudio, CONVERT_MUSIC_TO_OGG, _TR("Convert to ogg"));

	// View > Minimap menu.
	Item* hViewMm = myMinimapMenu = newMenu();
	add(hViewMm, MINIMAP_SET_NOTES, _TR("Notes"));
	add(hViewMm, MINIMAP_SET_DENSITY, _TR("Density"));
	
	// View > Background menu.
	Item* hViewBg = myBgStyleMenu = newMenu();
	add(hViewBg, BACKGROUND_HIDE, _TR("Hide"));
	sep(hViewBg);
	add(hViewBg, BACKGROUND_INCREASE_ALPHA, _TR("More visible"));
	add(hViewBg, BACKGROUND_DECREASE_ALPHA, _TR("Less visible"));
	sep(hViewBg);
	add(hViewBg, BACKGROUND_SET_STRETCH, _TR("Stretch"));
	add(hViewBg, BACKGROUND_SET_LETTERBOX, _TR("Letterbox"));
	add(hViewBg, BACKGROUND_SET_CROP, _TR("Crop"));

	// View > Zoom menu.
	Item* hViewZoom = newMenu();
	add(hViewZoom, OPEN_DIALOG_ZOOM, _TR("Options"));
	sep(hViewZoom);
	add(hViewZoom, ZOOM_RESET, _TR("Reset"));
	sep(hViewZoom);
	add(hViewZoom, ZOOM_IN, _TR("Zoom in"));
	add(hViewZoom, ZOOM_OUT, _TR("Zoom out"));
	add(hViewZoom, SCALE_INCREASE, _TR("Scale increase"));
	add(hViewZoom, SCALE_DECREASE, _TR("Scale decrease"));

	// View > Snap menu.
	Item* hViewSnap = newMenu();
	add(hViewSnap, SNAP_RESET, _TR("Reset"));
	sep(hViewSnap);
	add(hViewSnap, OPEN_DIALOG_CUSTOM_SNAP, _TR("Set Snap"));
	add(hViewSnap, SNAP_PREVIOUS, _TR("Previous"));
	add(hViewSnap, SNAP_NEXT, _TR("Next"));

	// View > Cursor menu.
	Item* hViewCursor = newMenu();
	add(hViewCursor, CURSOR_UP, _TR("Up"));
	add(hViewCursor, CURSOR_DOWN, _TR("Down"));
	sep(hViewCursor);
	add(hViewCursor, CURSOR_PREVIOUS_BEAT, _TR("Previous beat"));
	add(hViewCursor, CURSOR_NEXT_BEAT, _TR("Next beat"));
	add(hViewCursor, CURSOR_PREVIOUS_MEASURE, _TR("Previous measure"));
	add(hViewCursor, CURSOR_NEXT_MEASURE, _TR("Next measure"));
	sep(hViewCursor);
	add(hViewCursor, CURSOR_STREAM_START, _TR("Stream start"));
	add(hViewCursor, CURSOR_STREAM_END, _TR("Stream end"));
	sep(hViewCursor);
	add(hViewCursor, CURSOR_SELECTION_START, _TR("Selection start"));
	add(hViewCursor, CURSOR_SELECTION_END, _TR("Selection end"));
	sep(hViewCursor);
	add(hViewCursor, CURSOR_CHART_START, _TR("First beat"));
	add(hViewCursor, CURSOR_CHART_END, _TR("Last beat"));

	// View > Statusbar menu.
	myStatusMenu = newMenu();
	add(myStatusMenu, TOGGLE_STATUS_CHART, _TR("Show chart"));
	add(myStatusMenu, TOGGLE_STATUS_SNAP, _TR("Show snap"));
	add(myStatusMenu, TOGGLE_STATUS_BPM, _TR("Show BPM"));
	add(myStatusMenu, TOGGLE_STATUS_ROW, _TR("Show row"));
	add(myStatusMenu, TOGGLE_STATUS_BEAT, _TR("Show beat"));
	add(myStatusMenu, TOGGLE_STATUS_MEASURE, _TR("Show measure"));
	add(myStatusMenu, TOGGLE_STATUS_TIME, _TR("Show time"));
	add(myStatusMenu, TOGGLE_STATUS_TIMING_MODE, _TR("Show timing mode"));

	// View menu.
	myViewMenu = newMenu();
	add(myViewMenu, TOGGLE_SHOW_WAVEFORM, _TR("Show waveform"));
	add(myViewMenu, TOGGLE_SHOW_BEAT_LINES, _TR("Show beat lines"));
	add(myViewMenu, TOGGLE_SHOW_TEMPO_BOXES, _TR("Show tempo boxes"));
	add(myViewMenu, TOGGLE_SHOW_TEMPO_HELP, _TR("Show tempo help"));
	add(myViewMenu, TOGGLE_SHOW_NOTES, _TR("Show notes"));
	add(myViewMenu, TOGGLE_CHART_PREVIEW, _TR("Use SM-style preview"));
	sep(myViewMenu);
	add(myViewMenu, TOGGLE_REVERSE_SCROLL, _TR("Reverse scroll"));
	sep(myViewMenu);
	add(myViewMenu, USE_TIME_BASED_VIEW, _TR("Time based (C-mod)"));
	add(myViewMenu, USE_ROW_BASED_VIEW, _TR("Row based (X-mod)"));
	sep(myViewMenu);
	add(myViewMenu, OPEN_DIALOG_WAVEFORM_SETTINGS, _TR("Waveform..."));
	add(myViewMenu, 0 /*dummy*/, _TR("Noteskins"));
	sub(myViewMenu, myMinimapMenu, _TR("Minimap"));
	sub(myViewMenu, myBgStyleMenu, _TR("Background"));
	sub(myViewMenu, hViewZoom, _TR("Zoom"));
	sub(myViewMenu, hViewSnap, _TR("Snap"));
	sub(myViewMenu, hViewCursor, _TR("Cursor"));
	sub(myViewMenu, myStatusMenu, _TR("Status"));

	// Help menu.
	Item* hHelp = newMenu();
	add(hHelp, SHOW_SHORTCUTS, _TR("Shortcuts..."));
	add(hHelp, SHOW_MESSAGE_LOG, _TR("Message Log..."));
	add(hHelp, SHOW_DEBUG_LOG, _TR("Debug Log..."));
	sep(hHelp);
	add(hHelp, SHOW_ABOUT, _TR("About..."));

	// Top level menu.
	sub(menu, hFile, _TR("File"));
	sub(menu, hEdit, _TR("Edit"));
	sub(menu, hChart, _TR("Chart"));
	sub(menu, hNotes, _TR("Notes"));
	sub(menu, hTempo, _TR("Tempo"));
	sub(menu, hAudio, _TR("Audio"));
	sub(menu, myViewMenu, _TR("View"));
	sub(menu, hHelp, _TR("Help"));

	update(ALL_PROPERTIES);
}

// ================================================================================================
// Menubar :: menu update functions.

void registerUpdateFunctions()
{
	using namespace Action;

	myUpdateFunctions[OPEN_FILE] = []
	{
		MENU->myFileMenu->setEnabled(FILE_CLOSE, gSimfile->isOpen());
		MENU->myFileMenu->setEnabled(FILE_SAVE, gSimfile->isOpen());
		MENU->myFileMenu->setEnabled(FILE_SAVE_AS, gSimfile->isOpen());
	};
	myUpdateFunctions[RECENT_FILES] = []
	{
		Item* recent = newMenu();
		int numFiles = min(gEditor->getNumRecentFiles(), (int)Action::MAX_RECENT_FILES);
		if(numFiles > 0)
		{
			recent->addItem(FILE_CLEAR_RECENT_FILES, _TR("Clear list"));
			recent->addSeperator();
			for(int i = 0; i < numFiles; ++i)
			{
				recent->addItem(FILE_OPEN_RECENT_BEGIN + i, gEditor->getRecentFile(i));
			}
		}
		MENU->myFileMenu->replaceSubmenu(1, recent, _TR("Recent files"), (numFiles == 0));
	};
	myUpdateFunctions[SHOW_WAVEFORM] = []
	{
		MENU->myViewMenu->setChecked(TOGGLE_SHOW_WAVEFORM, gNotefield->hasShowWaveform());
	};
	myUpdateFunctions[SHOW_BEATLINES] = []
	{
		MENU->myViewMenu->setChecked(TOGGLE_SHOW_BEAT_LINES, gNotefield->hasShowBeatLines());
	};
	myUpdateFunctions[SHOW_NOTES] = []
	{
		MENU->myViewMenu->setChecked(TOGGLE_SHOW_NOTES, gNotefield->hasShowNotes());
	};
	myUpdateFunctions[SHOW_TEMPO_BOXES] = []
	{
		MENU->myViewMenu->setChecked(TOGGLE_SHOW_TEMPO_BOXES, gTempoBoxes->hasShowBoxes());
	};
	myUpdateFunctions[SHOW_TEMPO_HELP] = []
	{
		MENU->myViewMenu->setChecked(TOGGLE_SHOW_TEMPO_HELP, gTempoBoxes->hasShowHelp());
	};
	myUpdateFunctions[USE_JUMP_TO_NEXT_NOTE] = []
	{
		MENU->myEditMenu->setChecked(TOGGLE_JUMP_TO_NEXT_NOTE, gEditing->hasJumpToNextNote());
	};
	myUpdateFunctions[USE_UNDO_REDO_JUMP] = []
	{
		MENU->myEditMenu->setChecked(TOGGLE_UNDO_REDO_JUMP, gEditing->hasUndoRedoJump());
	};
	myUpdateFunctions[USE_TIME_BASED_COPY] = []
	{
		MENU->myEditMenu->setChecked(TOGGLE_TIME_BASED_COPY, gEditing->hasTimeBasedCopy());
	};
	myUpdateFunctions[VISUAL_SYNC_ANCHOR] = []
	{
		MENU->myVisualSyncMenu->setChecked(SET_VISUAL_SYNC_CURSOR_ANCHOR, gEditing->getVisualSyncMode() == Editing::VisualSyncAnchor::CURSOR);
		MENU->myVisualSyncMenu->setChecked(SET_VISUAL_SYNC_RECEPTOR_ANCHOR, gEditing->getVisualSyncMode() == Editing::VisualSyncAnchor::RECEPTORS);
	};
	myUpdateFunctions[USE_REVERSE_SCROLL] = []
	{
		MENU->myViewMenu->setChecked(TOGGLE_REVERSE_SCROLL, gView->hasReverseScroll());
	};
	myUpdateFunctions[USE_CHART_PREVIEW] = []
	{
		MENU->myViewMenu->setChecked(TOGGLE_CHART_PREVIEW, gView->hasChartPreview());
	};
	myUpdateFunctions[VIEW_MODE] = []
	{
		MENU->myViewMenu->setChecked(USE_ROW_BASED_VIEW, !gView->isTimeBased());
		MENU->myViewMenu->setChecked(USE_TIME_BASED_VIEW, gView->isTimeBased());
	};
	myUpdateFunctions[VIEW_MINIMAP] = []
	{
		auto mode = gMinimap->getMode();
		MENU->myMinimapMenu->setChecked(MINIMAP_SET_NOTES, mode == Minimap::NOTES);
		MENU->myMinimapMenu->setChecked(MINIMAP_SET_DENSITY, mode == Minimap::DENSITY);
	};
	myUpdateFunctions[VIEW_BACKGROUND] = []
	{
		auto bg = gEditor->getBackgroundStyle();
		MENU->myBgStyleMenu->setChecked(BACKGROUND_SET_STRETCH, bg == BG_STYLE_STRETCH);
		MENU->myBgStyleMenu->setChecked(BACKGROUND_SET_LETTERBOX, bg == BG_STYLE_LETTERBOX);
		MENU->myBgStyleMenu->setChecked(BACKGROUND_SET_CROP, bg == BG_STYLE_CROP);
	};
	myUpdateFunctions[VIEW_NOTESKIN] = []
	{
		Item* hSkins = System::MenuItem::create();
		int numValid = 0;
		int numTypes = min(gNoteskin->getNumTypes(), (int)Action::MAX_NOTESKINS);
		int activeType = gNoteskin->getType();
		for(int type = 0; type < numTypes; ++type)
		{
			if(gNoteskin->isSupported(type))
			{
				hSkins->addItem(SET_NOTESKIN_BEGIN + type, gNoteskin->getName(type));
				if(type == activeType)
				{
					hSkins->setChecked((Action::Type)(SET_NOTESKIN_BEGIN + type), true);
				}
				++numValid;
			}
		}
		// If the active type was zero, set it to the first skin in the list
		if (!activeType)
		{
			hSkins->setChecked((Action::Type)(SET_NOTESKIN_BEGIN), true);
		}
		MENU->myViewMenu->replaceSubmenu(13, hSkins, "Noteskins", (numValid == 0));
	};
	myUpdateFunctions[STATUSBAR_CHART] = []
	{
		MENU->myStatusMenu->setChecked(TOGGLE_STATUS_CHART, gStatusbar->hasChart());
	};
	myUpdateFunctions[STATUSBAR_SNAP] = []
	{
		MENU->myStatusMenu->setChecked(TOGGLE_STATUS_SNAP, gStatusbar->hasSnap());
	};
	myUpdateFunctions[STATUSBAR_BPM] = []
	{
		MENU->myStatusMenu->setChecked(TOGGLE_STATUS_BPM, gStatusbar->hasBpm());
	};
	myUpdateFunctions[STATUSBAR_ROW] = []
	{
		MENU->myStatusMenu->setChecked(TOGGLE_STATUS_ROW, gStatusbar->hasRow());
	};
	myUpdateFunctions[STATUSBAR_BEAT] = []
	{
		MENU->myStatusMenu->setChecked(TOGGLE_STATUS_BEAT, gStatusbar->hasBeat());
	};
	myUpdateFunctions[STATUSBAR_MEASURE] = []
	{
		MENU->myStatusMenu->setChecked(TOGGLE_STATUS_MEASURE, gStatusbar->hasMeasure());
	};
	myUpdateFunctions[STATUSBAR_TIME] = []
	{
		MENU->myStatusMenu->setChecked(TOGGLE_STATUS_TIME, gStatusbar->hasTime());
	};
	myUpdateFunctions[STATUSBAR_TIMING_MODE] = []
	{
		MENU->myStatusMenu->setChecked(TOGGLE_STATUS_TIMING_MODE, gStatusbar->hasTimingMode());
	};
}

void update(Property prop)
{
	if(prop == ALL_PROPERTIES)
	{
		for(int i = 1; i < NUM_PROPERTIES; ++i)
		{
			myUpdateFunctions[i]();
		}
	}
	else
	{
		myUpdateFunctions[prop]();
	}
}

}; // MenuBarImpl
}; // anonymous namespace.

// ================================================================================================
// Menubar API.

Menubar* gMenubar = nullptr;

void Menubar::create()
{
	gMenubar = new MenuBarImpl;
}

void Menubar::destroy()
{
	delete (MenuBarImpl*)gMenubar;
	gMenubar = nullptr;
}

}; // namespace Vortex
