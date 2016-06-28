// Headers
#include "basic_lib.h"
#include "IP/IP.h"

// Maximum Ranges
#define MAX_PORT_RANGE 65536
#define MAX_PROTOCOL_RANGE 2

// Flow format
typedef struct{
	IP srcIP, dstIP;
	int srcPort, dstPort;
	int protocol;
	double arrivalTime;
	int flowSize;
	double dataRate;
}FLOW;
vector<FLOW>flow;

// Flow inter-arrival time
int arrvUSEC[] = {10, 37, 53, 73, 90, 441, 623, 659, 830, 972, 
	2581, 5743, 9149, 41622};
int arrvPERC[] = {8, 7, 4, 6, 4, 2, 10, 8, 10, 14, 8, 8, 8, 3};
vector<int>flowArrv;

// Flow data rate (Mbps/sec)
/*double rateMBPS[] = {0.0005, 0.001, 0.005, 0.01, 0.05, 0.1, 0.5,
	1, 5, 10, 50, 100};
int ratePERC[] = {6, 7, 14, 16, 13, 9, 7, 10, 5, 4, 5, 4};*/
double rateMBPS[] = {0.001, 0.01, 0.1, 0.5, 1, 10, 100};
int ratePERC[] = {10, 30, 20, 10, 10, 15, 5};
//double durationSecond[] = {1, 5, 10, 50, 75, 100, 105, 110};
//int durationPERC[] = {52, 8, 4, 6, 10, 5, 5, 10};
double durationSecond[] = {0.01, 0.1, 1, 5, 10, 50, 75, 100};
int durationPERC[] = {37, 6, 20, 7, 7, 10, 5, 8};
vector<double>flowRate;

// Flow size
/*int sizeBYTE[] = {73, 100, 505, 820, 2421, 6210, 9052, 38421, 
	73947, 407894, 952631, 6447368};*/
//flow data rate
//int sizePERC[] = {10, 30, 20, 20, 15, 5};

vector<int>flowSize;
vector<double>flowDurnation;

// Ratio of flow: ToR, Pod, Core
int ratio[3] = {2, 3, 5};

// Read input
void readInput(int& k, double& usec, double& scale){
//	fprintf(stderr, "Input k: ");
	scanf("%d", &k);
	if(k % 2) k--;
	if(k < 2) k = 2;
//	fprintf(stderr, "Input interval time (usec): ");
	scanf("%lf", &usec);
//	fprintf(stderr, "Input scale [1-100]: ");
	scanf("%lf", &scale);
//	fprintf(stderr, "Your input: (k = %d, t = %.3lf, sc = %.3lf)\n", k, usec, scale);
}

// Initialize
void init(void){
	int i, j, n;
	double fix = 0.125;
	/* Convert: 1 Mbps = 0.125 bytes/usec */

	// Flow inter-arrival rate
	n = sizeof(arrvUSEC)/sizeof(int);
	for(i = 0; i < n; i++)
		for(j = 0; j < arrvPERC[i]; j++)
			flowArrv.push_back(arrvUSEC[i]);

	// Flow data rate
	/* One-to-one mapping to flow size: follows dist. of flow size */
	n = sizeof(rateMBPS)/sizeof(double);
	for(i = 0; i < n; i++)
		for(j = 0; j < ratePERC[i]; j++)
			flowRate.push_back(rateMBPS[i] * fix);

	// Flow size
	/*n = sizeof(sizeBYTE)/sizeof(int);
	for(i = 0; i < n; i++)
		for(j = 0; j < sizePERC[i]; j++)
			flowSize.push_back(sizeBYTE[i]);*/
			
	n = sizeof(durationSecond)/sizeof(double);
	for(i = 0; i < n; i++)
		for(j = 0; j < durationPERC[i]; j++)
			flowDurnation.push_back(durationSecond[i]);
	
	
	
	// Random seeds
	srand((unsigned)time(NULL));
}

