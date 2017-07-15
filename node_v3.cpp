#include <iostream>
#include <set>
#include <ctime>
#include <cmath>
#include "node.h"
using namespace std;

const double v = 0.5;
const double t = 1;
const double r = 0.5;
double recevEng = 1;
double sendEng = 0.5;
int truenum = 0;
int tpnum = 0;
int falsenum = 0;
int fnnum = 0;

set<Node*> nodeset;
set<Head*> headset;

double kmax(Position  nodeposi, Head* h1, Head* h2)
{
	double nodex = nodeposi.x;
	double nodey = nodeposi.y;
	double h1x =  h1->posi.x;
	double h1y =  h1->posi.y;
	double h2x =  h2->posi.x;
	double h2y =  h2->posi.y;
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
	int i = rand() % num;
	return i;
}

void SybilNode::changeId()
{
	this->id = changeNum(10);
}
void Node::changeId(){ return; }

double dis(Position fir, Position sec)
{
	double d = sqrt((fir.x - sec.x)*(fir.x - sec.x) + (fir.y - sec.y)*(fir.y - sec.y));
	return d;
}

Position genePosi(int border_x, int border_y)
{
	srand((unsigned)time(0));
	int i = rand() % (border_x * border_y);
	double inner_x = double(i) / border_y;
	double inner_y = double(i) / border_x;
	Position posi;
	posi.x = inner_x;
	posi.y = inner_y;
	return posi;
}

Head::Head(int id)
{
	this->id = id;
	switch (id)
	{
	case 1: 
		posi.x = 25;
		posi.y = 25;

	case 2:
		posi.x = 75;
		posi.y = 25;
	case 3:
		posi.x = 25;
		posi.y = 75;
	case 4:
		posi.x = 75;
		posi.y = 75;
	}
}

bool Head::isSmaller(Head * another)
{
	int myId = this->id;
	int anoId = another->id;
	return myId < anoId;
}

Node::Node(int id)
{
	this->id = id;
	this->originalid = id;
	this->energy = 100;
	this->posi = genePosi(100, 100);
}

void Node::changePosi()
{
	srand((unsigned)time(0));
	int tmp_x = rand() % 500;
	double delta_x = double(tmp_x) / 1000;
	
	double delta_y = sqrt(r*r - delta_x * delta_x);

	srand((unsigned)time(0));
	int xdrct = rand() % 2;

	srand((unsigned)time(0));
	int ydrct = rand() % 2;

	double new_x = 0 , new_y = 0;
	switch (xdrct)
	{
	case 0: new_x = posi.x + delta_x;
	case 1: new_x = posi.x - delta_x;
	}
	switch (ydrct)
	{
	case 0: new_y = posi.y + delta_y;
	case 1: new_y = posi.y - delta_y;
	}

	if (new_x <= 0 || new_x >= 100) return;
	if (new_y <= 0 || new_y >= 100) return;
	posi.x = new_x;
	posi.y = new_y;
}

void Node::selehead(set<Head*> allheads)
{
	closeHeads.clear();

	double closefir = INFINITY;
	double closesec = INFINITY;
	Head* dup = new Head;
	Head* dupdup = new Head ;
	
	for (Head* head : allheads)
	{
		Position  headposi = head->posi;
		double dis = sqrt((posi.x - headposi.x)*(posi.x - headposi.x) + (posi.y - headposi.y)*(posi.y - headposi.y));
		if (dis < closefir) 
		{ 
			dup = head;
			closefir = dis; 
		}
	}
	for (Head* head : allheads)
	{
		if (head == dup) continue;        
		Position headposi = head->posi;
		double dis = sqrt((posi.x - headposi.x)*(posi.x - headposi.x) + (posi.y - headposi.y)*(posi.y - headposi.y));
		if (dis < closefir)
		{
			dupdup = head;
			closesec = dis;
		}
	}
	closeHeads.push_back(dup);
	closeHeads.push_back(dupdup);
}

