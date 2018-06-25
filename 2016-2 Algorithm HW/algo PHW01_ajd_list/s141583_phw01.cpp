#include "Graph_adj_list.h"
#include "Graph_adj_list_extern.h"
#include "Graph_adj_list_function.h"

void Read_Graph_adj_list(int Vnum, vertex *V, int Enum, edge *E) {
	// Read graph information and form an adjacent list
	int i, exist=0;
	ptr_L *check;
	for(i=0 ; i<Vnum ; i++){ // vertex array �ʱ�ȭ
		V[i].name = i;
		V[i].p = NULL;
	}
	for(i=0 ; i<Enum ; i++){
		E[i].name = i;
		scanf("%d %d %d", &E[i].vf, &E[i].vr, &E[i].cost); // edge array �ʱ�ȭ
		
		check = V[ E[i].vf ].p;
		while(check != NULL){
			if( V[ E[i].vf ].p ->i == i){ // list�� �߰��ϰ��� �ϴ� edge index�� adj list�� �̹� �����Ѵٸ�
				exist = 1; // ǥ���� ����� while���� Ż��
				break;
			}
			else
				check = check->p;
		}
		if(exist == 0)
			pushQ_ptr_L( &V[ E[i].vf ].p , i); // ǥ���� �������� �ʾҴٸ� adj list�� edge index�� push
		else
			exist = 0;

		check = V[ E[i].vr ].p; // ���� ������ �۾��� vr�� ���ؼ��� ����
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
			popQ_ptr_L(&V[i].p); // vertex array�� ��� index�� ���Ͽ� adj list�� pop�� ����
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
		if( E[ check->i ].flag == false){ // Ž���� �õ��Ϸ��� edge�� �̹� �湮�� ���� �ִ��� üũ
			if( V[ E[ check->i ].vf ].flag == false){
				DFS_Tree_adj_list(E[ check->i ].vf, Vnum, V, Enum, E, &tcost);
				E[ check->i ].flag = true;
				tcost++;
			} // �ش� edge�� ����� �� vertex (vf or vr) ��, �湮�� ���� ���� vertex�� ã�� �� vertex�� src�� �ϴ� DFS�� ����
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