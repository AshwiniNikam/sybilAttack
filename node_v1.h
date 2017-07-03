#include <iostream>
#include <set>
#include <map>
using namespace std;

struct PaFromNode                                          // node给head发包类型
{
	int id;
	double energy;
	Position *posi;
	Head* otherHead;
};

struct PaFromHead                                      // head给head发包类型
{
	int id;
	double energy;
	Position *posi;
	Head* otherHead;
	double dis;
};

struct Position                                         // position(x,y)
{
	double x;
	double y;
};
class Head
{
	friend void sendPackage(Head * fir, Head * sec);    // 让node有权限更改信息，即发包

public:
	Head(int id);                                       // head的id为1，2 ……
	Position *getPosi();
	int getId();

	void headToHead(Head* another);                          // 发包给另一个head
	bool isSmaller(Head * another);                          // 比较两个head的id大小， id小的head给id大的head发包

	map<int , double> energyStorage;
	map<int , PaFromNode*> packFromNode;
	map<int , PaFromHead*> packFromHead;


private:
	Position *posi;
	int id;
};


class Node
{
public:
	Node(int id);                                  // node的id为1，2……
	void selehead(map<int, Head*> allheads);       // 计算与每个head的距离，选择最近的两个head
	void sendPackage(Head * fir, Head * sec);      // 向最近的两个head发包
private:
	set<Node*> otherNodes;
	int id;
	Position *posi;
	set< Head*> closeHeads;
	double energy;
};

class SybilNode:public::Node                        // sybil node 是 node 的子类
{
public:
	void changeId();

};