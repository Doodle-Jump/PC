#pragma once

class board // ������
{
protected:
	double x,y;
public:
	board() {}
	board(double _x,double _y):x(_x),y(_y) {}
	void move_y(double dt); // ��������
	double get_x() {return x;} // ����x
	virtual double get_y() {return y;} // ����y��֮�������麯������Ϊ�Ұ���Ե��
	bool is_click(); // �ж������Ƿ���а���
	virtual int get_state() {return 0;} // ������̰��ӣ����ذ����ϵ���Ʒ���
	virtual void go_ahead(double) {} // ����������ӻ�Ұ��ӣ��˶�
	virtual bool destory() {return false;} // ����ǰװ��ӣ������һ�κ���ʧ
	virtual bool disappear(double dt) {return false;}  // ����ǻư��ӣ�ʱ����������ʧ
	virtual int get_id() {return 0;} // ���ذ��ӱ��
	virtual bool has_item() {return false;} // ������̰��ӣ������Ƿ�����Ʒ
	virtual void down() {} // ������̰�����������Ʒ������Ʒ�����˶�
	virtual void destory_item(int _id) {} // ������̰��ӣ�����Ʒ�ұ���ã���ô��Ʒ��ʧ
	virtual double get_left() {return -1;} // ������̰��ӣ�������Ʒ��������Ʒ��˾��������˳���
	virtual hgeQuad* get_quad() {return NULL;}  // ������̰��ӣ�������Ʒ��������Ʒ���Ե�hgeQuad��ָ��
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
class bullet //�ӵ���
{
private:
	double x,y,vx,vy;
public:
	bullet() {}
	bullet(double _x,double _y);
	double get_x() {return x;}
	double get_y() {return y;}
	void go(double dt); // �ӵ�ǰ��
	void down(double dt); // �ӵ�����
	bool shoot(); // �ж��Ƿ���й���
	bool destory(); // �ж��ӵ��Ƿ���߽�
};
class Monster // ������
{
private:
	double x,y,speed;
	bool dir;
public:
	Monster(); // ���ɹ���
	bool get_dir(); // ���ع��﷽��
	void go(double dt); // ����ǰ��
	void down(double dt); // ��������
	bool dead(); // ����Ƿ�����
	bool destory(); // �жϹ����Ƿ���߽�
	friend class bullet;
};
class player // �����
{
private:
	double x,y,vx,vy;
	double jump_y;
	bool dir;
	bool spring,hat,rocket,shoes,move;
	int shoes_time;
public:
	bool get_dir() {return dir;} // ���ط���
	bool is_move() {return move;} // ������Ļ�Ƿ��˶�
	bool is_flying() {return (spring || hat || rocket || shoes);} // �����Ƿ�������Ʒ״̬
	double get_x() {return x;}
	double get_y() {return y;}
	double get_vy() {return vy;}
	void set_move(bool _move) {move=_move;} // ������Ļ�Ƿ��˶�
	void set_state(int _id); // ������Ʒ״̬��1Ϊ�ȵ��˹̶����ɣ�2Ϊñ�ӣ�3Ϊ�����4Ϊ����Ь
	void monster() {vy=0;} // �ж��Ƿ�
	int print_item(); // �������Ʒ��������Ʒ��xy��txy���꣬������Ʒ���
	int get_state(); // ������Ʒ״̬��0��������Ʒ
	void go_vx(bool,double); // �����ٶȱ仯
	void go_vy(double dt,bool c=false); // �����ٶȱ仯
	void go_x(double); // �����˶�
	void go_y(double dt,bool c=false); // �����˶�
	void reset(); // ��������ӣ�״̬����
	void click(); // �����Ӻ��ж�������Ļ�Ƿ������
	void init_player(); // ��ʼ��
	void set_quad(hgeQuad*); // �������Ʒ��������Ʒ
	friend class board;
	friend int greenboard::get_state();
	friend class bullet;
	friend class Monster;
};
void another_board(int l=-1); // �������һ���°���
void set_quad_txy(hgeQuad* quad,double x1,double y1,double x2,double y2); // ����hgeQuad��tx,ty
void set_quad_xy(hgeQuad* quad,double x1,double y1,double x2,double y2); // ����hgeQuad��x,y
double random(double start,double end); // ���������
void init(); // ��Ϸ��ʼ��
void restart(); // ���¿�ʼ��Ϸ
void set_statistics(int e); // ����ͳ�����ݣ����e==0������룬e==1����д�룬e==2��������