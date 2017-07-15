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


struct PaFromNode                                          // node��head��������
{
	int id;
	double energy;
	Position posi;
	Head* otherHead;
	Node* thisnode;
};

struct PaFromHead                                      // head��head��������
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
	friend void sendPackage(Head * fir, Head * sec);		 // ��node��Ȩ�޸�����Ϣ��������

public:
	Head::Head(){};
	Head(int id);											 // head��idΪ1��2 ����
	
	bool isSmaller(Head * another);                          // �Ƚ�����head��id��С�� idС��head��id���head����
	void headToHead_fir(Head* another);                      // ��һ�η�������һ��head
	void headToHead_sec(Head* another);						 // �ڶ��η�������һ��head

	double calcu(int nodeid);
	
	map<int ,double> engStrg_fir;
	map<int ,double> engStrg_sec;
	map<int ,double> engStrg_last;
	

	map<int , PaFromNode > packFromNode_fir;
	map<int , PaFromHead > packFromHead_fir;
	map<int,  PaFromNode > packFromNode_sec;
	map<int,  PaFromHead > packFromHead_sec;

	void judge();
	void clear();

	Position posi;
	int id;
}; 

class Node
{
public:
	Node(int id);										// node��idΪ1��2����
	void selehead(set<Head*> allheads);					// ������ÿ��head�ľ��룬ѡ�����������head
	void sendPackage_fir(Head * fir, Head * sec);       // ��һ�������������head����
	void sendPackage_sec(Head * fir, Head * sec);	
	virtual void changeId();
	
	void engConsm();
	void changePosi();
	
	vector< Head*> closeHeads;

	Position posi;
	double energy;
    int originalid;
	int id;
};

class SybilNode:public::Node                        // sybil node �� node ������
{
public:
	SybilNode() : Node(1){};
	void changeId();                                // ����id
};