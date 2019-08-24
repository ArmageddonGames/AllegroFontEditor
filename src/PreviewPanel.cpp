/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#include "PreviewPanel.h"

PreviewPanel::PreviewPanel() : PanelSunken(), text(NULL), font(NULL) {
}


PreviewPanel::~PreviewPanel() {
	if (text) {
		delete [] text;
		text = NULL;
	}

	font = NULL;
}


void PreviewPanel::SetText(const char *t) {
	if (t == NULL)	return;
	if (text != NULL)	delete [] text;

	text = new char[ustrsizez(t)];
	ustrcpy(text, t);
	Redraw();
}


void PreviewPanel::SetFont(MyFont *f) {
	font = f;
	Redraw();
}


void PreviewPanel::Draw(Bitmap &canvas) {
	PanelSunken::Draw(canvas);

	if (!font) return;

	int len = ustrlen(text);
	int tw=0;
	int ch;
	for (int i=0; i<len; ++i) {
		ch = ugetat(text, i);
		tw += font->GlyphWidth(ch-32);
	}

	int tx = (w() - tw)/2;
	int ty = (h() - font->GlyphHeight())/2;

	set_clip_rect(canvas, 4, 4, w()-5, h()-5);
	rectfill(canvas, 4, 4, w()-5, h()-5, skin->c_face);
	for (int i=0; i<len; ++i) {
		ch = ugetat(text, i);
		font->DrawGlyph(ch-32, canvas, tx, ty);
		tx += font->GlyphWidth(ch-32);
		if (tx > x2()) break;
	}
	set_clip_rect(canvas, 0, 0, canvas.w()-1, canvas.h()-1);
}
