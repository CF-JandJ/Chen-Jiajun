// 飞机大战
// EasyX
#include <iostream>
#include <graphics.h>//easyx
#include <vector>
#include <conio.h>
using namespace std;

constexpr auto swidth = 600;
constexpr auto sheight = 1100;
constexpr unsigned int SHP = 2; 
//伤害冷却时间
constexpr auto hurttime = 500;

bool PointInRect(int x, int y, RECT& r)
{
	return (r.left <= x && x <= r.right && r.top <= y && y <= r.bottom);
}

//碰撞检测             小矩形 ， 大矩形 
bool RectDuangRect(RECT& r1, RECT& r2)
{
	RECT r;
	r.left = r1.left - (r2.right - r2.left);
	r.right = r1.right;
	r.top = r1.top - (r2.bottom - r2.top);
	r.bottom = r1.bottom;

	return (r.left <= r2.left&& r2.left <= r.right && r.top <= r2.top && r2.top <= r.bottom);
}


// 一个开始界面
void Welcome()
{
	LPCTSTR title = _T("飞机大战");
	LPCTSTR tplay = _T("开始游戏");
	LPCTSTR texit = _T("退出游戏");

	RECT tplayr, texitr;

	BeginBatchDraw();
	setbkcolor(WHITE);
	cleardevice();
	settextstyle(60, 0, _T("黑体"));
	settextcolor(BLACK);
	outtextxy(swidth / 2 - textwidth(title) / 2, sheight / 5, title);

	settextstyle(40, 0, _T("黑体"));
	tplayr.left = swidth / 2 - textwidth(tplay) / 2;
	tplayr.right = tplayr.left + textwidth(tplay);
	tplayr.top = sheight / 5 * 2.5;
	tplayr.bottom = tplayr.top + textheight(tplay);

	texitr.left = swidth / 2 - textwidth(texit) / 2;
	texitr.right = texitr.left + textwidth(texit);
	texitr.top = sheight / 5 * 3;
	texitr.bottom = texitr.top + textheight(texit);

	outtextxy(tplayr.left, tplayr.top, tplay);
	outtextxy(texitr.left, texitr.top, texit);

	EndBatchDraw();

	while (true)
	{
		ExMessage mess;
		getmessage(&mess, EX_MOUSE);
		if (mess.lbutton)
		{
			if (PointInRect(mess.x, mess.y, tplayr))
			{
				return;
			}
			else if (PointInRect(mess.x, mess.y, texitr))
			{
				exit(0);
			}
		}
	}
}

// 结束界面
void Over(unsigned long long &kill) {
	TCHAR *str = new TCHAR[128];
	_stprintf_s(str, 128, _T("击杀数：%llu"), kill);

	settextcolor(RED);
	outtextxy(swidth / 2 - textwidth(str) / 2, sheight / 5, str);

	//键盘返回 Enter
	LPCTSTR info = _T("按Enter 返回");
	settextstyle(20, 0, _T("黑体"));
	outtextxy(swidth - textwidth(info), sheight - textheight(info), info);
	while (true)
	{
		ExMessage mess;
		getmessage(&mess, EX_KEY);
		if (mess.vkcode == VK_RETURN) {
			return;
		}
	}
}

//背景，敌机，英雄，子弹
class BK {
public:
	BK(IMAGE & img):img(img),y(-sheight) {};
	void show() {
		if (y == 0)  y = -sheight ;
		y += 4;
		putimage(0, y, &img);
	}
private:
	IMAGE& img;
	int y;
};

class Hero {
public:
	Hero(IMAGE& imag) :imag(imag),HP(SHP) {
		rect.left   = swidth / 2 - imag.getwidth() / 2; 
		rect.top    = sheight - imag.getheight();
		rect.right  = rect.left + imag.getwidth();
		rect.bottom = sheight;
	};
	void Show() {
		setlinecolor(RED);
		setlinestyle(PS_SOLID, 3);
		putimage(rect.left, rect.top, &imag);
		line(rect.left, rect.top, rect.left + imag.getwidth() / SHP * HP, rect.top);
	}
	//鼠标控制
	void Control() {
		ExMessage mess;
		if (peekmessage(&mess, EX_MOUSE)) {
			rect.left = mess.x - imag.getwidth() / 2;
			rect.top  = mess.y - imag.getheight() / 2;
			rect.right = rect.left + imag.getwidth();
			rect.bottom = rect.top + imag.getheight();
		}
	}

	bool hurt() {
		HP--;
		return (HP == 0) ? false : true;
	}

	RECT& GetRect() { return rect; }
private:
	IMAGE& imag;
	RECT rect;

	unsigned int HP;
};

class Enemy {
public:
	Enemy(IMAGE& imag, int x, IMAGE* boom) :imag(imag),isdie(false),boomsum(0) {
		selfboom[0] = boom[0];
		selfboom[1] = boom[1];
		selfboom[2] = boom[2];
		rect.left   = x;
		rect.right  = rect.left + imag.getwidth();
		rect.top    = -imag.getheight();
		rect.bottom = 0;
	}
	bool Show() {
		if (isdie) {
			if (boomsum == 3) return false;
			putimage(rect.left, rect.top, &selfboom[boomsum]);
			boomsum++;
			return true;
		}
		if (rect.top >= sheight) return false;
		rect.top += 4;
		rect.bottom += 4;
		putimage(rect.left, rect.top, &imag);

		return true;
	}

	void Isdie() { isdie = true; }

	RECT& GetRect() { return rect; }

private:
	IMAGE& imag;
	RECT rect;
	IMAGE selfboom[3];
	bool isdie;
	int boomsum;
};

