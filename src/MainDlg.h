/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#ifndef		MAINDIALOG_H
#define		MAINDIALOG_H

#include <MASkinG.h>
#include "FontPanel.h"
#include "MyFont.h"
#include "PreviewPanel.h"
#include "PalettePanel.h"
#include "Range.h"
#include <vector>
using namespace std;
using namespace MAS;


class MainDlg : public Dialog {
	private:
		// The GUI controls
		Wallpaper desktop;
		Menu fileMenu, glyphMenu, glyphPushMenu, effectsMenu, paletteMenu, editMenu, helpMenu, mainMenu;
		PanelRaised panel;
		Label rangeText;
		ListBox characterList, rangeList;
		FontPanel glyphEd;
		Button nextButton, prevButton;
		Label previewText;
		EditBox previewEdit;
		Button previewButton;
		PreviewPanel previewPanel;
		Shortcut shortcut[15];
		PanelRaised helpPanel;
		Label helpText;
		PalettePanel palettePanel;
		CheckBox showGridCheck;
		CheckBox updateCheck;

		void MakeMenu();

		// The callback functions
		void OnNew();
		void OnOpen();
		void OnSave();
		void OnSaveAs();
		void OnClear();
		void OnNext();
		void OnPrevious();
		void OnHelp();
		void OnAbout();
		void OnListSelChange(int index);
		void OnSetPreview();
		void OnGlyphResize();
		void OnEdit(int params);
		void OnToggleGrid();
		void OnGradient();
		void OnOutline();
		void OnSmooth();
		void OnBlur();
		void OnNoise();
		void OnCutout();
		void OnMakeWave();
		void OnLight();
		void OnPaletteGradient();
		void OnDefaultPalette();
		void OnLoadPalette();
		void OnPushUp();
		void OnPushDown();
		void OnPushLeft();
		void OnPushRight();
		void OnCut();
		void OnCopy();
		void OnPaste();
		void OnAddRange();
		void OnRemoveRange();
		void OnRangeSelChange();
		void OnResizeCanvas();
		void OnGlyphResizeCanvas();
		void OnGlyphExport();
		void OnGlyphImport();

		enum {
			FILE_NEW = 1000,
			FILE_OPEN2,
			FILE_SAVE,
			FILE_SAVEAS,
			FILE_QUIT,
			EDIT_CUT,
			EDIT_COPY,
			EDIT_PASTE,
			EDIT_ADDRANGE,
			EDIT_REMOVERANGE,
			EDIT_RESIZE_CANVAS,
			GLYPH_CLEAR,
			GLYPH_PREVIOUS,
			GLYPH_NEXT,
			GLYPH_RESIZE,
			GLYPH_RESIZE_CANVAS,
			GLYPH_PUSHUP,
			GLYPH_PUSHDOWN,
			GLYPH_PUSHLEFT,
			GLYPH_PUSHRIGHT,
			GLYPH_EXPORT,
			GLYPH_IMPORT,
			PALETTE_GRADIENT,
			PALETTE_DEFAULT,
			PALETTE_LOAD,
			EFFECTS_GRADIENT,
			EFFECTS_OUTLINE,
			EFFECTS_SMOOTH,
			EFFECTS_BLUR,
			EFFECTS_NOISE,
			EFFECTS_CUTOUT,
			EFFECTS_WAVE,
			EFFECTS_LIGHT,
			HELP_INDEX2,
			HELP_ABOUT2
		};

		MyFont fnt;
		char path[2048];
		BITMAP *copy;
		vector<Range> ranges;
		int currentIndex;

		int GetCurrentIndex();
		void SaveScript();
		BITMAP *GetBitmapFromClipboard();
		void PutBitmapOnClipboard(BITMAP *bmp);

	protected:
		void HandleEvent(Widget &obj, int msg, int arg1=0, int arg2=0);
		void MsgInitSkin();
		bool MsgClose();


	public:
		MainDlg();
		~MainDlg();
};

#endif
