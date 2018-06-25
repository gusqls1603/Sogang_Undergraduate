#include <stdio.h>
#include <stdlib.h>

typedef struct town_array{
	int num;
	struct town_array *next;  // next의 도움을 받아 자식노드의 자매노드에 접근할 수 있다
} arr;

typedef struct town_tree{
	int yes;  // 해당 마을이 우수마을로 지정될 경우 우수마을 주민 총합이 저장된다
	int no;  // 반대로 우수마을이 안될 경우 총합 저장
	arr *children;
	struct town_tree *parent;
} town ;

void f_town (int a);  // recursive하게 작업이 진행된다

town *T;
arr *A;

int main()
{
	int N, i, a1, a2, tmp;  arr *check;
	scanf("%d", &N);

	T = (town*)malloc(sizeof(town)*N);
	A = (arr*)malloc(sizeof(arr)*N);

	for(i=0 ; i<N ; i++){  // 초기화
		scanf("%d", &(T[i].yes) );
		T[i].parent = NULL;
		T[i].children = NULL;
		T[i].no = 0;
		A[i].next = NULL;
	}
	
	for(i=0 ; i<N-1 ; i++){  // 입력받는 인접마을 index 정보를 토대로 트리를 구성
		scanf("%d %d", &a1, &a2);
		if(a1>a2){
			tmp = a1;
			a1 = a2;
			a2 = tmp; }
		A[i].num = a2;  // a2번째 마을은 a1번째 마을의 children이 되며,
		
		if(T[a1-1].children == NULL)  // a1번째 마을의 children list에 a2번째 마을의 index가 추가된다.
			T[a1-1].children = A+i ;
		else{
			check = T[a1-1].children;
			while(check->next != NULL)
				check = check->next;
			
			check->next = A+i ;
		}

		T[a2-1].parent = T + a1 -1;  // a1번째 마을을 a2번째 마을의 parent로 설정
	}

	f_town(1);  // 이 함수가 종료될 때 우수마을 주민수의 총합이 1번째 마을의 yes 또는 no에 저장된다

	if(T[0].yes >= T[0].no)
		printf("%d\n", T[0].yes);
	else
		printf("%d\n", T[0].no);

	return 0;
}

void f_town(int a)
{
	arr *chk = T[a-1].children;
	while(chk != NULL){  // 현재마을(a번째 마을)의 자식노드 마을을 먼저 작업하기 위한 재귀호출
		f_town(chk->num);
		chk = chk->next;
	}

	if(T[a-1].parent != NULL){  // 현재마을의 부모노드 마을이 존재할 경우
		if(T[a-1].yes >= T[a-1].no){  // 현재마을이 우수마을로 지정되는 것이 이득이라면
			(T[a-1].parent)->no += T[a-1].yes;  // 부모노드는 우수마을이 아니어야 하지만
			(T[a-1].parent)->yes += T[a-1].no;
		}
		else{  // 현재마을이 우수마을로 지정되는 것이 이득이 아니라면
			(T[a-1].parent)->no += T[a-1].no;  // 부모노드는 우수마을이 아닐수도 있고
			(T[a-1].parent)->yes += T[a-1].no;  // 우수마을일 수도 있다
		}
	}
}