#include "Graph_include.h"
#include <queue>

void Error_Exit ( char *s );
int  Tree_Check(int Vnum, vertex *V, int Enum, edge *E, int *visited);
void Read_Graph(int Vnum, vertex *V, int Enum, edge *E);
int SPT_Dijkstra(		// return SPT cost
	int src,			// source vertex index
	int Vnum, vertex *V, int Enum, edge *E,		// Graph data(the same as in HW04, 05)
	int *P1, int *P2, int *P3, int *P4,			// scratch int  storage of size (Vnum+1)
	bool *P5,									// scratch bool storage of size (Vnum+1)
	int *Maxd1,		// The largest distance (output)
	int *Maxd3      // The third largest distance (output)
);

void main ( void ) {
	int Vnum, Enum, src;
	vertex *V;
	edge   *E;
	int    *PV1, *PV2, *PV3, *PV4;
	bool   *PV5;
	int    Tree_cost, Md1, Md3;
	int    Tnum; // # of test cases
	clock_t start, finish;
	double cmpt;

	scanf_s("%d", &Tnum);

	for (int t = 0; t < Tnum; t++ ) {
		scanf_s("%d %d %d", &Vnum, &Enum, &src);
		V = new vertex [Vnum];
		E = new edge [Enum];
		if ( V == NULL || E == NULL ) {
			Error_Exit("Memory Allocation Error");
		}
		Read_Graph(Vnum, V, Enum, E);

		PV1 = new int [Vnum+1]; PV2 = new int [Vnum+1]; PV3 = new int [Vnum+1]; PV4 = new int [Vnum+1];
		PV5 = new bool [Vnum+1];

		if ( PV1 == NULL || PV2 == NULL || PV3 == NULL || PV4 == NULL || PV5 == NULL ) {
			Error_Exit("Memory Allocation Error");
		}

		// SPT by Dijkstra
		start = clock();

		Tree_cost = SPT_Dijkstra( src, Vnum, V, Enum, E, PV1, PV2, PV3, PV4, PV5, &Md1, &Md3 );
        if (Tree_Check(Vnum, V, Enum, E, PV1) == 0) {
			Error_Exit("The resultis not a tree");
		}

		finish = clock();
		cmpt = ((double)(finish - start)) / (double)CLK_TCK;
		if ( t != 0 ) printf("\n");
		printf("**T%d (Dijkstra) (V = %d, E = %d, time = %.3f sec) Tree Cost = %d\n\t 1st max dist = %d, 3rd max dist = %d", 
			t+1, Vnum, Enum, cmpt, Tree_cost, Md1, Md3);

		delete PV1; delete PV2; delete PV3; delete PV4; delete PV5;
	}
}

#define PARENT_OF_ROOT  -100

int Tree_Check(int Vnum, vertex *V, int Enum, edge *E, int *visited) {
    // Check if the given graph is tree using BFS
	
	int u = 0, ep, w;
	std::queue<int> q; // q is a queue of unexplored vertices.		 
	for (int i=0; i < Vnum; i++ )
		visited[i] = NONE;
	visited[u] = PARENT_OF_ROOT;
 	do {
		// check adjacent list at v0 and set Costi_Ni
		for ( ep = V[u].f_hd; ep != NONE; ep = E[ep].fp ) { // forward
			if ( E[ep].flag == false ) {
				continue;
			}
			w = E[ep].vr;
			if ( visited[w] == NONE ) {
				visited[w] = u;
				q.push(w);
			}
			else if ( visited[u] != w ) { // w is visited and w is not the parent of v,
										  // implies that G has a cycle
				while ( !q.empty() ) {
					q.pop();
				}
				return 0;
			}
		}
		for ( ep = V[u].r_hd; ep != NONE; ep = E[ep].rp ) { // backward
			if ( E[ep].flag == false ) {
				continue;
			}
			w = E[ep].vf;
			if ( visited[w] == NONE ) {
				visited[w] = u;
				q.push(w);
			}
			else if ( visited[u] != w ) { // w is visited and w is not the parent of v,
										  // implies that G has a cycle
				while ( !q.empty() ) {
					q.pop();
				}
				return 0;
			}
		}
		if ( q.empty() ) {
			for (int i=0; i < Vnum; i++ )
				if (visited[i] == NONE)
					return 0;			// the graph is not connected
			return 1;
		}
		u = q.front();
		q.pop();	
	} while ( 1 );
}

void Error_Exit ( char *s ) {
  printf("%s\n", s);
  exit(-1);
}