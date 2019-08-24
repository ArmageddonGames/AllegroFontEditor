/********************************************************************
 *                   Allegro Font Editor v1.19                      *
 *                               by                                 *
 *                Miran Amon (miran_amon@gmail.com)                 *
 *               copyright (c) Miran Amon 2003-2005                 *
 ********************************************************************/
#include "MyFont.h"
#include <math.h>

RGB_MAP MyFont::rgbTable;

class SineWave {
	private:
		static int x;
		static float A;
		static float f;
		static int offset;

	public:
		static void init(float A_, float f_, int offset_) {
			A = A_;
			f = f_;
			offset = offset_;
			x = 0;
		}

		static int next() {
			return (int)(A*sin(f*(++x)*3.14159f/180.0f) + offset);
		}
};

int SineWave::x = 0;
float SineWave::A = 1.0f;
float SineWave::f = 1.0f;
int SineWave::offset = 0;



MyFont::MyFont()
	:Font(),
	w(8),
	h(8),
	aa(false),
	fg(1),
	bg(0),
	lastX(0),
	lastY(0),
	pal(NULL)
{
}


MyFont::~MyFont() {
	DestroyEx();
	pal = NULL;
}


void MyFont::Create(int w, int h, int n) {
	DestroyEx();
	this->w = w;
	this->h = h;
	BITMAP *bmp;
	for (int i=0; i<n; ++i) {
		bmp = create_bitmap_ex(8, w, h);
		clear_to_color(bmp, 0);
		glyphs.push_back(bmp);
	}
	PaintFont(0, 32, n);
}


void MyFont::Create(int n) {
	DestroyEx();
	this->w = TextLength("M");
	this->h = TextHeight();
	BITMAP *bmp;
	char buf[12];
	for (int i=0; i<n; ++i) {
		usprintf(buf, "%c\0", i+32);
		int w = TextLength(buf);
		bmp = create_bitmap_ex(8, w, h);
		if (w > this->w) {
			this->w = w;
		}
		clear_to_color(bmp, 0);
		glyphs.push_back(bmp);
	}
	PaintFont(0, 32, n);
}


void MyFont::AddRange(int startIndex, int startChar, int n) {
	vector<BITMAP *>::iterator i = glyphs.begin();
	for (int count = 0; count<startIndex; ++count) {
		++i;
	}

	char buf[12];
	memset(buf, 0, 12);
	for (int count = 0; count<n; ++count) {
		usprintf(buf, "%c\0", startChar + count);
		int w = TextLength(buf);
		BITMAP *bmp = create_bitmap_ex(8, w, h);
		if (w > this->w) {
			this->w = w;
		}
		clear_to_color(bmp, 0);
		i = glyphs.insert(i, bmp);
		++i;
	}

	PaintFont(startIndex, startChar, n);
}


void MyFont::RemoveRange(int startIndex, int n) {
	vector<BITMAP *>::iterator i = glyphs.begin();
	for (int count = 0; count<startIndex; ++count) {
		++i;
	}

	char buf[12];
	for (int count = 0; count<n; ++count) {
		destroy_bitmap(*i);
		*i = NULL;
		i = glyphs.erase(i);
	}
}


void MyFont::PaintFixedFont(int startIndex, int startChar, int n) {
	vector<BITMAP *>::iterator i = glyphs.begin();
	for (int count = 0; count<startIndex; ++count) {
		++i;
	}

	int ch = startChar;
	for (int count = 0; count < n; ++count) {
		char buf[12];
		usprintf(buf, "%c", ch);
		if (aa) {
			bool oldAA = Settings::antialiasing;
			Settings::antialiasing = true;
			Bitmap g = *i;
			Textout(g, buf, 0, 0, fg, bg, 0);
			for (int y=0; y<g.h(); ++y) {
				for (int x=0; x<g.w(); ++x) {
					if (_getpixel(g, x, y) == bg) {
						_putpixel(g, x, y, 0);
					}
				}
			}
			Settings::antialiasing = oldAA;
		}
		else {
			Bitmap g = *i;
			Textout(g, buf, 0, 0, fg, 0, 0);
		}

		++ch;
		++i;
	}
}


