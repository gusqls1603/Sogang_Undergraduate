#include "Graph_adj_list.h"
#include "Graph_adj_list_global.h"
#include "Graph_adj_list_function.h"

void main ( void ) {
	int Vnum, Enum, src;
	vertex *V;
	edge   *E;
	int    Tree_cost; // DFS_tree cost
	int    Tnum;      // # of test cases
	int    Memsize_for_array;
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
		Memsize_for_array = Vnum * sizeof(vertex) + Enum * sizeof(edge);
		if ( Used_Memory_for_array < Memsize_for_array ) {
			Used_Memory_for_array = Memsize_for_array;
		}
		Read_Graph_adj_list(Vnum, V, Enum, E);
		// Do DFS
		start = clock();
	    for ( int i = 0; i < Vnum; i++ ) {
		  V[i].flag = false;  // clear flag
	    }
	    for ( int i = 0; i < Enum; i++ ) {
		  E[i].flag = false;  // clear flag
	    }
		Tree_cost = 0;
		DFS_Tree_adj_list (src, Vnum, V, Enum, E, &Tree_cost );

		finish = clock();
		cmpt = ((double)(finish - start)) / (double)CLK_TCK;

        if (Tree_Check_adj_list(Vnum, V, Enum, E ) == 0) { // check if spanning tree
			if ( t != 0 ) printf("\n");
			printf("**T%d (DFS) (V = %d, E = %d, time = %.3f sec) ERROR: NOT A TREE!!", 
				t+1, Vnum, Enum, cmpt);
		}
		else {
			if ( t != 0 ) printf("\n");
			printf("**T%d (DFS) (V = %d, E = %d, time = %.3f sec) Tree Cost = %d", 
				t+1, Vnum, Enum, cmpt, Tree_cost);
		}

		Free_Graph_adj_list(Vnum, V, Enum, E); // free linked lists for adj list
		if (ptr_L_cnt != 0) {
			printf("ERROR: ptr_L_cnt must be zero.");
			exit(-1);
		}
	    delete V; delete E;   // free V and E array
	}
	printf("\nTotal memory used = %d", Used_Memory_for_ptr + Used_Memory_for_array);
	Free_ptr_L_pool ( ); // return ptr_Ls in the pool to the system memory
}

void Error_Exit ( char *s ) {
  printf("%s\n", s);
  exit(-1);
}