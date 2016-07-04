
// Headers
#include "../basic_lib.h"
#include "../node/node.h"
#include "../entry/entry.h"
#include "../event/event.h"
#include "../packet/packet.h"
#include "../mylist/mylist.h"

// Switch class
#ifndef SWITCH_H
#define SWITCH_H
class Switch: public Node{
	public:
		Switch(int);
		Event forward(double,Packet);
		vector<Packet>que;
		map<Packet,bool>isSetup;
		double posXY[2];
		vector<EDGE>wlink;
		vector< vector<int> >iList;
		vector<Entry> cache;
		double APrate;
		MyLL TCAMactive;
		//MyLL TCAMinactive;
		map<Packet,LLNODE*>TCAMmapA;
		//map<Packet,LLNODE*>TCAMmapI;
		void Add_Rule_Replacement(void);
		int Get_Rule_Replacement(void); 		 
	private:
		int num_rule_replacement;
};
#endif
