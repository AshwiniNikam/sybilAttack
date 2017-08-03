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
	double h1x = h1->posi.x;
	double h1y = h1->posi.y;
	double h2x = h2->posi.x;
	double h2y = h2->posi.y;
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
	srand((unsigned)time(0)+rand());
	int i = rand() % num;
	return i;
}

void SybilNode::changeId()
{
	this->id = changeNum(100);
}

void Node::changeId(){ return; }

double dis(Position fir, Position sec)
{
	double d = sqrt((fir.x - sec.x)*(fir.x - sec.x) + (fir.y - sec.y)*(fir.y - sec.y));
	return d;
}

Position genePosi(int border_x, int border_y)
{
	srand((unsigned)time(0)+rand());
	int x = rand() % (border_x *100);
	double posi_x = x / 100.0;
	srand((unsigned)time(0) + rand());
	int y = rand() % (border_x * 100);
	double posi_y = y / 100.0;

	Position posi;
	posi.x = posi_x;
	posi.y = posi_y;
	return posi;
}

Head::Head(int id)
{
	this->id = id;
	switch (id)
	{
	case 1:
		this->posi.x = 25;
		this->posi.y = 25;
		break;

	case 2:
		this->posi.x = 75;
		this->posi.y = 25;
		break;
	case 3:
		this->posi.x = 25;
		this->posi.y = 75;
		break;
	case 4:
		this->posi.x = 75;
		this->posi.y = 75;
		break;
	}

}

bool Head::isSmaller(Head * another)
{
	int myId = this->id;
	int anoId = another->id;
	if (myId == 4 && anoId == 0) return true;
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
	srand((unsigned)time(0)+rand());
	int tmp_x = rand() % 500;
	double delta_x = double(tmp_x) / 1000;

	double delta_y = sqrt(r*r - delta_x * delta_x);

	srand((unsigned)time(0)+rand());
	int xdrct = rand() % 2;

	srand((unsigned)time(0)+rand());
	int ydrct = rand() % 2;

	double new_x = 0, new_y = 0;
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
	Head* dupdup = new Head;

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
		if (dis < closesec)
		{
			dupdup = head;
			closesec = dis;
		}
	}
	closeHeads.push_back(dup);
	closeHeads.push_back(dupdup);
} 

void Node::clear()
{
	closeHeads.clear();
}

void Head::clear()
{
	packFromHead_fir.clear();
	packFromHead_sec.clear();
	packFromNode_fir.clear();
	packFromNode_sec.clear();
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
		}
		else																	  // 包中这个id是假节点
		{
		/*	map<int, PaFromNode>::iterator mapit_fir = fir->packFromNode_fir.find(this->id);
			if (mapit_fir != fir->packFromNode_fir.end())       
			{		
				fir->packFromNode_fir.erase(mapit_fir);
			}

			set<Node*>::iterator setit_fir = nodeset.find(fir->packFromNode_fir[this->id].thisnode);
			if (setit_fir != nodeset.end())
			{	
				nodeset.erase(setit_fir);
			} */
			falsenum++;
			fnnum++;

			if (this->id != this->originalid)
			{
				falsenum++;
				fnnum++;
			/*	set<Node*>::iterator setit_sec = nodeset.find(this);
				if (setit_sec != nodeset.end())
				{
					
					nodeset.erase(setit_sec);
				}
				*/
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
	
	PaFromNode pack_otr;
	pack_otr.energy = this->energy;
	pack_otr.id = this->id;
	pack_otr.otherHead = fir;
	pack_otr.posi = this->posi;
	pack_otr.thisnode = this;

	if (sec->packFromNode_fir.find(this->id) != sec->packFromNode_fir.end())   // 第2轮发给sec出现id冲突 
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
		/*	map<int, PaFromNode>::iterator mapit_sec = sec->packFromNode_fir.find(this->id);
			if (mapit_sec != sec->packFromNode_fir.end())
			{
				
				sec->packFromNode_fir.erase(mapit_sec);
			}

			set<Node*>::iterator setit_trd = nodeset.find(sec->packFromNode_fir[this->id].thisnode);
			if (setit_trd != nodeset.end())
			{
				
				nodeset.erase(setit_trd);
			}*/
			falsenum++;
			fnnum++;
			if (this->id != this->originalid)
			{
				falsenum++;
				fnnum++;
			/*	set<Node*>::iterator setit_fth = nodeset.find(this);
				if (setit_fth != nodeset.end())
				{
					nodeset.erase(setit_fth);
				} */
				return;
			}
			else
			{
				sec->packFromNode_fir[this->id] = pack_otr;
			}
		}
	}
	else
	{
		sec->packFromNode_fir[this->id] = pack_otr;
		sec->engStrg_fir[this->id] = this->energy;
	}
}

