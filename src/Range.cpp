/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#include "Range.h"

Range::Range(int m, int M) : min(m), max(M) {
}


Range::Range() {
}


Range::~Range() {
}


void Range::Print(char *buffer) {
	usprintf(buffer, "%#X - %#X\0", min, max);
}
