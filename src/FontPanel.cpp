/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#include "FontPanel.h"

FontPanel::FontPanel() : Widget(), font(NULL), glyph(0), pal(NULL) {
}


FontPanel::~FontPanel() {
	font = NULL;
	pal = NULL;
}


void FontPanel::SetFont(MyFont *f) {
	font = f;
	Redraw();
}


void FontPanel::SetPalette(PalettePanel *p) {
	pal = p;
	Redraw();
}


void FontPanel::SelectGlyph(int i) {
	glyph = i;
	Redraw();
}


void FontPanel::Draw(Bitmap &canvas) {
	canvas.Clear(makecol(212,212,212));

	if (font) {
		int cw = font->GlyphWidth(glyph);
		int ch = font->GlyphHeight(glyph);

		int pw = w()/cw;
		if (pw * ch > h()) {
			pw = h()/ch;
		}

		int ox, oy;
		ox = (w() - pw*cw)/2;
		oy = (h() - pw*ch)/2;

		int currX = ox, currY = oy;
		for (int y=0; y<ch; ++y) {
			for (int x=0; x<cw; ++x) {
				int col = font->GetPixel(glyph, x, y);
				if (col <= 0) {
					col = font->BackgroundColor();
					col = col <= 0 ? (int)(skin->c_face) : pal->GetColor(col);
				}
				else {
					col = pal->GetColor(col);
				}
				rectfill(canvas, currX, currY, currX+pw, currY+pw, col);
				if (grid) rect(canvas, currX, currY, currX+pw, currY+pw, makecol(180,180,180));
				currX += pw;
			}
			currX = ox;
			currY += pw;
		}
	}

	rect(canvas, 0, 0, w()-1, h()-1, makecol(0,0,0));
}


void FontPanel::MsgMousemove(const Point &d) {
	Widget::MsgMousemove(d);

	if (mouse_b) OnMouseDown();
}


void FontPanel::MsgClick() {
	Widget::MsgClick();
	OnMouseDown();
}


void FontPanel::OnMouseDown() {
	Point mp = GetMousePos();
	int mx = mp.x();
	int my = mp.y();

	if (font) {
		int cw = font->GlyphWidth(glyph);
		int ch = font->GlyphHeight(glyph);

		int pw = w()/cw;
		if (pw * ch > h()) {
			pw = h()/ch;
		}

		int ox, oy;
		ox = (w() - pw*cw)/2;
		oy = (h() - pw*ch)/2;

		int x = (mx - ox)/pw;
		int y = (my - oy)/pw;

		if (x < 0 || x >= cw || y < 0 || y >= ch) {
			return;
		}

		int param = x | (y<<16);
		GetParent()->HandleEvent(*this, MSG_ACTIVATE, param);
	}
}


void FontPanel::EnableGrid() {
	grid = true;
	Redraw();
}


void FontPanel::DisableGrid() {
	grid = false;
	Redraw();
}


bool FontPanel::MsgChar(int c) {
	Widget::MsgChar(c);

	if (key_shifts & KB_SHIFT_FLAG) {
		switch (c>>8) {
			case KEY_UP:	GetParent()->HandleEvent(*this, MSG_CHAR, KEY_UP);		break;
			case KEY_DOWN:	GetParent()->HandleEvent(*this, MSG_CHAR, KEY_DOWN);	break;
			case KEY_LEFT:	GetParent()->HandleEvent(*this, MSG_CHAR, KEY_LEFT);	break;
			case KEY_RIGHT:	GetParent()->HandleEvent(*this, MSG_CHAR, KEY_RIGHT);	break;
			default: return false;	break;
		};
	}
	else {
		switch (c>>8) {
			case KEY_UP:
			case KEY_DOWN:
			case KEY_LEFT:
			case KEY_RIGHT:
				return true;	break;
			default:return false;	break;
		};
	}

	return true;
}


void FontPanel::MsgGotmouse() {
	Widget::MsgGotmouse();
	SetCursor(Skin::MOUSE_CROSSHAIR);
}


void FontPanel::MsgLostmouse() {
	Widget::MsgLostmouse();
	SetCursor(Skin::MOUSE_NORMAL);
}