void Node::sendPackage_sec(Head* fir, Head *sec)
{
	PaFromNode pack;
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
			
		}
		else
		{
		/*	if (fir->packFromNode_sec.find(this->id) != fir->packFromNode_sec.end())
			{
				map<int, PaFromNode>::iterator itposi = fir->packFromNode_sec.find(this->id);
				fir->packFromNode_sec.erase(itposi);
			}
			if (nodeset.find(fir->packFromNode_sec[this->id].thisnode) != nodeset.end())
			{
				set<Node*>::iterator itposi = nodeset.find(fir->packFromNode_sec[this->id].thisnode);
				nodeset.erase(itposi);
			} */
			falsenum++;
			fnnum++;
			if (this->id != this->originalid)
			{
				falsenum++;
				fnnum++;
			/*	if (nodeset.find(this) != nodeset.end())
				{
					set<Node*>::iterator itposi = nodeset.find(this);
					nodeset.erase(itposi);
				} */
				
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
	
	PaFromNode pack_otr;
	pack_otr.energy = this->energy;
	pack_otr.id = this->id;
	pack_otr.otherHead = fir;
	pack_otr.posi = this->posi;
	pack_otr.thisnode = this;

	if (sec->packFromNode_sec.find(this->id) != sec->packFromNode_sec.end())   // 第2轮发包出现id冲突 
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
		/*	if (sec->packFromNode_sec.find(this->id) != sec->packFromNode_sec.end())
			{
				map<int, PaFromNode>::iterator itposi = sec->packFromNode_sec.find(this->id);
				sec->packFromNode_sec.erase(itposi);
			}
			if (nodeset.find(sec->packFromNode_sec[this->id].thisnode) != nodeset.end())
			{
				set<Node*>::iterator itposi = nodeset.find(sec->packFromNode_sec[this->id].thisnode);
				nodeset.erase(itposi);
			}*/
			falsenum++;
			fnnum++;
			if (this->id != this->originalid)
			{
				falsenum++;
				fnnum++;
			/*	if (nodeset.find(this) != nodeset.end())
				{
					set<Node*>::iterator itposi = nodeset.find(this);
					nodeset.erase(itposi);
				} */
				return;
			}
			else
			{
				sec->packFromNode_sec[this->id] = pack_otr;
			}
		}
	}
	else
	{
		sec->packFromNode_sec[this->id] = pack_otr;
		sec->engStrg_sec[this->id] = this->energy;
	}
}

void Head::headToHead_fir(Head* another)
{
	if (!isSmaller(another)) return;

	map<int, PaFromNode> ::iterator it;
	for (it = this->packFromNode_fir.begin(); it != this->packFromNode_fir.end(); it++)
	{
		if (it == this->packFromNode_fir.end()) return;
		if (it->second.otherHead != another) continue;

		double d = dis(posi, it->second.posi);

		PaFromHead pack;
		pack.id = it->second.id;
		pack.energy = it->second.energy;
		pack.posi = it->second.posi;
		pack.toHead = it->second.otherHead;
		pack.dis = d;
		pack.thisnode = it->second.thisnode;
		pack.fromHead = this;

		another->packFromHead_fir[it->second.id] = pack;
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

		PaFromHead pack;
		pack.id = it->second.id;
		pack.energy = it->second.energy;
		pack.posi = it->second.posi;
		pack.toHead = it->second.otherHead;
		pack.dis = d;
		pack.thisnode = it->second.thisnode;
		pack.fromHead = this;

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

void Head::judge()
{
	map<int, PaFromHead>::iterator it;
	for (it = packFromHead_sec.begin(); it != packFromHead_sec.end(); it++)
	{
		int nodeid = it->first;
		if (packFromHead_fir.find(nodeid) != packFromHead_fir.end())
		{
			double rssi = calcu(nodeid);
			double km = kmax(packFromHead_fir[nodeid].posi, this, packFromHead_fir[nodeid].fromHead);
			if ((rssi < (1 / km)) || (rssi > km))
			{
				if (nodeset.find(packFromHead_fir[nodeid].thisnode)!= nodeset.end())
				{
					nodeset.erase(packFromHead_fir[nodeid].thisnode);
				}
			
				falsenum++;
				if (packFromHead_fir[nodeid].thisnode->originalid != nodeid) fnnum++;

			}
			else
			{
				truenum++;
				if (packFromHead_fir[nodeid].thisnode->originalid == nodeid) tpnum++;
			}
			
		}
		else
		{
			falsenum++;
			if (it->second.thisnode->originalid != it->second.id) fnnum++;
		}
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