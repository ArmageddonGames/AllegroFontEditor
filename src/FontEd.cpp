/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#include "MainDlg.h"

#ifdef USE_PNGLOADER
#include <loadpng.h>
#endif

int main() {
	set_uformat(U_UTF8);
	Error err = InstallMASkinG("FontEd.cfg");
	if (err) {
		err.Report();
	}

#ifdef USE_PNGLOADER
	loadpng_init();
#endif

	MainDlg *dlg = new MainDlg;
	dlg->Execute();
	delete dlg;

	ExitMASkinG();
	return 0;
}
END_OF_MAIN();
