#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "stdbool.h"
#include "unistd.h"
#include "pthread.h"

#define DATA_SET_SIZE 50

typedef struct {
	int x;
	int y;
	int label;
} datum;

typedef struct {
	datum* unknownData;
	datum* myData;
	int startPoint;
	int numCPU;
} threadInput;

void train (datum set[], bool setLabel);
// Need a predict function
void* predict (void* args);
// Figure out which point is the closest
void closest (datum* unknown, datum set[]);
// Gotta calculate distance somehow
double CalculateDistance (int x1, int y1, int x2, int y2);

int main () {
	
	printf ("Splitting up threads\n");

	int numCPU = sysconf(_SC_NPROCESSORS_ONLN) - 1;
	pthread_t pth[numCPU];
	int i;
	threadInput input;


	printf ("Number of worker CPUs: %i\n", numCPU);

	printf ("Create Input...\n");

	datum myData [DATA_SET_SIZE];
	datum testData [DATA_SET_SIZE];

	printf ("Training...\n");

	srand (time (NULL));
	train (myData, true);
	train (testData, false);
	
	printf ("predicting...\n");

	for (i=0; i<numCPU; i++) {
		input.unknownData = &(testData [numCPU / DATA_SET_SIZE * i]);
		input.myData = myData;
		input.startPoint = i;
		input.numCPU = numCPU;

		pthread_create (&pth[i], NULL, predict, &input);
	}

	for (i=0; i<numCPU; i++) {
		pthread_join (pth [i], NULL);
	}

	return 0;
}

void train (datum set[], bool setLabel) {

	int i;

	if (setLabel) {
		for (i=0; i<DATA_SET_SIZE; i++) {
			set[i].x = rand() % 1000;
			set[i].y = rand() % 1000;
			set[i].label = set[i].x + set[i].y < 500 ? 0 : 1;
//			printf ("(%i, %i)\n", set[i].x, set[i].y);
		}
	} else {
		for (i=0; i<DATA_SET_SIZE; i++) {
			set[i].x = rand() % 1000;
			set[i].y = rand() % 1000;
//			printf ("(%i, %i)\n", set[i].x, set[i].y);
		}
	}
}

void* predict (void* args) {

	threadInput* imp  = (threadInput*) args;
	int count = imp->numCPU;

	int i; 

//	printf ("Start Point = %i", imp->startPoint);

	for (i=0; i<DATA_SET_SIZE / count; i++) {
		closest (&(imp->unknownData [i]), imp->myData);
	}

	return NULL;
}

void closest (datum* unknown, datum set [DATA_SET_SIZE]) {

	double bestDist = CalculateDistance (unknown->x, unknown->y, set[0].x, set[0].y);
	int bestIndex = 0;
	int i;

	for (i=0; i<DATA_SET_SIZE; i++) {
		double dist = CalculateDistance (unknown->x, unknown->y, set[i].x, set[i].y);
//		printf ("dist=%f best=%f\n", dist, bestDist);

		if (unknown->x == set[i].x && unknown->y == set[i].y) {
			printf ("Funny business at %i! Real vals: (%i, %i) and (%i,%i)\n", i, unknown->x, unknown->y, set[i].x, set[i].y);
		}

		if (dist < bestDist) {
			bestDist = dist;
			bestIndex = i;
		}
	}

	unknown->label = set [bestIndex].label;
	printf ("Distance from (%i, %i) to (%i, %i) is %f, making it a %i\n", unknown->x, unknown->y, set [bestIndex].x, set [bestIndex].y, bestDist, unknown->label);
}

double CalculateDistance (int x1, int y1, int x2, int y2) {

	double diffx = x1 - x2;
	double diffy = y1 - y2;
	double diffxSqr = diffx * diffx;
	double diffySqr = diffy * diffy;

//	printf ("Distance between %i, %i and %i, %i is %f\n", x1, y1, x2, y2, sqrt (diffxSqr + diffySqr));

	return sqrt (diffxSqr + diffySqr);
}
