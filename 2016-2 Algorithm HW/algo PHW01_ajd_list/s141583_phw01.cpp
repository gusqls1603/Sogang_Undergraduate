#include "Graph_adj_list.h"
#include "Graph_adj_list_extern.h"
#include "Graph_adj_list_function.h"

void Read_Graph_adj_list(int Vnum, vertex *V, int Enum, edge *E) {
	// Read graph information and form an adjacent list
	int i, exist=0;
	ptr_L *check;
	for(i=0 ; i<Vnum ; i++){ // vertex array 초기화
		V[i].name = i;
		V[i].p = NULL;
	}
	for(i=0 ; i<Enum ; i++){
		E[i].name = i;
		scanf("%d %d %d", &E[i].vf, &E[i].vr, &E[i].cost); // edge array 초기화
		
		check = V[ E[i].vf ].p;
		while(check != NULL){
			if( V[ E[i].vf ].p ->i == i){ // list에 추가하고자 하는 edge index가 adj list에 이미 존재한다면
				exist = 1; // 표식을 남기고 while문을 탈출
				break;
			}
			else
				check = check->p;
		}
		if(exist == 0)
			pushQ_ptr_L( &V[ E[i].vf ].p , i); // 표식이 남겨지지 않았다면 adj list에 edge index를 push
		else
			exist = 0;

		check = V[ E[i].vr ].p; // 위와 동일한 작업을 vr에 대해서도 수행
		while(check != NULL){
			if( V[ E[i].vr ].p ->i == i){
				exist = 1;
				break;
			}
			else
				check = check->p;
		}
		if(exist == 0)
			pushQ_ptr_L( &V[ E[i].vr ].p , i);
		else
			exist = 0;
	}
	return;
}

void Free_Graph_adj_list(int Vnum, vertex *V, int Enum, edge *E) {
	// remove the links (ptr_Ls) for the adjacent list
	for(int i=0 ; i<Vnum ; i++){
		while(V[i].p != NULL)
			popQ_ptr_L(&V[i].p); // vertex array의 모든 index에 대하여 adj list의 pop을 진행
	}
	return;
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
	V[src].flag = true;
	ptr_L *check = V[src].p; int tcost = *cost;
	while(check != NULL){
		if( E[ check->i ].flag == false){ // 탐색을 시도하려는 edge가 이미 방문된 적이 있는지 체크
			if( V[ E[ check->i ].vf ].flag == false){
				DFS_Tree_adj_list(E[ check->i ].vf, Vnum, V, Enum, E, &tcost);
				E[ check->i ].flag = true;
				tcost++;
			} // 해당 edge에 연결된 두 vertex (vf or vr) 중, 방문된 적이 없는 vertex를 찾아 그 vertex를 src로 하는 DFS를 진행
			else if( V[ E[ check->i ].vr ].flag == false){
				DFS_Tree_adj_list(E[ check->i ].vr, Vnum, V, Enum, E, &tcost);
				E[ check->i ].flag = true;
				tcost++;
			}
			else
				check = check->p;
		}
		else
			check = check->p;
	}
	*cost = tcost;
	return ;
}