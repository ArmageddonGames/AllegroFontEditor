/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#include "PalettePanel.h"

PalettePanel::PalettePanel()
	:Widget(),
	selLeft(1),
	selRight(0)
{
	MakeDefaultPalette();
}


void PalettePanel::Draw(Bitmap &canvas) {
	float dx = w() / 16.0f;
	float dy = h() / 16.0f;
	float cx = 0.0f;
	float cy = 0.0f;

	rectfill(canvas, 0, 0, w()-1, h()-1, makecol(0,0,0));
	int count = 0;
	for (int j=0; j<16; ++j) {
		for (int i=0; i<16; ++i) {
			int col = makecol(pal[count].r<<2, pal[count].g<<2, pal[count].b<<2);
			if (col == makecol(252,0,252)) col = makecol(252,4,252);
			rectfill(canvas, int(cx)+1, int(cy)+1, int(cx+dx)-1, int(cy+dy)-1, col);
			if (count == selLeft) {
				rect(canvas, int(cx)+1, int(cy)+1, int(cx+dx)-1, int(cy+dy)-1, makecol(255,0,0));
			} else
			if (count == selRight) {
				rect(canvas, int(cx)+1, int(cy)+1, int(cx+dx)-1, int(cy+dy)-1, makecol(0,0,255));
			}
			++count;
			cx += dx;
		}
		cx = 0.0f;
		cy += dy;
	}
}


int PalettePanel::MousePosition() {
	Point mp = GetMousePos();
	int mx = mp.x();
	int my = mp.y();

	float dx = w() / 16.0f;
	float dy = h() / 16.0f;

	int ix = (int)((float)mx/dx);
	int iy = (int)((float)my/dy);

	return iy*16 + ix;
}


void PalettePanel::MsgLPress() {
	Widget::MsgLPress();

	int mp = MousePosition();
	if (mp != 255) {
		selLeft = mp;
		Redraw();
	}
}


void PalettePanel::MsgRPress() {
	Widget::MsgRPress();

	int mp = MousePosition();
	if (mp != 255) {
		selRight = mp;
		Redraw();
	}
}


void PalettePanel::MsgDClick() {
	Widget::MsgDClick();

	int c;
	if (mouse_b & 1) {
		int mp = MousePosition();
		if (mp == 0 || mp == 255) return;
		ColorSelect dlg("Select foreground color", GetColor(LeftColor()));
		c = dlg.Popup(GetParent());
		if (c != -1) {
			pal[selLeft].r = getr(c)>>2;
			pal[selLeft].g = getg(c)>>2;
			pal[selLeft].b = getb(c)>>2;
		}
	}
	else if (mouse_b & 2) {
		int mp = MousePosition();
		if (mp == 0 || mp == 255) return;
		ColorSelect dlg("Select background color", GetColor(RightColor()));
		c = dlg.Popup(GetParent());
		if (c != -1) {
			pal[selRight].r = getr(c)>>2;
			pal[selRight].g = getg(c)>>2;
			pal[selRight].b = getb(c)>>2;
		}
	}

	GetParent()->HandleEvent(*this, MSG_DCLICK);
	set_palette(pal);
	Redraw();
}


void PalettePanel::MsgGotmouse() {
	Widget::MsgGotmouse();
	SetCursor(Skin::MOUSE_SELECT);
}


void PalettePanel::MsgLostmouse() {
	Widget::MsgLostmouse();
	SetCursor(Skin::MOUSE_NORMAL);
}


int PalettePanel::LeftColor() {
	return selLeft;
}


int PalettePanel::RightColor() {
	return selRight;
}


int PalettePanel::GetColor(int i) {
	return makecol(pal[i].r<<2, pal[i].g<<2, pal[i].b<<2);
}


void PalettePanel::UpdatePalette(const char *fileName) {
	if (fileName) {
		BITMAP *tmp = load_bitmap(fileName, pal);
		destroy_bitmap(tmp);
		set_palette(pal);
	}
	else {
		get_palette(pal);
	}
	Redraw();
}


