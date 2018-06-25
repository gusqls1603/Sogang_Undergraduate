#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void Error_Exit ( char *s );
int Return_Min (int a, int b, int c);

void Edit_Distance ( 
	// inputs
	char *SS,		// Source string array
	char *TS,		// Edited string array
	int ins_cost, int del_cost, int sub_cost,	
					// insertion, deletion, substitution cost
	// outputs
	int **Table,    // Cost Table Calculated (2-d array (|SS|+1) x (|TS|+1) will be set)
	char **SR,		// Source string with insertion to T marks '*'
	char **OP,		// Operation sequence('i', 'd', 's', or '.' (nop))
	char **TR,       // Edited string with deletion form S mark '*'
					// NOTE: ST, OP, TR must be allocated inside the function as needed(minimum as possible)
	int  *Mem_Allocated
)
{
	int i, j, m, n, Alpha, SR_len=(int)strlen(SS), k, Min;  // SR_len 값이 이후 SR, OP, TR 배열의 길이가 된다
	m = (int)strlen(SS); n = (int)strlen(TS);

	for (i=0 ; i <= m ; i++) Table[i][0] = i * del_cost;     // Table 초기화 시작
	for (j=1 ; j <= n ; j++) Table[0][j] = j * ins_cost;     // Table[0][0]에 대한 계산은 윗줄에서 수행하기 때문에 j=1부터 계산한다

	for (i=1 ; i <= m ; i++){
		for (j=1 ; j <= n ; j++){
			if (SS[i-1] != TS[j-1]) Alpha = sub_cost;
			else Alpha = 0;
			Table[i][j] = Return_Min (Table[i][j-1]+ins_cost , Table[i-1][j]+del_cost , Table[i-1][j-1]+Alpha);
		}
	}       // Table 초기화 종료


	i = m; j = n;    // 1차 backtracking. 이 과정을 통해 SR, OP, TR 배열의 길이를 정하게 된다.
	while(i || j){   // 배열 SS에 insertion이 적용되는 횟수를 계산한 뒤, 배열 SS의 길이 + insertion 발생 횟수를 계산하여 SR, OP, TR 배열의 길이로 삼는다.
		if(i==0){
			SR_len++; j--; continue; }
		if(j==0){
			i--; continue; }

		if(SS[i-1] != TS[j-1]){    // Alpha가 0이 아니었다면
			Min = Return_Min (Table[i][j-1]+ins_cost , Table[i-1][j]+del_cost , Table[i-1][j-1] +sub_cost);  // 가장 작은 값 계산
			if( Min == Table[i][j-1] + ins_cost ){   //  parent의 선택이 insertion이라 예상되는 경우
				if( Min == Table[i-1][j] + del_cost  &&  ins_cost > del_cost ){  // deletion 또한 parent의 선택이 될 수 있다면, 즉
					i--; continue; }  //deletion                                 // parent의 선택이 둘 이상이라면 ins_cost와 del_cost의 값을 비교하여
				                                                                 // ins_cost가 작다면 insertion을, del_cost가 작다면 deletion을 선택한다.
				SR_len++;
				j--; continue;   // insertion
			}
			else if( Min == Table[i-1][j] + del_cost ){
				i--; continue;   // deletion
			}
			else{
				i--; j--; continue;  // substitution
			}
		}
		else{     // Alpha가 0이었다면
			Min = Return_Min (Table[i][j-1]+ins_cost , Table[i-1][j]+del_cost , Table[i-1][j-1]);  // 가장 작은 값 계산
			if( Min == Table[i][j-1] + ins_cost ){
				if( Min == Table[i-1][j] + del_cost  &&  ins_cost > del_cost ){
					i--; continue; }  // deletion
				
				SR_len++;
				j--; continue;  // insertion
			}
			else if( Min == Table[i-1][j] + del_cost ){
				i--; continue;  // deletion
			}
			else{
				i--; j--; continue;  // no operation
			}
		}
	}

	*SR = new char [SR_len+1]; *OP = new char [SR_len+1]; *TR = new char [SR_len+1];
	if( *SR==NULL || *OP==NULL || *TR==NULL )
		Error_Exit("Memory Allocation Error");   // 예외처리

	i = m; j = n; k=SR_len;
	*(*SR +k) = *(*OP +k) = *(*TR +k) = 0;   // SR, OP, TR 배열의 끝자락에 NULL 처리
	
	while(i || j){  // 2차 backtracking. 작업방식은 1차 backtracking과 같지만, 2차 backtracking은 SR, OP, TR 배열에 대한 작업을 병행한다는 점이 다르다.
		if(i==0){
			*(*SR +k-1) = '*';
			*(*OP +k-1) = 'i';
			*(*TR +k-1) = TS[j-1];
			k--; j--; continue;   // insertion
		}
		if(j==0){
			*(*SR +k-1) = SS[i-1];
			*(*OP +k-1) = 'd';
			*(*TR +k-1) = '*';
			k--; i--; continue;   // deletion
		}
			
		if(SS[i-1] != TS[j-1]){    // Alpha가 0이 아니었다면
			Min = Return_Min (Table[i][j-1]+ins_cost , Table[i-1][j]+del_cost , Table[i-1][j-1] +sub_cost);  // 가장 작은 값 계산
			if( Min == Table[i][j-1] + ins_cost ){
				if( Min == Table[i-1][j] + del_cost  &&  ins_cost > del_cost ){  // parent의 선택이 insertion, deletion 둘 다 될 수 있을 때
					*(*SR +k-1) = SS[i-1];                                       // ins_cost와 del_cost를 비교하여 적절한 작업을 수행한다.
					*(*OP +k-1) = 'd';
					*(*TR +k-1) = '*';
					k--; i--; continue;   // deletion
				}

				*(*SR +k-1) = '*';
				*(*OP +k-1) = 'i';
				*(*TR +k-1) = TS[j-1];
				k--; j--; continue;   // insertion
			}
			else if( Min == Table[i-1][j] + del_cost ){
				*(*SR +k-1) = SS[i-1];
				*(*OP +k-1) = 'd';
				*(*TR +k-1) = '*';
				k--; i--; continue;   // deletion
			}
			else{
				*(*SR +k-1) = SS[i-1];
				*(*OP +k-1) = 's';
				*(*TR +k-1) = TS[j-1];
				k--; i--; j--; continue;   // substitution
			}
		}
		else{     // Alpha가 0이었다면
			Min = Return_Min (Table[i][j-1]+ins_cost , Table[i-1][j]+del_cost , Table[i-1][j-1]);  // 가장 작은 값 계산
			if( Min == Table[i][j-1] + ins_cost ){
				if( Min == Table[i-1][j] + del_cost  &&  ins_cost > del_cost ){
					*(*SR +k-1) = SS[i-1];
					*(*OP +k-1) = 'd';
					*(*TR +k-1) = '*';
					k--; i--; continue;   // deletion
				}

				*(*SR +k-1) = '*';
				*(*OP +k-1) = 'i';
				*(*TR +k-1) = TS[j-1];
				k--; j--; continue;   // insertion
			}
			else if( Min == Table[i-1][j] + del_cost ){
				*(*SR +k-1) = SS[i-1];
				*(*OP +k-1) = 'd';
				*(*TR +k-1) = '*';
				k--; i--; continue;   // deletion
			}
			else{
				*(*SR +k-1) = SS[i-1];
				*(*OP +k-1) = '.';
				*(*TR +k-1) = TS[j-1];
				k--; i--; j--; continue;   // no operation
			}
		}      // 미리 SR, OP, TR의 길이를 계산한 뒤, backtracking 과정을 통해 배열의 끝부터 한칸씩 앞으로 돌아오며 적절한 값을 넣어주는 방식으로 진행.
	}
	*Mem_Allocated = (int)(strlen(*SR) + strlen(*TR) + strlen(*OP)) + 3;
}

int Return_Min (int a, int b, int c)   // 세 정수 중 가장 작은 값을 리턴하는 함수
{
	if(a>b){
		if (b>c) return c;
		else return b;
	}
	else{
		if (a>c) return c;
		else return a;
	}
}