void Node::sendPackage_fir(Head* fir, Head *sec)
{
	PaFromNode pack;
	pack.energy = this->energy;
	pack.id = this->id;
	pack.otherHead = sec;
	pack.posi = this->posi;
	pack.thisnode = this;                 

	if (fir->packFromNode_fir.find(this->id) != fir->packFromNode_fir.end())   // 第一轮包中已有这个id 
	{
		if (fir->packFromNode_fir[this->id].thisnode->originalid == this->id)    // 包中这个id是真节点
		{
			truenum++;
			tpnum++;
			falsenum++;
			fnnum++;
			return;
		}
		else																	  // 包中这个id是假节点
		{
			if (fir->packFromNode_fir.find(this->id) != fir->packFromNode_fir.end())
			{
				fir->packFromNode_fir.erase(this->id);
			}
			if (nodeset.find(fir->packFromNode_fir[this->id].thisnode) != nodeset.end())
			{
				nodeset.erase(fir->packFromNode_fir[this->id].thisnode);
			}
			falsenum++;
			fnnum++;
			if (this->id != this->originalid)  
			{
				falsenum++;
				fnnum++;
				if (nodeset.find(this) != nodeset.end())
				{
					nodeset.erase(this);
				}
				return;
			}
			else 
			{
				fir->packFromNode_fir[this->id] = pack;			
			}
		}
	}
	else
	{
		fir->packFromNode_fir[this->id] = pack;               
		fir->engStrg_fir[this->id] = this->energy;
	}

	if (sec->packFromNode_fir.find(this->id) != sec->packFromNode_fir.end())   // 第一轮发给sec出现id冲突 
	{
		if (sec->packFromNode_fir[this->id].thisnode->originalid == this->id)
		{
			truenum++;
			tpnum++;
			falsenum++;
			fnnum++;
			return;
		}
		else
		{
			if (sec->packFromNode_fir.find(this->id) != sec->packFromNode_fir.end())
			{
				sec->packFromNode_fir.erase(this->id);
			}
			if (nodeset.find(sec->packFromNode_fir[this->id].thisnode) != nodeset.end())
			{
				nodeset.erase(sec->packFromNode_fir[this->id].thisnode);
			}
			falsenum++;
			fnnum++;
			if (this->id != this->originalid)
			{
				falsenum++;
				fnnum++;
				if (nodeset.find(this) != nodeset.end())
				{
					nodeset.erase(this);
				}
				return;
			}
			else
			{
				sec->packFromNode_fir[this->id] = pack;
			}
		}
	}
	else
	{
		sec->packFromNode_fir[this->id] = pack;
		sec->engStrg_fir[this->id] = this->energy;
	}
}

void Node::sendPackage_sec(Head* fir, Head *sec)
{
	PaFromNode pack ;
	pack.energy = this->energy;
	pack.id = this->id;
	pack.otherHead = sec;
	pack.posi = this->posi;
	pack.thisnode = this;                 // 要改

	if (fir->packFromNode_sec.find(this->id) != fir->packFromNode_sec.end())   // 第一轮发包出现id冲突 
	{
		if (fir->packFromNode_sec[this->id].thisnode->originalid == this->id)
		{
			truenum++;
			tpnum++;
			falsenum++;
			fnnum++;
			return;
		}
		else
		{
			if (fir->packFromNode_sec.find(this->id) != fir->packFromNode_sec.end())
			{
				fir->packFromNode_sec.erase(this->id);
			}
			if (nodeset.find(fir->packFromNode_sec[this->id].thisnode) != nodeset.end())
			{
				nodeset.erase(fir->packFromNode_sec[this->id].thisnode);

			}
			falsenum++;
			fnnum++;
			if (this->id != this->originalid)
			{
				falsenum++;
				fnnum++;
				if (nodeset.find(this) != nodeset.end())
				{
					nodeset.erase(this);
				}
				return;
			}
			else
			{
				fir->packFromNode_sec[this->id] = pack;
			}
		}
	}
	else
	{
		fir->packFromNode_sec[this->id] = pack;                
		fir->engStrg_sec[this->id] = this->energy;
	}

	if (sec->packFromNode_sec.find(this->id) != sec->packFromNode_sec.end())   // 第一轮发包出现id冲突 
	{
		if (sec->packFromNode_sec[this->id].thisnode->originalid == this->id)
		{
			truenum++;
			tpnum++;
			falsenum++;
			fnnum++;
			return;
		}
		else
		{
			if (sec->packFromNode_sec.find(this->id) != sec->packFromNode_sec.end())
			{
				sec->packFromNode_sec.erase(this->id);
			}
			if (nodeset.find(sec->packFromNode_sec[this->id].thisnode) != nodeset.end())
			{
				nodeset.erase(sec->packFromNode_sec[this->id].thisnode);
			}
			falsenum++;
			fnnum++;
			if (this->id != this->originalid)
			{
				falsenum++;
				fnnum++;
				if (nodeset.find(this) != nodeset.end())
				{
					nodeset.erase(this);
				}
				return;
			}
			else
			{
				sec->packFromNode_sec[this->id] = pack;
			}
		}
	}
	else
	{
		sec->packFromNode_sec[this->id] = pack;
		sec->engStrg_sec[this->id] = this->energy;
	}
}

