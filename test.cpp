#include <iostream>
#include "OrientationMath.h"

int main(int argc, char *argv[]) {
	// payload
	double payloadLat = 43.0389;
	double payloadLon = -87.9065;
	double payloadAlt = 120000;

	// tracker
	double myLat = 43.0731;
	double myLon = -89.4012;
	double myAlt = 266.09;
	
		
	double payload[3] = {payloadLat, payloadLon, payloadAlt};
	double tracker[3] = {myLat, myLon, myAlt};
	
	double results[2] = {0,0};
	getBearingElevation(results, payload, tracker);

	printf("Target Bearing is: %f, Target Elevation is: %f\n", results[0], results[1]);



}