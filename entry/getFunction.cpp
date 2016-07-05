
// Headers
#include "../basic_lib.h"
#include "../packet/packet.h"
#include "../entry/entry.h"
#include "../IP/IP.h"

int Entry::getOutputPort(void){
	return outputPort;
}
int Entry::getPriority(void){
	return priority;
}
int Entry::getSID(void) const{
	return this->sid;
}
double Entry::getRate(void){
	return this->rate;
}
double Entry::getValue(void){
	return value;
}
bool Entry::isWireless(void){
	return this->wireless;
}
bool Entry::isExpired(int curTime){
	return this->expireTime <= curTime;
}
bool Entry::getRecovery(void){
	return this->recovery;
}
int Entry::getProtocol(void){
	return protocol;
}
int Entry::getSrcPort(void){
	return srcPort;
}
int Entry::getDstPort(void){
	return dstPort;
}
Packet Entry::getSample(void){
	IP srcIP(srcMask[0], srcMask[1], srcMask[2], srcMask[3]);
	IP dstIP(dstMask[0], dstMask[1], dstMask[2], dstMask[3]);
	Packet pkt(srcIP, dstIP, srcPort, dstPort, protocol);
	return pkt;
}
