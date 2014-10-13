#include "stdafx.h"

extern double g,length,board_length,speed,spring_length,least,hat_length,rocket_length,shoes_length,monster_length;
extern char msg[20];
extern player doodle;
extern Monster *monster;
extern double total_up;
extern HTEXTURE ht_monster;
extern hgeQuad quad_item,quad_monster;
extern bool dir_shoot;

void player::go_x(double dt)
{
	x+=vx*dt;
	if (x>LENGTH-length/2)
		x=-length/2;
	if (x<-length/2)
		x=LENGTH-length/2;
}
void player::go_y(double dt,bool c)
{
	if (c) y+=vy*dt;
	else y-=vy*dt;
}
void player::go_vx(bool _dir,double dt)
{
	dir=_dir;
	if (dir)
	{
		if (vx<=0) vx+=3*speed*dt;
		vx+=speed*dt;
	}
	else
	{
		if (vx>=0) vx-=3*speed*dt;
		vx-=speed*dt;
	}
}
void player::go_vy(double dt,bool c)
{
	if (c==true) vy+=g*dt;
	else vy-=g*dt;
}
void player::reset()
{
	vx=0;
	if (spring || shoes)
		vy=1400;
	else if (hat)
		vy=6000;
	else if (rocket)
		vy=13000;
	else
		vy=600;
	jump_y=least;
}
void player::init_player()
{
	x=160;y=500;vx=0;vy=0.7;dir=true;
	shoes=hat=spring=rocket=move=false;
}
void player::click()
{
	jump_y=y;
	if (jump_y<least)
	{
		total_up=least-jump_y;
		move=true;
	}
}
void player::set_quad(hgeQuad* quad)
{
	set_quad_xy(quad,x,y-32,x+32,y);
}
void player::set_state(int _id)
{
	if (_id==0)
	{
		spring=hat=rocket=false;
		if (shoes)
			shoes_time--;
		if (shoes_time==0)
			shoes=false;
		vy=0;
	}
	switch (_id)
	{
	case 1:
		move=spring=true;
		break;
	case 2:
		move=hat=true;
		shoes=false;
		shoes_time=0;
		break;
	case 3:
		move=rocket=true;
		shoes=false;
		shoes_time=0;
		break;
	case 4:
		move=shoes=true;
		shoes_time=9;
	}
}
int player::get_state()
{
	if (spring)
		return 1;
	if (hat)
		return 2;
	if (rocket)
		return 3;
	if (shoes)
		return 4;
	return 0;
}
int player::print_item()
{
	if (hat)
	{
		set_quad_txy(&quad_item,223.0/610.0,278.0/512.0,249.0/610.0,299.0/512.0);
		if (dir)
			set_quad_xy(&quad_item,x-1,y+1-21-26,x+hat_length-1,y+1-26);
		else
			set_quad_xy(&quad_item,x+8,y+1-21-26,x+8+hat_length,y+1-26);
		return 2;
	}
	if (rocket)
	{
		set_quad_txy(&quad_item,197.0/610.0,264.0/512.0,222.0/610.0,301.0/512.0);
		if (dir)
			set_quad_xy(&quad_item,x-10,y-33,x+15,y-1);
		else
			set_quad_xy(&quad_item,x+16,y-33,x+41,y-1);
		return 3;
	}
	if (shoes)
	{
		set_quad_txy(&quad_item,299.0/610.0,202.0/512.0,329.0/610.0,226.0/512.0);
		if (dir)
			set_quad_xy(&quad_item,x-3,y-5,x-3+shoes_length,y+19);
		else
			set_quad_xy(&quad_item,x+4,y-5,x+4+shoes_length,y+19);
		return 4;
	}
	return 0;
}
bool board::is_click()
{
	return (doodle.y-get_y()>=0 && doodle.y-get_y()<=10 && doodle.x>=x-length && doodle.x<=x+board_length);
}
int greenboard::get_state()
{
	if (spring && (doodle.x>=x+get_left()-length && doodle.x<=x+get_left()+spring_length))
		return 1;
	if (hat && (doodle.x>=x+get_left()-length && doodle.x<=x+get_left()+hat_length))
	{
		destory_item(2);
		return 2;
	}
	if (rocket && (doodle.x>=x+get_left()-length && doodle.x<=x+get_left()+rocket_length))
	{
		destory_item(3);
		return 3;
	}
	if (shoes && (doodle.x>=x+get_left()-length && doodle.x<=x+get_left()+shoes_length))
	{
		destory_item(4);
		return 4;
	}
	return 0;
}
void board::move_y(double dt)
{
	y+=dt*doodle.vy;
}
greenboard::greenboard(double _x,double _y,int _id,double __x):board(_x,_y)
{
	shoes=spring=hat=rocket=false;
	left=__x;
	h=l=0;
	switch (_id)
	{
	case 1:
		spring=true;
		break;
	case 2:
		hat=true;
		break;
	case 3:
		rocket=true;
		break;
	case 4:
		shoes=true;
		break;
	}
	quad.tex=ht_monster;
	quad.blend=BLEND_ALPHABLEND | BLEND_COLORMUL | BLEND_ZWRITE;
	for (int i=0;i<=3;i++)
	{
		quad.v[i].col=0xFFFFFFFF;
		quad.v[i].z=1;
	}
	if (spring)
	{
		set_quad_txy(&quad,402.0/610.0,96.0/512.0,422.0/610.0,110.0/512.0);
		h=14;l=spring_length;
	}
	if (hat)
	{
		set_quad_txy(&quad,223.0/610.0,278.0/512.0,249.0/610.0,299.0/512.0);
		h=21;l=hat_length;
	}
	if (rocket)
	{
		set_quad_txy(&quad,197.0/610.0,264.0/512.0,222.0/610.0,301.0/512.0);
		h=37;l=rocket_length;
	}
	if (shoes)
	{
		set_quad_txy(&quad,299.0/610.0,202.0/512.0,329.0/610.0,226.0/512.0);
		h=24;l=shoes_length;
	}
	set_quad_xy(&quad,x+left,y+2-h,x+left+l,y+2); 
}
bool greenboard::has_item()
{
	return (spring || hat || rocket || shoes);
}
void greenboard::down()
{
	set_quad_xy(&quad,x+left,y+2-h,x+left+l,y+2); 
}
void greenboard::destory_item(int _id)
{
	switch (_id)
	{
	case 2:
		hat=false;
		break;
	case 3:
		rocket=false;
		break;
	case 4:
		shoes=false;
		break;
	}
}
hgeQuad* greenboard::get_quad()
{
	if (spring || hat || rocket || shoes)
		return &quad;
	return NULL;
}
blueboard::blueboard(double _x,double _y):board(_x,_y)
{
	speed=100*random(0.5,1.5);
}
void blueboard::go_ahead(double dt)
{
	if (x>0 && x<LENGTH-board_length)
	{
		x+=speed*dt;
		return;
	}
	speed=-speed;
	while (x>LENGTH-board_length || x<0)
		x+=speed*dt;
}
brownboard::brownboard(double _x,double _y):board(_x,_y)
{
	has_up=0;
	up=random(50,200);
	speed=100*random(0.5,1.5);
}
void brownboard::go_ahead(double dt)
{
	if (has_up<=up && has_up>=-up)
	{
		has_up+=speed*dt;
		return;
	}
	speed=-speed;
	while (has_up>up || has_up<=-up)
		has_up+=speed*dt;	
}
yelloboard::yelloboard(double _x,double _y):board(_x,_y)
{
	decrease=total_time=6;
}
bool yelloboard::disappear(double dt)
{
	if (get_y()<0)
		return false;
	decrease-=dt;
	if (decrease>=0)
		return false;
	return true;
}
int yelloboard::get_id()
{
	if (decrease>=3)
		return 4;
	if (decrease>=1.5)
		return 5;
	if (decrease>=0.7)
		return 6;
	if (decrease>=0.35)
		return 7;
	if (decrease>=0.175)
		return 8;
	return 9;
}
Monster::Monster()
{
	x=random(0,LENGTH-monster_length);
	y=-30;
	speed=100*random(0.7,1.3);
	dir=1;
}
void Monster::go(double dt)
{
	if (x>0 && x<LENGTH-monster_length)
	{
		x+=speed*dt;
		return;
	}
	speed=-speed;
	dir=(speed>0)?1:0;
	while (x>LENGTH-monster_length || x<0)
		x+=speed*dt;
}
void Monster::down(double dt)
{
	y+=dt*doodle.vy;
	set_quad_xy(&quad_monster,x,y-49,x+monster_length,y);
}
bool Monster::dead()
{
	if (doodle.x+length>=x && doodle.x+length<=x+monster_length)
	{
		if (doodle.y-32>=y-49 && doodle.y-32<=y)
			return true;
		if (doodle.y>=y-49 && doodle.y<=y)
			return true;
	}
	if (doodle.x>=x && doodle.x<=x+monster_length)
	{
		if (doodle.y-32>=y-49 && doodle.y-32<=y)
			return true;
		if (doodle.y>=y-49 && doodle.y<=y)
			return true;
	}
	return false;
}
bool Monster::destory()
{
	return y-50>HEIGHT;
}
bool Monster::get_dir()
{
	set_quad_xy(&quad_monster,x,y-49,x+monster_length,y);
	return dir;
}
bullet::bullet(double _x,double _y):x(_x),y(_y)
{
	if (!dir_shoot || monster==NULL) {vx=0;vy=700;}
	else if (monster->y+monster_length/2>=y)
	{
		vx=0;vy=700;
	}
	else
	{
		double delta_x=monster->x+monster_length/2-x,delta_y=y-monster->y+22;
		double delta=sqrt(delta_x*delta_x+delta_y*delta_y);
		vx=700*delta_x/delta;
		vy=700*delta_y/delta;
	}
}
void bullet::go(double dt)
{
	x+=vx*dt;
	vy-=g*dt;
	y-=vy*dt;
}
void bullet::down(double dt)
{
	y+=dt*doodle.vy;
}
bool bullet::shoot()
{
	return (x>monster->x && x<monster->x+monster_length && y>monster->y-49 && y<monster->y);
}
bool bullet::destory()
{
	return (x<0 || x>LENGTH || y>HEIGHT);
}