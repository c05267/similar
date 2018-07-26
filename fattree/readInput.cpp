
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
#include "time.h" 

// Read packet/flow
void Fattree::readInput(void){

	// Variables
	int byte[4], srcPort, dstPort, protocol, seq, flowSize, numberOfPacket;
	char charSrcIP[20], charDstIP[20];
	double timeStamp, dataRate;
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
		
		totFlow++;
		
		//printf("numberOfPacket: %d, Flow ID %d \n", numberOfPacket, seq );
		
		// Setup Packet Info

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
		
		if(dataRate >= 0.125)
			metric_num_largeflow++;
		else if(dataRate <= 0.00125)
			metric_num_mouseflow++;
		else
			metric_num_normalflow++;
		
		//add packet attribution
		pkt.setFirstPacket(true);
		pkt.setIsDivided(false);
		
		//printf("Flow ID: %d, timestamp: %f, protocol: %d \n", pkt.getSequence(), timeStamp, protocol);
		
		//last packet
		if(flowSize <= PKT_SIZE)
			pkt.setLastPacket(true);
		else
			pkt.setLastPacket(false);
		
		// Put into event queue
		evt.setTimeStamp(timeStamp);
		evt.setEventType(EVENT_FORWARD);
		evt.setPacket(pkt);
		//Delivery to first switch
		hostID = numberOfCore + numberOfAggregate + numberOfEdge + 
		srcIP.byte[1]*pod*pod/4 + srcIP.byte[2]*pod/2 + srcIP.byte[3]-2;
		evt.setID(node[hostID]->link[0].id);
		eventQueue.push(evt);
		seq++;
		
		// Record flow arrival time
		metric_flowArrivalTime[seq-1] = timeStamp;
		//start1 = clock();
		//metric_flowRTTArrivalTime[seq-1] = (double) start1 / CLOCKS_PER_SEC;
		
		numOfPackets[pkt] = numberOfPacket - 1;
		//printf("Number of remaining packets: %d \n", numOfPackets[pkt]);
	}
	flowArr = new int [totFlow];
	for(int i=0 ; i < totFlow; i++)
	{
		flowArr[i] = 0;
	}
	//printf("Hello\n");
}