void MyFont::PaintVariableFont(int startIndex, int startChar, int n) {
	vector<BITMAP *>::iterator i = glyphs.begin();
	for (int count = 0; count<startIndex; ++count) {
		++i;
	}

	PALETTE pal;
	get_palette(pal);
	int fg = makecol(pal[this->fg].r*4, pal[this->fg].g*4, pal[this->fg].b*4);
	int bg = makecol(pal[this->bg].r*4, pal[this->bg].g*4, pal[this->bg].b*4);
	if (this->bg == 0) {
		bg = -1;
	}
	int magenta = Color(255,0,255);

	int ch = startChar;
	int glyphIndex = startIndex;
	for (int count = 0; count < n; ++count) {
		char buf[12];
		usprintf(buf, "%c", ch);
		BITMAP *g = *i;

		// make a temporary bitmap twice the size of the glyph
		Bitmap bmp3 = Bitmap(8*g->w, g->h);
		bmp3.Clear(magenta);

		// render the glyph to the centre of the temp bitmap
		bool oldAA = Settings::antialiasing;
		if (aa) {
			Settings::antialiasing = true;
			Textout(bmp3, buf, bmp3.w()>>1, 0, fg, bg, 2);
			/*
			for (int y=0; y<bmp3.h(); ++y) {
				for (int x=0; x<bmp3.w(); ++x) {
					if (getpixel(bmp3, x, y) == bg) {
						putpixel(bmp3, x, y, magenta);
					}
				}
			}
			*/
		}
		else {
			Settings::antialiasing = false;
			Textout(bmp3, buf, bmp3.w()>>1, 0, fg, -1, 2);
		}
		Settings::antialiasing = oldAA;

		// see where in the temp bitmap the glyph starts and exactly
		// how wide it is
		int x, w;
		BITMAP *bmp2 = create_bitmap_ex(8, bmp3.w(), bmp3.h());
		clear_to_color(bmp2, 0);
		blit(bmp3, bmp2, 0, 0, 0, 0, bmp3.w(), bmp3.h());
		GetGlyphExtents(bmp2, x, w);

		// resize the glyph to the actual width
		SetGlyphWidth(glyphIndex, w+1);

		// copy the rendered glyph from the temp bitmap to the actual
		// glyph bitmap
		g = *i;
		blit(bmp2, g, x, 0, 0, 0, w, g->h);

		destroy_bitmap(bmp2);

		++ch;
		++i;
		++glyphIndex;
	}
}


void MyFont::GetGlyphExtents(BITMAP *g, int &outx, int &outw) {
	int h = g->h;
	int w = g->w;
	int minw = w;

	// find the leftmost nonzero pixel
	int minx = w;
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < minx; x++) {
			if (_getpixel(g, x, y) != 0) {
				minx = x;
				break;
			}
		}
	}
	if (minx == w) {
		minx = 0;
	}

	// find the rightmost nonzero pixel
	int maxx = 0;
	for (int y = 0; y < h; ++y) {
		for (int x = w-1; x >= maxx; --x) {
			if (_getpixel(g, x, y) != 0) {
				maxx = x;
				break;
			}
		}
	}
	if (maxx == 0) {
		maxx = w/4;
	}

	// calculate output
	outx = minx;
	outw = maxx - minx + 1;
}


void MyFont::PaintFont(int startIndex, int startChar, int n) {
	if (IsScalable()) {
		PaintVariableFont(startIndex, startChar, n);
	}
	else {
		PaintFixedFont(startIndex, startChar, n);
	}
}


void MyFont::DestroyEx() {
	if (!glyphs.empty()) {
		vector<BITMAP *>::iterator i;
		for (i = glyphs.begin(); i != glyphs.end(); ++i) {
			destroy_bitmap(*i);
			*i = NULL;
		}

		glyphs.clear();
	}
}


void MyFont::Load(const char *fileName, int n, bool aa, int fg, int bg) {
	Font::Load(fileName);

	this->aa = aa;
	this->fg = fg;
	this->bg = bg;
	Create(n);
}


