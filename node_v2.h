#include <iostream>
#include <set>
#include <map>
#include <vector>
using namespace std;

struct PaFromNode                                          // node给head发包类型
{
	int id;
	double energy;
	Position *posi;
	Head* otherHead;
	Node* myself;
};

struct PaFromHead                                      // head给head发包类型
{
	int id;
	double energy;
	Position *posi;
	Head* toHead;
	Head* fromHead;
	double dis;
	
};

struct Position                                         // position(x,y)
{
	double x;
	double y;
};

class Head
{
	friend void sendPackage(Head * fir, Head * sec);		 // 让node有权限更改信息，即发包

public:
	Head(int id);											 // head的id为1，2 ……
	Position *getPosi();
	int getId();
	
	bool isSmaller(Head * another);                          // 比较两个head的id大小， id小的head给id大的head发包
	void headToHead_fir(Head* another);                      // 第一次发包给另一个head
	void headToHead_sec(Head* another);						 // 第二次发包给另一个head

	double calcu_fir(int nodeid);
	double calcu_sec(int nodeid);

	map<int, double>  rssiStorage_fir;                       // 记录第一轮计算的rssi
	map<int, double>  rssiStorage_sec;                       // 记录第二轮计算的rssi

	map<int , double> energyStorage;

	
	map<int , vector<PaFromNode*>  > packFromNode_fir;
	map<int , vector<PaFromHead*>  > packFromHead_fir;
	map<int,  vector<PaFromNode*>  > packFromNode_sec;
	map<int,  vector<PaFromHead*>  > packFromHead_sec;

	void judge();

	Position *posi;
	int id;
};


class Node
{
public:
	Node(int id);                                  // node的id为1，2……
	void selehead(map<int, Head*> allheads);       // 计算与每个head的距离，选择最近的两个head
	void sendPackage_fir(Head * fir, Head * sec);      // 第一次向最近的两个head发包
	void sendPackage_sec(Head * fir, Head * sec);
	virtual void changeId();
	int id;

	void changePosi();
	set<Node*> otherNodes;
	Position *posi;
	set< Head*> closeHeads;
	double energy;
    int originalid;
};

class SybilNode:public::Node                        // sybil node 是 node 的子类
{
public:
	void changeId();                                // 更改id
};