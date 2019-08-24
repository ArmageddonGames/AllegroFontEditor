/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#ifndef		PALETTEPANEL_H
#define		PALETTEPANEL_H

#include <MASkinG.h>
using namespace MAS;

class PalettePanel : public Widget {
	private:
		int selLeft, selRight;
		PALETTE pal;
		int MousePosition();

	protected:
		void Draw(Bitmap &canvas);
		bool MsgWantfocus() { return true; }
		void MsgLPress();
		void MsgRPress();
		void MsgDClick();
		void MsgGotmouse();
		void MsgLostmouse();

	public:
		PalettePanel();

		int LeftColor();
		int RightColor();
		int GetColor(int i);
		void UpdatePalette(const char *fileName=NULL);
		void MakePaletteGradient(int i, int j, int fg, int bg);
		void MakeDefaultPalette();
};

#endif		//PALETTEPANEL_H
