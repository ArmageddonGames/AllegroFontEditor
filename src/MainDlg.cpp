/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#include "MainDlg.h"
#include "MyFont.h"
#include "ListItemFont.h"
#include <allegro/internal/aintern.h>


////////////////////////////////////////////////////////////////////////////////
// Constructor
MainDlg::MainDlg() : Dialog(), copy(NULL), currentIndex(0) {
	// Initialize the objects
	MakeMenu();
	panel.Setup(		  5, 30,630,420,	0,	0);
	characterList.Setup( 10, 35,132,205,	0,	0);
	rangeText.Setup(	 10,244,132, 16,	0,	0, "Range", 0);
	rangeList.Setup(	 10,262,132, 80,	0,	0);
	glyphEd.Setup(		152, 40,256,256,	0,	0);
	nextButton.Setup(	422, 40,120, 22,	0,	0, "Next");
	prevButton.Setup(	422, 66,120, 22,	0,	0, "Previous");
	showGridCheck.Setup(422, 92,120, 22,	0,	D_SELECTED, "Show grid");
	updateCheck.Setup(	422,118,200, 22,	0,	D_SELECTED, "Update preview while drawing");
	previewText.Setup(	152,304,120, 16,	0,	0, "Preview text", 0);
	previewEdit.Setup(	152,320,418, 22,	0,	0, "The quick brown fox jumps over the lazy dog", 100);
	previewButton.Setup(	572,320, 58,	22,	0,	0, "Set");
	previewPanel.Setup(	 10,345,620,100,	0,	0);
	previewPanel.SetText(previewEdit.GetText());
	helpPanel.Setup(	  0,460,640, 20,	0,	0);
	helpText.ClearFlag(D_AUTOSIZE);
	helpText.Setup(		 10,463,620, 14,	0,	0, "Ready", 0);
	palettePanel.Setup(	422,160,128,128,	0,	0);

	shortcut[0].Setup(KEY_N, KB_CTRL_FLAG);
	shortcut[1].Setup(KEY_O, KB_CTRL_FLAG);
	shortcut[2].Setup(KEY_S, KB_CTRL_FLAG);
	shortcut[3].Setup(KEY_Q, KB_CTRL_FLAG);
	shortcut[4].Setup(KEY_DEL);
	shortcut[5].Setup(KEY_PGUP);
	shortcut[6].Setup(KEY_PGDN);
	shortcut[7].Setup(KEY_F1);
	shortcut[8].Setup(KEY_A, KB_CTRL_FLAG);
	shortcut[9].Setup(KEY_R, KB_CTRL_FLAG);
	shortcut[10].Setup(KEY_ENTER);
	shortcut[11].Setup(KEY_F12);
	shortcut[12].Setup(KEY_X, KB_CTRL_FLAG);
	shortcut[13].Setup(KEY_C, KB_CTRL_FLAG);
	shortcut[14].Setup(KEY_V, KB_CTRL_FLAG);

	ranges.reserve(16);
	Range r(32,127);
	ranges.push_back(r);
	char buf[32];
	r.Print(buf);
	rangeList.InsertItem(new ListItemString(buf), 0);
	OnRangeSelChange();

	fnt.Create(8,8,96);
	glyphEd.SetFont(&fnt);
	glyphEd.SetPalette(&palettePanel);
	glyphEd.EnableGrid();
	fnt.SetPalette(&palettePanel);
	previewPanel.SetFont(&fnt);

	// Add the objects to the dialog
	Add(desktop);
	Add(mainMenu);
	Add(panel);
	Add(characterList);
	Add(rangeText);
	Add(rangeList);
	Add(glyphEd);
	Add(nextButton);
	Add(prevButton);
	Add(showGridCheck);
	Add(updateCheck);
	Add(previewText);
	Add(previewEdit);
	Add(previewButton);
	Add(previewPanel);
	for (int i=0; i<15; ++i) Add(shortcut[i]);
	Add(helpPanel);
	Add(helpText);
	Add(palettePanel);

	int drive=0;
#if (defined ALLEGRO_DOS) || (defined ALLEGRO_WINDOWS)
	drive = _al_getdrive();
#endif
	_al_getdcwd(drive, path, 2048);
	put_backslash(path);

#ifndef ALLEGRO_DOS
	set_window_title("Allegro Font Editor");
#endif
}


MainDlg::~MainDlg() {
	if (copy) {
		destroy_bitmap(copy);
		copy = NULL;
	}

	ranges.clear();
}


