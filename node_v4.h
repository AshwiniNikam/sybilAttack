#include <iostream>
#include <set>
#include <map>
#include <vector>
using namespace std;
class Head;
class Node;

struct Position                                         // position(x,y)
{
	double x;
	double y;
};


struct PaFromNode                                          // node给head发包类型
{
	int id;
	double energy;
	Position posi;
	Head* otherHead;
	Node* thisnode;
};

struct PaFromHead                                      // head给head发包类型
{
	int id;
	double energy;
	Position posi;
	Head* toHead;
	Head* fromHead;
	Node* thisnode;
	double dis;

};


class Head
{
	//friend void sendPackage_fir(Head * fir, Head * sec);		 // 让node有权限更改信息，即发包
	//friend void sendPackage_sec(Head * fir, Head * sec);		 // 让node有权限更改信息，即发包

public:
	Head::Head(){};
	Head(int id);											 // head的id为1，2 ……

	bool isSmaller(Head * another);                          // 比较两个head的id大小， id小的head给id大的head发包
	void headToHead_fir(Head* another);                      // 第一次发包给另一个head
	void headToHead_sec(Head* another);						 // 第二次发包给另一个head

	double calcu(int nodeid);

	map<int, double> engStrg_fir;
	map<int, double> engStrg_sec;
	map<int, double> engStrg_last;


	map<int, PaFromNode > packFromNode_fir;
	map<int, PaFromHead > packFromHead_fir;
	map<int, PaFromNode > packFromNode_sec;
	map<int, PaFromHead > packFromHead_sec;

	void judge();
	void clear();

	Position posi;
	int id;
};

class Node
{
public:
	Node(int id);										// node的id为1，2……
	void selehead(set<Head*> allheads);					// 计算与每个head的距离，选择最近的两个head
	void sendPackage_fir(Head * fir, Head * sec);       // 第一次向最近的两个head发包
	void sendPackage_sec(Head * fir, Head * sec);
	virtual void changeId();

	void engConsm();
	void changePosi();

	vector< Head*> closeHeads;
	void clear();

	Position posi;
	double energy;
	int originalid;
	int id;
};

class SybilNode :public::Node                        // sybil node 是 node 的子类
{
public:
	SybilNode() : Node(1){};
	void changeId();                                // 更改id
};
