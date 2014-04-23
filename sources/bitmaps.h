#ifndef __BITMAPS_HEADER
#define __BITMAPS_HEADER

#ifndef BYTE
#define BYTE unsigned char
#endif

class Bitmap {
public:
	Bitmap() {image=NULL;ancho=0,alto=0;};
	Bitmap(char *nombre) {image=NULL;ancho=0,alto=0;B_loadpcx(nombre);};
	~Bitmap() {delete image;};

	bool B_loadpcx(char *nombre);

	void draw(int x,int y,int sx,int sy,BYTE *screen,int screen_x,int screen_y,int pitch);
	void draw_sprite(int x1,int y1,int spx,int spy,int x,int y,int sx,int sy,BYTE *screen,int screen_x,int screen_y,int pitch);
	void draw_sprite_cut(int x1,int y1,int spx,int spy,int x,int y,int sx,int sy,BYTE *screen,int screen_x,int screen_y,int pitch,int cut);
	void draw_sprite_mask(BYTE type,int x1,int y1,int spx,int spy,int x,int y,int sx,int sy,BYTE *screen,int screen_x,int screen_y,int pitch);
	int  coltest(int x1,int y1,int spx,int spy,int x,int y,int sx,int sy,BYTE *screen,int screen_x,int screen_y,int pitch);
	bool to_tile_colision(Bitmap *b,int x1,int y1,int x2,int y2,int dx1,int dy1,int dx2,int dy2,int offs1,int offs2);

	char *get_image() {return image;};
	char *get_palete() {return pal;};
	int get_ancho() {return ancho;};
	int get_alto() {return alto;};

private:
	char *image,pal[768];
	int ancho,alto;
	int type;
};


#endif

