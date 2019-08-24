/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#ifndef		LISTITEMFONT_H
#define		LISTITEMFONT_H

#include "MyFont.h"

class ListItemFont : public ListItemString {
	public:
		ListItemFont(const char *text, int ch, MyFont *fnt);
		virtual ~ListItemFont();
		void Draw(Bitmap &canvas, Bitmap &skinBitmap, int state);
		int h();
		MyFont *fnt;
		int ch;
};

#endif
