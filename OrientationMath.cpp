#include <stdio.h>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include "OrientationMath.h"

// Modified based on work done in collaboration with GitHub user @treyfortmuller 

using namespace std;

double degreesToRadians(double degs){
	return (degs * M_PI / 180.0);
}

double radiansToDegrees(double rads){
	return (rads * 180.0 / M_PI);
}

// takes LLA coordinates in degrees and outputs an array of ecef coords
// LLA coordinates are standard coordinates
// ECEF coordinates: Earth centered (https://en.wikipedia.org/wiki/ECEF) 
void llaToEcef(double* coordArray) {
	int earthRadius = 6371000; // Earth radius in meters

	double radLat = degreesToRadians(coordArray[0]);
	double radLon = degreesToRadians(coordArray[1]);
	double alt = coordArray[2];

	coordArray[0] = (earthRadius + alt) * cos(radLat) * cos(radLon);
	coordArray[1] = (earthRadius + alt) * cos(radLat) * sin(radLon);
	coordArray[2] = (earthRadius + alt) * sin(radLat);
	
}

// Calculate diff in ECEF coordinates
void ecefDiffs(double* output, double* tracker, double* payload) {
	output[0] = payload[0] - tracker[0];
	output[1] = payload[1] - tracker[1];
	output[2] = payload[2] - tracker[2];

}

// direction cosine matrix for transformation from ECEF to NED frame
// NED -- https://en.wikipedia.org/wiki/Local_tangent_plane_coordinates#Local_north,_east,_down_(NED)_coordinates 

void ecefToNed(double* coordArray, double* ecefDeltas, double lat, double lon, double alt) {

	double radLat = degreesToRadians(lat); 
	double radLon = degreesToRadians(lon);

	double CDM[3][3] = {{-sin(radLat) * cos(radLon), -sin(radLat) * sin(radLon), cos(radLat)},
						{-sin(radLon), cos(radLon), 0},
						{-cos(radLat) * cos(radLon), -cos(radLat) * sin(radLon), -sin(radLat)}};

	// matrix multiplication of the direction cosine matrix with ecef deltas
	coordArray[0] = (CDM[0][0] * ecefDeltas[0]) + (CDM[0][1] * ecefDeltas[1]) + (CDM[0][2] * ecefDeltas[2]);
	coordArray[1] = (CDM[1][0] * ecefDeltas[0]) + (CDM[1][1] * ecefDeltas[1]) + (CDM[1][2] * ecefDeltas[2]);
	coordArray[2] = (CDM[2][0] * ecefDeltas[0]) + (CDM[2][1] * ecefDeltas[1]) + (CDM[2][2] * ecefDeltas[2]);

}

// get bearing in degrees 
double getBearing(double* NED) {
	double bearing;

	if (NED[0] > 0 && NED[1] > 0) {
		bearing = radiansToDegrees(atan(NED[1] / NED[0]));
	}

	else if (NED[0] < 0 && NED[1] > 0) {
	    bearing = 180 - radiansToDegrees(atan(NED[1] / abs(NED[0])));
	}

	else if (NED[0] < 0 && NED[1] < 0) {
	    bearing = radiansToDegrees(atan(abs(NED[1]) / abs(NED[0]))) - 180;
	}

	else if (NED[0] > 0 && NED[1] < 0) {
	    bearing = radiansToDegrees(atan(abs(NED[1]) / NED[0])) - 90;
	}

	return bearing;
}

// get elevation in degrees
double getElevation(double* NED) {
	double elevation;

	if (NED[2] > 0){ 
		elevation = radiansToDegrees(-(atan(abs(NED[2]) / sqrt(pow(NED[0], 2) + pow(NED[1], 2)))));
	}
	else{
		elevation = radiansToDegrees(atan(abs(NED[2]) / sqrt(pow(NED[0], 2) + pow(NED[1], 2))));
	}

	return elevation;
}

void getBearingElevation(double *toReturn, double *payload, double *tracker){
	double payloadLat = payload[0];
	double payloadLon = payload[1];
	double payloadAlt = payload[2];
	
	llaToEcef(payload);
	llaToEcef(tracker);
	
	double diff[3] = {payload[0] - tracker[0], payload[1] - tracker[1], payload[2] - tracker[2]};
	
	double nedDelta[3] = {0, 0, 0};
	ecefToNed(nedDelta, diff, payloadLat, payloadLon, payloadAlt);
	
	toReturn[0] = getBearing(nedDelta);
	toReturn[1] = getElevation(nedDelta);
}