#include <stdio.h>
#include <stdlib.h>

typedef struct town_array{
	int num;
	struct town_array *next;  // next�� ������ �޾� �ڽĳ���� �ڸų�忡 ������ �� �ִ�
} arr;

typedef struct town_tree{
	int yes;  // �ش� ������ ��������� ������ ��� ������� �ֹ� ������ ����ȴ�
	int no;  // �ݴ�� ��������� �ȵ� ��� ���� ����
	arr *children;
	struct town_tree *parent;
} town ;

void f_town (int a);  // recursive�ϰ� �۾��� ����ȴ�

town *T;
arr *A;

int main()
{
	int N, i, a1, a2, tmp;  arr *check;
	scanf("%d", &N);

	T = (town*)malloc(sizeof(town)*N);
	A = (arr*)malloc(sizeof(arr)*N);

	for(i=0 ; i<N ; i++){  // �ʱ�ȭ
		scanf("%d", &(T[i].yes) );
		T[i].parent = NULL;
		T[i].children = NULL;
		T[i].no = 0;
		A[i].next = NULL;
	}
	
	for(i=0 ; i<N-1 ; i++){  // �Է¹޴� �������� index ������ ���� Ʈ���� ����
		scanf("%d %d", &a1, &a2);
		if(a1>a2){
			tmp = a1;
			a1 = a2;
			a2 = tmp; }
		A[i].num = a2;  // a2��° ������ a1��° ������ children�� �Ǹ�,
		
		if(T[a1-1].children == NULL)  // a1��° ������ children list�� a2��° ������ index�� �߰��ȴ�.
			T[a1-1].children = A+i ;
		else{
			check = T[a1-1].children;
			while(check->next != NULL)
				check = check->next;
			
			check->next = A+i ;
		}

		T[a2-1].parent = T + a1 -1;  // a1��° ������ a2��° ������ parent�� ����
	}

	f_town(1);  // �� �Լ��� ����� �� ������� �ֹμ��� ������ 1��° ������ yes �Ǵ� no�� ����ȴ�

	if(T[0].yes >= T[0].no)
		printf("%d\n", T[0].yes);
	else
		printf("%d\n", T[0].no);

	return 0;
}

void f_town(int a)
{
	arr *chk = T[a-1].children;
	while(chk != NULL){  // ���縶��(a��° ����)�� �ڽĳ�� ������ ���� �۾��ϱ� ���� ���ȣ��
		f_town(chk->num);
		chk = chk->next;
	}

	if(T[a-1].parent != NULL){  // ���縶���� �θ��� ������ ������ ���
		if(T[a-1].yes >= T[a-1].no){  // ���縶���� ��������� �����Ǵ� ���� �̵��̶��
			(T[a-1].parent)->no += T[a-1].yes;  // �θ���� ��������� �ƴϾ�� ������
			(T[a-1].parent)->yes += T[a-1].no;
		}
		else{  // ���縶���� ��������� �����Ǵ� ���� �̵��� �ƴ϶��
			(T[a-1].parent)->no += T[a-1].no;  // �θ���� ��������� �ƴҼ��� �ְ�
			(T[a-1].parent)->yes += T[a-1].no;  // ��������� ���� �ִ�
		}
	}
}