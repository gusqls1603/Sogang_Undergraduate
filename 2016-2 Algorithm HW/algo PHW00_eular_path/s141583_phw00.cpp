#include <stdio.h>
#include <stdlib.h>
#include "LinkedList_ptr_L.h"


void Error_Exit(char *s);
void Find_a_Path(char **G, int N, int start, ptr_L **path, ptr_L **path_end);
void Print_Path(ptr_L *path);

ptr_L *Find_Euler(char **G, int N, int start) {
	ptr_L *path = NULL, *path_end = NULL;

	// Write your own code
	
	Find_a_Path(G, N, start, &path, &path_end); // 초기 경로를 세팅한 뒤,
	ptr_L *head = path, *rear = path_end, *scan_vtx = head; // 찾은 경로의 처음과 끝을 각각 head와 rear 포인터에 저장.
	int i = 0;

	while(scan_vtx){ // scan_vtx 포인터는 찾은 경로의 vtx들을 순차적으로 체크하며 아직 사용되지 않은 에지가 있는지 체크한다.
		for(i=0 ; i<N ; i++){
			if(G[scan_vtx->i][i] != 0){ // 해당 vtx와 연결된, 아직 사용되지 않은 에지가 있는지 확인
				path = NULL, path_end = NULL;
				Find_a_Path(G, N, scan_vtx->i, &path, &path_end); // 해당 vtx를 시작점으로 하는 추가 경로를 찾는 작업

				path_end->p = scan_vtx->p;
				scan_vtx->p = path->p;
				Free_ptr_L(path); // 찾은 추가 경로를 초기 경로에 이어준 뒤, 중복되는 vtx 하나를 할당해제하는 작업
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
			vtx_queue = addFIFO_ptr_L (path, path_end, vtx_index); // 더이상 vtx에 연결된 에지가 없을 경우 루프를 빠져나간 뒤 함수를 종료
			break;
		}

		if(G[vtx_index][i] != 0){ // vtx에 연결된 에지가 있을 경우
			vtx_queue = addFIFO_ptr_L (path, path_end, vtx_index); // 해당 vtx의 index 정보를 저장하는 queue를 생성
			G[vtx_index][i]--;
			G[i][vtx_index]--; // 에지를 G에서 하나씩 지워주는 작업
			
			vtx_index = i; // vtx에 연결된 에지를 통해 다음 vtx로 이동
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
			degree += G[i][j]; // i번째 vtx의 degree를 구하는 작업
		if(degree %2 == 1){ // degree가 홀수인지 체크
			oddDegree++;
			if(flag == 0){
				_start = i;
				flag = 1; // degree가 홀수인 첫번째 vtx를 시작지점으로 결정
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