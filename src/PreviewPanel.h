/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#ifndef		PREVIEWPANEL_H
#define		PREVIEWPANEL_H

#include <MASkinG.h>
#include "MyFont.h"
using namespace MAS;

class PreviewPanel : public PanelSunken {
	private:
		char *text;
		MyFont *font;

	public:
		PreviewPanel();
		~PreviewPanel();

		void SetText(const char *t);
		void Draw(Bitmap &canvas);
		void SetFont(MyFont *f);
};

#endif		//PREVIEWPANEL_H
