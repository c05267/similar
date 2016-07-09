// Headers
#include "../basic_lib.h"
#include "../packet/packet.h"
#include "../node/node.h"
#include "../switch/core.h"
#include "../switch/aggregate.h"
#include "../switch/edge.h"
#include "../host/host.h"
#include "../fattree/fattree.h"
#include "../event/event.h"
#include "../event/eventType.h"
#include "../mysort/mysort.h"

// Controller
void Fattree::controller(Event ctrEvt){

	// Variables
	int nid;
	int pathLen;
	int nowFlowID;
	double temp;
	double delay;
	double flowSetupDelay = FLOW_SETUP_DELAY;
	double computePathDelay = CONTROL_PATH_DELAY;
	Event evt, ret;
	Packet pkt;
	Entry ent;
	vector<Event>flowSetupEvent;
	vector<Entry>vent;
	vector<Entry>copyVENT;
	bool hasHandle = false;
	int k;

	// Classify events
	for(int i = 0; i < cumQue.size(); i++){
		evt = cumQue[i];
		nid = evt.getID();
		pkt = evt.getPacket();

		// Flow Setup Request
		if(evt.getEventType() == EVENT_FLOWSETUP){

			// Illegal destination address
			if(!legalAddr(pkt.getDstIP())){
				delay = 1.0;
				ret.setTimeStamp(ctrEvt.getTimeStamp() + delay);
				ret.setEventType(EVENT_UNKNOWN);
				this->eventQueue.push(ret);
				continue;
			}

			// Known flow
			else if(rcdFlowID[pkt]){
				nowFlowID = rcdFlowID[pkt];
				vent.clear();

				// Extract original entry
				if(rule(nid, allEntry[nowFlowID], ent)){
					ent.setExpire(ctrEvt.getTimeStamp() + flowSetupDelay + ENTRY_EXPIRE_TIME);
					// Install the new entry
					ret.setEventType(EVENT_INSTALL);
					ret.setTimeStamp(ctrEvt.getTimeStamp() + flowSetupDelay);
					ret.setID(nid);
					ret.setPacket(pkt);
					ret.setEntry(ent);
					eventQueue.push(ret);
				}

				// Rule not found
				else{
					/* Maybe it's caused by rule deletion of starting edge policy? */
					fprintf(stderr, "Error: extract original flow entry failed.\n");
				}
			}

			// Require to setup along the path
			else flowSetupEvent.push_back(evt);
		}

		// Unknown events
		else{
			delay = 1;
			ret.setTimeStamp(ctrEvt.getTimeStamp() + delay);
			ret.setEventType(EVENT_UNKNOWN);
			eventQueue.push(ret);
		}
	}
	if(((int)cumQue.size()) > 0) hasHandle = true;
	cumQue.clear();

	// Sort with the largest gap between wired & wireless
	mySort msrt(this);
	sort(flowSetupEvent.begin(), flowSetupEvent.end(), msrt);

	// Currently, all flow setup apply wired policy
	for(int j = 0; j < flowSetupEvent.size(); j++){
		evt = flowSetupEvent[j];
		nid = evt.getID();
		pkt = evt.getPacket();
	
		// Assign flow ID
		nowFlowID = flowIDCount ++;
		rcdFlowID[pkt] = nowFlowID;

		// Clear entry
		vent.clear();

		// LARGE FLOW!!!!!!
		if(pkt.getDataRate() >= 0.125){

			// You MUST use wired :)
			temp = ctrEvt.getTimeStamp() + flowSetupDelay + computePathDelay;
			if(wired(nid, pkt, vent, temp)){
				
				// Reserve capacity
				modifyCap(vent, -pkt.getDataRate(), false);
				
				// Install rule
				for(int i = 0; i < vent.size(); i++){

					// Switch side event
					ret.setEventType(EVENT_INSTALL);
					ret.setTimeStamp(ctrEvt.getTimeStamp() + flowSetupDelay + computePathDelay);
					ret.setID(vent[i].getSID());
					ret.setPacket(pkt);
					ret.setEntry(vent[i]);
					eventQueue.push(ret);
				}

				// Record inserted entries
				allEntry.push_back(vent);

				// Clear Entry
				vent.clear();
			}

			// What?? No wired path!?
			else{
				fprintf(stderr, "Error: %s to %s: ", pkt.getSrcIP().fullIP.c_str(), pkt.getDstIP().fullIP.c_str());
				fprintf(stderr, "No such WIRED path exists.\n");
			}
			continue;
		}

		// Wireless seems better
		if(wiredHop(pkt) > wirelessHop(pkt)){

			// Wireless policy first, then wired policy
			temp = ctrEvt.getTimeStamp() + flowSetupDelay + computePathDelay;

			// Wireless CAP
			if(wireless(nid, pkt, vent, temp)){

				// Copy for later use
				copyVENT = vent;

				// Wireless TCAM
				if(isTCAMfull(vent, false)){

					// Wired CAP
					if(wired(nid, pkt, vent, temp)){

						// Wired TCAM
						if(!isTCAMfull(vent, true)){
							
							// Reserve capacity
							modifyCap(vent, -pkt.getDataRate(), false);
							
							// Install wired rule
							for(int i = 0; i < vent.size(); i++){

								// Switch side event
								ret.setEventType(EVENT_INSTALL);
								ret.setTimeStamp(ctrEvt.getTimeStamp() + flowSetupDelay + computePathDelay);
								ret.setID(vent[i].getSID());
								ret.setPacket(pkt);
								ret.setEntry(vent[i]);
								eventQueue.push(ret);
							}
							// Record inserted entries
							allEntry.push_back(vent);
							continue;
						}
					}
				}
				
				// Reserve capacity
				modifyCap(copyVENT, -pkt.getDataRate(), true);

				// Install wireless rule
				for(int i = 0; i < copyVENT.size(); i++){

					// Switch side event
					ret.setEventType(EVENT_INSTALL);
					ret.setTimeStamp(ctrEvt.getTimeStamp() + flowSetupDelay + computePathDelay);
					ret.setID(copyVENT[i].getSID());
					ret.setPacket(pkt);
					ret.setEntry(copyVENT[i]);
					eventQueue.push(ret);
				}
				// Record inserted entries
				allEntry.push_back(copyVENT);
			}

			// Wired CAP
			else if(wired(nid, pkt, vent, temp)){
				
				// Reserve capacity
				modifyCap(vent, -pkt.getDataRate(), false);
				
				// Install wired rule
				for(int i = 0; i < vent.size(); i++){

					// Switch side event
					ret.setEventType(EVENT_INSTALL);
					ret.setTimeStamp(ctrEvt.getTimeStamp() + flowSetupDelay + computePathDelay);
					ret.setID(vent[i].getSID());
					ret.setPacket(pkt);
					ret.setEntry(vent[i]);
					eventQueue.push(ret);
				}
				// Record inserted entries
				allEntry.push_back(vent);
			}

			// No such path exists
			else{
				fprintf(stderr, "Error: %s to %s: ", pkt.getSrcIP().fullIP.c_str(), pkt.getDstIP().fullIP.c_str());
				fprintf(stderr, "No such path exists.\n");
			}
		}

		// Wired seems better
		else{

			// Wired policy first, then wireless policy
			temp = ctrEvt.getTimeStamp() + flowSetupDelay + computePathDelay;
		
			// Wired CAP
			if(wired(nid, pkt, vent, temp)){
				
			
				// Copy for later use
				copyVENT = vent;

				// Wired TCAM
				if(isTCAMfull(vent, true)){

					// Wireless CAP
					if(wireless(nid, pkt, vent, temp)){

						// Wireless TCAM
						if(!isTCAMfull(vent, false)){
							
							// Reserve capacity
							modifyCap(vent, -pkt.getDataRate(), true);

							// Install wireless rule
							for(int i = 0; i < vent.size(); i++){

								// Switch side event
								ret.setEventType(EVENT_INSTALL);
								ret.setTimeStamp(ctrEvt.getTimeStamp() + flowSetupDelay + computePathDelay);
								ret.setID(vent[i].getSID());
								ret.setPacket(pkt);
								ret.setEntry(vent[i]);
								eventQueue.push(ret);
							}
							// Record inserted entries
							allEntry.push_back(vent);
							continue;
						}
					}
				}
				
				// Reserve capacity
				modifyCap(copyVENT, -pkt.getDataRate(), false);

				// Install wired rule
				for(int i = 0; i < copyVENT.size(); i++){

					// Switch side event
					ret.setEventType(EVENT_INSTALL);
					ret.setTimeStamp(ctrEvt.getTimeStamp() + flowSetupDelay + computePathDelay);
					ret.setID(copyVENT[i].getSID());
					ret.setPacket(pkt);
					ret.setEntry(copyVENT[i]);
					eventQueue.push(ret);
				}
				// Record inserted entries
				allEntry.push_back(copyVENT);
			}
			
			// Wireless CAP
			else if(wireless(nid, pkt, vent, temp)){
				
				// Reserve capacity
				modifyCap(vent, -pkt.getDataRate(), true);

				// Install wireless rule
				for(int i = 0; i < vent.size(); i++){

					// Switch side event
					ret.setEventType(EVENT_INSTALL);
					ret.setTimeStamp(ctrEvt.getTimeStamp() + flowSetupDelay + computePathDelay);
					ret.setID(vent[i].getSID());
					ret.setPacket(pkt);
					ret.setEntry(vent[i]);
					eventQueue.push(ret);
				}
				// Record inserted entries
				allEntry.push_back(vent);
			}

			// No such path exists
			else{
				fprintf(stderr, "Error: %s to %s: ", pkt.getSrcIP().fullIP.c_str(), pkt.getDstIP().fullIP.c_str());
				fprintf(stderr, "No such path exists.\n");
				/* Here we may need to handle such situation */
			}
		}
	}

	// DEBUG: if no event handled, stop
	if(!eventQueue.size()) return;

	// DEBUG log
//if(hasHandle) printf("[%6.1lf] Controller: Waiting for next handle...\n", ctrEvt.getTimeStamp());

	// The next timeout time
	/*evt = ctrEvt;
	evt.setEventType(EVENT_INTERVAL);
	evt.setTimeStamp(evt.getTimeStamp()+CONTROL_BATCH);
	eventQueue.push(evt);*/
	return;
}
