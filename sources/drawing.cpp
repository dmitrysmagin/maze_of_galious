#include "string.h"

#include "MOGtypes.h"
#include "drawing.h"
#include "line_clip.h"

#define MAX_POINTS	8

extern int SCREEN_X;
extern int SCREEN_Y;

void PutPixel8B(BYTE *video,int x,int y,long pitch,BYTE color)
{
	video[x + y*pitch]=color;
} /* PutPixel6B */ 

void Line8B(BYTE *video,int x1,int y1,int x2,int y2,long pitch,BYTE color)
{
  long incy,rincy,incx,errterm,a;
  long d_x,d_y;
  BYTE *ptr;

  if (!line_clip(x1,y1,x2,y2,0,0,SCREEN_X,SCREEN_Y)) return;

  ptr=video+x1+y1*pitch;
  errterm=0;
  d_x=x2-x1;
  d_y=y2-y1;

  if (d_y<0) {
     incy=-pitch;
     rincy=-1;
     d_y=-d_y;
     } else {
     incy=pitch;
     rincy=1;
     } /* if */ 
  if (d_x<0) {
     incx=-1;
     d_x=-d_x;
     } else {
     incx=1;
     } /* if */ 

  if (d_x>d_y) {
     /* xline */ 
     for(a=0;a<=d_x;a++) {
	   *ptr=color;
       errterm+=d_y;
       if (errterm>=d_x) {
          errterm-=d_x;
          ptr+=incy;
          } /* if */ 
       ptr+=incx;
       } /* while */ 
     } else {
     /* yline */ 
     for(a=0;a<=d_y;a++) {
	   *ptr=color;
       errterm+=d_x;
       if (errterm>=d_y) {
          errterm-=d_y;
          ptr+=incx;
          } /* if */ 
       ptr+=incy;
       } /* while */ 
     } /* if */ 

} /* Line8B */ 


void Line8B_transp(BYTE *video,int x1,int y1,int x2,int y2,long pitch,BYTE color)
{
  long incy,rincy,incx,errterm,a;
  long d_x,d_y;
  BYTE *ptr;

  if (!line_clip(x1,y1,x2,y2,0,0,SCREEN_X,SCREEN_Y)) return;

  ptr=video+x1+y1*pitch;
  errterm=0;
  d_x=x2-x1;
  d_y=y2-y1;

  if (d_y<0) {
     incy=-pitch;
     rincy=-1;
     d_y=-d_y;
     } else {
     incy=pitch;
     rincy=1;
     } /* if */ 
  if (d_x<0) {
     incx=-1;
     d_x=-d_x;
     } else {
     incx=1;
     } /* if */ 

  if (d_x>d_y) {
     /* xline */ 
     for(a=0;a<d_x;a++) {
	   *ptr=(color+*ptr)>>1;
       errterm+=d_y;
       if (errterm>=d_x) {
          errterm-=d_x;
          ptr+=incy;
          } /* if */ 
       ptr+=incx;
       } /* while */ 
     } else {
     /* yline */ 
     for(a=0;a<d_y;a++) {
	   *ptr=(color+*ptr)>>1;
       errterm+=d_x;
       if (errterm>=d_y) {
          errterm-=d_y;
          ptr+=incx;
          } /* if */ 
       ptr+=incy;
       } /* while */ 
     } /* if */ 

} /* Line8B_transp */ 

int line_mean(int x1,int y1,int x2,int y2,BYTE *orig,long pitch)
{
  long incy,errterm,a;
  long incx;
  long d_x,d_y;
  BYTE *ptr;
  int color=0;

  ptr=orig+x1+y1*pitch;
  errterm=0;
  d_x=x2-x1;
  d_y=y2-y1;

  if (d_y<0) {
     incy=-pitch;
     d_y=-d_y;
     } else {
     incy=pitch;
     } /* if */ 
  if (d_x<0) {
     d_x=-d_x;
	 incx=-1;
	 } else {
	  incx=1;
	 } /* if */ 

  if (d_x>d_y) {
	 /* xline */ 
//		 d_x=min(d_x,MAX_POINTS);
	 for(a=d_x;a;a--) {
	   color+=*ptr;
	   errterm+=d_y;
	   if (errterm>=d_x) {
		  errterm-=d_x;
		  ptr+=incy;
		  } /* if */ 
	   ptr+=incx;
	   } /* while */ 

	 return color/(d_x+1);
	 } else {
	 /* yline */ 
//		 d_y=min(d_y,MAX_POINTS);
	 for(a=d_y;a;a--) {
	   color+=*ptr;
	   errterm+=d_x;
	   if (errterm>=d_y) {
		  errterm-=d_y;
		  ptr+=incx;
		  } /* if */ 
	   ptr+=incy;
	   } /* while */ 

	 return color/(d_y+1);
	 } /* if */ 

} /* line_mean */ 


