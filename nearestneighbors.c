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
double CalculateDistance (int p1[], int p2[], int len);

int main () {
	printf ("Splitting up threads\n");

	int i;
	int numCPU = sysconf(_SC_NPROCESSORS_ONLN) - 1;
	pthread_t pth[numCPU];
	threadInput input [numCPU];

	printf ("Number of worker CPUs: %i\n", numCPU);
	printf ("Create Input...\n");

	datum myData [DATA_SET_SIZE];
	datum testData [DATA_SET_SIZE];

	printf ("Training...\n");

	srand (time (NULL)); // TEMP
	train (myData, true);
	train (testData, false);
	
	printf ("Predicting...\n");

	// Split up work between workers
	for (i=0; i<numCPU; i++) {
//		printf ("Starpoint for node %i should be: %i because numCPU = %i and DATA_SET_SIZE = %i\n", i, DATA_SET_SIZE / numCPU * i, numCPU, DATA_SET_SIZE);
		input[i].unknownData = &(testData [DATA_SET_SIZE / numCPU * i]);
		input[i].myData = myData;
		input[i].startPoint = i;
		input[i].numCPU = numCPU;

		pthread_create (&pth[i], NULL, predict, &input[i]);
	}

	// Wait for all the workers to finish
	for (i=0; i<numCPU; i++) {
		pthread_join (pth [i], NULL);
	}

	return 0;
}

void train (datum set[], bool setLabel) {
	int i;

	if (setLabel) {
		for (i=0; i<DATA_SET_SIZE; i++) {
			set[i].x = rand() % 1000; // TEMP
			set[i].y = rand() % 1000; // TEMP
			set[i].label = set[i].x + set[i].y < 1000 ? 0 : 1; // TEMP
//			printf ("(%i, %i)\n", set[i].x, set[i].y);
		}
	} else {
		for (i=0; i<DATA_SET_SIZE; i++) {
			set[i].x = rand() % 1000; // TEMP
			set[i].y = rand() % 1000; // TEMP
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
	int unknownArray [2] = {unknown->x, unknown->y}; // TEMP
	int setArray [2] = {set[0].x, set[0].y}; // TEMP
	int len = 2; // TEMP
	double bestDist = CalculateDistance (unknownArray, setArray, len);
	int bestIndex = 0;
	int i;

	for (i=0; i<DATA_SET_SIZE; i++) {
		setArray [0] = set[i].x; // TEMP
	       	setArray [1] = set[i].y; // TEMP
		double dist = CalculateDistance (unknownArray, setArray, len);
//		printf ("dist=%f best=%f\n", dist, bestDist);

		// TEMP
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

double CalculateDistance (int p1[], int p2[], int len) {
	int i;
	int diff [len];
	double ret = 0.0;

	for (i=0; i<len; i++) {
		diff [i] = p1[i] - p2[i];
		diff [i] = diff [i] * diff [i];
		ret += diff [i];
	}

	return sqrt (ret);
}
