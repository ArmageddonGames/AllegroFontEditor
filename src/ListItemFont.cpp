/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#include "ListItemFont.h"


ListItemFont::ListItemFont(const char *text, int c, MyFont *f) : ListItemString(text), fnt(NULL) {
	fnt = f;
	ch = c;
}


ListItemFont::~ListItemFont() {
	fnt = NULL;
}


void ListItemFont::Draw(Bitmap &canvas, Bitmap &bmp, int state) {
	ListItem::Draw(canvas, bmp, state);

	if (text) {
		int s;
		switch (state) {
			case 0:
			case 1:
				s = 0;	break;
			case 2:
			case 3:
				s = 3;	break;
			case 4:
			case 5:
				s = 1;	break;
			case 6:
			case 7:
				s = 2;	break;
		};

		// get the button colors and font
		Color fg = parent->GetParent()->GetFontColor(s);
		Color bg = parent->GetParent()->GetShadowColor(s);
		Font f = parent->GetParent()->GetFont(s);

		f.GUITextout(canvas, text, 3, (h() - f.TextHeight())/2, fg, bg, -1, 0);
		int xx = 6 + f.TextLength(text);
		fnt->DrawGlyph(ch, canvas, xx, 1);
	}
}


int ListItemFont::h() {
	int h1 = fnt->GlyphHeight(ch);
	return h1 > 11 ? h1+3 : 11;
}
