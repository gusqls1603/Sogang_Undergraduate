// Graph_adj_list_main.cpp
void Error_Exit ( char *s );

// Graph_adj_list_ptr_L.cpp
void	pushQ_ptr_L ( ptr_L **Q, int i );
int		popQ_ptr_L ( ptr_L **Q );
ptr_L	*Alloc_ptr_L( void );
void	Free_ptr_L( ptr_L *ptr );
void	Free_ptr_L_pool ( void );

// Graph_adj_list_tree_check.cpp
int  Tree_Check_adj_list(int Vnum, vertex *V, int Enum, edge *E);

// Graph_adj_list_data_in.cpp
void Read_Graph_adj_list(int Vnum, vertex *V, int Enum, edge *E);
void Free_Graph_adj_list(int Vnum, vertex *V, int Enum, edge *E);

// Graph_adj_list_tree_check.cpp
void DFS_Tree_adj_list ( 
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex *V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge   *E,     // edge structure array (starting index 0)
	int    *cost   // accumulated tree cost
);