//子弹
class Bullet {
public:
	Bullet(IMAGE& imag,RECT& pr) :imag(imag) {
		rect.left   = pr.left + (pr.right - pr.left) / 2 - (imag.getwidth()) / 2;
		rect.right  = rect.left + imag.getwidth();
		rect.top    = pr.top - imag.getheight();
		rect.bottom = pr.top;
	}
	bool Show() {
		if (rect.bottom <= 0)  return false;
		rect.top -= 3;
		rect.bottom -= 3;
		putimage(rect.left, rect.top, &imag);
		return true;
	}
	RECT& GetRect() { return rect; }
protected:
	IMAGE& imag;
	RECT rect;
};

//敌机子弹
class EBullet : public Bullet {
public:
	EBullet(IMAGE& imag,RECT &pr):Bullet(imag,pr) {
		rect.left = pr.left + (pr.right - pr.left) / 2 - (imag.getwidth()) / 2;
		rect.right = rect.left + imag.getwidth();
		rect.top = pr.bottom;
		rect.bottom = pr.top + imag.getheight();
	}
	bool Show() {
		if (rect.top >= sheight)  return false;
		rect.top += 5;
		rect.bottom += 5;
		putimage(rect.left, rect.top, &imag);
		return true;
	}
};

//生成敌机
bool AddEnemy(vector<Enemy*>& es, IMAGE& enemying,IMAGE * eboom) {
	
	Enemy* e = new Enemy(enemying, abs(rand()) % (swidth - enemying.getwidth()), eboom);
	for (auto& c : es) {
		if (RectDuangRect(c->GetRect(), e->GetRect())) {
			delete e;
			return false;
		}
	}
	es.push_back(e);
	return true;
}


bool Play() {

	setbkcolor(WHITE);
	cleardevice();

	bool is_play = true;

	IMAGE heroimg, enemying, bkimg, bulletimg;
	IMAGE eboom[3];

	loadimage(&heroimg, _T("images\\me1.png"));
	loadimage(&enemying, _T("images\\enemy1.png"));
	loadimage(&bkimg, _T("images\\bk2.png"), swidth, sheight * 2);
	loadimage(&bulletimg, _T("images\\bullet1.png"));
	loadimage(&eboom[0], _T("images\\enemy1_down2.png"));
	loadimage(&eboom[1], _T("images\\enemy1_down3.png"));
	loadimage(&eboom[2], _T("images\\enemy1_down4.png"));


	BK bk = BK(bkimg);
	Hero hp = Hero(heroimg);

	vector<Enemy*>  es;
	vector<Bullet*> bs;
	vector<EBullet*> ebs;


	//限制帧数
	int bsing = 0;

	clock_t hurtlast = clock();

	//击杀数
	unsigned long long kill = 0;

	for (int i = 0; i < 5; i++) {
		AddEnemy(es, enemying,eboom);
	}

	while (is_play)
	{
		bsing++;
		//每bsing帧加入一个子弹
		if (bsing % 10 == 0) {
			bs.push_back(new Bullet(bulletimg, hp.GetRect()));
		}

		if (bsing == 60) {
			bsing = 0;
			for (auto& i : es) {
				ebs.push_back(new EBullet(bulletimg, i->GetRect()));
			}
		}

		//防止屏闪，批量绘图
		BeginBatchDraw();

		bk.show();
		//停止
		Sleep(6);

		//暂停
		if (_kbhit()) {
			char v = _getch();
			if (v == VK_SPACE) {
				Sleep(500);
				while (true)
				{
					if (_kbhit()) { v = _getch(); if (v == VK_SPACE)break; }
					Sleep(16);
				}

			}
		}

		flushmessage();
		Sleep(6);

		hp.Control();
		hp.Show();

		//子弹绘制，删除
		auto bsit = bs.begin();
		while (bsit != bs.end())
		{
			if (!(*bsit)->Show()) {
				bsit = bs.erase(bsit);
			}
			else {
				bsit++;
			}
		}

		//敌机子弹绘制，删除
		auto ebsit = ebs.begin();
		while (ebsit != ebs.end())
		{
			if (RectDuangRect((*ebsit)->GetRect(), hp.GetRect())) {
				if (clock() - hurtlast >= hurttime) {
					is_play = hp.hurt();
					hurtlast = clock();
				}
			}

			if (!(*ebsit)->Show()) {
				ebsit = ebs.erase(ebsit);
			}

			else {
				ebsit++;
			}
		}

		//是否发生碰撞，并且显示
		auto it = es.begin();
		while (it!=es.end())
		{
			if (RectDuangRect((*it)->GetRect(), hp.GetRect())) {
				if (clock() - hurtlast >= hurttime) {
					is_play = hp.hurt();
					hurtlast = clock();
				}
			}
			//检验是否和子弹相撞
			auto bit = bs.begin();
			while (bit != bs.end())
			{
				if (RectDuangRect((*bit)->GetRect(), (*it)->GetRect())) {
					(*it)->Isdie();
					delete (*bit);
					bs.erase(bit);

					kill++;
					break;
				}
				bit++;
			}

			if (!(*it)->Show()) {
				delete (*it);
				es.erase(it);
				it = es.begin();
			}
			else it++;
		}

		for (int i = 0; i < 5 - es.size(); i++) {
			AddEnemy(es, enemying,eboom);
		}


		//结束时关闭批量绘图
		EndBatchDraw();
	}
	
	//游戏结束画面
	Over(kill);

	return true;
}

int main()
{
	// easyx初始化
	initgraph(swidth, sheight, EW_NOMINIMIZE | EW_SHOWCONSOLE);
	bool is_live = true;
	while (is_live)
	{
		Welcome();

		is_live = Play();

	}

	return 0;
}