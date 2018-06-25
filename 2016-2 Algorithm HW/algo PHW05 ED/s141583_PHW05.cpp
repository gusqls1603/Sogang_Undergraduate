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
	int i, j, m, n, Alpha, SR_len=(int)strlen(SS), k, Min;  // SR_len ���� ���� SR, OP, TR �迭�� ���̰� �ȴ�
	m = (int)strlen(SS); n = (int)strlen(TS);

	for (i=0 ; i <= m ; i++) Table[i][0] = i * del_cost;     // Table �ʱ�ȭ ����
	for (j=1 ; j <= n ; j++) Table[0][j] = j * ins_cost;     // Table[0][0]�� ���� ����� ���ٿ��� �����ϱ� ������ j=1���� ����Ѵ�

	for (i=1 ; i <= m ; i++){
		for (j=1 ; j <= n ; j++){
			if (SS[i-1] != TS[j-1]) Alpha = sub_cost;
			else Alpha = 0;
			Table[i][j] = Return_Min (Table[i][j-1]+ins_cost , Table[i-1][j]+del_cost , Table[i-1][j-1]+Alpha);
		}
	}       // Table �ʱ�ȭ ����


	i = m; j = n;    // 1�� backtracking. �� ������ ���� SR, OP, TR �迭�� ���̸� ���ϰ� �ȴ�.
	while(i || j){   // �迭 SS�� insertion�� ����Ǵ� Ƚ���� ����� ��, �迭 SS�� ���� + insertion �߻� Ƚ���� ����Ͽ� SR, OP, TR �迭�� ���̷� ��´�.
		if(i==0){
			SR_len++; j--; continue; }
		if(j==0){
			i--; continue; }

		if(SS[i-1] != TS[j-1]){    // Alpha�� 0�� �ƴϾ��ٸ�
			Min = Return_Min (Table[i][j-1]+ins_cost , Table[i-1][j]+del_cost , Table[i-1][j-1] +sub_cost);  // ���� ���� �� ���
			if( Min == Table[i][j-1] + ins_cost ){   //  parent�� ������ insertion�̶� ����Ǵ� ���
				if( Min == Table[i-1][j] + del_cost  &&  ins_cost > del_cost ){  // deletion ���� parent�� ������ �� �� �ִٸ�, ��
					i--; continue; }  //deletion                                 // parent�� ������ �� �̻��̶�� ins_cost�� del_cost�� ���� ���Ͽ�
				                                                                 // ins_cost�� �۴ٸ� insertion��, del_cost�� �۴ٸ� deletion�� �����Ѵ�.
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
		else{     // Alpha�� 0�̾��ٸ�
			Min = Return_Min (Table[i][j-1]+ins_cost , Table[i-1][j]+del_cost , Table[i-1][j-1]);  // ���� ���� �� ���
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
		Error_Exit("Memory Allocation Error");   // ����ó��

	i = m; j = n; k=SR_len;
	*(*SR +k) = *(*OP +k) = *(*TR +k) = 0;   // SR, OP, TR �迭�� ���ڶ��� NULL ó��
	
	while(i || j){  // 2�� backtracking. �۾������ 1�� backtracking�� ������, 2�� backtracking�� SR, OP, TR �迭�� ���� �۾��� �����Ѵٴ� ���� �ٸ���.
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
			
		if(SS[i-1] != TS[j-1]){    // Alpha�� 0�� �ƴϾ��ٸ�
			Min = Return_Min (Table[i][j-1]+ins_cost , Table[i-1][j]+del_cost , Table[i-1][j-1] +sub_cost);  // ���� ���� �� ���
			if( Min == Table[i][j-1] + ins_cost ){
				if( Min == Table[i-1][j] + del_cost  &&  ins_cost > del_cost ){  // parent�� ������ insertion, deletion �� �� �� �� ���� ��
					*(*SR +k-1) = SS[i-1];                                       // ins_cost�� del_cost�� ���Ͽ� ������ �۾��� �����Ѵ�.
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
		else{     // Alpha�� 0�̾��ٸ�
			Min = Return_Min (Table[i][j-1]+ins_cost , Table[i-1][j]+del_cost , Table[i-1][j-1]);  // ���� ���� �� ���
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
		}      // �̸� SR, OP, TR�� ���̸� ����� ��, backtracking ������ ���� �迭�� ������ ��ĭ�� ������ ���ƿ��� ������ ���� �־��ִ� ������� ����.
	}
	*Mem_Allocated = (int)(strlen(*SR) + strlen(*TR) + strlen(*OP)) + 3;
}

int Return_Min (int a, int b, int c)   // �� ���� �� ���� ���� ���� �����ϴ� �Լ�
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