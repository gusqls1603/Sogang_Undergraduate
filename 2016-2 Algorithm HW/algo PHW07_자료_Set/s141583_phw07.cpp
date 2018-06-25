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

	for(i=0 ; i<Vnum ; i++){   // �ʱ�ȭ
		P5[i] = 0; P1[i] = -1;
	}
	P5[src] = 1; P1[src] = 0;
	
	epf = V[src].f_hd;
	epr = V[src].r_hd;
	if(epf == -1 && epr == -1)   // ����ó��
		Error_Exit("G is not connected\n");

	while(epf != -1){   // �ʱ⿣ P1 �迭�� V array ������ cost�� ����ȴ�.
		P1[ E[epf].vr ] = E[epf].cost ;
		epf = E[epf].fp;
	}
	while(epr != -1){   // ó���� P1 �迭�� ����Ǵ� cost�� src�� ������ edge�鸸�� cost�̴�.
		P1[ E[epr].vf ] = E[epr].cost ;
		epr = E[epr].rp;
	}

	for(i=0 ; i<Vnum ; i++){   // P1 �迭�� �����͸� ���� P4 �迭�� heap�� �����ȴ�.
		if(i == src)
			continue;
		make_min_heap(i, P1, P2, P3, P4, heap_size);
		heap_size++;
	}

	epf = V[src].f_hd;
	epr = V[src].r_hd;
	while(epf != -1){   // ���� P1 �迭�� Ư�� Vertex�� �����ϱ� ���� Edge index�� �����ϴ� �뵵�� ���ȴ�.
		P1[ E[epf].vr ] = epf ;
		epf = E[epf].fp;
	}
	while(epr != -1){   // ���ϴ� Vertex�� �ٷ� ������ �����ϰ� �ǹǷ�, ��������� �ð����⵵�� ����ȴ�.
		P1[ E[epr].vf ] = epr ;
		epr = E[epr].rp;
	}

	for(i=0 ; i<Vnum-1 ; i++){
		u = P3[0];
		treecost = treecost + E[ P1[u] ].cost;
		E[ P1[u] ].flag = 1;
		P5[u] = 1;
		if(i==Vnum-2) break;   // heap�� �ϳ��� ��常�� �������� �� �ݺ����� ��Ż

		delete_from_min_heap(P2, P3, P4, heap_size);   // min heap�� root node�� ����
		heap_size--;

		epf = V[u].f_hd;
		epr = V[u].r_hd;
		while(epf != -1){
			w = E[epf].vr;
			if(E[epf].flag == 1  ||  w == src){   // ����ó��
				epf = E[epf].fp;
				continue;
			}

			if( P5[w] == 0 ){   // vertex w�� ���� �湮�� ���� ���� ���,
				if( P4[ P2[w] ] == -1  ||  P4[ P2[u] ] + E[epf].cost < P4[ P2[w] ] ){  // distance[w]�� ��ϵ��� �ʾҰų� distance[u]+cost(u,w)���� ũ�ٸ�
					P4[ P2[w] ] = P4[ P2[u] ] + E[epf].cost ;   // distance[w] ���� �����Ѵ�.
					P1[w] = epf;   // �� �� P1 �迭���� Vertex W�� �����ϱ� ���� edge index�� �Բ� �����Ѵ�.
					               // �̸� ���� P1 �迭�� �����ʹ� Ư�� Vertex�� �����ϴ� ���� ���� cost�� edge �������� �����ϰ� �ȴ�.
					adjust_heap(w, P2, P3, P4);
				}
			}
			epf = E[epf].fp;
		}
		while(epr != -1){   // ���� ���� �۾��� epr�� ���Ͽ� �����ϰ� ����
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

	*Maxd1 = P4[0];   // �������� heap�� �����ִ� node�� src�κ��� ���� �ָ� ������ distance�� ���� �ȴ�.
	if(P4[1]>P4[2])
		max3 = P4[2];
	else
		max3 = P4[1];
	for(i=3 ; i<7 ; i++){   // Maxd3 ���� ���ϴ� ����
		if(P4[i] > max3)
			max3 = P4[i];
	}
	*Maxd3 = max3;

	return treecost;
}

void Read_Graph(int Vnum, vertex *V, int Enum, edge *E) { 
	// construct the adjacency list of a graph using array style linked list
	int i, ep;
	for(i=0 ; i<Vnum ; i++)   // �ʱ�ȭ
		V[i].f_hd = V[i].r_hd = -1;
	for(i=0 ; i<Enum ; i++)
		E[i].fp = E[i].rp = -1;
		
	for(i=0 ; i<Enum ; i++){   // �Է� ���� �о�鿩 �ڷᱸ�� ����
		scanf("%d %d %d", &(E[i].vf), &(E[i].vr), &(E[i].cost) );
		E[i].flag = 0;
		
		if(V[ E[i].vf ].f_hd == -1)   // forward edge ���� ����� ����
			V[ E[i].vf ].f_hd = i;
		else{
			ep=V[ E[i].vf ].f_hd;
			while(E[ep].fp != -1)
				ep = E[ep].fp;
			E[ep].fp = i;
		}

		if(V[ E[i].vr ].r_hd == -1)   // reverse edge �� ���Ͽ� ������ �۾� ����
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

	P3[i] = vtx;   // ���۾�
	P4[i] = P1[vtx];
	P2[vtx] = i;
	if(P4[i] == -1)   // heap�� �������� �߰��Ǵ� distance�� ���Ѵ��� �۾��� �ߴ�
		return ;

	while(i){
		if(i%2 == 0)
			i--;
		i = i/2;

		if(P4[i]==-1  ||  P4[i] > P4[j]){   // �߰��� ���(j)�� �θ���(i)���� ���� ���� ������ �ִٸ� �� ���� ��ȯ
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
	while( (i+1)*2 <= size ){   // ���� ���(i)�� �ڽĳ�尡 ������ ��쿡�� ���� �۾��� ����
		if( (i+1)*2 == size){   // ���� ���(i)�� �ڽĳ�尡 �ϳ��� ������ ���
			if(P4[ (i+1)*2 -1 ] == -1  ||  P4[ (i+1)*2 -1 ] > P4[i]) break;
			else ti = (i+1)*2 -1;
		}
		else{   // �ڽĳ�尡 �ΰ� ������ ���
			if(P4[ (i+1)*2 ] == -1   &&   P4[ (i+1)*2 -1 ] == -1)
				break;
			else if(P4[ (i+1)*2 ] == -1)
				ti = (i+1)*2 -1;
			else if(P4[ (i+1)*2 -1 ] == -1)
				ti = (i+1)*2;
			else{   // �� �ڽĳ�� ��� ���Ѵ밡 �ƴ� �ٸ� ���� ������ ���� ��� �� ���� ���Ͽ� Ÿ��index�� ����
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

		if(P4[i] == -1  ||  P4[i] > P4[j]){   // �ݺ����� ���� �����ϴ� ������ make_min_heap �Լ��� ����
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