void Head::headToHead_fir(Head* another)
{
	if (!isSmaller(another)) return;

	map<int, PaFromNode> ::iterator it;
	for (it = packFromNode_fir.begin(); it != packFromNode_fir.end(); it++)
	{
		if (it->second.otherHead != another) continue;

		double d = dis(posi, it->second.posi);
		
		PaFromHead pack ;
		pack.id = it ->second.id;
		pack. energy = it->second.energy;
		pack. posi = it->second.posi;
		pack. toHead = it->second.otherHead;
		pack. dis = d;
		pack.thisnode = it->second.thisnode;

		another->packFromHead_fir[it->second.id] = pack ;
	}
}

void Head::headToHead_sec(Head* another)
{
	if (!isSmaller(another)) return;

	map<int, PaFromNode> ::iterator it;
	for (it = packFromNode_sec.begin(); it != packFromNode_sec.end(); it++)
	{

		if (it->second.otherHead != another) continue;

		double d = dis(posi, it->second.posi);

		PaFromHead pack ;
		pack.id = it->second.id;
		pack.energy = it->second.energy;
		pack.posi = it->second.posi;
		pack.toHead = it->second.otherHead;
		pack.dis = d;
		pack.thisnode = it->second.thisnode;

		another->packFromHead_sec[it->second.id] = pack;
	}
}

double Head::calcu(int nodeid)
{
	double d2 = dis(posi, packFromNode_sec[nodeid].posi);
	double d1 = packFromHead_sec[nodeid].dis;
	double rssi = d1 / d2;
	return rssi;
}

void Head::clear()
{
	packFromHead_fir.clear();
	packFromHead_sec.clear();
	packFromNode_fir.clear();
	packFromNode_sec.clear();
}

void Head::judge()
{
	map<int, PaFromHead>::iterator it;
	for (it = packFromHead_fir.begin(); it != packFromHead_fir.end(); it++)
	{
		int nodeid = it->first;
		if (packFromHead_sec.find(nodeid) != packFromHead_sec.end())
		{
			double rssi = calcu(nodeid);
			double km = kmax(packFromHead_sec[nodeid].posi, this, packFromHead_sec[nodeid].fromHead);
			if (rssi > (1 / km) && rssi < km)
			{
				//nodeset.erase(packFromHead_sec[nodeid].thisnode);
				falsenum++;
				if (packFromHead_sec[nodeid].thisnode->originalid != nodeid) fnnum++;
			}
			else
			{
				truenum++;
				if (packFromHead_sec[nodeid].thisnode->originalid == nodeid) tpnum++;
			}
			//packFromHead_fir.erase(it);
			//packFromHead_sec.erase(it);
		}	
	}
	for (it = packFromHead_fir.begin(); it != packFromHead_fir.end(); it++)
	{
		falsenum++;
		if (it->second.thisnode->originalid != it->second.id) fnnum++;	
	}

}

void Node::engConsm()
{
	srand((unsigned)time(0));
	int recev_times = rand() / 5;
	srand((unsigned)time(0));
	int send_times = rand() / 5;

	energy -= recev_times * recevEng + send_times * sendEng;
}