void PalettePanel::MakePaletteGradient(int i, int j, int fg, int bg) {
	float r1 = (float)(getr(fg)>>2);
	float g1 = (float)(getg(fg)>>2);
	float b1 = (float)(getb(fg)>>2);
	float r2 = (float)(getr(bg)>>2);
	float g2 = (float)(getg(bg)>>2);
	float b2 = (float)(getb(bg)>>2);
	int count = j - i;
	float dr = (float)(r2 - r1) / (float)count;
	float dg = (float)(g2 - g1) / (float)count;
	float db = (float)(b2 - b1) / (float)count;

	for (int index = i; index <= j; ++index) {
		pal[index].r = (int)r1;
		pal[index].g = (int)g1;
		pal[index].b = (int)b1;

		r1 += dr;
		g1 += dg;
		b1 += db;
	}

	set_palette(pal);
	Redraw();
}


void PalettePanel::MakeDefaultPalette() {
	// make the first color magenta and the last yellow (just for convenience)
	pal[0].r = 63;	pal[0].g = 0;	pal[0].b = 63;
	pal[255].r = 63;	pal[255].g = 63;	pal[255].b = 0;

	// black to white gradient
	MakePaletteGradient(1, 15, makecol(0,0,0), makecol(255,255,255));

	// red gradient
	MakePaletteGradient(16, 24, makecol(32,0,0), makecol(255,0,0));
	MakePaletteGradient(24, 32, makecol(255,0,0), makecol(255,255,255));

	// green gradient
	MakePaletteGradient(32, 40, makecol(0,32,0), makecol(0,255,0));
	MakePaletteGradient(40, 48, makecol(0,255,0), makecol(255,255,255));

	// blue gradient
	MakePaletteGradient(48, 56, makecol(0,0,32), makecol(0,0,255));
	MakePaletteGradient(56, 64, makecol(0,0,255), makecol(255,255,255));

	// yellow gradient
	MakePaletteGradient(64, 72, makecol(32,32,0), makecol(255,255,0));
	MakePaletteGradient(72, 80, makecol(255,255,0), makecol(255,255,255));

	// cyan gradient
	MakePaletteGradient(80, 88, makecol(0,32,32), makecol(0,255,255));
	MakePaletteGradient(88, 96, makecol(0,255,255), makecol(255,255,255));

	// magenta gradient
	MakePaletteGradient(96, 104, makecol(32,0,32), makecol(240,0,240));
	MakePaletteGradient(104, 112, makecol(240,0,240), makecol(255,255,255));

	// gray-red gradient
	MakePaletteGradient(112, 120, makecol(32,128,128), makecol(255,128,128));
	MakePaletteGradient(120, 128, makecol(255,128,128), makecol(255,255,255));

	// gray-green gradient
	MakePaletteGradient(128, 136, makecol(128,32,128), makecol(128,255,128));
	MakePaletteGradient(136, 144, makecol(128,255,128), makecol(255,255,255));

	// gray-blue gradient
	MakePaletteGradient(144, 152, makecol(128,128,32), makecol(128,128,255));
	MakePaletteGradient(152, 160, makecol(128,128,255), makecol(255,255,255));

	// gray-yellow gradient
	MakePaletteGradient(160, 168, makecol(32,32,128), makecol(255,255,128));
	MakePaletteGradient(168, 176, makecol(255,255,128), makecol(255,255,255));

	// gray-cyan gradient
	MakePaletteGradient(176, 184, makecol(128,32,32), makecol(128,255,255));
	MakePaletteGradient(184, 192, makecol(128,255,255), makecol(255,255,255));

	// gray-magenta gradient
	MakePaletteGradient(192, 200, makecol(32,128,32), makecol(255,128,255));
	MakePaletteGradient(200, 208, makecol(255,128,255), makecol(255,255,255));

	// some other gradients
	MakePaletteGradient(208, 216, makecol(255,32,0), makecol(255,255,0));
	MakePaletteGradient(216, 224, makecol(255,255,0), makecol(32,255,0));
	MakePaletteGradient(224, 232, makecol(255,0,32), makecol(240,0,240));
	MakePaletteGradient(232, 240, makecol(240,0,240), makecol(0,240,255));
	MakePaletteGradient(240, 248, makecol(32,32,255), makecol(32,255,255));
	MakePaletteGradient(248, 254, makecol(32,255,255), makecol(32,255,32));
}
