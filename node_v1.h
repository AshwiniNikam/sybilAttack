#include <iostream>
#include <set>
#include <map>
using namespace std;

struct PaFromNode                                          // node��head��������
{
	int id;
	double energy;
	Position *posi;
	Head* otherHead;
};

struct PaFromHead                                      // head��head��������
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
	friend void sendPackage(Head * fir, Head * sec);    // ��node��Ȩ�޸�����Ϣ��������

public:
	Head(int id);                                       // head��idΪ1��2 ����
	Position *getPosi();
	int getId();

	void headToHead(Head* another);                          // ��������һ��head
	bool isSmaller(Head * another);                          // �Ƚ�����head��id��С�� idС��head��id���head����

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
	Node(int id);                                  // node��idΪ1��2����
	void selehead(map<int, Head*> allheads);       // ������ÿ��head�ľ��룬ѡ�����������head
	void sendPackage(Head * fir, Head * sec);      // �����������head����
private:
	set<Node*> otherNodes;
	int id;
	Position *posi;
	set< Head*> closeHeads;
	double energy;
};

class SybilNode:public::Node                        // sybil node �� node ������
{
public:
	void changeId();

};