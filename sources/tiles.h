#ifndef __TILES_HEADER
#define __TILES_HEADER

#ifndef BYTE
#define BYTE unsigned char
#endif

class CTile {
public:
	CTile(Bitmap *src,int x,int y,int dx,int dy,int type,int x_offs=0,int y_offs=0) {m_sp=new CSprite(src,x,y,dx,dy,x_offs,y_offs);m_type=type;};
	~CTile() {delete m_sp;};

	void draw(int x,int y,int sx,int sy,BYTE *screen,BYTE *colbuffer,int screen_x,int screen_y,int pitch) {
		m_sp->draw(x,y,sx,sy,screen,screen_x,screen_y,pitch);
		m_sp->drawmask(m_type,x,y,sx,sy,colbuffer,screen_x,screen_y,pitch);
	} /* draw */ 

	void draw(int x,int y,BYTE *screen,BYTE *colbuffer,int screen_x,int screen_y,int pitch) {
		m_sp->draw(x,y,screen,screen_x,screen_y,pitch);
		m_sp->drawmask(m_type,x,y,colbuffer,screen_x,screen_y,pitch);
	} /* draw */ 

	void draw_cut(int x,int y,int sx,int sy,BYTE *screen,BYTE *colbuffer,int screen_x,int screen_y,int pitch,int cut) {
		m_sp->draw_cut(x,y,sx,sy,screen,screen_x,screen_y,pitch,cut);
	} /* draw */ 

	void draw_only_mask(int x,int y,int sx,int sy,BYTE *colbuffer,int screen_x,int screen_y,int pitch) {
		m_sp->drawmask(m_type,x,y,sx,sy,colbuffer,screen_x,screen_y,pitch);
	} /* draw */ 

	void draw_only_mask(int x,int y,BYTE *colbuffer,int screen_x,int screen_y,int pitch) {
		m_sp->drawmask(m_type,x,y,colbuffer,screen_x,screen_y,pitch);
	} /* draw */ 

	void draw_without_mask(int x,int y,int sx,int sy,BYTE *screen,int screen_x,int screen_y,int pitch) {
		m_sp->draw(x,y,sx,sy,screen,screen_x,screen_y,pitch);
	} /* draw */ 

	void draw_without_mask(int x,int y,BYTE *screen,int screen_x,int screen_y,int pitch) {
		m_sp->draw(x,y,screen,screen_x,screen_y,pitch);
	} /* draw */ 

	int coltest(int x,int y,int sx,int sy,BYTE *colbuffer,int screen_x,int screen_y,int pitch) {
		return m_sp->coltest(x,y,sx,sy,colbuffer,screen_x,screen_y,pitch);
	} /* coltest */ 

	int coltest(int x,int y,BYTE *colbuffer,int screen_x,int screen_y,int pitch) {
		return m_sp->coltest(x,y,colbuffer,screen_x,screen_y,pitch);
	} /* coltest */ 

	bool to_tile_colision(CTile *t,int x1,int y1,int x2,int y2) {
		return m_sp->to_tile_colision(t->m_sp,x1,y1,x2,y2);
	} /* to_tile_colision */ 

	CSprite *m_sp;
	int m_type;
};

#endif