void MyFont::SetSize(int s) {
	Font::SetSize(s);

	int n = glyphs.size();
	DestroyEx();
	Create(n);
}


void MyFont::Save(const char *fileName) {
	BITMAP *out = create_bitmap_ex(8, 16*(w+1)+1, (int)(ceil((float)glyphs.size()/16.0f)*(h+1)+1));
	PALETTE pal;
	get_palette(pal);
	clear_to_color(out, 255);
	vector<BITMAP *>::iterator i;
	int x = 1;
	int y = 1;
	int dx = w+1;
	int dy = h+1;
	int count=0;
	for (i = glyphs.begin(); i != glyphs.end(); ++i) {
		blit(*i, out, 0, 0, x, y, (*i)->w, (*i)->h);
		x += dx;
		if (x >= out->w - w) {
			x = 1;
			y += dy;
		}
		++count;
	}

	save_bitmap(fileName, out, pal);
}


void MyFont::SetPixel(int i, int x, int y, int col) {
	putpixel(glyphs[i], x, y, col);
	lastX = x;
	lastY = y;
}


void MyFont::LineTo(int i, int x, int y, int col) {
	line(glyphs[i], lastX, lastY, x, y, col);
	lastX = x;
	lastY = y;
}


void MyFont::FloodFill(int i, int x, int y, int col) {
	floodfill(glyphs[i], x, y, col);
	lastX = x;
	lastY = y;
}


void MyFont::ClearPixel(int i, int x, int y) {
	_putpixel(glyphs[i], x, y, 0);
}


int MyFont::GetPixel(int i, int x, int y) {
	return _getpixel(glyphs[i], x, y);
}


int MyFont::GetPixel(BITMAP *bmp, int x, int y) {
	if (x < 0 || x >= bmp->w || y < 0 || y >= bmp->h) {
		return -1;
	}
	else {
		return _getpixel(bmp, x, y);
	}
}


bool MyFont::IsColourFont() {
	for (int i=0; i<glyphs.size(); i++) {
		BITMAP *bmp = glyphs[i];
		int color = -1;
		for (int y=0; y<bmp->h; ++y) {
			for (int x=0; x<bmp->w; ++x) {
				int c = _getpixel(bmp, x, y);
				if (c != 0) {
					if (color == -1) {
						color = c;
					}
					else if (c != color) {
						return true;
					}
				}
			}
		}
	}

	return false;
}


void MyFont::SaveGlyph(int i, const char *path) {
	save_bitmap(path, glyphs[i], NULL);
}


void MyFont::LoadGlyph(int i, const char *path) {
	BITMAP *bmp = load_bitmap(path, NULL);
	if (!bmp) {
		return;
	}

	if (glyphs[i]) {
		destroy_bitmap(glyphs[i]);
	}

	BITMAP *newGlyph = create_bitmap_ex(8, bmp->w, bmp->h);
	blit(bmp, newGlyph, 0, 0, 0, 0, bmp->w, bmp->h);
	destroy_bitmap(bmp);
	glyphs[i] = newGlyph;
}


void MyFont::ClearGlyph(int i) {
	clear_to_color(glyphs[i], 0);
}


void MyFont::SetGlyphWidth(int i, int w) {
	if (glyphs[i]->w == w) {
		return;
	}

	BITMAP *bmp = create_bitmap_ex(8, w, h);
	clear_to_color(bmp, 0);
	blit(glyphs[i], bmp, 0, 0, 0, 0, glyphs[i]->w, h);
	destroy_bitmap(glyphs[i]);
	glyphs[i] = bmp;

	if (w > this->w) {
		this->w = w;
	}
}


void MyFont::GlyphResize(int i, int w, int h) {
	if (glyphs[i]->w == w && glyphs[i]->h == h) {
		return;
	}

	BITMAP *bmp = create_bitmap_ex(8, w, h);
	clear_to_color(bmp, 0);
	blit(glyphs[i], bmp, 0, 0, 0, 0, glyphs[i]->w, glyphs[i]->h);
	destroy_bitmap(glyphs[i]);
	glyphs[i] = bmp;

	if (w > this->w) {
		this->w = w;
	}

	if (h > this->h) {
		this->h = h;
	}
}


