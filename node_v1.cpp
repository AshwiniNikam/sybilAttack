#include <iostream>
#include <set>
#include <ctime>
#include <cmath>
#include "node.h"
using namespace std;

int sybilNum = 0;

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
}

Position *Head::getPosi()
{
	return posi;
}

Node::Node(int id)
{
	this->id = id;
	this->otherNodes.erase(this);

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

void Node::sendPackage(Head* fir, Head *sec)
{
	PaFromNode *pack;
	pack->energy = this->energy;
	pack->id = this->id;
	pack->otherHead = sec;
	pack->posi = this->posi;

	if (fir->packFromNode.find(this->id) != fir->packFromNode.end())
	{
		sybilNum++;
		return;
	}
	if (sec->packFromNode.find(this->id) != sec->packFromNode.end())
	{
		sybilNum++;
		return;
	}
	fir->packFromNode[this->id] = pack;                   // node 将 control package 发给两个head
	sec->packFromNode[this->id] = pack;
	fir->energyStorage[this->id]=  this->energy;
	sec->energyStorage[this->id] = this->energy;
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

void Head::headToHead(Head* another)
{
	if (!isSmaller(another)) return;

	map<int, PaFromNode*> ::iterator it;
	for (it = packFromNode.begin(); it != packFromNode.end(); it++)
	{
		if (it->second->otherHead != another) continue;

		double d = dis(posi, it->second->posi);
		
		PaFromHead* pack;
		pack-> id = it ->second->id;
		pack-> energy = it->second->energy;
		pack-> posi = it->second->posi;
		pack-> otherHead = it->second->otherHead;
		pack-> dis = d;

		packFromHead[it->second->id] = pack;
	}
}
