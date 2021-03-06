#include <iostream>
#include <set>
#include <map>
#include "node.h"
using namespace std;
#define BORDER_X = 100;
#define BORDER_Y = 100;

extern set<Node*> nodeset;
extern set<Head*> headset;
extern int truenum ;
extern int tpnum ;
extern int falsenum ;
extern int fnnum ;


int main()
{
	Head* h1 = new Head(1);
	Head* h2 = new Head(2);
	Head* h3 = new Head(3);
	Head* h4 = new Head(4);
	headset.insert(h1);
	headset.insert(h2);
	headset.insert(h3);
	headset.insert(h4);

	for (int i = 0; i < 10; i++)
	{
		Node* node = new Node(i);
		nodeset.insert(node);
	}

	for (int i = 0; i < 10; i++)
	{
		SybilNode* sybnode = new SybilNode;
		nodeset.insert(sybnode);
	}

	for (Node* node : nodeset)              // first round
	{
		node->changePosi();
		node->changeId();
		node->selehead(headset);
		node->sendPackage_fir(node->closeHeads[0], node->closeHeads[1]);
	}

	for (Head* head : headset)
	{		
		map<int, PaFromNode>::iterator it;
		for (it = head->packFromNode_fir.begin(); it != head->packFromNode_fir.end(); it++)
		{
			head->headToHead_fir(it->second.otherHead);	
		}	
	}

	for (Node* node : nodeset)               // second round
	{
		node->changeId();
		node->sendPackage_sec(node->closeHeads[0], node->closeHeads[1]);
	}
	for (Head* head : headset)
	{
		map<int, PaFromNode>::iterator it;
		for (it = head->packFromNode_sec.begin(); it != head->packFromNode_sec.end(); it++)
		{
			head->headToHead_fir(it->second.otherHead);
		}
	}
	for (Head* head : headset)
	{
		head->judge();
	}

	cout <<"True Positive Rate:" << tpnum / truenum << endl;
	cout << "False Positive Rate:" << fnnum / falsenum << endl;
	system("pause");
	return 0;



}