void MyFont::GlyphResizeCanvas(int i, int l, int r, int t, int b) {
	int w = glyphs[i]->w + l + r;
	int h = glyphs[i]->h + t + b;

	BITMAP *bmp = create_bitmap_ex(8, w, h);
	clear_to_color(bmp, 0);
	blit(glyphs[i], bmp, 0, 0, l, t, glyphs[i]->w, glyphs[i]->h);
	destroy_bitmap(glyphs[i]);
	glyphs[i] = bmp;

	if (w > this->w) {
		this->w = w;
	}

	if (h > this->h) {
		this->h = h;
	}
}


void MyFont::ResizeCanvas(int l, int r, int t, int b) {
	int nGlyphs = glyphs.size();
	for (int i=0; i<nGlyphs; i++) {
		GlyphResizeCanvas(i, l, r, t, b);
	}
}


int MyFont::GlyphWidth(int i) {
	if (i < 0 || i >= glyphs.size()) {
		return 0;
	}
	return glyphs[i]->w;
}


int MyFont::GlyphHeight(int i) {
	if (i < 0 || i >= glyphs.size()) {
		return 0;
	}

	return glyphs[i]->h;
}


void MyFont::DrawGlyph(int i, BITMAP *bmp, int x, int y) {
	if (i < 0 || i >= glyphs.size()) {
		return;
	}

	if (bg > 0) {
		rectfill(bmp, x, y, x+glyphs[i]->w, y+glyphs[i]->h, pal->GetColor(bg));
	}
	draw_sprite(bmp, glyphs[i], x, y);
}


void MyFont::MakeRGBMap() {
	PALETTE pal;
	get_palette(pal);
	create_rgb_table(&rgbTable, pal, NULL);
	rgb_map = &rgbTable;
}


void MyFont::ApplyGradient(int c1, int c2) {
	vector<BITMAP *>::iterator i;
	for (i = glyphs.begin(); i != glyphs.end(); ++i) {
		ApplyGradient(*i, c1, c2);
	}
}


void MyFont::ApplySmoothFilter() {
	MakeRGBMap();

	ResizeCanvas(1,1,1,1);
	vector<BITMAP *>::iterator i;
	for (i = glyphs.begin(); i != glyphs.end(); ++i) {
		ApplySmoothFilter(*i);
	}
}


void MyFont::ApplyBlurFilter() {
	MakeRGBMap();

	ResizeCanvas(1,1,1,1);
	vector<BITMAP *>::iterator i;
	for (i = glyphs.begin(); i != glyphs.end(); ++i) {
		ApplyBlurFilter(*i);
	}
}


void MyFont::ApplyNoise(int a) {
	MakeRGBMap();
	srand((unsigned)time(NULL));

	vector<BITMAP *>::iterator i;
	for (i = glyphs.begin(); i != glyphs.end(); ++i) {
		ApplyNoise(*i, a);
	}
}


void MyFont::ApplyOutline(int c) {
	MakeRGBMap();

	vector<BITMAP *>::iterator i;
	for (i = glyphs.begin(); i != glyphs.end(); ++i) {
		ApplyOutline(*i, c);
	}
}


void MyFont::ApplyCutout(int c, int t) {
	vector<BITMAP *>::iterator i;
	for (i = glyphs.begin(); i != glyphs.end(); ++i) {
		ApplyCutout(*i, c, t);
	}
}


void MyFont::ApplyWave(int c1, int c2, float A, float f, int offset) {
	SineWave::init(A,f,offset+TextHeight()/2);
	vector<BITMAP *>::iterator i;
	for (i = glyphs.begin(); i != glyphs.end(); ++i) {
		ApplyWave(*i, c1, c2);
	}
}


