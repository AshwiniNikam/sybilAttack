#include <iostream>
#include <set>
#include <ctime>
#include <cmath>
#include "node.h"
using namespace std;
const double v = 0.5;
const double t = 1;
const double r = 0.5;
int sybilNum = 2;
int sybilFind = 0;
set<Node*> nodeset;
map<int, Node*>  nodemap;

double kmax(Position * nodeposi, Head* h1, Head* h2)
{
	double nodex = nodeposi->x;
	double nodey = nodeposi->y;
	double h1x =  h1->posi->x;
	double h1y =  h1->posi->y;
	double h2x =  h2->posi->x;
	double h2y =  h2->posi->y;
	double c = (h2x - h1x) / 2;
	double x1 = nodex - h1x / 2 - h2x / 2;
	double y1 = nodey - h1y / 2 - h2y / 2;
	double A = 4 * x1*x1 * c*c - 4 * r*r * c*c;
	double B = -4 * x1 * c *(x1*x1 + y1*y1 + c*c - r*r);
	double C = x1*x1 + y1*y1 + c*c - r*r + r*r*c*c;
	double t = (-B + sqrt(B*B - 4 * A*C)) / (2 * A);
	double k = (t + 1) / (t - 1);
	return k;
}

int changeNum(int num)
{
	srand((unsigned)time(0));
	int i = rand() / num;
	return i;
}

void SybilNode::changeId()
{
	this->id = changeNum(4);
}
void Node::changeId(){ return; }

double dis(Position *fir, Position *sec)
{
	double d = sqrt((fir->x - sec->x)*(fir->x - sec->x) + (fir->y - sec->y)*(fir->y - sec->y));
	return d;
}

Position * genePosi(int border_x, int border_y)
{
	srand((unsigned)time(0));
	int i = rand() / (border_x * border_y);
	double inner_x = double(i) / border_y;
	double inner_y = double(i) / border_x;
	Position *posi;
	posi->x = inner_x;
	posi->y = inner_y;
	return posi;
}

Head::Head(int id)
{
	this->id = id;
	switch (id)
	{
	case 1: 
		posi->x = 25;
		posi->y = 25;

	case 2:
		posi->x = 75;
		posi->y = 25;
	case 3:
		posi->x = 25;
		posi->y = 75;
	case 4:
		posi->x = 75;
		posi->y = 75;
	}
}

int Head::getId()
{
	return this->id;
}

bool Head::isSmaller(Head * another)
{
	int myId = this->id;
	int anoId = another->id;
	return myId < anoId;
}

Position *Head::getPosi()
{
	return posi;
}

Node::Node(int id)
{
	this->id = id;
	this->otherNodes.erase(this);
	this->originalid = id;
}

void Node::changePosi()
{
	srand((unsigned)time(0));
	int tmp_x = rand() / 500;
	double delta_x = double(tmp_x) / 1000;
	
	double delta_y = sqrt(r*r - delta_x * delta_x);

	srand((unsigned)time(0));
	int xdrct = rand() / 2;

	srand((unsigned)time(0));
	int ydrct = rand() / 2;

	double new_x = 0 , new_y = 0;
	switch (xdrct)
	{
	case 0: new_x = posi->x + delta_x;
	case 1: new_x = posi->x - delta_x;
	}
	switch (ydrct)
	{
	case 0: new_y = posi->y + delta_y;
	case 1: new_y = posi->y - delta_y;
	}

	if (new_x <= 0 || new_x >= 100) return;
	if (new_y <= 0 || new_y >= 100) return;
	posi->x = new_x;
	posi->y = new_y;
}

void Node::selehead(map<int, Head*> allheads)
{
	closeHeads.clear();

	double closefir = INFINITY;
	double closesec = INFINITY;
	map<int, Head*> ::iterator it;
	map<int, Head*> ::iterator dupit;
	map<int, Head*> ::iterator dupdupit;
	for (it = allheads.begin(); it != allheads.end(); it++)
	{
		Head* head = it->second;	
		Position *headposi = head->getPosi();
		double dis = sqrt((posi->x - headposi->x)*(posi->x - headposi->x) + (posi->y - headposi->y)*(posi->y - headposi->y));
		if (dis < closefir) 
		{ 
			dupit = it;
			closefir = dis; 
		}
	}
	for (it = allheads.begin(); it != allheads.end(); it++)
	{
		if (dupdupit == dupit) continue;        
		Head* head = it->second;
		Position *headposi = head->getPosi();
		double dis = sqrt((posi->x - headposi->x)*(posi->x - headposi->x) + (posi->y - headposi->y)*(posi->y - headposi->y));
		if (dis < closefir)
		{
			dupdupit = it;
			closesec = dis;
		}
	}
	closeHeads.insert(dupit->second);
	closeHeads.insert(dupdupit->second);
}

