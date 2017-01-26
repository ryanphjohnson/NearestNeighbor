#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "stdbool.h"

#define DATA_SET_SIZE 100

typedef struct {
	int x;
	int y;
	int label;
} datum;

void train (datum set[], bool setLabel);
// Need a predict function
void predict (datum unknown[], datum set[]);
// Figure out which point is the closest
void closest (datum* unknown, datum set[]);
// Gotta calculate distance somehow
double CalculateDistance (int x1, int y1, int x2, int y2);

int main (int argc, char** argv) {
	
	printf ("Create Input...\n");
	datum myData [DATA_SET_SIZE];
	datum testData [DATA_SET_SIZE];

	printf ("Training...\n");
	train (myData, true);
	train (testData, false);
	
	printf ("predicting...\n");
	predict (testData, myData);

	return 0;
}

void train (datum set[], bool setLabel) {

	int i;
	srand (time (NULL));

	if (setLabel) {
		for (i=0; i<DATA_SET_SIZE; i++) {
			set[i].x = rand() % 10;
			set[i].y = rand() % 10;
			set[i].label = set[i].x + set[i].y < 5 ? 0 : 1;
		}
	} else {
		for (i=0; i<DATA_SET_SIZE; i++) {
			set[i].x = rand() % 10;
			set[i].y = rand() % 10;
		}
	}
}

void predict (datum unknown[], datum set[]) {

	int i;

	for (i=0; i<DATA_SET_SIZE; i++) {
		closest (&unknown [i], set);
		printf ("My crystal balls says (%i, %i) is a %i\n", unknown[i].x, unknown[i].y, unknown[i].label);
	}
}

void closest (datum* unknown, datum set[]) {

	double bestDist = CalculateDistance (unknown->x, unknown->y, set[0].x, set[0].y);
	int bestIndex = 0;
	int i;

	for (i=0; i<DATA_SET_SIZE; i++) {
		float dist = CalculateDistance (unknown->x, unknown->y, set[i].x, set[i].y);

		if (dist < bestDist) {
			bestDist = dist;
			bestIndex = i;
		}
	}

	unknown->label = set [bestIndex].label;
}

double CalculateDistance (int x1, int y1, int x2, int y2) {

	double diffx = x1 - x2;
	double diffy = y1 - y2;
	double diffxSqr = diffx * diffx;
	double diffySqr = diffy * diffy;

	//printf ("Distance between %i, %i and %i, %i is %f\n", x1, y1, x2, y2, sqrt (diffxSqr + diffySqr));

	return sqrt (diffxSqr + diffySqr);
}
