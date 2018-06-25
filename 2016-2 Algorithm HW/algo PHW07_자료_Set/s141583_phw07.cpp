#include "Graph_include.h"

// any function definitions here

void make_min_heap (int vtx, int *P1, int *P2, int *P3, int *P4, int size);
void delete_from_min_heap (int *P2, int *P3, int *P4, int size);
void adjust_heap (int vtx, int *P2, int *P3, int *P4);

// You must not declare any global variable

int SPT_Dijkstra(		// return SPT cost
	int src,			// source vertex index
	int Vnum, vertex *V, int Enum, edge *E,		// Graph data(the same as in HW04, 05)
	int *P1, int *P2, int *P3, int *P4,			// scratch int  storage of size (Vnum+1)
	bool *P5,									// scratch bool storage of size (Vnum+1)
	int *Maxd1,		// The largest distance (output)
	int *Maxd3      // The third largest distance (output)
) {
	// P1 for distanse[i], P2 for Heap index in the V array, P3 for V array index in the heap, P4 for heap, P5 for found[i]
	int u; // vertex u
	int w; // vertex w
	int i, epf, epr;
	int heap_size = 0;
	int treecost=0, max3;

	for(i=0 ; i<Vnum ; i++){   // 초기화
		P5[i] = 0; P1[i] = -1;
	}
	P5[src] = 1; P1[src] = 0;
	
	epf = V[src].f_hd;
	epr = V[src].r_hd;
	if(epf == -1 && epr == -1)   // 예외처리
		Error_Exit("G is not connected\n");

	while(epf != -1){   // 초기엔 P1 배열에 V array 순으로 cost가 저장된다.
		P1[ E[epf].vr ] = E[epf].cost ;
		epf = E[epf].fp;
	}
	while(epr != -1){   // 처음에 P1 배열에 저장되는 cost는 src에 인접한 edge들만의 cost이다.
		P1[ E[epr].vf ] = E[epr].cost ;
		epr = E[epr].rp;
	}

	for(i=0 ; i<Vnum ; i++){   // P1 배열의 데이터를 토대로 P4 배열에 heap이 생성된다.
		if(i == src)
			continue;
		make_min_heap(i, P1, P2, P3, P4, heap_size);
		heap_size++;
	}

	epf = V[src].f_hd;
	epr = V[src].r_hd;
	while(epf != -1){   // 이후 P1 배열은 특정 Vertex에 접근하기 위한 Edge index를 저장하는 용도로 사용된다.
		P1[ E[epf].vr ] = epf ;
		epf = E[epf].fp;
	}
	while(epr != -1){   // 원하는 Vertex에 바로 접근이 가능하게 되므로, 결과적으로 시간복잡도가 단축된다.
		P1[ E[epr].vf ] = epr ;
		epr = E[epr].rp;
	}

	for(i=0 ; i<Vnum-1 ; i++){
		u = P3[0];
		treecost = treecost + E[ P1[u] ].cost;
		E[ P1[u] ].flag = 1;
		P5[u] = 1;
		if(i==Vnum-2) break;   // heap에 하나의 노드만이 남아있을 때 반복문을 이탈

		delete_from_min_heap(P2, P3, P4, heap_size);   // min heap의 root node를 제거
		heap_size--;

		epf = V[u].f_hd;
		epr = V[u].r_hd;
		while(epf != -1){
			w = E[epf].vr;
			if(E[epf].flag == 1  ||  w == src){   // 예외처리
				epf = E[epf].fp;
				continue;
			}

			if( P5[w] == 0 ){   // vertex w가 아직 방문된 적이 없을 경우,
				if( P4[ P2[w] ] == -1  ||  P4[ P2[u] ] + E[epf].cost < P4[ P2[w] ] ){  // distance[w]가 기록되지 않았거나 distance[u]+cost(u,w)보다 크다면
					P4[ P2[w] ] = P4[ P2[u] ] + E[epf].cost ;   // distance[w] 값을 갱신한다.
					P1[w] = epf;   // 이 때 P1 배열에서 Vertex W로 접근하기 위한 edge index도 함께 갱신한다.
					               // 이를 통해 P1 배열의 데이터는 특정 Vertex로 접근하는 가장 낮은 cost의 edge 정보만을 보유하게 된다.
					adjust_heap(w, P2, P3, P4);
				}
			}
			epf = E[epf].fp;
		}
		while(epr != -1){   // 위와 같은 작업을 epr에 대하여 동일하게 진행
			w = E[epr].vf;
			if(E[epr].flag == 1  ||  w == src){
				epr = E[epr].rp;
				continue;
			}

			if( P5[w] == 0 ){
				if( P4[ P2[w] ] == -1  ||  P4[ P2[u] ] + E[epr].cost < P4[ P2[w] ] ){
					P4[ P2[w] ] = P4[ P2[u] ] + E[epr].cost ;
					P1[w] = epr;
					adjust_heap(w, P2, P3, P4);
				}
			}
			epr = E[epr].rp;
		}
	}

	*Maxd1 = P4[0];   // 마지막에 heap에 남아있는 node가 src로부터 가장 멀리 떨어진 distance를 갖게 된다.
	if(P4[1]>P4[2])
		max3 = P4[2];
	else
		max3 = P4[1];
	for(i=3 ; i<7 ; i++){   // Maxd3 값을 구하는 연산
		if(P4[i] > max3)
			max3 = P4[i];
	}
	*Maxd3 = max3;

	return treecost;
}