void MainDlg::MakeMenu() {
	fileMenu.Add("&New\tCtrl-N", FILE_NEW);
	fileMenu.Add("&Open\tCtrl-O", FILE_OPEN2);
	fileMenu.Add("&Save\tCtrl-S", FILE_SAVE);
	fileMenu.Add("Save &As\tF12", FILE_SAVEAS);
	fileMenu.Add();
	fileMenu.Add("&Quit\tCtrl-Q", FILE_QUIT);

	editMenu.Add("Cu&t\tCtrl-X", EDIT_CUT);
	editMenu.Add("&Copy\tCtrl-C", EDIT_COPY);
	editMenu.Add("&Paste\tCtrl-V", EDIT_PASTE);
	editMenu.Add();
	editMenu.Add("&Add range", EDIT_ADDRANGE);
	//editMenu.Add("&Remove range", EDIT_REMOVERANGE);
	editMenu.Add("&Resize canvas", EDIT_RESIZE_CANVAS);

	glyphMenu.Add("&Clear\tDel", GLYPH_CLEAR);
	glyphMenu.Add("&Previous\tPgUp", GLYPH_PREVIOUS);
	glyphMenu.Add("&Next\tPgDn", GLYPH_NEXT);
	glyphMenu.Add("&Resize\tCtrl-R", GLYPH_RESIZE);
	glyphMenu.Add("Resize c&anvas", GLYPH_RESIZE_CANVAS);
	glyphPushMenu.Add("&Up\tShift-Up", GLYPH_PUSHUP);
	glyphPushMenu.Add("&Down\tShift-Down", GLYPH_PUSHDOWN);
	glyphPushMenu.Add("&Left\tShift-Left", GLYPH_PUSHLEFT);
	glyphPushMenu.Add("&Right\tShift-Right", GLYPH_PUSHRIGHT);
	glyphMenu.Add("Push", glyphPushMenu);
	glyphMenu.Add();
	glyphMenu.Add("&Import", GLYPH_IMPORT);
	glyphMenu.Add("E&xport", GLYPH_EXPORT);

	paletteMenu.Add("&Default Palette", PALETTE_DEFAULT);
	paletteMenu.Add("&Load", PALETTE_LOAD);
	paletteMenu.Add("Palette &Gradient", PALETTE_GRADIENT);

	effectsMenu.Add("&Make gradient", EFFECTS_GRADIENT);
	effectsMenu.Add("&Outline", EFFECTS_OUTLINE);
	effectsMenu.Add("&Smooth", EFFECTS_SMOOTH);
	effectsMenu.Add("&Blur", EFFECTS_BLUR);
	effectsMenu.Add("&Add noise", EFFECTS_NOISE);
	effectsMenu.Add("&Cutout", EFFECTS_CUTOUT);
	effectsMenu.Add("Make &Wave", EFFECTS_WAVE);
	//effectsMenu.Add("&Light/Shadow", EFFECTS_LIGHT);

	helpMenu.Add("&Using\tF1", HELP_INDEX2);
	helpMenu.Add("&About\tCtrl-A", HELP_ABOUT2);

	mainMenu.Add("&File", fileMenu);
	mainMenu.Add("&Edit", editMenu);
	mainMenu.Add("&Glyph", glyphMenu);
	mainMenu.Add("&Palette", paletteMenu);
	mainMenu.Add("E&ffects", effectsMenu);
	mainMenu.Add("&Help", helpMenu);
}


void MainDlg::HandleEvent(Widget &obj, int msg, int arg1, int arg2) {
	Dialog::HandleEvent(obj, msg, arg1, arg2);

	switch (msg) {
	case MSG_ACTIVATE:
		if (obj == glyphEd)			OnEdit(arg1);		else
		if (obj == nextButton)		OnNext();			else
		if (obj == prevButton)		OnPrevious();		else
		if (obj == showGridCheck)	OnToggleGrid();		else
		if (obj == previewButton)	OnSetPreview();		else
		if (obj == previewEdit)		OnSetPreview();		else
		if (obj == shortcut[0])		OnNew();			else
		if (obj == shortcut[1])		OnOpen();			else
		if (obj == shortcut[2])		OnSave();			else
		if (obj == shortcut[11])	OnSaveAs();			else
		if (obj == shortcut[3])		Close();			else
		if (obj == shortcut[4])		OnClear();			else
		if (obj == shortcut[5])		OnPrevious();		else
		if (obj == shortcut[6])		OnNext();			else
		if (obj == shortcut[7])		OnHelp();			else
		if (obj == shortcut[8])		OnAbout();			else
		if (obj == shortcut[9])		OnGlyphResize();	else
		if (obj == shortcut[10])	previewPanel.SetFont(&fnt);	else
		if (obj == shortcut[12])	OnCut();			else
		if (obj == shortcut[13])	OnCopy();			else
		if (obj == shortcut[14])	OnPaste();
		break;
	case MSG_SCROLL:
		if (obj == characterList)	OnListSelChange(arg1);	else
		if (obj == rangeList)		OnRangeSelChange();
		break;
	case MSG_DCLICK:
		if (obj == palettePanel) {
			glyphEd.Redraw();
			previewPanel.Redraw();
		}
		break;
	case MSG_GOTMOUSE:
		if (obj == desktop)	helpText.SetText("Ready");	else
		if (obj == panel)	helpText.SetText("Ready");	else
		if (obj == glyphEd)	helpText.SetText("Drawing: LMB - fg, RMB - bg, 3rd button - clear, SHIFT - line, CTRL - floodfill, ENTER - update preview");	else
		if (obj == characterList)	helpText.SetText("Select a character you want to edit.");	else
		if (obj == previewPanel)	helpText.SetText("This is how the font looks like.");
		if (obj == palettePanel)	helpText.SetText("Palette: LMB - foreground, RMB - background, double click - select color");
		break;
	case MSG_GOTFOCUS:
		if (obj == previewEdit)	helpText.SetText("Type the font preview text here.");
		break;
	case MSG_CHAR:
		if (obj == glyphEd) {
			switch (arg1) {
				case KEY_UP:	OnPushUp();	break;
				case KEY_DOWN:	OnPushDown();	break;
				case KEY_LEFT:	OnPushLeft();	break;
				case KEY_RIGHT:	OnPushRight();	break;
			};
		}
		else if (obj == rangeList) {
			switch (arg1) {
				case KEY_DEL:		OnRemoveRange();	break;
				case KEY_INSERT:	OnAddRange();		break;
			};
		}
		break;

	case FILE_NEW:			OnNew();	break;
	case FILE_OPEN2:		OnOpen();	break;
	case FILE_SAVE:			OnSave();	break;
	case FILE_SAVEAS:		OnSaveAs();	break;
	case FILE_QUIT:			Close();	break;
	case EDIT_CUT:			OnCut();	break;
	case EDIT_COPY:			OnCopy();	break;
	case EDIT_PASTE:		OnPaste();	break;
	case EDIT_ADDRANGE:		OnAddRange();	break;
	case EDIT_REMOVERANGE:	OnRemoveRange();	break;
	case EDIT_RESIZE_CANVAS:OnResizeCanvas();	break;
	case GLYPH_CLEAR:		OnClear();	break;
	case GLYPH_PREVIOUS:	OnPrevious();	break;
	case GLYPH_NEXT:		OnNext();	break;
	case GLYPH_RESIZE:		OnGlyphResize();	break;
	case GLYPH_RESIZE_CANVAS:OnGlyphResizeCanvas();	break;
	case GLYPH_PUSHUP:		OnPushUp();	break;
	case GLYPH_PUSHDOWN:	OnPushDown();	break;
	case GLYPH_PUSHLEFT:	OnPushLeft();	break;
	case GLYPH_PUSHRIGHT:	OnPushRight();	break;
	case GLYPH_IMPORT:		OnGlyphImport();	break;
	case GLYPH_EXPORT:		OnGlyphExport();	break;
	case PALETTE_DEFAULT:	OnDefaultPalette();	break;
	case PALETTE_LOAD:		OnLoadPalette();	break;
	case PALETTE_GRADIENT:	OnPaletteGradient();	break;
	case EFFECTS_GRADIENT:	OnGradient();	break;
	case EFFECTS_OUTLINE:	OnOutline();	break;
	case EFFECTS_SMOOTH:	OnSmooth();	break;
	case EFFECTS_BLUR:		OnBlur();	break;
	case EFFECTS_NOISE:		OnNoise();	break;
	case EFFECTS_CUTOUT:	OnCutout();	break;
	case EFFECTS_WAVE:		OnMakeWave();	break;
	case EFFECTS_LIGHT:		OnLight();	break;
	case HELP_INDEX2:		OnHelp();	break;
	case HELP_ABOUT2:		OnAbout();	break;
	};
}


