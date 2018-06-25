#include <stdio.h>

int sudo (int idx);
int garo_check (int i, int j, int k);
int sero_check (int i, int j, int k);
int sagak_check (int i, int j, int k);

int mtrx[9][9];   // sudoku board
int P[70][2];   // it saves blank's row & col value
int end_flag = 0, pl = 0;  // pl means the length of P

int main()
{
	int i, j;

	for(i=0 ; i<9 ; i++){
		for(j=0 ; j<9 ; j++){
			scanf("%d", &(mtrx[i][j]) );   // reading
			if(mtrx[i][j] == 0){
				P[pl][0] = i; P[pl][1] = j;   // save every blank info
				pl++;
			}
		}
	}

	sudo(0);   // start at 1st blank
			
	for(i=0 ; i<9 ; i++)
		for(j=0 ; j<9 ; j++){
			printf("%d", mtrx[i][j] );   // printing
			if(j==8) printf("\n");
			else printf(" ");
		}

	return 0;
}

int sudo (int idx){   // means it will check (idx+1)th blank
	int i, j, k;
	i = P[idx][0];  j = P[idx][1];   // read (idx+1)th blank's row & col value from array P
	
	for(k=1; k<10 ; k++){   // k is the candidate value of the blank
		if( garo_check(i, j, k) && sero_check(i, j, k) && sagak_check(i, j, k) ){   // check if promising
			mtrx[i][j] = k;   // if promising, put k on the value
			
			if(idx==pl-1){   // if 'sudo' was check on (pl)th blank, it means it's all over.
				end_flag = 1;
				return 0;
			}
			sudo(idx+1);   // else, go to next blank
			if(end_flag==1) return 0;
			mtrx[i][j] = 0;   // if k is not promising, we must get rid of the value and make it back to blank
		}
	}

	return 0;
}

int garo_check (int i, int j, int k){   // row check
	int a;
	for(a=0 ; a<9 ; a++){
		if(mtrx[i][a] == k)
			return 0;
	}
	return 1;
}

int sero_check (int i, int j, int k){   // col check
	int b;
	for(b=0 ; b<9 ; b++){
		if(mtrx[b][j] == k)
			return 0;
	}
	return 1;
}

int sagak_check (int i, int j, int k){   // 3*3 square check
	int c, d, x, y;
	c = (i/3)*3;
	d = (j/3)*3;

	for(x=0 ; x<3 ; x++){
		for(y=0 ; y<3 ; y++){
			if(mtrx[c+x][d+y] == k)
				return 0;
		}
	}
	return 1;
}