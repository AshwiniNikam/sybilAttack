#include <iostream>
#include <set>
#include <map>
#include <vector>
using namespace std;

struct PaFromNode                                          // node��head��������
{
	int id;
	double energy;
	Position *posi;
	Head* otherHead;
	Node* myself;
};

struct PaFromHead                                      // head��head��������
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
	friend void sendPackage(Head * fir, Head * sec);		 // ��node��Ȩ�޸�����Ϣ��������

public:
	Head(int id);											 // head��idΪ1��2 ����
	Position *getPosi();
	int getId();
	
	bool isSmaller(Head * another);                          // �Ƚ�����head��id��С�� idС��head��id���head����
	void headToHead_fir(Head* another);                      // ��һ�η�������һ��head
	void headToHead_sec(Head* another);						 // �ڶ��η�������һ��head

	double calcu_fir(int nodeid);
	double calcu_sec(int nodeid);

	map<int, double>  rssiStorage_fir;                       // ��¼��һ�ּ����rssi
	map<int, double>  rssiStorage_sec;                       // ��¼�ڶ��ּ����rssi

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
	Node(int id);                                  // node��idΪ1��2����
	void selehead(map<int, Head*> allheads);       // ������ÿ��head�ľ��룬ѡ�����������head
	void sendPackage_fir(Head * fir, Head * sec);      // ��һ�������������head����
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

class SybilNode:public::Node                        // sybil node �� node ������
{
public:
	void changeId();                                // ����id
};