void MainDlg::MsgInitSkin() {
	Dialog::MsgInitSkin();

	//theSkin->LoadFont("./8x12.pcx", MASkin::N_OF_FONTS);
	characterList.SetFont(Skin::nFonts, Skin::NORMAL);
	characterList.SetFont(Skin::nFonts, Skin::SELECT);
	characterList.SetFont(Skin::nFonts, Skin::DISABLE);
	characterList.SetFont(Skin::nFonts, Skin::FOCUS);

	helpText.SetTextMode(skin->c_face);
}


////////////////////////////////////////////////////////////////////////////////
// Callback functions
////////////////////////////////////////////////////////////////////////////////

class SizeWin : public Window {
	protected:
		Label sizeText;
		EditBox sizeEdit;
		Button OKButton;
		Button CancelButton;

		void UpdateSize() {
			Window::UpdateSize();

			int cw = clientArea->w();
			if (!(Flags() & W_MINIMIZED)) {
				int l = skin->GetFont(0).TextLength(sizeText.GetText());
				sizeText.Shape(4,	16, 	l,	16);
				sizeEdit.Shape(cw-60,	12,	56,	22);
				OKButton.Shape(cw/12, 44, 9*cw/24,	22);
				CancelButton.Shape(13*cw/24, 44, 9*cw/24,	22);
			}
		}

	public:
		SizeWin(const char *t, const char *prompt, int def) : Window() {
			title.SetText(t);
			sizeText.SetText(prompt);
			char buf[24];
			usprintf(buf, "%d", def);
			sizeEdit.SetText(buf, 6);
			sizeEdit.MakeExit();
			OKButton.SetText("OK");
			OKButton.MakeExit();
			CancelButton.SetText("Cancel");
			CancelButton.MakeExit();

			// Add the objects to the dialog
			Add(sizeText);
			Add(sizeEdit);
			Add(OKButton);
			Add(CancelButton);

			int w = skin->GetFont(0).TextLength(prompt) + 70;
			clientArea->Resize(w, 72);
			Resize(w, 72);
		}

		// this is here because of a bug in MASkinG
		int Popup(Dialog *p) {
			Centre();
			Widget *fw = Window::Popup(p, x(), y(), &sizeEdit);
			if (!fw || fw == &CancelButton || fw == &iconExit) {
				return 0;
			}
			else {
				return 1;
			}
		}

		int GetSize() {
			return sizeEdit.GetInt();
		}
};

class SizeWin2 : public SizeWin {
	protected:
		Label size2Text;
		EditBox size2Edit;

		void UpdateSize() {
			SizeWin::UpdateSize();

			int cw = clientArea->w();
			if (!(Flags() & W_MINIMIZED)) {
				int l1 = skin->GetFont(0).TextLength(sizeText.GetText());
				int l2 = skin->GetFont(0).TextLength(size2Text.GetText());
				int l = l1>l2 ? l1 : l2;
				size2Text.Shape(4,	42, 	l,	16);
				size2Edit.Shape(cw-60,	38,	56,	22);
				OKButton.Shape(cw/12, 70, 9*cw/24,	22);
				CancelButton.Shape(13*cw/24, 70, 9*cw/24,	22);
			}
		}

