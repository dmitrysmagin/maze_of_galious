#ifndef __SPRITES_HEADER
#define __SPRITES_HEADER

class CSprite {
public:
	CSprite(Bitmap *src,int x,int y,int dx,int dy,int x_offs=0,int y_offs=0) {m_src=src;m_x=x;m_y=y;m_dx=dx;m_dy=dy;m_xoffs=x_offs;m_yoffs=y_offs;};
	
	void draw(int x,int y,int sx,int sy,BYTE *screen,int screen_x,int screen_y,int pitch)
	{
		m_src->draw_sprite(m_x,m_y,m_dx,m_dy,x-m_xoffs,y-m_yoffs,sx,sy,screen,screen_x,screen_y,pitch);
	}; /* draw */ 

	void draw(int x,int y,BYTE *screen,int screen_x,int screen_y,int pitch)
	{
		m_src->draw_sprite(m_x,m_y,m_dx,m_dy,x-m_xoffs,y-m_yoffs,m_dx,m_dy,screen,screen_x,screen_y,pitch);
	}; /* draw */ 

	void draw_cut(int x,int y,int sx,int sy,BYTE *screen,int screen_x,int screen_y,int pitch,int cut)
	{
		m_src->draw_sprite_cut(m_x,m_y,m_dx,m_dy,x-m_xoffs,y-m_yoffs,sx,sy,screen,screen_x,screen_y,pitch,cut);
	}; /* draw */ 

	void drawmask(BYTE type,int x,int y,int sx,int sy,BYTE *screen,int screen_x,int screen_y,int pitch)
	{
		m_src->draw_sprite_mask(type,m_x,m_y,m_dx,m_dy,x-m_xoffs,y-m_yoffs,sx,sy,screen,screen_x,screen_y,pitch);
	}; /* drawmask */ 

	void drawmask(BYTE type,int x,int y,BYTE *screen,int screen_x,int screen_y,int pitch)
	{
		m_src->draw_sprite_mask(type,m_x,m_y,m_dx,m_dy,x-m_xoffs,y-m_yoffs,m_dx,m_dy,screen,screen_x,screen_y,pitch);
	}; /* drawmask */ 

	int coltest(int x,int y,int sx,int sy,BYTE *colbuffer,int screen_x,int screen_y,int pitch) {
		return m_src->coltest(m_x,m_y,m_dx,m_dy,x-m_xoffs,y-m_yoffs,sx,sy,colbuffer,screen_x,screen_y,pitch);
	}; /* coltest */ 

	int coltest(int x,int y,BYTE *colbuffer,int screen_x,int screen_y,int pitch) {
		return m_src->coltest(m_x,m_y,m_dx,m_dy,x-m_xoffs,y-m_yoffs,m_dx,m_dy,colbuffer,screen_x,screen_y,pitch);
	}; /* coltest */ 
	
	bool to_tile_colision(CSprite *s,int x1,int y1,int x2,int y2) {
		return m_src->to_tile_colision(s->m_src,x1-m_xoffs,y1-m_yoffs,x2-s->m_xoffs,y2-s->m_yoffs,m_dx,m_dy,s->m_dx,s->m_dy,m_x+m_y*m_src->get_ancho(),s->m_x+s->m_y*s->m_src->get_ancho());
	} /* to_tile_colision */ 

	Bitmap *m_src;
	int m_x,m_y;
	int m_dx,m_dy;
	int m_xoffs,m_yoffs;
};

#endif

