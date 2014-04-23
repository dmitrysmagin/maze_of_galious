#include "stdio.h"
#include "stdlib.h"
#include "MOGtypes.h"

#include "bitmaps.h"
#include "filehandling.h"

bool Bitmap::B_loadpcx(char *nombre)
{
	FILE *fp;
	unsigned char byte;
	int a,b,c,columnas,filas,contador;
	int offset;

	fp=f1open(nombre,"rb",GAMEDATA);
	if (fp==NULL) {
		ancho=0;
		alto=0;
		image=NULL;
		return 1;
	} /* if */ 

	/* Cabecera: */ 
	if (fgetc(fp)!=10 || fgetc(fp)!=5 || fgetc(fp)!=1) return false;
	if (fgetc(fp)!=8) return false;

	/* Detecta la parte superior de la ventana(inservible!). */ 
	a=fgetc(fp);   
	b=fgetc(fp);
	ancho=a+b*256;
	a=fgetc(fp);
	b=fgetc(fp);
	alto=a+b*256;

	/* Ancho y alto: */ 
	a=fgetc(fp);   
	b=fgetc(fp);
	ancho=a+b*256+1;
	a=fgetc(fp);
	b=fgetc(fp);
	alto=a+b*256+1;
	columnas=ancho;
	filas=alto;

	/* Pasar el relleno de la cabecera PCX. */ 
	for(a=0;a<116;a++) fgetc(fp); 

	/* reservar el espacio requerido: */ 
	image=NULL;
	image=new char[ancho*alto];	
	if (image==NULL) {
		fclose(fp);
		return false;
	} /* if */ 

	/* Cargar imagen: */ 
	offset=0;
	while(offset<ancho*alto) {	
		columnas=ancho;
		while(columnas>0 && offset<ancho*alto) {
			byte=(unsigned char)fgetc(fp);
			if(byte<=192) {
//				if (columnas>1) 
					image[offset++]=byte;
				columnas--;
			} else {
				contador=byte&63; 
				byte=(unsigned char)fgetc(fp);
				for(; contador!=0 && offset<ancho*alto; contador--) {
//					if (columnas>1) 
						image[offset++]=byte; 
					columnas--;
				} /* for */ 
			} /* if */ 
		} /* while */ 
	} /* for */ 

	if (fgetc(fp)!=12) {
		fclose(fp);
//		delete image;
//		image=NULL;
		return false;
	} /* if */ 

	for(c=0;c<768;c++)
		pal[c]=((fgetc(fp))>>2);

	fclose(fp);

	return true;
} /* Bitmap::B_loadpcx */ 


void Bitmap::draw(int x,int y,int sx,int sy,BYTE *screen,int screen_x,int screen_y,int pitch)
{
	int i,j;
	int xt,yt;
	int y_error,x_error;
	int offso,offsd;

	if (image==NULL) return;

	if (sy>alto) {
		y_error=sy>>1;
		offso=0;
		offsd=y*pitch;
		yt=0;
		for(i=0;i<sy;i++) {
			if (i+y>=0 && i+y<screen_y && yt<alto) {
				/* Procesar la X: */ 
				if (sx>ancho) {
					x_error=sx>>1;
					xt=0;
					for(j=0;j<sx;j++) {
						if (j+x>=0 && j+x<screen_x && xt<ancho
							&& image[xt+offso]!=0) screen[x+j+offsd]=image[xt+offso];
						x_error-=ancho;
						if (x_error<0) {
							x_error+=sx;
							xt++;
						} /* if */ 
					} /* for */ 
				} else {
					x_error=ancho>>1;
					xt=0;
					for(j=0;j<ancho;j++) {
						if (xt+x>=0 && xt+x<screen_x
							&& image[j+offso]!=0) screen[x+xt+offsd]=image[j+offso];
						x_error-=sx;
						if (x_error<0) {
							x_error+=ancho;
							xt++;
						} /* if */ 
					} /* for */ 
				} /* if */ 
			} /* if */ 
			y_error-=alto;
			offsd+=pitch;
			if (y_error<0) {
				y_error+=sy;
				yt++;
				offso+=ancho;
			} /* if */ 
		} /* for */ 
	} else {
		y_error=alto>>2;
		offso=0;
		offsd=y*pitch;
		yt=0;
		for(i=0;i<alto;i++) {
			if (yt+y>=0 && yt+y<screen_y) {
				/* Procesar la X: */ 
				if (sx>ancho) {
					x_error=sx>>1;
					xt=0;
					for(j=0;j<sx;j++) {
						if (j+x>=0 && j+x<screen_x && xt<ancho
							&& image[xt+offso]!=0) screen[x+j+offsd]=image[xt+offso];
						x_error-=ancho;
						if (x_error<0) {
							x_error+=sx;
							xt++;
						} /* if */ 
					} /* for */ 
				} else {
					x_error=ancho>>1;
					xt=0;
					for(j=0;j<ancho;j++) {
						if (xt+x>=0 && xt+x<screen_x
							&& image[j+offso]!=0) screen[x+xt+offsd]=image[j+offso];
						x_error-=sx;
						if (x_error<0) {
							x_error+=ancho;
							xt++;
						} /* if */ 
					} /* for */ 
				} /* if */ 
			} /* if */ 
			y_error-=sy;
			offso+=ancho;
			if (y_error<0) {
				y_error+=alto;
				yt++;
				offsd+=pitch;
			} /* if */ 
		} /* for */ 
	} /* if */ 


} /* Bitmap::draw */ 



