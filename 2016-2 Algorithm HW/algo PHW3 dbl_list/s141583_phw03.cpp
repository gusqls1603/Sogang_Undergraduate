#include <stdio.h>
#include <stdlib.h>

typedef struct ptr_list{
	char pw;
	struct ptr_list *lp;
	struct ptr_list *rp;
} ptr;  // 패스워드 정보를 저장하는 Linked List

void Shift_or_Delete (char c);  // <, >, - 입력 시 실행되는 함수
void Insert (char c);  // 위 3가지 외에 올바른 입력에 대한 작업을 수행하는 함수
void Error_Exit(char* c);  // 예외처리 함수

ptr *Head, *Cursor, *Target;  // 포인터변수 Cursor가 '커서' 역할을 맡게 된다

int main(){
	int TestCase, i;
	char input;

	scanf("%d", &TestCase);
	Head = (ptr*)malloc(sizeof(ptr)*TestCase);
	if(Head==NULL)
		Error_Exit("Memory Allocation Error!");  // 동적할당 예외처리

	for(i=0 ; i<TestCase ; i++){
		Head[i].lp = Head[i].rp = NULL;  // 각 테스트케이스의 헤드포인터로서 역할을 맡을 Head의 초기화 과정
	}  // Head[i].pw 는 사용될 일이 없으므로 초기화하지 않았음 (메모리낭비를 야기할 가능성이 있음)
	getchar();  // TestCase 이후의 개행 문자를 걸러내는 작업

	i=0; Cursor = Head;
	while(i < TestCase){
		input = getchar();
		if (input=='<' || input=='>' || input=='-')
			Shift_or_Delete(input);  // <, >, - 입력 시 수행
		
        else if (input=='\n'){
			i++;
			if(Head+i)
				Cursor = Head + i;  // 개행 문자가 올 경우 해당 TestCase가 끝났음을 의미하므로
			continue;               // 다음 TestCase가 있을 경우, 그 헤드포인터로 커서를 옮긴다
		}
        
		else if(input<32 || input>126)
			Error_Exit("Input Password Error!");  // 잘못된 입력에 대한 예외처리
		else
			Insert(input);  // 정상 입력 시 수행
	}
	
	for(i=0 ; i<TestCase ; i++){  // 입력 수행을 마친 뒤 결과를 출력함과 동시에 메모리 해제를 진행하는 과정
		Cursor = (Head + i) -> rp;
		while(Cursor){
			printf("%c", Cursor -> pw);  // 출력
			Target = Cursor;
			Cursor = Cursor -> rp;
			free(Target);  // 방금 출력한 주소는 더이상 쓰지 않으므로 메모리 해제를 진행한다
		}
		printf("\n");  // 1개 TestCase의 출력 및 해제를 마칠 때마다 개행
	}
	free(Head);  // 마지막으로 Head list 전체에 대한 메모리 할당 해제를 진행한다
	return 0;
}

void Shift_or_Delete (char c){
	if(c=='<'){
		if(Cursor -> lp == NULL)  // 커서가 가장 좌측에 있을 경우 스킵
			return ;
		Cursor = Cursor -> lp;
	}
    
	else if(c=='>'){
		if(Cursor -> rp == NULL)  // 커서가 가장 우측에 있을 경우 스킵
			return ;
		Cursor = Cursor -> rp;
	}
    
	else{
		if(Cursor -> lp == NULL)  // 커서가 가장 좌측에 있을 경우 삭제할 대상이 없으므로 스킵
			return ;
		if(Cursor -> rp != NULL)  // 커서가 가장 우측에 있지 않다면 밑줄의 작업을 추가로 수행해야 한다
			Cursor -> rp -> lp = Cursor -> lp;
		Cursor -> lp -> rp = Cursor -> rp;

		Target = Cursor;
		Cursor = Cursor -> lp;
		free(Target);  // Delete 수행
	}
    
	return ;
}

void Insert (char c){
	Target = (ptr*)malloc(sizeof(ptr));
	if(Target==NULL)
		Error_Exit("Memory Allocation Error!");  // 예외처리
	Target -> pw = c;

	Target -> lp = Cursor;  // 커서의 위치에 문자를 삽입하는 과정
	Target -> rp = Cursor -> rp;
	if(Cursor -> rp != NULL)
		Cursor -> rp -> lp = Target;  // 커서가 가장 우측에 위치하지 않을 경우 이 작업이 함께 진행된다
	Cursor -> rp = Target;
	
	Cursor = Target;  // 새로 삽입한 문자에 커서를 위치시킨다
	return ;
}

void Error_Exit (char* c){
	printf("%s\n", c);
	exit(-1);
}