void MyFont::ApplyLight(int c, float a) {
	MakeRGBMap();
	vector<BITMAP *>::iterator i;
	int angle = (int)(a * 180.0f / AL_PI);
	for (i = glyphs.begin(); i != glyphs.end(); ++i) {
		ApplyLight(*i, c, angle);
	}
	//ApplyLight(glyphs['A'-32], c, angle);
}


void MyFont::ApplyGradient(BITMAP *bmp, int c1, int c2) {
	int h = bmp->h;
	float c = (float)c1;
	float dc = (float)(c2 - c1) / (float)h;
	int bg8 = (bg >= 0) ? bg : 0;

	for (int y=0; y<h; ++y) {
		for (int x=0; x<bmp->w; ++x) {
			int oldc = _getpixel(bmp, x, y);
			if (oldc == bg8 || oldc == 0) {
				continue;
			}
			_putpixel(bmp, x, y, (int)c);
		}
		c += dc;
	}
}


void MyFont::ApplySmoothFilter(BITMAP *bmp) {
	int r[9];
	int g[9];
	int b[9];
	int c[9];

	BITMAP *tmp = create_bitmap_ex(8, bmp->w, bmp->h);
	blit(bmp, tmp, 0, 0, 0, 0, bmp->w, bmp->h);
	for (int y=0; y<bmp->h; ++y) {
		for (int x=0; x<bmp->w; ++x) {
			int count = 0;
			int empty = 0;
			for (int j=-1; j<=1; ++j) {
				for (int i=-1; i<=1; ++i) {
					c[count] = GetPixel(tmp, x+i, y+j);
					if (c[count] <= 0) {
						++empty;
						c[count] = bg == 0 ? pal->RightColor() : bg;
					}
					++count;
				}
			}

			if (empty == 9 || (pal->RightColor()==0 && _getpixel(tmp, x, y)==0)) {
				continue;
			}

			for (int i=0; i<9; ++i) {
				int color = pal->GetColor(c[i]);
				r[i] = getr(color);
				g[i] = getg(color);
				b[i] = getb(color);
			}

			int r2 = ((r[1] + r[3] + r[5] + r[7]) + (r[4]<<2))>>3;
			int g2 = ((g[1] + g[3] + g[5] + g[7]) + (g[4]<<2))>>3;
			int b2 = ((b[1] + b[3] + b[5] + b[7]) + (b[4]<<2))>>3;

			_putpixel(bmp, x, y, makecol8(r2,g2,b2));
		}
	}

	destroy_bitmap(tmp);
}


void MyFont::ApplyBlurFilter(BITMAP *bmp) {
	int r[9];
	int g[9];
	int b[9];
	int c[9];

	BITMAP *tmp = create_bitmap_ex(8, bmp->w, bmp->h);
	blit(bmp, tmp, 0, 0, 0, 0, bmp->w, bmp->h);
	for (int y=0; y<bmp->h; ++y) {
		for (int x=0; x<bmp->w; ++x) {
			int count = 0;
			int empty = 0;
			for (int j=-1; j<=1; ++j) {
				for (int i=-1; i<=1; ++i) {
					c[count] = GetPixel(tmp, x+i, y+j);
					if (c[count] <= 0) {
						++empty;
						c[count] = bg == 0 ? pal->RightColor() : bg;
					}
					++count;
				}
			}

			if (empty == 9 || (pal->RightColor()==0 && _getpixel(tmp, x, y)==0)) {
				continue;
			}

			for (int i=0; i<9; ++i) {
				int color = pal->GetColor(c[i]);
				r[i] = getr(color);
				g[i] = getg(color);
				b[i] = getb(color);
			}

			int r2 = (r[0] + r[2] + r[6] + r[8] + ((r[1] + r[3] + r[5] + r[7])<<1) + (r[4]<<2))>>4;
			int g2 = (g[0] + g[2] + g[6] + g[8] + ((g[1] + g[3] + g[5] + g[7])<<1) + (g[4]<<2))>>4;
			int b2 = (b[0] + b[2] + b[6] + b[8] + ((b[1] + b[3] + b[5] + b[7])<<1) + (b[4]<<2))>>4;

			_putpixel(bmp, x, y, makecol8(r2,g2,b2));
		}
	}

	destroy_bitmap(tmp);
}