void Bitmap::draw_sprite(int x1,int y1,int spx,int spy,int x,int y,int sx,int sy,BYTE *screen,int screen_x,int screen_y,int pitch)
{
	int i,j;
	int xt,yt;
	int y_error,x_error;
	int offso,offsd;

	if (image==NULL) return;

	if (sy>spy) {
		y_error=sy>>1;
		offso=y1*ancho;
		offsd=y*pitch;
		yt=0;
		for(i=0;i<sy;i++) {
			if (i+y>=0 && i+y<screen_y && yt<spy) {
				/* Procesar la X: */ 
				if (sx>spx) {
					x_error=sx>>1;
					xt=x1;
					for(j=0;j<sx;j++) {
						if (j+x>=0 && j+x<screen_x && xt<ancho
							&& image[xt+offso]!=0) screen[x+j+offsd]=image[xt+offso];
						x_error-=spx;
						if (x_error<0) {
							x_error+=sx;
							xt++;
						} /* if */ 
					} /* for */ 
				} else {
					x_error=spx>>1;
					xt=0;
					for(j=0;j<spx;j++) {
						if (xt+x>=0 && xt+x<screen_x
							&& image[x1+j+offso]!=0) screen[x+xt+offsd]=image[x1+j+offso];
						x_error-=sx;
						if (x_error<0) {
							x_error+=spx;
							xt++;
						} /* if */ 
					} /* for */ 
				} /* if */ 
			} /* if */ 
			y_error-=spy;
			offsd+=pitch;
			if (y_error<0) {
				y_error+=sy;
				yt++;
				offso+=ancho;
			} /* if */ 
		} /* for */ 
	} else {
		y_error=spy>>2;
		offso=y1*ancho;
		offsd=y*pitch;
		yt=0;
		for(i=0;i<spy;i++) {
			if (yt+y>=0 && yt+y<screen_y) {
				/* Procesar la X: */ 
				if (sx>spx) {
					x_error=sx>>1;
					xt=x1;
					for(j=0;j<sx;j++) {
						if (j+x>=0 && j+x<screen_x && xt<ancho
							&& image[xt+offso]!=0) screen[x+j+offsd]=image[xt+offso];
						x_error-=spx;
						if (x_error<0) {
							x_error+=sx;
							xt++;
						} /* if */ 
					} /* for */ 
				} else {
					x_error=spx>>1;
					xt=0;
					for(j=0;j<spx;j++) {
						if (xt+x>=0 && xt+x<screen_x
							&& image[x1+j+offso]!=0) screen[x+xt+offsd]=image[x1+j+offso];
						x_error-=sx;
						if (x_error<0) {
							x_error+=spx;
							xt++;
						} /* if */ 
					} /* for */ 
				} /* if */ 
			} /* if */ 
			y_error-=sy;
			offso+=ancho;
			if (y_error<0) {
				y_error+=spy;
				yt++;
				offsd+=pitch;
			} /* if */ 
		} /* for */ 
	} /* if */ 

} /* Bitmap::draw_sprite */ 


