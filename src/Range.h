/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#ifndef		RANGE_H
#define		RANGE_H

#include <allegro.h>

class Range {
	public:
		Range(int m, int M);
		Range();
		~Range();

		int min;
		int max;

		void Print(char *buffer);
};


#endif		//RANGE_H
