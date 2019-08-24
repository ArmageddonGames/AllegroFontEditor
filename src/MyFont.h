/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#ifndef		MYFONT_H
#define		MYFONT_H

#include <MASkinG.h>
#include "PalettePanel.h"
#include <vector>
using namespace std;
using namespace MAS;

inline int Clamp(int x, int min, int max) { return (x < min) ? min : ((x > max) ? max : x); }

class MyFont : public Font {
	private:
		int w, h;
		bool aa;
		int fg;
		int bg;
		int lastX, lastY;
		vector<BITMAP *> glyphs;
		PalettePanel *pal;
		static RGB_MAP rgbTable;

		void PaintFont(int startIndex, int startChar, int n);
		void PaintFixedFont(int startIndex, int startChar, int n);
		void PaintVariableFont(int startIndex, int startChar, int n);
		void GetGlyphExtents(BITMAP *g, int &x, int &w);
		void Create(int n);
		void ApplyGradient(BITMAP *bmp, int c1, int c2);
		void ApplySmoothFilter(BITMAP *bmp);
		void ApplyBlurFilter(BITMAP *bmp);
		void ApplyOutline(BITMAP *bmp, int c);
		void ApplyNoise(BITMAP *bmp, int a);
		void ApplyCutout(BITMAP *bmp, int c, int t);
		void ApplyWave(BITMAP *bmp, int c1, int c2);
		void ApplyLight(BITMAP *bmp, int c, int a);
		int GetPixel(BITMAP *bmp, int x, int y);

		int Distance(BITMAP *bmp, int x, int y, int t, int &i, int &j);
		void MakeRGBMap();

	public:
		MyFont();
		~MyFont();

		void Create(int w, int h, int n);
		void DestroyEx();
		void AddRange(int startIndex, int startChar, int n);
		void RemoveRange(int startIndex, int n);

		void Load(const char *fileName, int n, bool aa, int fg, int bg);
		void Save(const char *fileName);

		void SetPixel(int glyph, int x, int y, int col);
		int GetPixel(int glyph, int x, int y);
		void ClearPixel(int glyph, int x, int y);
		void LineTo(int glyph, int x, int y, int col);
		void FloodFill(int glyph, int x, int y, int col);

		int GlyphWidth(int glyph);
		int GlyphHeight(int glyph);
		int GlyphHeight() { return h; };
		void GlyphResize(int glyph, int w, int h);
		void GlyphResizeCanvas(int glyph, int left, int right, int top, int bottom);
		void ResizeCanvas(int left, int right, int top, int bottom);
		void SetGlyphWidth(int glyph, int w);

		void SetSize(int s);
		void ClearGlyph(int i);
		void SaveGlyph(int glyph, const char *path);
		void LoadGlyph(int glyph, const char *path);
		void DrawGlyph(int glyph, BITMAP *bmp, int x, int y);

		int FontColor() { return fg; }
		int BackgroundColor() { return bg; }
		void SetColor(int c) { fg = c; }
		void SetBackground(int c) { bg = c; }
		void SetPalette(PalettePanel *p) { pal = p; }
		bool IsColourFont();

		void ApplyGradient(int c1, int c2);
		void ApplySmoothFilter();
		void ApplyBlurFilter();
		void ApplyOutline(int c);
		void ApplyNoise(int a);
		void ApplyCutout(int c, int t);
		void ApplyWave(int c1, int c2, float A, float f, int offset);
		void ApplyLight(int c, float a);

		void PushUp(int i);
		void PushDown(int i);
		void PushLeft(int i);
		void PushRight(int i);
		BITMAP *Cut(int i);
		BITMAP *Copy(int i);
		void Paste(BITMAP *bmp, int i);
};

#endif		//MYFONT_H
