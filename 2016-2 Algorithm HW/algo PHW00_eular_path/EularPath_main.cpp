#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "LinkedList_ptr_L.h"


void Error_Exit(char *s) {
	printf("%s",s);
	exit(-1);
}

ptr_L *Find_Euler(char **G, int N, int start);
char **Allocate_and_Read_Graph( int N );
int getOddDegreeNum(char **G, int N, int *start);
void Free_Graph(char **G, char **G_tmp, int N);
void Print_Graph(char **G, int N); // for test
char **Copy_Graph(char **G, int N); // for test
bool Check_if_Euler(char **G_tmp, int N, ptr_L *path); // for test

int main( ) {
	int T, N;
	char **G, **G_tmp;
	int oddDegreeNum, start;
	ptr_L *path, *p;
	clock_t start_time, finish_time;
	double cmpt;

	scanf("%d", &T);
	for (int t = 1; t <= T; t++ ) {
		scanf("%d", &N);
		G = Allocate_and_Read_Graph( N ); // alloc memory and read a graph
		G_tmp = Copy_Graph(G, N);

		start_time = clock(); // set the start time

		oddDegreeNum = getOddDegreeNum(G, N, &start); // if 2 odd degree vtx, start will be determined
		if ( oddDegreeNum == 0 || oddDegreeNum == 2 ) {
			if (oddDegreeNum == 0) {
				start = 0; // set any vertex
			}
			path = Find_Euler(G, N, start); // find an Euler path or cycle

			finish_time = clock(); // set finish time
			cmpt = ((double)(finish_time - start_time)) / CLK_TCK; // compute the time passed

			if (oddDegreeNum == 0) {
				printf("Test %2d (%.3f sec) : An Euler cycle exisits.\n", t, cmpt);
			}
			else if ( oddDegreeNum == 2 ) {
				printf("Test %2d (%.3f sec) : An Euler path exisits.\n",t, cmpt);
			}
			if (Check_if_Euler(G_tmp, N, path) == false) {
				printf("***ERROR the path is not Euler!!!\n");
			}
			for ( p = path; p != NULL; p = p->p ) { // output the path or cycle
				printf("%d ",p->i);
			}
			while ( path != NULL ) {  
				popQ_ptr_L(&path);  // free allocated linked list
			}
		}
		else {
			printf("Test %2d : Neither Euler path nor cycle eixst.",t);
		}
		if ( t != T ) {
			printf("\n");
		}
		Free_Graph(G, G_tmp, N);
	}
	Free_ptr_L_pool ();
	return 0;
}

bool Check_if_Euler(char **G_tmp, int N, ptr_L *path) {
	int h, t;

	// remove edges in G_tmp following the path
	for(ptr_L *p = path; p != NULL; p = p->p) { 
		if ( p->p == NULL ) {
			break;
		}
		h = p->i;
		t = p->p->i;
		if (G_tmp[h][t] > 0)
			--G_tmp[h][t];
		else if (G_tmp[h][t] <= 0)
			return false;
		if (G_tmp[t][h] > 0)
			--G_tmp[t][h];
		else if (G_tmp[t][h] <= 0)
			return false;
	}
	// Check if no edge in G_tmp
	for(int r = 0; r < N; r++ ) {
		for(int c = 0; c < N; c++) {
			if ( G_tmp[r][c] != 0 )
				return false;
		}
	}
	return true;
}

void Print_Graph(char **G, int N) {
	for(int r = 0; r < N; r++ ) {
		for(int c = 0; c < N; c++) {
			printf("%d ", G[r][c]);
		}
		printf("\n");
	}
}

void Free_Graph(char **G, char **G_tmp, int N) {
	for(int r = 0; r < N; r++){  // free memory
		free(G[r]);
		free(G_tmp[r]);
	}
	free(G);
	free(G_tmp);
	G = G_tmp = NULL;
}

char **Allocate_and_Read_Graph( int N ) {
	char **G;
	int ed;

	G = (char **)malloc(sizeof(char *) * N);    // allocate memory
	if ( G == NULL ) {
		Error_Exit("Memory Allocation Error");
	}
	for(int r = 0; r < N; r++){
		G[r] = (char *)malloc(sizeof(char) * N);
		if ( G[r] == NULL ) {
			Error_Exit("Memory Allocation Error");
		}
	}

	for (int r = 0; r < N; r++ ) {
		for (int c = 0; c < N; c++ ) {
			scanf("%d", &ed);
			G[r][c] = (char) ed;
		}
	}

	return G;
}

char **Copy_Graph(char **G, int N) {
	char **G_tmp = (char **)malloc(sizeof(char *) * N);    // allocate memory
	if ( G_tmp == NULL ) {
		Error_Exit("Memory Allocation Error");
	}
	for(int r = 0; r < N; r++){
		G_tmp[r] = (char *)malloc(sizeof(char) * N);
		if ( G_tmp[r] == NULL ) {
			Error_Exit("Memory Allocation Error");
		}
	}
	for (int r = 0; r < N; r++) {
		for ( int c = 0; c < N; c++ ) {
			G_tmp[r][c] = G[r][c];
		}
	}
	return G_tmp;
}