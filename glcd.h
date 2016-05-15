#define LCDWIDTH 128
#define LCDHEIGHT 64

#define swap(a, b) { unsigned char t = a; a = b; b = t; }
void setpixel(unsigned int *buff, unsigned int x, unsigned int y, unsigned int color);
//void setpixel(unsigned char *buff, unsigned char x, unsigned char y, unsigned char color);

void drawrect(unsigned char *buff,
	      unsigned char x, unsigned char y, unsigned char w, unsigned char h,
	      unsigned char color);
//void drawline(unsigned char *buff,
//	      unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1,
//	      unsigned char color);

	      void drawline(unsigned int *buff,
	      unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1,
	      unsigned int color);

void drawbitmap(unsigned char *buff, unsigned char x, unsigned char y,
		const unsigned char bitmap, unsigned char w, unsigned char h,
		unsigned char color);
void drawchar(unsigned char *buff, unsigned char x, unsigned char line, unsigned char c);
void drawcircle(unsigned char *buff,
	      unsigned char x0, unsigned char y0, unsigned char r,
		unsigned char color);

void fillrect(unsigned char *buff,
	      unsigned char x, unsigned char y, unsigned char w, unsigned char h,
	      unsigned char color);

void fillcircle(unsigned char *buff,
	      unsigned char x0, unsigned char y0, unsigned char r,
	      unsigned char color);

	      void drawcircle(unsigned char *buff,
	      unsigned char x0, unsigned char y0, unsigned char r,
	      unsigned char color);

void drawchar(unsigned char *buff, unsigned char x, unsigned char line, unsigned char c);

void drawstring(unsigned char *buff, unsigned char x, unsigned char line, unsigned char *c);
