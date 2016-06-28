
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

// Read packet/flow
void Fattree::readInput(void){

	// Variables
	int byte[4], srcPort, dstPort, protocol, seq, flowSize, numberOfPacket;
	char charSrcIP[20], charDstIP[20];
	double timeStamp, dataRate;
	int firstPktSize;
	int lastPktSize;
	IP dstIP, srcIP;
	Event evt;
	Packet pkt;
	int hostID;
	

	// Packets (5-tuples and arrival time)
	seq = 1;
	numberOfPacket = 0;
	while(scanf("%s %s %d %d %d %lf %d %lf", charSrcIP, charDstIP, 
				&srcPort, &dstPort, &protocol, &timeStamp, &flowSize, &dataRate) == 8){

		if(flowSize <= PKT_SIZE)
			numberOfPacket = 1;
		else
			numberOfPacket =  ceil((double)flowSize/(double)PKT_SIZE);
		
		lastPktSize = (flowSize % PKT_SIZE) ? (flowSize % PKT_SIZE) : PKT_SIZE;
		totFlow++;
		
		//printf("numberOfPacket: %d, Flow ID %d \n", numberOfPacket, seq );
		
		// Setup Packet Info
		for(int i=0; i<numberOfPacket; i++)
		{
			srcIP.setIP(charSrcIP);
			dstIP.setIP(charDstIP);
			pkt.setSrcIP(srcIP);
			pkt.setDstIP(dstIP);
			pkt.setSrcPort(srcPort);
			pkt.setDstPort(dstPort);
			pkt.setProtocol(protocol);
			pkt.setSequence(seq);
			pkt.setFlowSize(flowSize);
			pkt.setDataRate(dataRate);
			// Put into event queue
			if(i==0)
			{
				evt.setTimeStamp(timeStamp);
				pkt.setFirstPacket(true);
				//printf("Flow ID: %d, timestamp: %f, protocol: %d \n", pkt.getSequence(), timeStamp, protocol);
			}
			else 
			{
				evt.setTimeStamp(timeStamp + i*(PKT_SIZE/dataRate));
				pkt.setFirstPacket(false);
			}

			//last packet
			if(i == numberOfPacket-1)
				pkt.setLastPacket(true);
			else
				pkt.setLastPacket(false);
			

			evt.setEventType(EVENT_FORWARD);
			evt.setPacket(pkt);
			hostID = numberOfCore + numberOfAggregate + numberOfEdge + 
						srcIP.byte[1]*pod*pod/4 + srcIP.byte[2]*pod/2 + srcIP.byte[3]-2;
			evt.setID(node[hostID]->link[0].id);
			eventQueue.push(evt);
		}
		seq++;


		// Record flow arrival time
		metric_flowArrivalTime[seq-1] = timeStamp;
	}

}
