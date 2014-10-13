#pragma once

class board // 板子类
{
protected:
	double x,y;
public:
	board() {}
	board(double _x,double _y):x(_x),y(_y) {}
	void move_y(double dt); // 板子下移
	double get_x() {return x;} // 返回x
	virtual double get_y() {return y;} // 返回y，之所以用虚函数，因为灰板子缘故
	bool is_click(); // 判断人物是否击中板子
	virtual int get_state() {return 0;} // 如果是绿板子，返回板子上的物品编号
	virtual void go_ahead(double) {} // 如果是蓝板子或灰板子，运动
	virtual bool destory() {return false;} // 如果是白板子，被打击一次后消失
	virtual bool disappear(double dt) {return false;}  // 如果是黄板子，时间限制性消失
	virtual int get_id() {return 0;} // 返回板子编号
	virtual bool has_item() {return false;} // 如果是绿板子，返回是否有物品
	virtual void down() {} // 如果是绿板子且上有物品，则物品跟着运动
	virtual void destory_item(int _id) {} // 如果是绿板子，有物品且被获得，那么物品消失
	virtual double get_left() {return -1;} // 如果是绿板子，且有物品，返回物品左端距离板子左端长度
	virtual hgeQuad* get_quad() {return NULL;}  // 如果是绿板子，且有物品，返回物品所对的hgeQuad的指针
};
class greenboard:public board
{
private:
	double left,l,h;
	hgeQuad quad;
	bool spring,hat,rocket,shoes;
public:
	greenboard() {}
	greenboard(double _x,double _y,int _id=0,double __x=0);
	hgeQuad* get_quad();
	int get_id() {return 0;}
	void destory_item(int _id);
	int get_state();
	bool has_item();
	void down();
	double get_left() {return left;}
};
class blueboard:public board
{
private:
	double speed;
public:
	blueboard() {}
	blueboard(double _x,double _y);
	void go_ahead(double dt);
	int get_id() {return 1;}
};
class brownboard:public board
{
private:
	double speed;
	double up,has_up;
public:
	brownboard() {}
	brownboard(double _x,double _y);
	int get_id() {return 2;}
	void go_ahead(double dt);
	double get_y() {return y+has_up;}
};
class whiteboard:public board
{
public:
	whiteboard() {}
	whiteboard(double _x,double _y):board(_x,_y) {}
	bool destory() {return true;}
	int get_id() {return 3;}
};
class yelloboard:public board
{
private:
	double total_time;
	double decrease;
public:
	yelloboard() {}
	yelloboard(double _x,double _y);
	bool disappear(double dt);
	int get_id();
};
class bullet //子弹类
{
private:
	double x,y,vx,vy;
public:
	bullet() {}
	bullet(double _x,double _y);
	double get_x() {return x;}
	double get_y() {return y;}
	void go(double dt); // 子弹前进
	void down(double dt); // 子弹下落
	bool shoot(); // 判断是否击中怪物
	bool destory(); // 判断子弹是否出边界
};
class Monster // 怪物类
{
private:
	double x,y,speed;
	bool dir;
public:
	Monster(); // 生成怪物
	bool get_dir(); // 返回怪物方向
	void go(double dt); // 怪物前进
	void down(double dt); // 怪物下落
	bool dead(); // 玩家是否死亡
	bool destory(); // 判断怪物是否出边界
	friend class bullet;
};
class player // 玩家类
{
private:
	double x,y,vx,vy;
	double jump_y;
	bool dir;
	bool spring,hat,rocket,shoes,move;
	int shoes_time;
public:
	bool get_dir() {return dir;} // 返回方向
	bool is_move() {return move;} // 返回屏幕是否运动
	bool is_flying() {return (spring || hat || rocket || shoes);} // 返回是否处于有物品状态
	double get_x() {return x;}
	double get_y() {return y;}
	double get_vy() {return vy;}
	void set_move(bool _move) {move=_move;} // 设置屏幕是否运动
	void set_state(int _id); // 设置物品状态，1为踩到了固定弹簧，2为帽子，3为火箭，4为弹簧鞋
	void monster() {vy=0;} // 判断是否
	int print_item(); // 如果有物品，生成物品的xy与txy坐标，返回物品编号
	int get_state(); // 返回物品状态，0代表无物品
	void go_vx(bool,double); // 横向速度变化
	void go_vy(double dt,bool c=false); // 纵向速度变化
	void go_x(double); // 横向运动
	void go_y(double dt,bool c=false); // 纵向运动
	void reset(); // 如果触板子，状态归零
	void click(); // 触板子后判断整个屏幕是否该下移
	void init_player(); // 初始化
	void set_quad(hgeQuad*); // 如果有物品，设置物品
	friend class board;
	friend int greenboard::get_state();
	friend class bullet;
	friend class Monster;
};
void another_board(int l=-1); // 随机生成一个新板子
void set_quad_txy(hgeQuad* quad,double x1,double y1,double x2,double y2); // 设置hgeQuad的tx,ty
void set_quad_xy(hgeQuad* quad,double x1,double y1,double x2,double y2); // 设置hgeQuad的x,y
double random(double start,double end); // 生成随机数
void init(); // 游戏初始化
void restart(); // 重新开始游戏
void set_statistics(int e); // 操作统计数据，如果e==0代表读入，e==1代表写入，e==2代表清零