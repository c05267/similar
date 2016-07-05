
// Headers
#include "../basic_lib.h"
#include "../event/event.h"
#include "../event/eventType.h"
#include "../packet/packet.h"
#include "../IP/IP.h"
#include "../switch/switch.h"

// Switch class
Switch::Switch(int id):Node(id){
	num_rule_replacement = 0;
}
Event Switch::forward(double timeStamp, Packet pkt){

	// Variables
	double TCAMDelay = TCAM_SEARCH_DELAY;
	double forwardDelay;
	int outputPort;
	bool arrive;
	Packet tmpPkt;
	Event evt;
	IP dstIP;
	IP selfIP;

	// Arrival of destination
	arrive = true;
	selfIP = this->getIP();
	dstIP = pkt.getDstIP();
	for(int i = 0; i < 4; i++){
		if(dstIP.byte[i] != selfIP.byte[i]){
			arrive = false;
			break;
		}
	}
	if(arrive){
		evt.setTimeStamp(timeStamp);
		evt.setEventType(EVENT_DONE);
		evt.setPacket(pkt);
		return evt;
	}

	// Remove expired entries (Active & inactive)
	while(!TCAMactive.empty()){
		if(TCAMactive.front().isExpired(timeStamp)){
			tmpPkt = TCAMactive.front().getSample();
			TCAMmapA.erase(tmpPkt);
			TCAMactive.pop_front();
		}
		else break;
	}
	/*while(!TCAMinactive.empty()){
		if(TCAMinactive.front().isExpired(timeStamp)){
			tmpPkt = TCAMinactive.front().getSample();
			TCAMmapI.erase(tmpPkt);
			TCAMinactive.pop_front();
		}
		else break;
	}*/

	// Search in TCAM
	int pri = -1;
	Entry result;
	if(TCAMmapA.count(pkt) > 0){
		result = TCAMmapA[pkt]->ent;
		pri = 0;

		// Update entry expire time & Install at the tail (LRU)
		result.setExpire(timeStamp + ENTRY_EXPIRE_TIME);
		TCAMactive.remove(TCAMmapA[pkt]);
		TCAMmapA[pkt] = TCAMactive.push_back(result);
		if(this->getID() == 19)
			printf("Hit SrcPort: %d, DstPort: %d \n", result.getSrcPort(),result.getDstPort());
	}
	/*else if(TCAMmapI.count(pkt) > 0){
		result = TCAMmapI[pkt]->ent;
		pri = 0;

		// Remove from inactive
		TCAMinactive.remove(TCAMmapI[pkt]);
		TCAMmapI.erase(pkt);
		
		// Update timestamp &  Install at the tail (LRU)
		result.setExpire(timeStamp + ENTRY_EXPIRE_TIME);
		TCAMmapA[pkt] = TCAMactive.push_back(result);
	}*/

	// Entry not found
	if(pri == -1){

		// Check if this flow already requesting flow setup
		if(isSetup[pkt]){

			// Push into queue
			que.push_back(pkt);

			// No OP
			evt.setTimeStamp(timeStamp);
			evt.setEventType(EVENT_NOP);
			return evt;
		}

		// Flow setup request
		else{
			// Record & put into queue
			isSetup[pkt] = true;
			que.push_back(pkt);

			// TCAM searching delay
			evt.setTimeStamp(timeStamp + TCAMDelay);
			evt.setEventType(EVENT_FLOWSETUP);
			evt.setPacket(pkt);
			evt.setID(this->getID());
			return evt;
		}
	}

	// Clear
	isSetup[pkt] = false;

	// Foward after link available
	outputPort = result.getOutputPort();
	timeStamp = timeStamp + TCAMDelay;
	
	if(pkt.getLastPacket())
		printf("Last Switch ID: %d, Src: %d, Dst: %d \n", this->getID(), pkt.getSrcPort(), pkt.getDstPort());
	
	// Forward event
	if(PKT_SIZE > pkt.getFlowSize())
		forwardDelay = pkt.getFlowSize() / pkt.getDataRate();
	else
		forwardDelay = PKT_SIZE / pkt.getDataRate();
	evt.setTimeStamp(timeStamp + forwardDelay);
	evt.setEventType(EVENT_FORWARD);
	evt.setPacket(pkt);	
	
	//delete tail entry, and install the entry from cache
	if(result.getRecovery() && cache.size() > 0)
	{
		tmpPkt = this->TCAMactive.back().getSample();
		this->TCAMmapA.erase(tmpPkt);
		this->TCAMactive.pop_back();
		//printf("find a recovery entry \n");
		
		tmpPkt = cache[cache.size()-1].getSample();
		//if no the entry in the TCAM entry, no flow setup from controller
		if(this->TCAMmapA.count(tmpPkt) <=0)
		{
			TCAMmapA[tmpPkt] = TCAMactive.push_front(cache[cache.size()-1]);
			//printf("replacement done \n");
		}
		
		cache.pop_back();
		
	}
	
	// Forward event
	/*forwardDelay = pkt.getFlowSize() / pkt.getDataRate();
	evt.setTimeStamp(timeStamp + forwardDelay);
	evt.setEventType(EVENT_FORWARD);
	evt.setPacket(pkt);*/

	// Wired/Wireless
	if(result.isWireless())
		evt.setID(wlink[result.getOutputPort()].id);
	else
		evt.setID(link[result.getOutputPort()].id);

	// Return event
	return evt;
}

void Switch::Add_Rule_Replacement(void)
{
	num_rule_replacement++;
}

int Switch::Get_Rule_Replacement(void)
{
	return num_rule_replacement;
}
