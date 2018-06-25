#include <stdio.h>
#include <stdlib.h>
#include "LinkedList_ptr_L.h"


void Error_Exit(char *s);
void Find_a_Path(char **G, int N, int start, ptr_L **path, ptr_L **path_end);
void Print_Path(ptr_L *path);

ptr_L *Find_Euler(char **G, int N, int start) {
	ptr_L *path = NULL, *path_end = NULL;

	// Write your own code
	
	Find_a_Path(G, N, start, &path, &path_end); // �ʱ� ��θ� ������ ��,
	ptr_L *head = path, *rear = path_end, *scan_vtx = head; // ã�� ����� ó���� ���� ���� head�� rear �����Ϳ� ����.
	int i = 0;

	while(scan_vtx){ // scan_vtx �����ʹ� ã�� ����� vtx���� ���������� üũ�ϸ� ���� ������ ���� ������ �ִ��� üũ�Ѵ�.
		for(i=0 ; i<N ; i++){
			if(G[scan_vtx->i][i] != 0){ // �ش� vtx�� �����, ���� ������ ���� ������ �ִ��� Ȯ��
				path = NULL, path_end = NULL;
				Find_a_Path(G, N, scan_vtx->i, &path, &path_end); // �ش� vtx�� ���������� �ϴ� �߰� ��θ� ã�� �۾�

				path_end->p = scan_vtx->p;
				scan_vtx->p = path->p;
				Free_ptr_L(path); // ã�� �߰� ��θ� �ʱ� ��ο� �̾��� ��, �ߺ��Ǵ� vtx �ϳ��� �Ҵ������ϴ� �۾�
			}
		}

		scan_vtx = scan_vtx->p;
	}
	path = head;

	return path;
}

void Find_a_Path(char **G, int N, int start, ptr_L **path, ptr_L **path_end) {

	// Write your own code
	ptr_L *vtx_queue;
	int i=0, vtx_index = start;
	while(1){
		if(i>=N){
			vtx_queue = addFIFO_ptr_L (path, path_end, vtx_index); // ���̻� vtx�� ����� ������ ���� ��� ������ �������� �� �Լ��� ����
			break;
		}

		if(G[vtx_index][i] != 0){ // vtx�� ����� ������ ���� ���
			vtx_queue = addFIFO_ptr_L (path, path_end, vtx_index); // �ش� vtx�� index ������ �����ϴ� queue�� ����
			G[vtx_index][i]--;
			G[i][vtx_index]--; // ������ G���� �ϳ��� �����ִ� �۾�
			
			vtx_index = i; // vtx�� ����� ������ ���� ���� vtx�� �̵�
			i = 0;
		}
		else
			i++;
	}

	return;
}

int getOddDegreeNum(char **G, int N, int *start) {
	int oddDegree = 0;
	
	// Write your own code
	int degree = 0, flag = 0, _start;

	for(int i=0, j=0 ; i<N ; i++){
		for(j=0 ; j<N ; j++)
			degree += G[i][j]; // i��° vtx�� degree�� ���ϴ� �۾�
		if(degree %2 == 1){ // degree�� Ȧ������ üũ
			oddDegree++;
			if(flag == 0){
				_start = i;
				flag = 1; // degree�� Ȧ���� ù��° vtx�� ������������ ����
			}
		}
		degree = 0;
	}

	if(oddDegree == 2)
		*start = _start;

	return oddDegree;
}

void Print_Path(ptr_L *path) {
	ptr_L *p;
	for (p = path; p != NULL; p = p->p) {
		printf("%d ",p->i);
	}
	printf("\n");
}