	public:
		SizeWin2(const char *title, const char *prompt1, const char *prompt2, int def1, int def2) : SizeWin(title, prompt1, def1) {
			size2Text.SetText(prompt2);
			char buf[24];
			usprintf(buf, "%d", def2);
			size2Edit.SetText(buf, 6);
			size2Edit.MakeExit();

			// Add the objects to the dialog
			Add(size2Text);
			Add(size2Edit);

			int w1 = skin->GetFont(0).TextLength(prompt1) + 70;
			int w2 = skin->GetFont(0).TextLength(prompt2) + 70;
			clientArea->Resize(w1>w2 ? w1 : w2, 96);
			Resize(w1>w2 ? w1 : w2, 96);
		}

		int GetSize(int i) {
			switch (i) {
				case 2:		return size2Edit.GetInt();	break;
				default:	return SizeWin::GetSize();	break;
			};
		}
};


class SizeWin3 : public SizeWin2 {
	protected:
		Label size3Text;
		EditBox size3Edit;

		void UpdateSize() {
			SizeWin2::UpdateSize();

			int cw = clientArea->w();
			if (!(Flags() & W_MINIMIZED)) {
				int l1 = skin->GetFont(0).TextLength(sizeText.GetText());
				int l2 = skin->GetFont(0).TextLength(size2Text.GetText());
				int l3 = skin->GetFont(0).TextLength(size3Text.GetText());
				int l = l1>l2 ? l1 : l2;
				l = l>l3 ? l : l3;
				size3Text.Shape(4,	68, 	l,	16);
				size3Edit.Shape(cw-60,	64,	56,	22);
				OKButton.Shape(cw/12, 96, 9*cw/24,	22);
				CancelButton.Shape(13*cw/24, 96, 9*cw/24,	22);
			}
		}

	public:
		SizeWin3(const char *title, const char *prompt1, const char *prompt2, const char *prompt3, int def1, int def2, int def3) : SizeWin2(title, prompt1, prompt2, def1, def2) {
			size3Text.SetText(prompt3);
			char buf[24];
			usprintf(buf, "%d", def3);
			size3Edit.SetText(buf, 6);
			size3Edit.MakeExit();

			// Add the objects to the dialog
			Add(size3Text);
			Add(size3Edit);

			int w1 = skin->GetFont(0).TextLength(prompt1) + 70;
			int w2 = skin->GetFont(0).TextLength(prompt2) + 70;
			int w3 = skin->GetFont(0).TextLength(prompt3) + 70;
			int w = w1>w2 ? w1 : w2;
			w = w>w3 ? w : w3;
			clientArea->Resize(w, 122);
			Resize(w, 122);
		}

		int GetSize(int i) {
			switch (i) {
				case 3:		return size3Edit.GetInt();	break;
				default:	return SizeWin2::GetSize(i);	break;
			};
		}
};


class SizeWin4 : public SizeWin3 {
	protected:
		Label size4Text;
		EditBox size4Edit;

		void UpdateSize() {
			SizeWin3::UpdateSize();

			int cw = clientArea->w();
			if (!(Flags() & W_MINIMIZED)) {
				int l1 = skin->GetFont(0).TextLength(sizeText.GetText());
				int l2 = skin->GetFont(0).TextLength(size2Text.GetText());
				int l3 = skin->GetFont(0).TextLength(size3Text.GetText());
				int l4 = skin->GetFont(0).TextLength(size4Text.GetText());
				int l = l1>l2 ? l1 : l2;
				l = l>l3 ? l : l3;
				l = l>l4 ? l : l4;
				size4Text.Shape(4,	94, 	l,	16);
				size4Edit.Shape(cw-60,	90,	56,	22);
				OKButton.Shape(cw/12, 122, 9*cw/24,	22);
				CancelButton.Shape(13*cw/24, 122, 9*cw/24,	22);
			}
		}

	public:
		SizeWin4(const char *title, const char *prompt1, const char *prompt2, const char *prompt3, const char *prompt4, int def1, int def2, int def3, int def4) : SizeWin3(title, prompt1, prompt2, prompt3, def1, def2, def3) {
			size4Text.SetText(prompt4);
			char buf[24];
			usprintf(buf, "%d", def4);
			size4Edit.SetText(buf, 6);
			size4Edit.MakeExit();

			// Add the objects to the dialog
			Add(size4Text);
			Add(size4Edit);

			int w1 = skin->GetFont(0).TextLength(prompt1) + 70;
			int w2 = skin->GetFont(0).TextLength(prompt2) + 70;
			int w3 = skin->GetFont(0).TextLength(prompt3) + 70;
			int w4 = skin->GetFont(0).TextLength(prompt4) + 70;
			int w = w1>w2 ? w1 : w2;
			w = w>w3 ? w : w3;
			w = w>w4 ? w : w4;
			clientArea->Resize(w, 148);
			Resize(w, 148);
		}

		int GetSize(int i) {
			switch (i) {
				case 4:		return size4Edit.GetInt();	break;
				default:	return SizeWin3::GetSize(i);	break;
			};
		}
};


class FixedWin : public Window {
	protected:
		Label sizeText;
		ListBox sizeList;
		Button OKButton;

		void UpdateSize() {
			Window::UpdateSize();

			int cw = clientArea->w();
			if (!(Flags() & W_MINIMIZED)) {
				int l = skin->GetFont(0).TextLength(sizeText.GetText());
				sizeText.Shape(4,	4, 	l,	16);
				sizeList.Shape(4,      22,     cw-8,120);
				OKButton.Shape(cw/4,	146,	cw/2,	22);
			}
		}

