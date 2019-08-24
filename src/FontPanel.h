/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#ifndef		FONTPANEL_H
#define		FONTPANEL_H

#include "MyFont.h"
#include "PalettePanel.h"

class FontPanel : public Widget {
	private:
		MyFont *font;
		PalettePanel *pal;
		int glyph;
		bool grid;

		void OnMouseDown();

	protected:
		void Draw(Bitmap &canvas);
		bool MsgWantfocus() { return true; }
		void MsgMousemove(const Point &d);
		void MsgClick();
		bool MsgChar(int c);
		void MsgGotmouse();
		void MsgLostmouse();

	public:
		FontPanel();
		~FontPanel();

		void SetFont(MyFont *f);
		void SetPalette(PalettePanel *p);
		void SelectGlyph(int i);

		void EnableGrid();
		void DisableGrid();
};

#endif		//FONTPANEL_H