// Generate flows
void gen(int k, double usec, double scale){

	// Variables
	int itmp, low, upp, itmp2;
	int srcHost, dstHost, srcEdge, dstEdge, srcPod, dstPod;
	double curTime;
	FLOW ftmp;

	// Fattree information
	int core = k*k/4;
	int aggr = k*k/2;
	int edge = k*k/2;
	int host = k*k*k/4;

	// Record information
	int coreCnt = 0;
	int aggrCnt = 0;
	int edgeCnt = 0;

	// nof flows
	curTime = 0.0;
	while(curTime < usec){

		// Same Edge
		itmp = rand()%10;
		if(itmp < ratio[0]){
			srcEdge = dstEdge = rand()%edge;
			edgeCnt ++;
		}

		// Same Pod
		else if(itmp < ratio[0] + ratio[1]){
			srcPod = rand()%k;
			low = srcPod * (k/2);
			upp = (srcPod + 1) * (k/2);
			srcEdge = dstEdge = rand()%(upp-low) + low;
			while(dstEdge == srcEdge)
				dstEdge = rand()%(upp-low) + low;
			aggrCnt ++;
		}

		// Out to core
		else{
			srcPod = dstPod = rand()%k;
			low = srcPod * (k/2);
			upp = (srcPod + 1) * (k/2);
			srcEdge = rand()%(upp-low) + low;
			while(dstPod == srcPod)
				dstPod = rand()%k;
			low = dstPod * (k/2);
			upp = (dstPod + 1) * (k/2);
			dstEdge = rand()%(upp-low) + low;
			coreCnt ++;
		}

		// Source
		low = srcEdge * (k/2);
		upp = (srcEdge + 1) * (k/2);
		srcHost = rand()%(upp-low) + low;
		ftmp.srcIP.setIP(10, srcHost/(k*k/4), (srcHost%(k*k/4))/(k/2), (srcHost%(k*k/4))%(k/2)+2);
		ftmp.srcPort = rand()%MAX_PORT_RANGE;

		// Destination
		dstHost = srcHost;
		low = dstEdge * (k/2);
		upp = (dstEdge + 1) * (k/2);
		while(dstHost == srcHost)
			dstHost = rand()%(upp-low) + low;
		ftmp.dstIP.setIP(10, dstHost/(k*k/4), (dstHost%(k*k/4))/(k/2), (dstHost%(k*k/4))%(k/2)+2);
		ftmp.dstPort = rand()%MAX_PORT_RANGE;

		// Protocol
		ftmp.protocol = rand()%MAX_PROTOCOL_RANGE;

		// Flow information
		curTime += flowArrv[ rand()%(flowArrv.size()) ] / scale;
		ftmp.arrivalTime = curTime;
		itmp = rand()%(flowRate.size());
		itmp2 = rand()%(flowDurnation.size());
		if(flowDurnation[ itmp2 ] > 1 && flowRate[ itmp ] > 0.1)
		{
			while(true)
			{
				itmp = rand()%(flowRate.size());
				if(flowRate[ itmp ] <= 1)
					break;
			}
		}
		else if(flowRate[ itmp ] > 1 && flowDurnation[ itmp2 ] > 0.1)
		{
			while(true)
			{
				itmp2 = rand()%(flowDurnation.size());
				if(flowDurnation[ itmp2 ] < 1)
					break;
			}
		}

		/* One-one mapping */
		ftmp.dataRate = flowRate[ itmp ];
		ftmp.flowSize = int(flowRate[ itmp ]*flowDurnation[ itmp2 ]*1000000);
		


	
		
		// Create flow
		flow.push_back(ftmp);
		//printf("%d : %e\n", flow[flow.size()-1].flowSize, flowDurnation[itmp2]);
	}

	// Created Information
//	fprintf(stderr, "Out to core: %d\n", coreCnt);
//	fprintf(stderr, "Same pod: %d\n", aggrCnt);
//	fprintf(stderr, "Same edge: %d\n", edgeCnt);
//	fprintf(stderr, "Total: %d\n", coreCnt + aggrCnt + edgeCnt);
}

// Output
void output(void){
	int i;
	/* SrcIP DstIP SrcPort DstPort Protocol Arrival FlowSize DataRate */
	for(i = 0; i < flow.size(); i++)
		printf("%s %s %d %d %d %e %d %e\n", flow[i].srcIP.fullIP.c_str(), flow[i].dstIP.fullIP.c_str(),
				flow[i].srcPort, flow[i].dstPort, flow[i].protocol, flow[i].arrivalTime,
				flow[i].flowSize, flow[i].dataRate);

}

// Main
int main(void)
{
	int k;
	double usec, scale;

	// Read input
	readInput(k, usec, scale);

	// Initialize
	init();

	// Generate flow
	gen(k, usec, scale);

	// Output
	output();
	return 0;
}
