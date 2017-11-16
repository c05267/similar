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
#define MAX_TCAM_ENTRY 1500
#define ENTRY_EXPIRE_TIME 1e7

/***** Threshold *****/
#define THR_WIRED 0.0
#define THR_WIRELESS 0.0
#define THR_TCAM_FULL 1.0
#define Long_lived_THR 0.01
#define PKT_SIZE 1400
#endif