void MyFont::ApplyOutline(BITMAP *bmp, int cIndex) {
	int c[9];
	int col = pal->GetColor(cIndex);

	for (int y=0; y<bmp->h; ++y) {
		for (int x=0; x<bmp->w; ++x) {
			int count = 0;
			int empty = 0;
			for (int j=-1; j<=1; ++j) {
				for (int i=-1; i<=1; ++i) {
					if (i!=0 || j!=0) {
						c[count] = GetPixel(bmp, x+i, y+j);
						if (c[count] == -1 || c[count] == 0) {
							++empty;
							c[count] = makecol8(255,255,255);
						}
					}
					++count;
				}
			}

			c[4] = _getpixel(bmp, x, y);
			if (c[4] == 0 || empty == 0) {
				continue;
			}

			int r, g, b;
			int col1 = pal->GetColor(c[4]);
			int o = (empty >= 3) ? (255) : ((empty == 2) ? 192 : 128);
			r = (getr(col)*o + getr(col1)*(255-o))>>8;
			g = (getg(col)*o + getg(col1)*(255-o))>>8;
			b = (getb(col)*o + getb(col1)*(255-o))>>8;
			_putpixel(bmp, x, y, makecol8(r,g,b));
		}
	}
}


void MyFont::ApplyNoise(BITMAP *bmp, int amplitude) {
	for (int y=0; y<bmp->h; ++y) {
		for (int x=0; x<bmp->w; ++x) {
			int c = _getpixel(bmp, x, y);
			if (c == 0 || c == bg) {
				continue;
			}

			int delta = rand()%amplitude - (amplitude>>1);
			c = pal->GetColor(c);
			int r = Clamp(getr(c) + delta, 0, 255);
			int g = Clamp(getg(c) + delta, 0, 255);
			int b = Clamp(getb(c) + delta, 0, 255);
			_putpixel(bmp, x, y, makecol8(r,g,b));
		}
	}
}


void MyFont::ApplyCutout(BITMAP *bmp, int color, int threshold) {
	int col1, col2, r, g, b, i, j;
	for (int y=0; y<bmp->h; ++y) {
		for (int x=0; x<bmp->w; ++x) {
			int d = Distance(bmp, x, y, threshold, i, j);
			if (d > threshold) {
				_putpixel(bmp, x, y, color);
			}
		}
	}
}


void MyFont::ApplyLight(BITMAP *bmp, int color, int a1) {
	int col1;
	float r, g, b;
	int col2, r2, g2, b2;
	int i, j;

	//theLog << "Aplying light:\n" << "color = " << color << "\n" << "angle = " << a1 << "\n";

	// light colour in truecolor format
	col1 = pal->GetColor(color);
	r = (float)getr(col1);
	g = (float)getg(col1);
	b = (float)getb(col1);

	int t = bmp->h>>1;
	for (int y=0; y<bmp->h; ++y) {
		for (int x=0; x<bmp->w; ++x) {
			// get the distance of the current pixel from the nearest
			// empty pixel and the vector between them
			//theLog << x << "," << y << ": ";
			int d = Distance(bmp, x, y, t, i, j);
			if (d == 0 || d > t) {
				//theLog << "d = 0\n";
				continue;
			}
			else {
				//theLog << "d = " << d << ", ";
			}

			// get the angle [0,360]
			int a2 = (int)(atan2(j,i)*180.0f/AL_PI) + 180;
			//theLog << "a2 = " << a2 << ", ";

			// get the absolute difference between the two angles [-90,90]
			int da = ABS(a2 - a1);
			if (da>180) da = 360-da;
			//theLog << "da = " << da << ", ";
			da -= 90;

			// calculate light factor [-1, 1]
			float o = (float)da*(t-d)/(float)(t*90.0f);
			//theLog << "o = " << o << ", ";

			// calculate the new colour
			col2 = pal->GetColor(_getpixel(bmp, x, y));
			r2 = getr(col2) + (int)(o*r);	r2 = Clamp(r2,0,255);
			g2 = getg(col2) + (int)(o*g);	g2 = Clamp(g2,0,255);
			b2 = getb(col2) + (int)(o*b);	b2 = Clamp(b2,0,255);

			//theLog << "col2 = " << makecol8(r2,g2,b2) << " (";
			//theLog << r2 << "," << g2 << "," << b2 << ")\n";

			// write color
			_putpixel(bmp, x, y, makecol8(r2,g2,b2));
		}
	}
}


