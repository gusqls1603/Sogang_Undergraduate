#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void   Error_Exit(char *s);

void   Sort_by_X_or_Y ( double *A, unsigned int *P, unsigned int N );
  // input  : A[] (x or y position array of N points)
  //          P[] (index array of X (initital) or Y (during combine))
  //          N   number of points to sort
  // output : P[] (the indices are sorted by x or y position)

double Closest_Pair_DC(
	unsigned int L,    unsigned int R,		// left and right indices
	unsigned int *pt1, unsigned int *pt2,   // min dist points inices
    double *X, double *Y,					// input points position array
	unsigned int *P1, unsigned int *P2, unsigned int *P3, // temp index arrays
	unsigned int THR	// threshold value
);
  // Input : L, R, X[], Y[], P1[]
  // Output : pt1, pt2 ( the distance between (X[pt1],Y[pt1]) and
  //                     (X[pt2],Y[pt2]) is the closest )
  //          return the distance value
  // Others ; P2[], P3[] (scratch arrays for D&C algorithm)

void main ( void ) {
    int TNUM, THR;
	unsigned int N, pt1, pt2;
	double cmpt, cd; // cd : closest distance
	clock_t start, finish;
	double *X, *Y;
	unsigned int *P1, *P2, *P3;

	scanf_s("%d", &TNUM); // read the number of tests
	for ( int t = 0; t < TNUM; t++ ) {  // for each test case
		scanf_s("%u %d", &N, &THR); // read point number and threshold
		X  = new double [N];        Y = new double [N];  // allocate arrays
		P1 = new unsigned int [N]; P2 = new unsigned int [N];
		P3 = new unsigned int [N];
		if ( X == NULL || Y == NULL || P1 == NULL || P2 == NULL || P3 == NULL ) {
			Error_Exit("Memory allocation error!");
		}
		for (unsigned int i = 0; i < N; i++) { // read points
			scanf_s("%le %le",&X[i], &Y[i]); 
			P1[i] = i; // set indices of points
		}
		start = clock();
		// ** Apply the D&C algorithm
			Sort_by_X_or_Y ( X, P1, N ); // sort point indices by x-coords
			cd = Closest_Pair_DC(0, N-1, &pt1, &pt2, X, Y, P1, P2, P3, THR);
		// **End of thr algorithm
		finish = clock();
		cmpt = ((double)(finish - start)) / CLK_TCK;
		if ( t != 0 ) printf("\n");
		printf("**Test Case %d (N = %u, time = %.3f sec)", t+1, N, cmpt);
		printf("\npt pair = (%u, %u), min_dist = %23.16le", pt1, pt2, cd);
		delete X; delete Y;
		delete P1; delete P2; delete P3;
	}
}

void Error_Exit(char *s) {
	printf("%s", s);
	exit(-100);
}