void Bitmap::draw_sprite_cut(int x1,int y1,int spx,int spy,int x,int y,int sx,int sy,BYTE *screen,int screen_x,int screen_y,int pitch,int cut)
{
	int i,j;
	int xt,yt;
	int y_error,x_error;
	int offso,offsd;
	int v1=0,v2=0;

	if (cut<0) v1=-cut;
	if (cut>0) v2=cut;

	if (image==NULL) return;

	if (sy>spy) {
		y_error=sy>>1;
		offso=(y1+v2-v1)*ancho;
		offsd=y*pitch;
		yt=0;
		for(i=v1;i<sy-v2;i++) {
			if (i+y>=0 && i+y<screen_y && yt>=v1 && yt<(spy-v2)) {
				/* Procesar la X: */ 
				if (sx>spx) {
					x_error=sx>>1;
					xt=x1;
					for(j=0;j<sx;j++) {
						if (j+x>=0 && j+x<screen_x && xt<ancho
							&& image[xt+offso]!=0) screen[x+j+offsd]=image[xt+offso];
						x_error-=spx;
						if (x_error<0) {
							x_error+=sx;
							xt++;
						} /* if */ 
					} /* for */ 
				} else {
					x_error=spx>>1;
					xt=0;
					for(j=0;j<spx;j++) {
						if (xt+x>=0 && xt+x<screen_x
							&& image[x1+j+offso]!=0) screen[x+xt+offsd]=image[x1+j+offso];
						x_error-=sx;
						if (x_error<0) {
							x_error+=spx;
							xt++;
						} /* if */ 
					} /* for */ 
				} /* if */ 
			} /* if */ 
			y_error-=spy;
			offsd+=pitch;
			if (y_error<0) {
				y_error+=sy;
				yt++;
				offso+=ancho;
			} /* if */ 
		} /* for */ 
	} else {
		y_error=spy>>2;
		offso=(y1+v2-v1)*ancho;
		offsd=y*pitch;
		yt=0;
		for(i=0;i<spy;i++) {
			if (yt+y>=0 && yt+y<screen_y && yt>=v1 && yt<(spy-v2)) {
				/* Procesar la X: */ 
				if (sx>spx) {
					x_error=sx>>1;
					xt=x1;
					for(j=0;j<sx;j++) {
						if (j+x>=0 && j+x<screen_x && xt<ancho
							&& image[xt+offso]!=0) screen[x+j+offsd]=image[xt+offso];
						x_error-=spx;
						if (x_error<0) {
							x_error+=sx;
							xt++;
						} /* if */ 
					} /* for */ 
				} else {
					x_error=spx>>1;
					xt=0;
					for(j=0;j<spx;j++) {
						if (xt+x>=0 && xt+x<screen_x
							&& image[x1+j+offso]!=0) screen[x+xt+offsd]=image[x1+j+offso];
						x_error-=sx;
						if (x_error<0) {
							x_error+=spx;
							xt++;
						} /* if */ 
					} /* for */ 
				} /* if */ 
			} /* if */ 
			y_error-=sy;
			offso+=ancho;
			if (y_error<0) {
				y_error+=spy;
				yt++;
				offsd+=pitch;
			} /* if */ 
		} /* for */ 
	} /* if */ 

} /* Bitmap::draw_sprite_cut */ 


void Bitmap::draw_sprite_mask(BYTE type,int x1,int y1,int spx,int spy,int x,int y,int sx,int sy,BYTE *screen,int screen_x,int screen_y,int pitch)
{
	int i,j;
	int xt,yt;
	int y_error,x_error;
	int offso,offsd;

	if (image==NULL) return;

	if (sy>spy) {
		y_error=sy>>1;
		offso=y1*ancho;
		offsd=y*pitch;
		yt=0;
		for(i=0;i<sy;i++) {
			if (i+y>=0 && i+y<screen_y && yt<spy) {
				/* Procesar la X: */ 
				if (sx>spx) {
					x_error=sx>>1;
					xt=x1;
					for(j=0;j<sx;j++) {
						if (j+x>=0 && j+x<screen_x && xt<ancho
							&& image[xt+offso]!=0) screen[x+j+offsd]|=type;
						x_error-=spx;
						if (x_error<0) {
							x_error+=sx;
							xt++;
						} /* if */ 
					} /* for */ 
				} else {
					x_error=spx>>1;
					xt=0;
					for(j=0;j<spx;j++) {
						if (xt+x>=0 && xt+x<screen_x
							&& image[x1+j+offso]!=0) screen[x+xt+offsd]|=type;
						x_error-=sx;
						if (x_error<0) {
							x_error+=spx;
							xt++;
						} /* if */ 
					} /* for */ 
				} /* if */ 
			} /* if */ 
			y_error-=spy;
			offsd+=pitch;
			if (y_error<0) {
				y_error+=sy;
				yt++;
				offso+=ancho;
			} /* if */ 
		} /* for */ 
	} else {
		y_error=spy>>2;
		offso=y1*ancho;
		offsd=y*pitch;
		yt=0;
		for(i=0;i<spy;i++) {
			if (yt+y>=0 && yt+y<screen_y) {
				/* Procesar la X: */ 
				if (sx>spx) {
					x_error=sx>>1;
					xt=x1;
					for(j=0;j<sx;j++) {
						if (j+x>=0 && j+x<screen_x && xt<ancho
							&& image[xt+offso]!=0) screen[x+j+offsd]|=type;
						x_error-=spx;
						if (x_error<0) {
							x_error+=sx;
							xt++;
						} /* if */ 
					} /* for */ 
				} else {
					x_error=spx>>1;
					xt=0;
					for(j=0;j<spx;j++) {
						if (xt+x>=0 && xt+x<screen_x
							&& image[x1+j+offso]!=0) screen[x+xt+offsd]|=type;
						x_error-=sx;
						if (x_error<0) {
							x_error+=spx;
							xt++;
						} /* if */ 
					} /* for */ 
				} /* if */ 
			} /* if */ 
			y_error-=sy;
			offso+=ancho;
			if (y_error<0) {
				y_error+=spy;
				yt++;
				offsd+=pitch;
			} /* if */ 
		} /* for */ 
	} /* if */ 

} /* Bitmap::draw_sprite_mask */ 


