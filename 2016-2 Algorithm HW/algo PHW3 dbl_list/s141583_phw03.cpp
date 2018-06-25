#include <stdio.h>
#include <stdlib.h>

typedef struct ptr_list{
	char pw;
	struct ptr_list *lp;
	struct ptr_list *rp;
} ptr;  // �н����� ������ �����ϴ� Linked List

void Shift_or_Delete (char c);  // <, >, - �Է� �� ����Ǵ� �Լ�
void Insert (char c);  // �� 3���� �ܿ� �ùٸ� �Է¿� ���� �۾��� �����ϴ� �Լ�
void Error_Exit(char* c);  // ����ó�� �Լ�

ptr *Head, *Cursor, *Target;  // �����ͺ��� Cursor�� 'Ŀ��' ������ �ð� �ȴ�

int main(){
	int TestCase, i;
	char input;

	scanf("%d", &TestCase);
	Head = (ptr*)malloc(sizeof(ptr)*TestCase);
	if(Head==NULL)
		Error_Exit("Memory Allocation Error!");  // �����Ҵ� ����ó��

	for(i=0 ; i<TestCase ; i++){
		Head[i].lp = Head[i].rp = NULL;  // �� �׽�Ʈ���̽��� ��������ͷμ� ������ ���� Head�� �ʱ�ȭ ����
	}  // Head[i].pw �� ���� ���� �����Ƿ� �ʱ�ȭ���� �ʾ��� (�޸𸮳��� �߱��� ���ɼ��� ����)
	getchar();  // TestCase ������ ���� ���ڸ� �ɷ����� �۾�

	i=0; Cursor = Head;
	while(i < TestCase){
		input = getchar();
		if (input=='<' || input=='>' || input=='-')
			Shift_or_Delete(input);  // <, >, - �Է� �� ����
		
        else if (input=='\n'){
			i++;
			if(Head+i)
				Cursor = Head + i;  // ���� ���ڰ� �� ��� �ش� TestCase�� �������� �ǹ��ϹǷ�
			continue;               // ���� TestCase�� ���� ���, �� ��������ͷ� Ŀ���� �ű��
		}
        
		else if(input<32 || input>126)
			Error_Exit("Input Password Error!");  // �߸��� �Է¿� ���� ����ó��
		else
			Insert(input);  // ���� �Է� �� ����
	}
	
	for(i=0 ; i<TestCase ; i++){  // �Է� ������ ��ģ �� ����� ����԰� ���ÿ� �޸� ������ �����ϴ� ����
		Cursor = (Head + i) -> rp;
		while(Cursor){
			printf("%c", Cursor -> pw);  // ���
			Target = Cursor;
			Cursor = Cursor -> rp;
			free(Target);  // ��� ����� �ּҴ� ���̻� ���� �����Ƿ� �޸� ������ �����Ѵ�
		}
		printf("\n");  // 1�� TestCase�� ��� �� ������ ��ĥ ������ ����
	}
	free(Head);  // ���������� Head list ��ü�� ���� �޸� �Ҵ� ������ �����Ѵ�
	return 0;
}

void Shift_or_Delete (char c){
	if(c=='<'){
		if(Cursor -> lp == NULL)  // Ŀ���� ���� ������ ���� ��� ��ŵ
			return ;
		Cursor = Cursor -> lp;
	}
    
	else if(c=='>'){
		if(Cursor -> rp == NULL)  // Ŀ���� ���� ������ ���� ��� ��ŵ
			return ;
		Cursor = Cursor -> rp;
	}
    
	else{
		if(Cursor -> lp == NULL)  // Ŀ���� ���� ������ ���� ��� ������ ����� �����Ƿ� ��ŵ
			return ;
		if(Cursor -> rp != NULL)  // Ŀ���� ���� ������ ���� �ʴٸ� ������ �۾��� �߰��� �����ؾ� �Ѵ�
			Cursor -> rp -> lp = Cursor -> lp;
		Cursor -> lp -> rp = Cursor -> rp;

		Target = Cursor;
		Cursor = Cursor -> lp;
		free(Target);  // Delete ����
	}
    
	return ;
}

void Insert (char c){
	Target = (ptr*)malloc(sizeof(ptr));
	if(Target==NULL)
		Error_Exit("Memory Allocation Error!");  // ����ó��
	Target -> pw = c;

	Target -> lp = Cursor;  // Ŀ���� ��ġ�� ���ڸ� �����ϴ� ����
	Target -> rp = Cursor -> rp;
	if(Cursor -> rp != NULL)
		Cursor -> rp -> lp = Target;  // Ŀ���� ���� ������ ��ġ���� ���� ��� �� �۾��� �Բ� ����ȴ�
	Cursor -> rp = Target;
	
	Cursor = Target;  // ���� ������ ���ڿ� Ŀ���� ��ġ��Ų��
	return ;
}

void Error_Exit (char* c){
	printf("%s\n", c);
	exit(-1);
}