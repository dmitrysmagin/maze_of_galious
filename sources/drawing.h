#ifndef __DRAWING_HEADER
#define __DRAWING_HEADER

void PutPixel8B(BYTE *video,int x,int y,long pitch,BYTE color);
void Line8B(BYTE *video,int x1,int y1,int x2,int y2,long pitch,BYTE color);
void Line8B_transp(BYTE *video,int x1,int y1,int x2,int y2,long pitch,BYTE color);
int  line_mean(int x1,int y1,int x2,int y2,BYTE *orig,long pitch);
void B_polygon(int *xp,int *yp,int nump,BYTE col,BYTE *dir,int pitch);

void GausianDot(BYTE *screen,int x,int y,int pitch,int radius,BYTE color);
void Circle(BYTE *screen,int x,int y,int pitch,int radius,BYTE color);
void FillCircle(BYTE *screen,int x,int y,int pitch,int radius,BYTE color);
void B_rectangle(BYTE *screen,int x,int y,int dx,int dy,int pitch,BYTE color);
void B_rectangle2(BYTE *screen,int x,int y,int dx,int dy,int pitch,BYTE color);

#endif