void Node::sendPackage_fir(Head* fir, Head *sec)
{
	PaFromNode *pack;
	pack->energy = this->energy;
	pack->id = this->id;
	pack->otherHead = sec;
	pack->posi = this->posi;
	pack->myself = this;                 // 要改

	fir->packFromNode_fir[this->id].push_back (pack);                // node 将 control package 发给两个head
	sec->packFromNode_fir[this->id].push_back (pack);
	fir->energyStorage[this->id] =  this->energy;
	sec->energyStorage[this->id] =  this->energy;
}

void Node::sendPackage_sec(Head* fir, Head *sec)
{
	PaFromNode *pack;
	pack->energy = this->energy;
	pack->id = this->id;
	pack->otherHead = sec;
	pack->posi = this->posi;
	pack->myself = this;                 // 要改

	fir->packFromNode_sec[this->id].push_back(pack);                // node 将 control package 发给两个head
	sec->packFromNode_sec[this->id].push_back(pack);
	fir->energyStorage[this->id] = this->energy;
	sec->energyStorage[this->id] = this->energy;
}


void Head::headToHead_fir(Head* another)
{
	if (!isSmaller(another)) return;

	map<int, vector<PaFromNode*>> ::iterator it;
	for (it = packFromNode_fir.begin(); it != packFromNode_fir.end(); it++)
	{
		if (it->second[0]->otherHead != another) continue;

		double d = dis(posi, it->second[0]->posi);
		
		PaFromHead* pack;
		pack-> id = it ->second[0]->id;
		pack-> energy = it->second[0]->energy;
		pack-> posi = it->second[0]->posi;
		pack-> toHead = it->second[0]->otherHead;
		pack-> dis = d;

		packFromHead_fir[it->second[0]->id].push_back( pack);
	}
}

void Head::headToHead_sec(Head* another)
{
	if (!isSmaller(another)) return;

	// 判断map<int , vector<PaFromHead*>> 里vector的size

	map<int, vector<PaFromNode*>> ::iterator it;
	for (it = packFromNode_sec.begin(); it != packFromNode_sec.end(); it++)
	{
		if (it->second.size() > 1)                     // 若第二轮收到的包中有多个相同的id
		{
			vector<PaFromNode*> newvec;
			for (int i = 0; i < it->second.size(); i++)
			{
				int nodeid = it->second[i]->id;
				if (it->second[i]->myself->originalid != nodeid)  // 这是sybil node
				{
					nodemap.erase(it->second[i]->id);
					sybilFind++ ;
				}
				else
				{
					newvec.push_back(it->second[i]);

				}
			}
			packFromNode_sec[it->first] = newvec;	
		}

		if (it->second[0]->otherHead != another) continue;

		double d = dis(posi, it->second[0]->posi);

		PaFromHead* pack;
		pack->id = it->second[0]->id;
		pack->energy = it->second[0]->energy;
		pack->posi = it->second[0]->posi;
		pack->toHead = it->second[0]->otherHead;
		pack->dis = d;

		packFromHead_sec[it->second[0]->id].push_back(pack);
	}
}

double Head::calcu_fir(int nodeid)
{
	double d2 = dis(posi, packFromNode_fir[nodeid][0]->posi);
	double d1 = packFromHead_fir[nodeid][0]->dis;
	double rssi = d1 / d2;
	rssiStorage_fir[nodeid] = rssi;
}

double Head::calcu_sec(int nodeid)
{
	double d2 = dis(posi, packFromNode_sec[nodeid][0]->posi);
	double d1 = packFromHead_sec[nodeid][0]->dis;
	double rssi = d1 / d2;
	rssiStorage_sec[nodeid] = rssi;
}

void Head::judge()
{
	while (!packFromHead_fir.empty())                 // 处理两轮id相同的
	{
		map<int, vector<PaFromHead*>>::iterator it;
		it = packFromHead_fir.begin();

		if (packFromHead_sec.find(id) != packFromHead_sec.end())
		{
			double rssi = calcu_sec(id);
			double km = kmax(packFromHead_sec[id][0]->posi, this, packFromHead_sec[id][0]->fromHead);
			if (rssi > (1 / km) &&  rssi < km)  
			{
				packFromHead_fir.erase[id];
				packFromHead_sec.erase[id];
			}		
		}
	}
	if (packFromHead_fir.size() == 1   &&  packFromHead_sec.size() == 1)
	{
		double d1 = packFromHead_fir[0][0]->dis;
		double d2 = packFromHead_sec[0][0]->dis;
		double rssi = d1 / d2;
		double km = kmax(packFromHead_sec[id][0]->posi, this, packFromHead_sec[id][0]->fromHead);
		
		if (rssi < (1 / km) || rssi > km)       // 这是sybil node
		{
			sybilFind++;

		}
		
	}


}