	public:
		FixedWin(const char *t, const char *prompt, int nSizes, const int *sizeTable) : Window() {
			title.SetText(t);
			sizeText.SetText(prompt);
			sizeText.AlignRight();

			for (int i=0; i<nSizes; ++i) {
				char buf[8];
				usprintf(buf, "%d", sizeTable[i]);
				sizeList.InsertItem(new ListItemString(buf), i);
			}
			sizeList.MakeExit();
			OKButton.SetText("OK");
			OKButton.MakeExit();

			// Add the objects to the dialog
			Add(sizeText);
			Add(sizeList);
			Add(OKButton);

			int w = skin->GetFont(0).TextLength(prompt) + 8;
			clientArea->Resize(w, 172);
			Resize(w, 172);
		}

		int Popup(Dialog *p) {
			Centre();
			Window::Popup(p);
			//theSkin->ChangeMouseCursor(MASkin::MOUSE_NORMAL);
			return sizeList.Selection();
		}

		int GetSelection() {
			return sizeList.Selection();
		}
};


void MainDlg::OnNew() {
	SizeWin2 win("New font", "width (pixels)", "height (pixels)", 8, 8);
	if (win.Popup(this) == 0) return;
	int w = win.GetSize(1);
	int h = win.GetSize(2);

	ranges.clear();
	Range r(32,127);
	ranges.push_back(r);
	char buf[32];
	r.Print(buf);
	rangeList.DeleteAllItems();
	rangeList.InsertItem(new ListItemString(buf), 0);
	OnRangeSelChange();

	fnt.SetColor(palettePanel.LeftColor());
	fnt.SetBackground(palettePanel.RightColor());
	fnt.Font::Create(::font);
	fnt.Create(w, h, 96);
	glyphEd.SetFont(&fnt);
	previewPanel.SetFont(&fnt);
}


void MainDlg::OnOpen() {
#ifdef USE_PNGLOADER
	FileSelect dlg("Open file...", path, "All fonts (*.pcx;*.bmp;*.tga;*.png;*.lbm;*.fnt;*.txt;*.itf;*.fon;*.ttf;*.ttc;*.pfa;*.pfb;*.cff;*.pcf)|Bitmap fonts (*.pcx;*.bmp;*.tga;*.lbm)|BIOS and GRX (*.fnt)|Windows fonts (*.fnt;*.fon)|Scripted fonts (*.txt)|IT fonts (*.itf)|TrueType fonts (*.ttf;*.ttc)|Type1 fonts (*.pfa;*.pfb)|CFF fonts (*.cff)|X11 PCF fonts (*.pcf)|All files (*.*)", 0xFF);
#else
	FileSelect dlg("Open file...", path, "All fonts (*.pcx;*.bmp;*.tga;*.lbm;*.fnt;*.txt;*.itf;*.fon;*.ttf;*.ttc;*.pfa;*.pfb;*.cff;*.pcf)|Bitmap fonts (*.pcx;*.bmp;*.tga;*.lbm)|BIOS and GRX (*.fnt)|Windows fonts (*.fnt;*.fon)|Scripted fonts (*.txt)|IT fonts (*.itf)|TrueType fonts (*.ttf;*.ttc)|Type1 fonts (*.pfa;*.pfb)|CFF fonts (*.cff)|X11 PCF fonts (*.pcf)|All files (*.*)", 0xFF);
#endif
	char *selPath = dlg.Popup(this);

	if (selPath) {
		ranges.clear();
		Range r(32,127);
		ranges.push_back(r);
		char buf[32];
		r.Print(buf);
		rangeList.DeleteAllItems();
		rangeList.InsertItem(new ListItemString(buf), 0);
		OnRangeSelChange();

		ustrcpy(path, selPath);

		// use the bitmap's palette if it's a bitmap font
		char *ext = get_extension(path);
		if (ustrcmp(ext, "pcx") == 0 || ustrcmp(ext, "bmp") == 0 || ustrcmp(ext, "tga") == 0 || ustrcmp(ext, "lbm") == 0) {
			palettePanel.UpdatePalette(path);
		}

		RGB col;
		col.r = 63;	col.g = 0;	col.b = 63;
		set_color(0, &col);
		col.g = 63;	col.b = 0;
		set_color(255, &col);

		int fg = palettePanel.LeftColor();
		int bg = palettePanel.RightColor();

		// load the font to see what type it is
		fnt.Load(path, 96, false, -1, bg);

		// if necessary load the font again with real properties
		if (fnt.IsFixed()) {
			const int *table = fnt.GetAvailableFixedSizes();
			int nSizes = fnt.GetNOfAvailableFixedSizes();

			if (nSizes > 1) {
				FixedWin win("Select size", "available font sizes", nSizes, table);
				int size = table[win.Popup(this)];

				if (!fnt.IsColourFont()) {
					fnt.Load(path, 96, false, fg, bg);
				}

				fnt.SetSize(size);
			}
			else {
				if (!fnt.IsColourFont()) {
					fnt.Load(path, 96, false, fg, bg);
				}
			}
		}
		else if (fnt.IsScalable()) {
			MessageBox aadlg("Message", "Do you want antialiasing?", NULL, NULL, "Yes", "No", NULL);
			bool aa = false;
			if (aadlg.Popup(this)==1) {
				aa = true;
			}

			SizeWin win("Select font size", "height (pixels)", 36);
			win.Popup(this);
			int size = win.GetSize();
			size = Clamp(size, 4, 256);

			fnt.Load(path, 96, aa, fg, bg);
			fnt.SetSize(size);
		}
		else {
			if (!fnt.IsColourFont()) {
				fnt.Load(path, 96, false, fg, bg);
			}
		}

		glyphEd.SetFont(&fnt);
		previewPanel.SetFont(&fnt);

		if (path[ustrlen(path)] != '\\' && path[ustrlen(path)] != '/') {
			selPath = get_filename(path);
			selPath[0] = 0;
		}
	}
}