void B_polygon(int *xp,int *yp,int nump,BYTE col,BYTE *dir,int pitch)
{
int ymin=200,vs1=0,vs2,ve1,ve2,a;
int xs1,xs2,ys1,ys2;
int xe1,xe2,ye1,ye2;
long int errt1,errt2,ydif1,ydif2,xdif1,xdif2,xunit1,xunit2;
int np1,np2,len;
long int start;
unsigned int desp1;
unsigned int desp2;

if (nump<3) return;
for(a=0;a<nump;a++)
	if (yp[a]<ymin) {
		vs1=a;
		ymin=yp[a];
		}
vs2=vs1;
ve1=vs1-1;
ve2=vs2+1;
if (ve1<0) ve1=nump-1;
if (ve2==nump) ve2=0;
xs1=xp[vs1];
ys1=yp[vs1];
xs2=xp[vs2];
ys2=yp[vs2];
xe1=xp[ve1];
ye1=yp[ve1];
xe2=xp[ve2];
ye2=yp[ve2];
a=nump-1;
while(a>0) {
	if (ys1>=SCREEN_Y || ys2>=SCREEN_Y) return;
	desp1=pitch*ys1+xs1;
	desp2=pitch*ys2+xs2;
	errt1=0;
	errt2=0;
	xunit1=1;
	xunit2=1;
	if ((ydif1=ye1-ys1)<0) ydif1=-ydif1;
	if ((ydif2=ye2-ys2)<0) ydif2=-ydif2;
	if ((xdif1=xe1-xs1)<0) {
		xdif1=-xdif1;
		xunit1=-1;
		}
	if ((xdif2=xe2-xs2)<0) {
		xdif2=-xdif2;
		xunit2=-1;
		}

	if (xdif1>ydif1) {
		np1=xdif1;
		if (xdif2>ydif2) {
		// CASO 1
		np2=xdif2;
		while(np1 && np2) {
		while((errt1<xdif1)&&(np1>0)) {
		if (np1--) {
			desp1+=xunit1;
			xs1+=xunit1;
			}
		errt1+=ydif1;
		if (errt1>xdif1) dir[desp1]=col;
		}
		errt1-=xdif1;

		while((errt2<xdif2)&&(np2>0)) {
		if (np2--) {
			desp2+=xunit2;
			xs2+=xunit2;
			}
		errt2+=ydif2;
		if (errt2>xdif2) dir[desp2]=col;
		}
		errt2-=xdif2;

		len=desp2-desp1;
		if (len<0) {
			len=-len;
			start=(long int)dir+desp2;
			} else start=(long int)dir+desp1;
		len++;

		memset((void *)start,col,len);

		desp1+=pitch;
		ys1++;
		desp2+=pitch;
		ys2++;
		}
		} else {
		// CASO 2
		np2=ydif2;
		while(np1 && np2) {
		while((errt1<xdif1)&&(np1>0)) {
		if (np1--) {
			desp1+=xunit1;
			xs1+=xunit1;
			}
		errt1+=ydif1;
		if (errt1>xdif1) dir[desp1]=col;
		}
		errt1-=xdif1;

		errt2+=xdif2;
		if (errt2>=ydif2) {
			errt2-=ydif2;
			desp2+=xunit2;
			xs2+=xunit2;
			}
		np2--;

		len=desp2-desp1;
		if (len<0) {
			len=-len;
			start=(long int)dir+desp2;
			} else start=(long int)dir+desp1;
		len++;

		memset((void *)start,col,len);

		desp1+=pitch;
		ys1++;
		desp2+=pitch;
		ys2++;
		}
		}
		} else {
		np1=ydif1;
		if (xdif2>ydif2) {
		np2=xdif2;
		// CASO 3
		while(np1 && np2) {
		errt1+=xdif1;
		if (errt1>=ydif1) {
			errt1-=ydif1;
			desp1+=xunit1;
			xs1+=xunit1;
			}
		np1--;

		while((errt2<xdif2)&&(np2>0)) {
		if (np2--) {
			desp2+=xunit2;
			xs2+=xunit2;
			}
		errt2+=ydif2;
		if (errt2>xdif2) dir[desp2]=col;
		}
		errt2-=xdif2;

		len=desp2-desp1;
		if (len<0) {
			len=-len;
			start=(long int)dir+desp2;
			} else start=(long int)dir+desp1;
		len++;

		memset((void *)start,col,len);

		desp1+=pitch;
		ys1++;
		desp2+=pitch;
		ys2++;
		}
	} else {
		np2=ydif2;
		// CASO 4
		while(np1 && np2) {
		errt1+=xdif1;
		if (errt1>=ydif1) {
			errt1-=ydif1;
			desp1+=xunit1;
			xs1+=xunit1;
			}
		np1--;

		errt2+=xdif2;
		if (errt2>=ydif2) {
			errt2-=ydif2;
			desp2+=xunit2;
			xs2+=xunit2;
			}
		np2--;

		len=desp2-desp1;
		if (len<0) {
			len=-len;
			start=(long int)dir+desp2;
			} else start=(long int)dir+desp1;
		len++;

		memset((void *)start,col,len);

		desp1+=pitch;
		ys1++;
		desp2+=pitch;
		ys2++;
		}
		}
		}
	if (!np1) {
		a--;
		vs1=ve1;
		ve1--;
		if (ve1<0) ve1=nump-1;
		xe1=xp[ve1];
		ye1=yp[ve1];
		}
	if (!np2) {
		a--;
		vs2=ve2;
		ve2++;
		if (ve2==nump) ve2=0;
		xe2=xp[ve2];
		ye2=yp[ve2];
		}
	} /* while(a>0) */ 
} /* B_polygon */ 


void B_rectangle(BYTE *screen,int x,int y,int dx,int dy,int pitch,BYTE color)
{
	int i;

	for(i=0;i<dy;i++) {
		memset(screen+x+(y+i)*pitch,color,dx);
	} /* for */ 
} /* B_rectangle */ 


void B_rectangle2(BYTE *screen,int x,int y,int dx,int dy,int pitch,BYTE color)
{
	Line8B(screen,x,y,x+dx,y,pitch,color);
	Line8B(screen,x+dx,y,x+dx,y+dy,pitch,color);
	Line8B(screen,x+dx,y+dy,x,y+dy,pitch,color);
	Line8B(screen,x,y+dy,x,y,pitch,color);
} /* B_rectangle2 */ 