int MyFont::Distance(BITMAP *bmp, int x, int y, int t, int &i, int &j) {
	int d = 0;
	int c = _getpixel(bmp, x,y);
	if (c == 0 || c == bg) return d;

	while (true) {
		++d;

		// check top line
		j=y-d;
		for (i=x-d; i<=x+d; ++i) {
			c = GetPixel(bmp, i,j);
			if (c <= 0 || c == bg) return d;
		}

		// check bottom line
		j=y+d;
		for (i=x-d; i<=x+d; ++i) {
			c = GetPixel(bmp, i,j);
			if (c <= 0 || c == bg) return d;
		}

		// check left side
		i=x-d;
		for (j=y-d; j<=y+d; ++j) {
			c = GetPixel(bmp, i,j);
			if (c <= 0 || c == bg) return d;
		}

		// check right side
		i=x+d;
		for (j=y-d; j<=y+d; ++j) {
			c = GetPixel(bmp, i,j);
			if (c <= 0 || c == bg) return d;
		}

		if (d >= t) {
			return d+1;
		}
	}
}


void MyFont::ApplyWave(BITMAP *bmp, int c1, int c2) {
	for (int x=0; x<bmp->w; ++x) {
		int h = SineWave::next();
		h = Clamp(h, 0, bmp->h);
		for (int y=0; y<h; ++y) {
			int p = _getpixel(bmp,x,y);
			if (p <= 0 || p == bg) {
				continue;
			}
			_putpixel(bmp,x,y,c1);
		}
		for (int y=h; y<bmp->h; ++y) {
			int p = _getpixel(bmp,x,y);
			if (p <= 0 || p == bg) {
				continue;
			}
			_putpixel(bmp,x,y,c2);
		}
	}
}


void MyFont::PushUp(int i) {
	BITMAP *bmp = glyphs[i];
	blit(bmp, bmp, 0, 1, 0, 0, bmp->w, bmp->h-1);
	hline(bmp, 0, bmp->h-1, bmp->w-1, bg);
}


void MyFont::PushDown(int i) {
	BITMAP *bmp = glyphs[i];
	blit(bmp, bmp, 0, 0, 0, 1, bmp->w, bmp->h-1);
	hline(bmp, 0, 0, bmp->w-1, bg);
}


void MyFont::PushLeft(int i) {
	BITMAP *bmp = glyphs[i];
	blit(bmp, bmp, 1, 0, 0, 0, bmp->w-1, bmp->h);
	vline(bmp, bmp->w-1, 0, bmp->h-1, bg);
}


void MyFont::PushRight(int i) {
	BITMAP *bmp = glyphs[i];
	blit(bmp, bmp, 0, 0, 1, 0, bmp->w-1, bmp->h);
	vline(bmp, 0, 0, bmp->h-1, bg);
}


BITMAP *MyFont::Cut(int i) {
	BITMAP *g = glyphs[i];
	BITMAP *ret = create_bitmap_ex(8, g->w, g->h);
	blit(g, ret, 0, 0, 0, 0, g->w, g->h);
	ClearGlyph(i);
	return ret;
}


BITMAP *MyFont::Copy(int i) {
	BITMAP *g = glyphs[i];
	BITMAP *ret = create_bitmap_ex(8, g->w, g->h);
	blit(g, ret, 0, 0, 0, 0, g->w, g->h);
	return ret;
}


void MyFont::Paste(BITMAP *bmp, int i) {
	GlyphResize(i, bmp->w, bmp->h);
	blit(bmp, glyphs[i], 0, 0, 0, 0, bmp->w, bmp->h);
}
