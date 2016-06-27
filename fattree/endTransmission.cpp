
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
	Entry ent;

	// Update count of alive flows
	//aliveFlow[pkt] --;

	// All such flows are done
	//if(aliveFlow[pkt] == 0){

		// All switches along the path
		nowFlowID = rcdFlowID[pkt];
		for(int i = 0; i < allEntry[nowFlowID].size(); i++){

			// Move from active to inactive
			ent = allEntry[nowFlowID][i];
			nid = ent.getSID();
			//printf("SID: %d, Flow ID: %d \n", nid, pkt.getSequence());
			//printf("A = %d, I = %d\n", sw[nid]->TCAMactive.size(), sw[nid]->TCAMinactive.size());
			// Remove from active
			if(sw[nid]->TCAMmapA.count(pkt) > 0){
				sw[nid]->TCAMactive.remove(sw[nid]->TCAMmapA[pkt]);
				sw[nid]->TCAMmapA.erase(pkt);
				//printf("XDDDggggg\n");
				// Install at the tail (LRU)
				if(sw[nid]->TCAMmapI.count(pkt) == 0)
				{
					sw[nid]->TCAMmapI[pkt] = sw[nid]->TCAMinactive.push_back(ent);
					//printf("XDDDDDDDXX\n");
				}
			}
			//printf("A = %d, I = %d\n", sw[nid]->TCAMactive.size(), sw[nid]->TCAMinactive.size());
		}
	//}
}
