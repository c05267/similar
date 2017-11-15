// This header defines the parameters
#ifndef PARAMETER_H
#define PARAMETER_H

/******** K *******/
#define SIZE_OF_K 20

/******** Delays ********/
#define TCAM_SEARCH_DELAY 28.5
#define FLOW_SETUP_DELAY 1
#define CONTROL_BATCH 500
#define CONTROL_PATH_DELAY 5000
#define FLOW_TABLE_DELAY 1000

/******** Link ********/
#define LINK_CAPACITY 125.0
#define WIRELESS_RANGE 10.0

/******** TCAM ********/
#define MIN_TCAM_ENTRY 500
#define MAX_TCAM_ENTRY 1500
#define ENTRY_EXPIRE_TIME 1e7
#define PKT_SIZE 1400
#endif