void MainDlg::OnSave() {
	int l = ustrsize(path);
	if (ugetat(path,l-1) == '\\' || ugetat(path,l-1) == '/') {
		OnSaveAs();
	}
	else {
		fnt.Save(path);
	}
}


void MainDlg::OnSaveAs() {
#ifdef USE_PNGLOADER
	FileSelect dlg("Save file as...", path, "Bitmap fonts (*.pcx;*.bmp;*.tga;*.lbm;*.png)|All files (*.*)", FA_ARCH);
#else
	FileSelect dlg("Save file as...", path, "Bitmap fonts (*.pcx;*.bmp;*.tga;*.lbm)|All files (*.*)", FA_ARCH);
#endif
	char *selPath = dlg.Popup(this);

	if (selPath) {
		ustrcpy(path, selPath);
		fnt.Save(path);

		if (rangeList.Size() > 1) {
			SaveScript();
		}
	}
}


void MainDlg::SaveScript() {
	char *tmp = get_filename(path);
	char filename[256];
	ustrcpy(filename, tmp);
	char *tmp2 = get_extension(filename);
	*tmp2 = 0;
	usprintf(tmp2, "txt");

	PACKFILE *file = pack_fopen(filename, "w");
	char buf[256];
	usprintf(buf, "%s %#04X %#04X\n", tmp, ranges[0].min, ranges[0].max);
	pack_fputs(buf, file);

	for (unsigned int i=1; i<ranges.size(); ++i) {
		char buf[256];
		usprintf(buf, "- %#04X %#04X\n", ranges[i].min, ranges[i].max);
		pack_fputs(buf, file);
	}

	pack_fclose(file);
}


bool MainDlg::MsgClose() {
	MessageBox message("Message", "Do you really want to exit?", NULL, NULL, "Yes", "No", NULL);
	return (message.Popup(this) == 1) ? true : false;
}


void MainDlg::OnClear() {
	fnt.ClearGlyph(currentIndex);
	glyphEd.Redraw();
	previewPanel.SetFont(&fnt);
}


void MainDlg::OnNext() {
	if (characterList.Selection() < characterList.Size()-1) {
		++currentIndex;
		characterList.Select(characterList.Selection()+1);
		characterList.ScrollToSelection();
		glyphEd.SelectGlyph(currentIndex);
	}
}


void MainDlg::OnPrevious() {
	if (characterList.Selection() > 0) {
		--currentIndex;
		characterList.Select(characterList.Selection()-1);
		characterList.ScrollToSelection();
		glyphEd.SelectGlyph(currentIndex);
	}
}


void MainDlg::OnHelp() {
	MessageBox message("Help", "left mouse button - draw with foreground color", "right mouse button - draw with background color", "3rd mouse button - clear pixel", "OK", NULL, NULL);
	message.Popup(this);
}


void MainDlg::OnAbout() {
	MessageBox message("About", "Allegro Font Editor v1.19", "Copyright (c) 2003-2005 by Miran Amon", allegro_id, "OK", NULL, NULL);
	message.Popup(this);
}


void MainDlg::OnListSelChange(int index) {
	// index doesn't work when the list is clicked (bug in MASkinG!!!)
	//glyphEd.SelectGlyph(index);

	//glyphEd.SelectGlyph(characterList.Selection());
	currentIndex = GetCurrentIndex();
	glyphEd.SelectGlyph(currentIndex);
	index = 0;
}


void MainDlg::OnSetPreview() {
	previewPanel.SetText(previewEdit.GetText());
}


