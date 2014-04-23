#ifndef __MOG_OBJECT
#define __MOG_OBJECT

class MOGObject {
public:
	int type;
	int enemy;

	int x,y;
	int xoffs,yoffs;
	int base_tile;
	int tile;

	int size;
	int state,state2,state3;
	int last_hit;
	int desired_floor;
	int in_ladder;

	int union_x,union_y;	/* just for the Butterflys */ 

	int experience;

	int draw_x,draw_y;		/* The object's position in the moment of drawing */ 

	int lightning[6];

private:

};

#endif


