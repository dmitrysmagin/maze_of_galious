#include "math.h"
#include "stdio.h"

bool line_clip(int &x1,int &y1,int &x2,int &y2,int min_x,int min_y,int max_x,int max_y)
{
	int dx=x2-x1,dy=y2-y1;
	float f;

	/* Recortar por x_min: */ 
	if (x1<min_x) {
		if (x2<min_x) {
			return false;
		} else {
			/* Buscar el punto de corte: */ 
			f=float(dy)/float(dx);
			x1=min_x;
			y1=int(f*min_x+(y1-x1*f));
		} /* if */ 
	} else {
		if (x2<min_x) {
			/* Buscar el punto de corte: */ 
			f=float(dy)/float(dx);
			x2=min_x;
			y2=int(f*min_x+(y1-x1*f));
		} else {
			/* Nada que hacer... */ 
		} /* if */ 
	} /* if */ 

	/* Recortar por x_max: */ 
	if (x1>=max_x) {
		if (x2>=max_x) {
			return false;
		} else {
			/* Buscar el punto de corte: */ 
			f=float(dy)/float(dx);
			x1=max_x-1;
			y1=int(f*(max_x-1)+(y1-x1*f));
		} /* if */ 
	} else {
		if (x2>max_x) {
			/* Buscar el punto de corte: */ 
			f=float(dy)/float(dx);
			x2=max_x-1;
			y2=int(f*(max_x-1)+(y1-x1*f));
		} else {
			/* Nada que hacer... */ 
		} /* if */ 
	} /* if */ 

	/* Recortar por y_min: */ 
	if (y1<min_y) {
		if (y2<min_y) {
			return false;
		} else {
			/* Buscar el punto de corte: */ 
			f=float(dx)/float(dy);
			y1=min_y;
			x1=int(f*min_y+(x1-y1*f));
		} /* if */ 
	} else {
		if (y2<min_y) {
			/* Buscar el punto de corte: */ 
			f=float(dx)/float(dy);
			y2=min_y;
			x2=int(f*min_y+(x1-y1*f));
		} else {
			/* Nada que hacer... */ 
		} /* if */ 
	} /* if */ 

	/* Recortar por y_max: */ 
	if (y1>=max_y) {
		if (y2>=max_y) {
			return false;
		} else {
			/* Buscar el punto de corte: */ 
			f=float(dx)/float(dy);
			y1=max_y-1;
			x1=int(f*(max_y-1)+(x1-y1*f));
		} /* if */ 
	} else {
		if (y2>=max_y) {
			/* Buscar el punto de corte: */ 
			f=float(dx)/float(dy);
			y2=max_y-1;
			x2=int(f*(max_y-1)+(x1-y1*f));
		} else {
			/* Nada que hacer... */ 
		} /* if */ 
	} /* if */ 

	if (x1<min_x) x1=min_x;
	if (x2<min_x) x2=min_x;

	if (x1>=max_x) x1=max_x-1;
	if (x2>=max_x) x2=max_x-1;

	if (y1<min_y) y1=min_y;
	if (y2<min_y) y2=min_y;

	if (y1>=max_y) y1=max_y-1;
	if (y2>=max_y) y2=max_y-1;

	return true;
} /* line_clip */ 