void MainDlg::OnGlyphResize() {
	SizeWin2 win("Character size", "new width", "new height", fnt.GlyphWidth(currentIndex), fnt.GlyphHeight(currentIndex));
	if (win.Popup(this) == 0) return;
	int w = win.GetSize(1); Clamp(w, 1, 256);
	int h = win.GetSize(2); Clamp(h, 1, 256);
	fnt.GlyphResize(currentIndex, w, h);
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnGlyphResizeCanvas() {
	SizeWin4 win("Canvas size", "left border", "right border", "top border", "bottom border", 0, 0, 0, 0);
	if (win.Popup(this) == 0) return;
	int l = win.GetSize(1); Clamp(l, -128, 128);
	int r = win.GetSize(2); Clamp(r, -128, 128);
	int t = win.GetSize(3); Clamp(t, -128, 128);
	int b = win.GetSize(4); Clamp(b, -128, 128);
	fnt.GlyphResizeCanvas(currentIndex, l, r, t, b);
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnResizeCanvas() {
	SizeWin4 win("Canvas size", "left border", "right border", "top border", "bottom border", 0, 0, 0, 0);
	if (win.Popup(this) == 0) return;
	int l = win.GetSize(1); Clamp(l, -128, 128);
	int r = win.GetSize(2); Clamp(r, -128, 128);
	int t = win.GetSize(3); Clamp(t, -128, 128);
	int b = win.GetSize(4); Clamp(b, -128, 128);
	fnt.ResizeCanvas(l, r, t, b);
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnEdit(int params) {
	int x = params & 0xffff;
	int y = params >> 16;

	if (mouse_b & 1) {
		if (key_shifts & KB_SHIFT_FLAG) {
			fnt.LineTo(currentIndex, x, y, palettePanel.LeftColor());
		}
		else if (key_shifts & KB_CTRL_FLAG) {
			fnt.FloodFill(currentIndex, x, y, palettePanel.LeftColor());
		}
		else {
			fnt.SetPixel(currentIndex, x, y, palettePanel.LeftColor());
		}
	}
	else if (mouse_b & 2) {
		if (key_shifts & KB_SHIFT_FLAG) {
			fnt.LineTo(currentIndex, x, y, palettePanel.RightColor());
		}
		else if (key_shifts & KB_CTRL_FLAG) {
			fnt.FloodFill(currentIndex, x, y, palettePanel.RightColor());
		}
		else {
			fnt.SetPixel(currentIndex, x, y, palettePanel.RightColor());
		}
	}
	else if (mouse_b & 4) {
		fnt.ClearPixel(currentIndex, x, y);
	}

	glyphEd.SetFont(&fnt);
	if (updateCheck.Selected()) {
		previewPanel.SetFont(&fnt);
	}
}


void MainDlg::OnToggleGrid() {
	if (showGridCheck.Selected()) {
		glyphEd.EnableGrid();
	}
	else {
		glyphEd.DisableGrid();
	}
}


void MainDlg::OnGradient() {
	fnt.ApplyGradient(palettePanel.LeftColor(), palettePanel.RightColor());
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnOutline() {
	fnt.ApplyOutline(palettePanel.LeftColor());
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnSmooth() {
	fnt.ApplySmoothFilter();
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnBlur() {
	fnt.ApplyBlurFilter();
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnNoise() {
	SizeWin dlg("Noise", "Noise amplitude (0-255)", 48);
	if (dlg.Popup(this) == 0) return;
	int nAmplitude = dlg.GetSize();
	nAmplitude = Clamp(nAmplitude, 0, 255);

	fnt.ApplyNoise(nAmplitude);
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnCutout() {
	SizeWin dlg("Cutout", "Threshold (pixels)", 2);
	if (dlg.Popup(this) == 0) return;
	int t = dlg.GetSize();
	t = Clamp(t, 0, 128);

	fnt.ApplyCutout(palettePanel.LeftColor(), t);
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnMakeWave() {
	SizeWin3 dlg("Wave effect", "amplitude (pixels)", "frequency (1-256)", "offset (pixels)", 2, 32, 0);
	if (dlg.Popup(this) == 0) return;

	int A = dlg.GetSize(1);
	int f = dlg.GetSize(2);
	int offset = dlg.GetSize(3);

	fnt.ApplyWave(palettePanel.LeftColor(), palettePanel.RightColor(), (float)A, (float)f/4.0f, -offset);
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnLight() {
	SizeWin dlg("Light/Shadow", "angle", 135);
	if (dlg.Popup(this) == 0) return;

	int a = dlg.GetSize();
	a = Clamp(a, 0, 360);

	fnt.ApplyLight(palettePanel.LeftColor(), (float)a*AL_PI/180.0f);
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnDefaultPalette() {
	palettePanel.MakeDefaultPalette();
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnLoadPalette() {
	FileSelect dlg("Open file...", path, "Bitmaps (*.pcx;*.bmp;*.tga;*.lbm)|All files (*.*)", FA_ARCH);
	char *selPath = dlg.Popup(this);

	if (selPath) {
		palettePanel.UpdatePalette(selPath);
		glyphEd.Redraw();
		previewPanel.Redraw();
		ustrcpy(path, selPath);
	}
}


void MainDlg::OnPaletteGradient() {
	ColorSelect *dlg = new ColorSelect("Select start colour", palettePanel.GetColor(palettePanel.LeftColor()));
	int c1 = dlg->Popup(this);
	delete dlg;
	if (c1 == -1) {
		return;
	}

	dlg = new ColorSelect("Select end colour", palettePanel.GetColor(palettePanel.RightColor()));
	int c2 = dlg->Popup(this);
	delete dlg;
	if (c2 == -1) {
		return;
	}

	palettePanel.MakePaletteGradient(palettePanel.LeftColor(), palettePanel.RightColor(), c1, c2);
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnPushUp() {
	fnt.PushUp(currentIndex);
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnPushDown() {
	fnt.PushDown(currentIndex);
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnPushLeft() {
	fnt.PushLeft(currentIndex);
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnPushRight() {
	fnt.PushRight(currentIndex);
	glyphEd.Redraw();
	previewPanel.Redraw();
}

#ifdef	ALLEGRO_WINDOWS
#include <winalleg.h>
#undef MessageBox
#endif

BITMAP *MainDlg::GetBitmapFromClipboard() {
#ifdef	ALLEGRO_WINDOWS
    BITMAP *result = NULL;

    if (!IsClipboardFormatAvailable(CF_BITMAP)) {
        return NULL;
    }

    if (OpenClipboard(win_get_window())) {
        HBITMAP hbmp;

        hbmp = GetClipboardData(CF_BITMAP);
        if (hbmp != NULL) {
            result = convert_hbitmap_to_bitmap(hbmp);
        }
        CloseClipboard();
    }

	BITMAP *ret = create_bitmap_ex(8, result->w, result->h);
	blit(result, ret, 0, 0, 0, 0, ret->w, ret->h);
    destroy_bitmap(result);
	return ret;
#else
	return copy;
#endif
}

void MainDlg::PutBitmapOnClipboard(BITMAP *bmp) {
#ifdef	ALLEGRO_WINDOWS
    if (OpenClipboard(win_get_window())) {
        if (EmptyClipboard()) {
			BITMAP *tmp = create_bitmap(bmp->w, bmp->h);
			blit(bmp, tmp, 0, 0, 0, 0, bmp->w, bmp->h);
			HBITMAP hbmp = convert_bitmap_to_hbitmap(tmp);
			destroy_bitmap(tmp);
            if (hbmp) {
                if (!SetClipboardData(CF_BITMAP, hbmp)) {
                    DeleteObject(hbmp);
                }
                CloseClipboard();
            }
        }
    }
#else
	if (copy) destroy_bitmap(copy);
	copy = bmp;
#endif
}


void MainDlg::OnCut() {
	PutBitmapOnClipboard(fnt.Cut(currentIndex));
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnCopy() {
	PutBitmapOnClipboard(fnt.Copy(currentIndex));
}


void MainDlg::OnPaste() {
	BITMAP *bmp = GetBitmapFromClipboard();
	if (!bmp) return;
	fnt.Paste(bmp, currentIndex);
	glyphEd.Redraw();
	previewPanel.Redraw();
}


void MainDlg::OnAddRange() {
	SizeWin2 dlg("Select range", "start character", "end character", 128, 255);
	if (dlg.Popup(this) == 0) return;

	Range r(dlg.GetSize(1), dlg.GetSize(2));

	if (r.min > r.max || r.min < 0x20 || r.max > 0xFFFF) {
		MessageBox msg("Error", "The range is invalid.", NULL, NULL, "OK", NULL, NULL);
		msg.Popup(this);
		return;
	}

	vector<Range>::iterator i;
	for (i = ranges.begin(); i != ranges.end(); ++i) {
		if ((r.min >= i->min && r.min <= i->max) || (r.max >= i->min && r.max <= i->max)) {
			MessageBox msg("Error", "The range is invalid.", NULL, NULL, "OK", NULL, NULL);
			msg.Popup(this);
			return;
		}
	}

	int count = 0;
	for (i = ranges.begin(); i != ranges.end(); ++i) {
		if (i->min > r.min) {
			break;
		}
		++count;
	}

	ranges.insert(i, r);
	char buf[32];
	r.Print(buf);
	rangeList.InsertItem(new ListItemString(buf), count);
	rangeList.Select(count);
	fnt.AddRange(GetCurrentIndex() - characterList.Selection(), r.min, r.max - r.min + 1);
	OnRangeSelChange();
}


void MainDlg::OnRemoveRange() {
	if (ranges.size() == 1) {
		MessageBox msg("Error", "You can't delete this range.", NULL, NULL, "OK", NULL, NULL);
		msg.Popup(this);
		return;
	}

	int sel = rangeList.Selection();
	Range r = ranges[sel];
	fnt.RemoveRange(GetCurrentIndex() - characterList.Selection(), r.max - r.min + 1);
	rangeList.DeleteItem(sel);
	vector<Range>::iterator i = ranges.begin();
	int count;
	for (count = 0; count < sel; ++sel) {
		++i;
	}
	ranges.erase(i);
	OnRangeSelChange();
}


void MainDlg::OnRangeSelChange() {
	characterList.DeleteAllItems();
	int selRange = rangeList.Selection();
	Range r = ranges[selRange];

	int rOffset = 0;
	for (int c=0; c<selRange; c++) {
		rOffset += ranges[c].max - ranges[c].min + 1;
	}

	int count = 0;
	for (int i=r.min; i<=r.max; ++i) {
		char buf[32];
		usprintf(buf, "%#04X(%c):", i, i);
		characterList.InsertItem(new ListItemFont(buf, count + rOffset, &fnt), count);
		count++;
	}

	OnListSelChange(0);
}


int MainDlg::GetCurrentIndex() {
	int ret = 0;
	for (int i=0; i<rangeList.Selection(); ++i) {
		ret += (ranges[i].max - ranges[i].min + 1);
	}

	ret += characterList.Selection();
	return ret;
}


void MainDlg::OnGlyphImport() {
#ifdef USE_PNGLOADER
	FileSelect dlg("Import glyph...", path, "All bitmaps (*.pcx;*.bmp;*.tga;*.lbm;*.png)|BMP files (*.bmp)|PCX files (*.pcx)|TGA files (*.tga)|LBM files (*.lbm)|All files (*.*)", 0xFF);
#else
	FileSelect dlg("Import glyph...", path, "All bitmaps (*.pcx;*.bmp;*.tga;*.lbm)|BMP files (*.bmp)|PCX files (*.pcx)|TGA files (*.tga)|LBM files (*.lbm)|All files (*.*)", 0xFF);
#endif
	char *selPath = dlg.Popup(this);

	if (selPath) {
		fnt.LoadGlyph(characterList.Selection(), selPath);
		ustrcpy(path, selPath);
	}
}


void MainDlg::OnGlyphExport() {
#ifdef USE_PNGLOADER
	FileSelect dlg("Export glyph...", path, "All bitmaps (*.pcx;*.bmp;*.tga;*.lbm;*.pcx)|BMP files (*.bmp)|PCX files (*.pcx)|TGA files (*.tga)|LBM files (*.lbm)|All files (*.*)", 0xFF);
#else
	FileSelect dlg("Export glyph...", path, "All bitmaps (*.pcx;*.bmp;*.tga;*.lbm)|BMP files (*.bmp)|PCX files (*.pcx)|TGA files (*.tga)|LBM files (*.lbm)|All files (*.*)", 0xFF);
#endif
	char *selPath = dlg.Popup(this);

	if (selPath) {
		fnt.SaveGlyph(characterList.Selection(), selPath);
		ustrcpy(path, selPath);
	}
}
