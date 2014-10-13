#include "stdafx.h"

HGE* hge=0;
hgeFont* fnt1=0,*fnt2=0;
hgeQuad quad_board,quad_left,quad_right,quad_item,quad_monster,quad_bullet;
HEFFECT he_click,he_dead,he_spring,he_hat,he_rocket,he_shoot,he_beat,he_monster,he_music;
HCHANNEL hc_hat,hc_rocket,hc_music;
HTEXTURE ht_monster,ht_left,ht_right,ht_bullet;
double g=900,length=32,board_length=60,speed=600,spring_length=20,least=500,hat_length=26,rocket_length=25;
double monster_length=38,shoes_length=30;
double mmx,mmy,total_up,score;
vector<board*> v;
vector<bullet*> vv;
player doodle;
Monster *monster;
char msg[20];
bool dead,want_to_continue,game_over,pause,game_start,setting,statistics;
bool music_on,use_board,dir_shoot;
FILE *data;
int highest_score,last_score,total_jump,max_jump,last_jump;
int total_time,max_time,last_time,total_play,total_score;
double this_time,average_score;
int this_jump;
double bullet_dt;

double random(double start,double end)
{
	return start+(end-start)*rand()/(RAND_MAX + 1.0);
}
void set_quad_xy(hgeQuad* quad,double x1,double y1,double x2,double y2)
{
	quad->v[0].x=quad->v[1].x=x1;
	quad->v[0].y=quad->v[3].y=y1;
	quad->v[2].x=quad->v[3].x=x2;
	quad->v[1].y=quad->v[2].y=y2;
}
void set_quad_txy(hgeQuad* quad,double x1,double y1,double x2,double y2)
{
	quad->v[0].tx=quad->v[1].tx=x1;
	quad->v[0].ty=quad->v[3].ty=y1;
	quad->v[2].tx=quad->v[3].tx=x2;
	quad->v[1].ty=quad->v[2].ty=y2;
}
void another_board(int l)
{
	if (l==-1)
		l=(int)(random(0,5));
	double t=0,p=0;
	int _id=0;
	switch(l)
	{
	case 0:
		t=random(0,1);
		if (t<=0.2)
		{
			p=random(0,board_length-spring_length);
			_id=1;
		}
		else if (t<=0.25)
		{
			p=random(0,board_length-hat_length);
			_id=2;
		}
		else if (t<=0.28)
		{
			p=random(0,board_length-rocket_length);
			_id=3;
		}
		else if (t<=0.33)
		{
			p=random(0,board_length-shoes_length);
			_id=4;
		}
		v.push_back(new greenboard(random(0,400-board_length),(*(v.end()-1))->get_y()-random(50,200),_id,p));
		break;
	case 1:
		v.push_back(new blueboard(random(0,400-board_length),(*(v.end()-1))->get_y()-random(50,200)));
		break;
	case 2:
		v.push_back(new brownboard(random(0,400-board_length),(*(v.end()-1))->get_y()-random(50,200)));
		break;
	case 3:
		v.push_back(new whiteboard(random(0,400-board_length),(*(v.end()-1))->get_y()-random(50,200)));
		break;
	case 4:
		v.push_back(new yelloboard(random(0,400-board_length),(*(v.end()-1))->get_y()-random(50,200)));
		break;
	}
}
void init()
{
	set_statistics(0);
	// fnt
	fnt1=new hgeFont("doodle-jump/font1.fnt");
	fnt2=new hgeFont("doodle-jump/font2.fnt");
	fnt1->SetColor(BLACK);
	fnt2->SetColor(BLACK);
	// ogg
	he_beat=hge->Effect_Load("doodle-jump/raw/beat.ogg");
	he_click=hge->Effect_Load("doodle-jump/raw/click.ogg");
	he_dead=hge->Effect_Load("doodle-jump/raw/dead.ogg");
	he_hat=hge->Effect_Load("doodle-jump/raw/hat.ogg");
	he_monster=hge->Effect_Load("doodle-jump/raw/monster.ogg");
	he_rocket=hge->Effect_Load("doodle-jump/raw/rocket.ogg");
	he_shoot=hge->Effect_Load("doodle-jump/raw/shoot.ogg");
	he_spring=hge->Effect_Load("doodle-jump/raw/spring.ogg");
	he_music=hge->Effect_Load("doodle-jump/raw/music.mp3");
	// png
	ht_monster=hge->Texture_Load("doodle-jump/photo.png");
	ht_left=hge->Texture_Load("doodle-jump/left.png");
	ht_right=hge->Texture_Load("doodle-jump/right.png");
	ht_bullet=hge->Texture_Load("doodle-jump/ball.png");
	quad_left.tex=ht_left;
	quad_right.tex=ht_right;
	quad_bullet.tex=ht_bullet;
	quad_monster.tex=quad_item.tex=quad_board.tex=ht_monster;
	quad_monster.blend=quad_item.blend=quad_board.blend=quad_right.blend=quad_left.blend=BLEND_ALPHABLEND | BLEND_COLORMUL | BLEND_ZWRITE;
	quad_bullet.blend=BLEND_ALPHABLEND | BLEND_COLORMUL | BLEND_ZWRITE;
	set_quad_txy(&quad_left,0,28.0/120.0,94.0/124.0,1.0);
	set_quad_txy(&quad_right,29.0/124.0,27.0/120.0,1.0,1.0);
	set_quad_txy(&quad_bullet,96.0/128.0,64.0/128.0,1.0,96.0/128.0);
	for (int i=0;i<=3;i++)
	{
		quad_bullet.v[i].z=quad_monster.v[i].z=quad_item.v[i].z=quad_board.v[i].z=quad_left.v[i].z=quad_right.v[i].z=1;
		quad_monster.v[i].col=quad_item.v[i].col=quad_board.v[i].col=quad_left.v[i].col=quad_right.v[i].col=0xFFFFFFFF;
		quad_bullet.v[i].col=0xFFFF0000;
	}
	dir_shoot=music_on=use_board=true;
	hc_music=hge->Effect_PlayEx(he_music,30,0,1,true);
}
void restart()
{
	v.push_back(new greenboard(140,500));
	for (int i=0;i<=5;i++) another_board(0);
	strcpy(msg,"click");
	doodle.init_player();
	doodle.set_quad(&quad_left);
	doodle.set_quad(&quad_right);
	pause=dead=setting=statistics=game_start=game_over=want_to_continue=false;
	score=0;
	this_time=this_jump=0;
	bullet_dt=0;
}
void set_statistics(int e)
{
	if (e==0)
	{
		data=fopen("doodle-jump/data.doj","r");
		fscanf(data,"%d %d %d %d %d %d %d %d %d %d",&highest_score,&total_score,&last_score,&total_jump,
			&max_jump,&last_jump,&total_time,&max_time,&last_time,&total_play);
		fclose(data);
		return;
	}
	data=fopen("doodle-jump/data.doj","w");
	if (e==2) fprintf(data,"0 0 0 0 0 0 0 0 0 0");
	else fprintf(data,"%d %d %d %d %d %d %d %d %d %d",highest_score,total_score,last_score,total_jump,
		max_jump,last_jump,total_time,max_time,last_time,total_play);
	fclose(data);
}
bool FrameFunc()
{
	double dt=hge->Timer_GetDelta();
	float mx,my;
	hge->Input_GetMousePos(&mx,&my);
	mmx=(double)mx;mmy=(double)my;
	if (hge->Input_GetKeyState(HGEK_ESCAPE)) return true;
	if (!game_start)
	{
		if (!setting && !statistics)
		{
			if (hge->Input_GetKeyState(HGEK_LBUTTON))
			{
				if (mmx>=139 && mmx<=260 && mmy>=202 && mmy<=223)
					game_start=true;
				if (mmx>=155 && mmx<=243 && mmy>=252 && mmy<=277)
					setting=true;
				if (mmx>=147 && mmx<=252 && mmy>=302 && mmy<=323)
				{
					statistics=true;
					set_statistics(0);
				}
				if (mmx>=179 && mmx<=219 && mmy>=352 && mmy<=373)
					return true;
			}
		}
		else if (setting)
		{
			if (hge->Input_GetKeyState(HGEK_LBUTTON))
			{
				if (mmx>=198 && mmy>=102 && mmx<=224 && mmy<=122)
				{
					music_on=true;
					hge->Channel_SetVolume(hc_music,30);
				}
				if (mmx>=265 && mmy>=102 && mmx<=305 && mmy<=122)
				{
					music_on=false;
					hge->Channel_SetVolume(hc_music,0);
				}
				if (mmx>=133 && mmy>=202 && mmx<=242 && mmy<=222)
					use_board=true;
				if (mmx>=282 && mmy>=202 && mmx<=350 && mmy<=222)
					use_board=false;
				if (mmx>=280 && mmy>=302 && mmx<=308 && mmy<=322)
					dir_shoot=true;
				if (mmx>=347 && mmy>=302 && mmx<=390 && mmy<=322)
					dir_shoot=false;
				if (mmx>=149 && mmy>=402 && mmx<=249 && mmy<=422)
				{
					music_on=use_board=dir_shoot=true;
					hge->Channel_SetVolume(hc_music,30);
				}
				if (mmx>=171 && mmy>=502 && mmx<=227 && mmy<=522)
					setting=false;
			}
		}
		else if (statistics)
		{
			if (hge->Input_GetKeyState(HGEK_LBUTTON))
			{
				if (mmx>=118 && mmy>=382 && mmx<=180 && mmy<=401)
				{
					set_statistics(2);
					set_statistics(0);
				}
				if (mmx>=220 && mmy>=382 && mmx<=277 && mmy<=401)
					statistics=false;
			}
		}
		return false;
	}
	if (pause)
	{
		if (!setting)
		{
			if (hge->Input_GetKeyState(HGEK_LBUTTON))
			{
				if (mmx>=242 && mmy>=352 && mmx<=316 && mmy<=372)
					setting=true;
				if (mmx>=79 && mmx<=160 && mmy>=352 && mmy<=372)
				{
					want_to_continue=true;
					return true;
				}
				if (mmx>=160 && mmy>=302 && mmx<=237 && mmy<=324)
				{
					pause=false;switch (doodle.get_state())
					{
					case 2:
						hge->Channel_Resume(hc_hat);
						break;
					case 3:
						hge->Channel_Resume(hc_rocket);
					}
				}
				if (mmx>=178 && mmy>=402 && mmx<=221 && mmy<=422)
					return true;
			}
		}
		else
		{
			if (hge->Input_GetKeyState(HGEK_LBUTTON))
			{
				if (mmx>=198 && mmy>=102 && mmx<=224 && mmy<=122)
				{
					music_on=true;
					hge->Channel_SetVolume(hc_music,30);
				}
				if (mmx>=265 && mmy>=102 && mmx<=305 && mmy<=122)
				{
					music_on=false;
					hge->Channel_SetVolume(hc_music,0);
				}
				if (mmx>=133 && mmy>=202 && mmx<=242 && mmy<=222)
					use_board=true;
				if (mmx>=282 && mmy>=202 && mmx<=350 && mmy<=222)
					use_board=false;
				if (mmx>=280 && mmy>=302 && mmx<=308 && mmy<=322)
					dir_shoot=true;
				if (mmx>=347 && mmy>=302 && mmx<=390 && mmy<=322)
					dir_shoot=false;
				if (mmx>=149 && mmy>=402 && mmx<=249 && mmy<=422)
					music_on=use_board=dir_shoot=true;
				if (mmx>=171 && mmy>=502 && mmx<=227 && mmy<=522)
					setting=false;
			}
		}
		return false;
	}
	if (game_over)
	{
		if (hge->Input_GetKeyState(HGEK_LBUTTON))
		{
			if (mmx>=79 && mmx<=160 && mmy>=302 && mmy<=322)
			{
				want_to_continue=true;
				return true;
			}
			if (mmx>=258 && mmx<=300 && mmy>=302 && mmy<=322)
				return true;
		}
		return false;
	}
	if (hge->Input_GetKeyState(HGEK_LBUTTON) && mmx>=298 && mmy>=32 && mmx<=368 && mmy<=52)
	{
		pause=true;
		switch (doodle.get_state())
		{
		case 2:
			hge->Channel_Pause(hc_hat);
			break;
		case 3:
			hge->Channel_Pause(hc_rocket);
		}
		return false;
	}
	this_time+=dt;
	bullet_dt+=dt;
	/*if (use_board && hge->Input_GetKeyState(HGEK_UP))
		doodle.monster();*/
	if (use_board && hge->Input_GetKeyState(HGEK_LEFT))
		doodle.go_vx(false,dt);
	if (!use_board && mmx<doodle.get_x()) 
		doodle.go_vx(false,dt);
	if (use_board && hge->Input_GetKeyState(HGEK_RIGHT))
		doodle.go_vx(true,dt);
	if (!use_board && mmx>doodle.get_x()+32)
		doodle.go_vx(true,dt);
	doodle.go_x(dt);
	if (use_board && bullet_dt>=0.2 && hge->Input_GetKeyState(HGEK_SPACE))
	{
		vv.push_back(new bullet(doodle.get_x()+16,doodle.get_y()-16));
		bullet_dt=0;
		if (music_on)
			hge->Effect_PlayEx(he_shoot,50);
	}
	if (!use_board && bullet_dt>=0.2 && hge->Input_GetKeyState(HGEK_LBUTTON))
	{
		vv.push_back(new bullet(doodle.get_x()+16,doodle.get_y()-16));
		bullet_dt=0;
		if (music_on)
			hge->Effect_PlayEx(he_shoot,50);
	}
	if (monster==NULL)
	{
		double temp=random(0,1);
		if (temp<=0.0001)
			monster=new Monster();
	}
	for (vector<board*>::iterator it=v.begin();it!=v.end();it++)
	{
		(*it)->go_ahead(dt);
		if ((*it)->disappear(dt))
		{
			delete *it;
			it=v.erase(it);
			if (it==v.end())
			{
				another_board();
				break;
			}
			else
				another_board();
		}
	}
	for (vector<bullet*>::iterator it=vv.begin();it!=vv.end();it++)
	{
		(*it)->go(dt);
		if ((*it)->destory())
		{
			delete *it;
			it=vv.erase(it);
			if (it==vv.end())
				break;
		}
		if (monster!=NULL && (*it)->shoot())
		{
			if (music_on)
				hge->Effect_PlayEx(he_beat,50);
			delete monster;
			monster=NULL;
			delete *it;
			it=vv.erase(it);
			if (it==vv.end())
				break;
		}
	}
	if (monster!=NULL)
		monster->go(dt);
	if (!strcmp(msg,"click"))
	{
		doodle.reset();
		strcpy(msg,"up");
	}
	else if (!strcmp(msg,"up"))
	{
		if (doodle.is_move())
		{
			if (doodle.get_y()>=2*HEIGHT/5)
				doodle.go_y(dt);
			else //if (!doodle.is_flying() || doodle.get_vy()>=800 || doodle.get_y()<=HEIGHT/3)
			{
				for (vector<board*>::iterator it=v.begin();it!=v.end();it++)
				{
					(*it)->move_y(dt);
					(*it)->down();
				}
				for (vector<bullet*>::iterator it=vv.begin();it!=vv.end();it++)
				{
					(*it)->down(dt);
					if ((*it)->destory())
					{
						delete *it;
						it=vv.erase(it);
						if (it==vv.end())
							break;
					}
				}
				if (monster!=NULL)
				{
					monster->down(dt);
					if (monster->destory())
					{
						delete monster;
						monster=NULL;
					}
				}
				total_up-=dt*doodle.get_y();
			}
			/*else
				doodle.go_y(dt);*/
			score+=doodle.get_y()*dt/10;
			if (!doodle.is_flying() && total_up<=0)
			{
				doodle.set_move(false);
				total_up=0;
			}
		}
		else
			doodle.go_y(dt);
		doodle.go_vy(dt);
		if(!doodle.is_flying() && monster!=NULL && !dead)
		{
			dead=monster->dead();
			if (dead)
				doodle.monster();
		}
		if (doodle.get_vy()<=0)
		{
			doodle.set_state(0);
			total_up=0;
			if (hge->Channel_IsPlaying(hc_hat))
				hge->Channel_Stop(hc_hat);
			if (hge->Channel_IsPlaying(hc_rocket))
				hge->Channel_Stop(hc_rocket);
			strcpy(msg,"down");
		}
	}
	else if (!strcmp(msg,"down"))
	{
		doodle.go_y(dt,true);
		doodle.go_vy(dt,true);
		if (!dead)
		{
			if (monster!=NULL && monster->dead())
			{
				this_jump++;
				doodle.click();
				if (music_on) hge->Effect_PlayEx(he_monster,50);
				strcpy(msg,"click");
				delete monster;
				monster=NULL;
			}
			for (vector<board*>::iterator it=v.begin();it!=v.end();it++)
			{
				if ((*it)->is_click())
				{
					this_jump++;
					doodle.click();
					doodle.set_state((*it)->get_state());
					if (music_on)
					{
						switch (doodle.get_state())
						{
						case 0:
							hge->Effect_PlayEx(he_click,50);
							break;
						case 1:case 4:
							hge->Effect_PlayEx(he_spring,50);
							break;
						case 2:
							hc_hat=hge->Effect_PlayEx(he_hat,50,0,1.0,true);
							break;
						case 3:
							hc_rocket=hge->Effect_PlayEx(he_rocket,50,0,1.0,true);
						}
					}
					strcpy(msg,"click");
					if ((*it)->destory())
					{
						delete *it;
						it=v.erase(it);
						another_board();
						break;
					}
					break;
				}
			}
		}
		if (doodle.get_y()>=HEIGHT+33)
		{
			game_over=true;
			if (music_on) hge->Effect_PlayEx(he_dead,50);
			last_jump=this_jump;
			last_score=score;
			last_time=(int)this_time;
			if (highest_score<score)
				highest_score=score;
			if (max_time<last_time)
				max_time=last_time;
			if (max_jump<last_jump)
				max_jump=last_jump;
			total_jump+=last_jump;
			total_time+=last_time;
			total_play++;
			total_score+=score;
			set_statistics(1);
		}
	}
	doodle.set_quad(&quad_left);
	doodle.set_quad(&quad_right);
	return false;
}
bool RenderFunc()
{
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(LIGHTPINK);
	for (int i=0;i<=HEIGHT/10;i++)
		hge->Gfx_RenderLine(0,10*i,LENGTH,10*i,PINK);
	for (int i=0;i<=LENGTH/10;i++)
		hge->Gfx_RenderLine(10*i,0,10*i,HEIGHT,PINK);
	fnt2->printf(10,10,HGETEXT_LEFT,"%.0lf",score);
	while ((*v.begin())->get_y()>HEIGHT)
	{
		delete (*v.begin());
		v.erase(v.begin());
		another_board();
	}
	for (vector<board*>::iterator it=v.begin();it!=v.end();it++)
	{
		switch ((*it)->get_id())
		{
		case 0:
			set_quad_txy(&quad_board,0,0,60.0/610.0,16.0/512.0);
			break;
		case 1:
			set_quad_txy(&quad_board,0,17.0/512.0,60.0/610.0,35.0/512.0);
			break;
		case 2:
			set_quad_txy(&quad_board,0,35.0/512.0,60.0/610.0,53.0/512.0);
			break;
		case 3:
			set_quad_txy(&quad_board,0,53.0/512.0,60.0/610.0,71.0/512.0);
			break;
		default:
			set_quad_txy(&quad_board,0,(182.0+18*((*it)->get_id()-4))/512.0,60.0/610.0,(182.0+18*((*it)->get_id()-3))/512.0);
		}
		set_quad_xy(&quad_board,(*it)->get_x(),(*it)->get_y(),(*it)->get_x()+board_length,(*it)->get_y()+16);
		hge->Gfx_RenderQuad(&quad_board);
		if ((*it)->has_item())
			hge->Gfx_RenderQuad((*it)->get_quad());
	}
	for (vector<bullet*>::iterator it=vv.begin();it!=vv.end();it++)
	{
		set_quad_xy(&quad_bullet,(*it)->get_x()-5,(*it)->get_y()-5,(*it)->get_x()+5,(*it)->get_y()+5);
		hge->Gfx_RenderQuad(&quad_bullet);
	}
	if (monster!=NULL)
	{
		if (!monster->get_dir())
			set_quad_txy(&quad_monster,106.0/610.0,188.0/512.0,144.0/610.0,237.0/512.0);
		else
			set_quad_txy(&quad_monster,65.0/610.0,188.0/512.0,103.0/610.0,237.0/512.0);
		hge->Gfx_RenderQuad(&quad_monster);
	}
	int temp=doodle.print_item();
	if (temp==3 || temp==4)
		hge->Gfx_RenderQuad(&quad_item);
	if (doodle.get_dir())
		hge->Gfx_RenderQuad(&quad_right);
	else
		hge->Gfx_RenderQuad(&quad_left);
	if (temp==2)
		hge->Gfx_RenderQuad(&quad_item);
	if (!game_over && game_start && !pause)
		fnt1->printf(300,30,HGETEXT_LEFT,"Pause");
	if (game_over)
	{
		fnt1->printf(200,200,HGETEXT_CENTER,"Game Over");
		fnt1->printf(200,230,HGETEXT_CENTER,"%.0lf",score);
		fnt1->printf(120,300,HGETEXT_CENTER,"Restart");
		fnt1->printf(280,300,HGETEXT_CENTER,"Exit");
	}
	if (!game_start)
	{
		if (!setting && !statistics)
		{
			fnt1->printf(200,200,HGETEXT_CENTER,"Start Game");
			fnt1->printf(200,250,HGETEXT_CENTER,"Settings");
			fnt1->printf(200,300,HGETEXT_CENTER,"Statistics");
			fnt1->printf(200,350,HGETEXT_CENTER,"Exit");
		}
		else if (setting)
		{
			fnt1->printf(200,100,HGETEXT_CENTER,"Music:     ON     OFF");
			fnt1->printf(music_on?190:257,100,HGETEXT_CENTER,"*");
			fnt1->printf(200,200,HGETEXT_CENTER,"Use:     Keyboard     Mouse");
			fnt1->printf(use_board?125:274,200,HGETEXT_CENTER,"*");
			fnt1->printf(200,300,HGETEXT_CENTER,"Directional shooting:     ON     OFF");
			fnt1->printf(dir_shoot?272:339,300,HGETEXT_CENTER,"*");
			fnt1->printf(200,400,HGETEXT_CENTER,"Reset All");
			fnt1->printf(200,500,HGETEXT_CENTER,"Back");
		}
		else if (statistics)
		{
			fnt1->printf(100,100,HGETEXT_LEFT,
				"Highest score: %d\nAverage score: %.2lf\nLast score: %d\nLast jump: %d\nMax jump: %d\nTotal jump: %d\nLast time: %d s\nMax time: %d s\nTotal time: %d s\nTotal play: %d",
	highest_score,total_score/(total_play+1e-9),
	last_score,last_jump,max_jump,total_jump,last_time,max_time,total_time,total_play);
			fnt1->printf(150,380,HGETEXT_CENTER,"Reset");
			fnt1->printf(250,380,HGETEXT_CENTER,"Back");
		}
	}
	if (pause)
	{
		if (!setting)
		{
			fnt1->printf(200,200,HGETEXT_CENTER,"Pausing......");
			fnt1->printf(200,300,HGETEXT_CENTER,"Continue");
			fnt1->printf(120,350,HGETEXT_CENTER,"Restart");
			fnt1->printf(280,350,HGETEXT_CENTER,"Setting");
			fnt1->printf(200,400,HGETEXT_CENTER,"Exit");
		}
		else
		{
			fnt1->printf(200,100,HGETEXT_CENTER,"Music:     ON     OFF");
			fnt1->printf(music_on?190:257,100,HGETEXT_CENTER,"*");
			fnt1->printf(200,200,HGETEXT_CENTER,"Use:     Keyboard     Mouse");
			fnt1->printf(use_board?125:274,200,HGETEXT_CENTER,"*");
			fnt1->printf(200,300,HGETEXT_CENTER,"Directional shooting:     ON     OFF");
			fnt1->printf(dir_shoot?272:339,300,HGETEXT_CENTER,"*");
			fnt1->printf(200,400,HGETEXT_CENTER,"Reset All");
			fnt1->printf(200,500,HGETEXT_CENTER,"Back");
		}
	}
	hge->Gfx_EndScene();
	return false;
}
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
{
	srand(unsigned(time(0)));
	hge=hgeCreate(HGE_VERSION);
	hge->System_SetState(HGE_FRAMEFUNC,FrameFunc);
	hge->System_SetState(HGE_USESOUND,true);
	hge->System_SetState(HGE_RENDERFUNC,RenderFunc);
	hge->System_SetState(HGE_TITLE,"Doodle-jump");
	hge->System_SetState(HGE_WINDOWED,true);
	hge->System_SetState(HGE_HIDEMOUSE,false);
	hge->System_SetState(HGE_SCREENWIDTH,LENGTH);
	hge->System_SetState(HGE_SCREENHEIGHT,HEIGHT);
	if (hge->System_Initiate())
	{
		init();
		while(1)
		{
			restart();
			hge->System_Start();
			while(!v.empty())
			{
				delete *v.begin();
				v.erase(v.begin());
			}
			while (!vv.empty())
			{
				delete *vv.begin();
				vv.erase(vv.begin());
			}
			if (monster!=NULL)
			{
				delete monster;
				monster=NULL;
			}
			if (!want_to_continue)
				break;
		}
		hge->Texture_Free(ht_left);
		hge->Texture_Free(ht_right);
		hge->Texture_Free(ht_monster);
		hge->Effect_Free(he_beat);
		hge->Effect_Free(he_hat);
		hge->Effect_Free(he_rocket);
		hge->Effect_Free(he_shoot);
		hge->Effect_Free(he_spring);
		hge->Effect_Free(he_click);
		hge->Effect_Free(he_monster);
		hge->Effect_Free(he_dead);
		hge->Effect_Free(he_music);
		hge->System_Shutdown();
	}
	hge->Release();
	return 0;
}