#include "Graph_adj_list.h"
#include "Graph_adj_list_extern.h"
#include "Graph_adj_list_function.h"

void Read_Graph_adj_list(int Vnum, vertex *V, int Enum, edge *E) {
	// Read graph information and form an adjacent list

}

void Free_Graph_adj_list(int Vnum, vertex *V, int Enum, edge *E) {
	// remove the links (ptr_Ls) for the adjacent list

}

void DFS_Tree_adj_list (
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex *V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge   *E,     // edge structure array (starting index 0)
	int    *cost   // accumulated tree cost
) {
	// preform DFS and set the flags of edges in the DFS tree
	// return the DFS tree cost.
	// Recursively do DFS.


}