int Bitmap::coltest(int x1,int y1,int spx,int spy,int x,int y,int sx,int sy,BYTE *screen,int screen_x,int screen_y,int pitch)
{
	int i,j;
	int xt,yt;
	int y_error,x_error;
	int offso,offsd;
	int retval=0;

	if (image==NULL) return retval;

	if (sy>spy) {
		y_error=sy>>1;
		offso=y1*ancho;
		offsd=y*pitch;
		yt=0;
		for(i=0;i<sy;i++) {
			if (i+y>=0 && i+y<screen_y && yt<spy) {
				/* Procesar la X: */ 
				if (sx>spx) {
					x_error=sx>>1;
					xt=x1;
					for(j=0;j<sx;j++) {
						if (j+x>=0 && j+x<screen_x && xt<ancho
							&& image[xt+offso]!=0) retval|=screen[x+j+offsd];
						x_error-=spx;
						if (x_error<0) {
							x_error+=sx;
							xt++;
						} /* if */ 
					} /* for */ 
				} else {
					x_error=spx>>1;
					xt=0;
					for(j=0;j<spx;j++) {
						if (xt+x>=0 && xt+x<screen_x
							&& image[x1+j+offso]!=0) retval|=screen[x+xt+offsd];
						x_error-=sx;
						if (x_error<0) {
							x_error+=spx;
							xt++;
						} /* if */ 
					} /* for */ 
				} /* if */ 
			} /* if */ 
			y_error-=spy;
			offsd+=pitch;
			if (y_error<0) {
				y_error+=sy;
				yt++;
				offso+=ancho;
			} /* if */ 
		} /* for */ 
	} else {
		y_error=spy>>2;
		offso=y1*ancho;
		offsd=y*pitch;
		yt=0;
		for(i=0;i<spy;i++) {
			if (yt+y>=0 && yt+y<screen_y) {
				/* Procesar la X: */ 
				if (sx>spx) {
					x_error=sx>>1;
					xt=x1;
					for(j=0;j<sx;j++) {
						if (j+x>=0 && j+x<screen_x && xt<ancho
							&& image[xt+offso]!=0) retval|=screen[x+j+offsd];
						x_error-=spx;
						if (x_error<0) {
							x_error+=sx;
							xt++;
						} /* if */ 
					} /* for */ 
				} else {
					x_error=spx>>1;
					xt=0;
					for(j=0;j<spx;j++) {
						if (xt+x>=0 && xt+x<screen_x
							&& image[x1+j+offso]!=0) retval|=screen[x+xt+offsd];
						x_error-=sx;
						if (x_error<0) {
							x_error+=spx;
							xt++;
						} /* if */ 
					} /* for */ 
				} /* if */ 
			} /* if */ 
			y_error-=sy;
			offso+=ancho;
			if (y_error<0) {
				y_error+=spy;
				yt++;
				offsd+=pitch;
			} /* if */ 
		} /* for */ 
	} /* if */ 

	return retval;
} /* Bitmap::coltest */ 


bool Bitmap::to_tile_colision(Bitmap *b,int x1,int y1,int x2,int y2,int dx1,int dy1,int dx2,int dy2,int offs1,int offs2)
{
	int i=0,j=0;
	int dx,dy;
	int xt,yt;
	
	dx=x1-x2;
	dy=y1-y2;

	for(j=0;j<dy1;j++) {
		for(i=0;i<dx1;i++) {
			if (image[offs1+i+j*ancho]!=0) {
				xt=i+dx;
				yt=j+dy;
				if (xt>=0 && xt<dx2 &&
					yt>=0 && yt<dy2) {
					if (b->image[offs2+xt+yt*b->ancho]!=0) {
						return true;
					} /* if */ 
				} /* if */ 
			} /* if */ 
		} /* for */ 
	} /* for */ 

	return false;
} /* ti_tile_colision */ 