void Read_Graph(int Vnum, vertex *V, int Enum, edge *E) { 
	// construct the adjacency list of a graph using array style linked list
	int i, ep;
	for(i=0 ; i<Vnum ; i++)   // 초기화
		V[i].f_hd = V[i].r_hd = -1;
	for(i=0 ; i<Enum ; i++)
		E[i].fp = E[i].rp = -1;
		
	for(i=0 ; i<Enum ; i++){   // 입력 값을 읽어들여 자료구조 형성
		scanf("%d %d %d", &(E[i].vf), &(E[i].vr), &(E[i].cost) );
		E[i].flag = 0;
		
		if(V[ E[i].vf ].f_hd == -1)   // forward edge 간의 연결고리 형성
			V[ E[i].vf ].f_hd = i;
		else{
			ep=V[ E[i].vf ].f_hd;
			while(E[ep].fp != -1)
				ep = E[ep].fp;
			E[ep].fp = i;
		}

		if(V[ E[i].vr ].r_hd == -1)   // reverse edge 에 대하여 동일한 작업 진행
			V[ E[i].vr ].r_hd = i;
		else{
			ep=V[ E[i].vr ].r_hd;
			while(E[ep].rp != -1)
				ep = E[ep].rp;
			E[ep].rp = i;
		}
	}
}

// You may write any functions here (eg. heap insert, delete, adjustment)

void make_min_heap (int vtx, int *P1, int *P2, int *P3, int *P4, int size){
	int i=size, j=size, temp;

	P3[i] = vtx;   // 밑작업
	P4[i] = P1[vtx];
	P2[vtx] = i;
	if(P4[i] == -1)   // heap의 마지막에 추가되는 distance가 무한대라면 작업을 중단
		return ;

	while(i){
		if(i%2 == 0)
			i--;
		i = i/2;

		if(P4[i]==-1  ||  P4[i] > P4[j]){   // 추가된 노드(j)가 부모노드(i)보다 낮은 값을 가지고 있다면 두 값을 교환
			// swap P4[i] and P4[j]
			temp = P4[i]; P4[i] = P4[j]; P4[j] = temp;
			// swap P3[i] and P3[j]
			temp = P3[i]; P3[i] = P3[j]; P3[j] = temp;
			// swap P2[ P3[i] ] and P2[ P3[j] ]
			temp = P2[ P3[i] ]; P2[ P3[i] ] = P2[ P3[j] ]; P2[ P3[j] ] = temp;
			
			j = i;
		}
		else
			break;
	}

}

void delete_from_min_heap (int *P2, int *P3, int *P4, int size){
	int ti;  // Target Index
	int temp, i=0;

	// swap P4[size-1] and P4[0]
	temp = P4[size-1]; P4[size-1] = P4[0]; P4[0] = temp;
	// swap P3[size-1] and P3[0]
	temp = P3[size-1]; P3[size-1] = P3[0]; P3[0] = temp;
	// swap P2[ P3[size-1] ] and P2[ P3[0] ]
	temp = P2[ P3[size-1] ]; P2[ P3[size-1] ] = P2[ P3[0] ]; P2[ P3[0] ] = temp;

	size--;
	while( (i+1)*2 <= size ){   // 현재 노드(i)의 자식노드가 존재할 경우에만 다음 작업을 수행
		if( (i+1)*2 == size){   // 현재 노드(i)의 자식노드가 하나만 존재할 경우
			if(P4[ (i+1)*2 -1 ] == -1  ||  P4[ (i+1)*2 -1 ] > P4[i]) break;
			else ti = (i+1)*2 -1;
		}
		else{   // 자식노드가 두개 존재할 경우
			if(P4[ (i+1)*2 ] == -1   &&   P4[ (i+1)*2 -1 ] == -1)
				break;
			else if(P4[ (i+1)*2 ] == -1)
				ti = (i+1)*2 -1;
			else if(P4[ (i+1)*2 -1 ] == -1)
				ti = (i+1)*2;
			else{   // 두 자식노드 모두 무한대가 아닌 다른 값을 가지고 있을 경우 두 값을 비교하여 타겟index를 설정
				if(P4[ (i+1)*2 -1 ] <= P4[ (i+1)*2 ])
					ti = (i+1)*2 -1;
				else
					ti = (i+1)*2;
			}
		}
		// swap P4[i] and P4[ti]
		temp = P4[i]; P4[i] = P4[ti]; P4[ti] = temp;
		// swap P3[i] and P3[ti]
		temp = P3[i]; P3[i] = P3[ti]; P3[ti] = temp;
		// swap P2[ P3[i] ] and P2[ P3[ti] ]
		temp = P2[ P3[i] ]; P2[ P3[i] ] = P2[ P3[ti] ]; P2[ P3[ti] ] = temp;

		i = ti;
	}

}

void adjust_heap (int vtx, int *P2, int *P3, int *P4){
	int i = P2[vtx];
	int j = i, temp;

	while(i){
		if(i%2 == 0)
			i--;
		i = i/2;

		if(P4[i] == -1  ||  P4[i] > P4[j]){   // 반복문을 통해 수행하는 동작은 make_min_heap 함수와 같다
			// swap P4[i] and P4[j]
			temp = P4[i]; P4[i] = P4[j]; P4[j] = temp;
			// swap P3[i] and P3[j]
			temp = P3[i]; P3[i] = P3[j]; P3[j] = temp;
			// swap P2[ P3[i] ] and P2[ P3[j] ]
			temp = P2[ P3[i] ]; P2[ P3[i] ] = P2[ P3[j] ]; P2[ P3[j] ] = temp;
			
			j = i;
		}
		else
			break;
	}
}
