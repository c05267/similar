
// Headers
#include "../basic_lib.h"
#include "../packet/packet.h"
#include "../fattree/fattree.h"
#include "../entry/entry.h"

// Install event and forward
void Fattree::endTransmission(double timeStamp, Packet pkt){

	// Variables
	int nowFlowID;
	int nid;
	Entry ent, tmpent;

	// Update count of alive flows
	//aliveFlow[pkt] --;

	// All such flows are done
	//if(aliveFlow[pkt] == 0){

		// All switches along the path
		nowFlowID = rcdFlowID[pkt];
		printf("End Transmission: %d, %d \n", pkt.getSrcPort(), pkt.getDstPort());
		for(int i = 0; i < allEntry[nowFlowID].size(); i++){

			// Move from active to inactive
			ent = allEntry[nowFlowID][i];
			nid = ent.getSID();

			// Remove from active
			if(sw[nid]->TCAMmapA.count(pkt) > 0){
				sw[nid]->TCAMactive.remove(sw[nid]->TCAMmapA[pkt]);
				sw[nid]->TCAMmapA.erase(pkt);
				//printf("flow id: %d, switch id: %d \n", pkt.getSequence(), nid);

				// Install at the tail (LRU)
				/*if(sw[nid]->TCAMmapI.count(pkt) == 0)
					sw[nid]->TCAMmapI[pkt] = sw[nid]->TCAMinactive.push_back(ent);*/
			}
			
			for(int i=0; i< sw[nid]->cache.size(); i++)
			{
				tmpent = sw[nid]->cache[i];
				if(tmpent.getSrcPort() == ent.getSrcPort() && tmpent.getDstPort() == ent.getDstPort())
				{
					sw[nid]->cache.erase(sw[nid]->cache.begin() + i);
					//printf("We remove a leaved flow entry from the cache \n");
				}
			}
		}
	//}
}
