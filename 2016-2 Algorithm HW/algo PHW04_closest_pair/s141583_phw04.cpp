#include <stdio.h>
#include <math.h>

void   Error_Exit(char *s);

void quicksort (unsigned int low, unsigned int high, double *A, unsigned int *P);
	// D&C 기법으로 정렬으 수행하는 퀵소트 함수

void partition (unsigned int low, unsigned int high, unsigned int *pivotpoint, double *A, unsigned int *P);
	// 퀵소트 정렬 시 pivotpoint를 설정하기 위해 호출되는 함수

double Brute_Force_dist(unsigned int L, unsigned int R, unsigned int *pt1, unsigned int *pt2,
						double *X, double *Y, unsigned int *P1);
	// THR 미만의 점들의 closest pair를 구하는 함수 - T(n) = O(n^2)

double Middle_side(unsigned int M, unsigned int *pt1, unsigned int *pt2, double *X, double *Y,
				   unsigned int *P1, unsigned int *P2, unsigned int *P3, double d,
				   unsigned int L, unsigned int R);
	// CPL, CPR에 대한 작업을 마친 뒤 중간영역에 대한 작업을 수행하는 함수


void   Sort_by_X_or_Y ( double *A, unsigned int *P, unsigned int N )
{
		quicksort(0, N-1, A, P); // 배열 P의 index 0~N-1을 정렬 
}



double Closest_Pair_DC(
	unsigned int L,    unsigned int R,		// left and right indices
	unsigned int *pt1, unsigned int *pt2,   // min dist points inices
    double *X, double *Y,					// input points position array
	unsigned int *P1, unsigned int *P2, unsigned int *P3, // temp index arrays
	unsigned int THR	// threshold value
)
{
	double CPL, CPR, CPM, d; unsigned int temp_pt1, temp_pt2;
	if(R-L+1 <= THR)										// 작업할 점의 수가 THR 이하일 경우
		return Brute_Force_dist(L, R, pt1, pt2, X, Y, P1);  // Brute Force Algorithm으로 closest pair를 찾음
	else{
		CPL = Closest_Pair_DC(L, (L+R)/2, pt1, pt2, X, Y, P1, P2, P3, THR);
		temp_pt1 = *pt1; temp_pt2 = *pt2; d = CPL;  // 이후 Closest_Pair_DC 함수가 반복 호출됨에 따라 pt1, pt2 값이 수시로 변하기 때문에
													// 앞서 계산되는 pt1과 pt2를 별도로 저장해 두는 과정이 필요하다.
		
		if( (L+R)/2 +1 != R){
			CPR = Closest_Pair_DC((L+R)/2 +1, R, pt1, pt2, X, Y, P1, P2, P3, THR);  // L과 R 사이에 점이 1개 존재할 경우, 왼쪽 코드를 통해
			if(CPL < CPR){														    // Closest_Pair_DC 함수를 호출할 때 (L+R)/2 +1이 R값과 같게 되어
			*pt1 = temp_pt1; *pt2 = temp_pt2;                                       // Closest_Pair_DC 함수를 호출하는 것이 무의미하게 된다.
			}                                                                       // 따라서 이러한 경우 CPR 값을 계산하지 않도록 하였다.
			else{                                                                   // 그 외의 경우, CPR과 CPL을 비교하여 알맞은 값이
				temp_pt1 = *pt1; temp_pt2 = *pt2; d = CPR;                          // d, pt1, pt2에 각각 저장되도록 한다.
			}
		}

		CPM = Middle_side((L+R)/2, pt1, pt2, X, Y, P1, P2, P3, d, L, R);  // CPL, CPR에 대한 작업을 마치면 중간지역에 대한 작업을 수행
		if(d < CPM){
			*pt1 = temp_pt1; *pt2 = temp_pt2;
			return d;   // 기존에 d에 저장된 값은 CPL, CPR 중 작은 값이고, 이 d와 CPM을 비교하여 더 작은 값이 리턴되도록 한다.
		}
		else
			return CPM;
	}
}


void quicksort (unsigned int low, unsigned int high, double *A, unsigned int *P)
{
	unsigned int pivotpoint = low;
	if(high > low){
		partition(low, high, &pivotpoint, A, P);  // 배열에서 pivotpoint의 위치를 지정하여, 그 왼쪽에 있는 값들은 모두 pivotpoint보다 작게 되도록 한다.
		if(pivotpoint > low)  // 예외처리를 위한 조건문
			quicksort(low, pivotpoint -1, A, P);
		if(pivotpoint < high)  // 예외처리를 위한 조건문
			quicksort(pivotpoint +1, high, A, P);
	}
}


void partition (unsigned int low, unsigned int high, unsigned int *pivotpoint, double *A, unsigned int *P)
{
	unsigned int i, j, temp;
	double pivotitem;
	pivotitem = A[ P[low] ]; j = low;
	for(i = low+1 ; i <=high ; i++){ // 대상 배열에 저장된 모든 수에 대하여
		if(A[ P[i] ] < pivotitem){   // 현재 탐색중인 값이 pivotitem보다 작을 경우
			j++;                     // 이후에 pivotpoint가 될 위치를 한 칸 옮긴 뒤
			temp = P[i];             // 해당 좌표를 i의 위치로 설정
			P[i] = P[j];
			P[j] = temp;
		}
	}
	*pivotpoint = j;
	temp = P[low];
	P[low] = P[*pivotpoint];
	P[*pivotpoint] = temp;
}


double Brute_Force_dist(unsigned int L, unsigned int R, unsigned int *pt1, unsigned int *pt2,
						double *X, double *Y, unsigned int *P1)
{
	double min=0, dis; unsigned int i, j;
	min = sqrt( ( X[P1[L+1]]-X[P1[L]] )*( X[P1[L+1]]-X[P1[L]] )  +  ( Y[P1[L+1]]-Y[P1[L]] )*( Y[P1[L+1]]-Y[P1[L]] ) ); // 초기값 설정
	*pt1 = P1[L]; *pt2 = P1[L+1];

	for (i=L ; i<R ; i++){
		for (j=i+1 ; j<=R ; j++){
			dis = sqrt( ( X[P1[j]]-X[P1[i]] )*( X[P1[j]]-X[P1[i]] )  +  ( Y[P1[j]]-Y[P1[i]] )*( Y[P1[j]]-Y[P1[i]] ) );
				// 직교좌표계에서 점과 점 사이의 거리 계산
			if(dis < min){
				min = dis;
				*pt1 = P1[i]; *pt2 = P1[j];
			}
		}
	}
	return min;
}


double Middle_side (unsigned int M, unsigned int *pt1, unsigned int *pt2, double *X, double *Y,
					unsigned int *P1, unsigned int *P2, unsigned int *P3, double d,
					unsigned int L, unsigned int R)
{
	unsigned int mid_L, mid_R, i, j, k; double mid_dist;
	i = M;
	while(i>=L){ // 좌측 탐색
			if(X[ P1[M] ] - X[ P1[i] ] < d){
				if(i==L){
					mid_L = i; break; } // i가 L에 도달하고도 M과 i 사이의 거리가 d보다 작을 경우 반복문 이탈
			i--; continue;
		} // unsigned type의 특성상 i값이 음수가 되면 안되므로, i값이 0일 때 i--문을 지나기 전에 반복문을 이탈한다.
		else{
			mid_L = i+1;
			break;
		}
	}

	i = M;
	while(i<=R){ // 위와 같은 방법으로 우측 탐색
		if(X[ P1[i] ] - X[ P1[M] ] < d){
			i++; continue;
		}
		else
			break;
	}
	mid_R = i-1;

	k = mid_R - mid_L +1;
	for(i = 0 ; i<k ; i++)
		P2[i] = P1[mid_L +i];  // 중간지역으로 간주되는 점들의 index를 p2배열에 저장

	Sort_by_X_or_Y(Y, P2, k); // p2배열에 저장된 점들을 y값에 따라 오름차순으로 정렬

	for(i=0 ; i<k-1 ; i++){
		for(j=i+1 ; j<k ; j++){
			if( Y[ P2[j] ] - Y[ P2[i] ] >= d)  // p2배열에 정렬된 중간지역의 점들을 탐색하며 거리가 d보다 작은 pair가 있는지 체크
				break;
			mid_dist = sqrt( ( X[P2[j]]-X[P2[i]] )*( X[P2[j]]-X[P2[i]] )  +  ( Y[P2[j]]-Y[P2[i]] )*( Y[P2[j]]-Y[P2[i]] ) );
			if(mid_dist < d){
				d = mid_dist;
				*pt1 = P2[i]; *pt2 = P2[j];
			}
		}
	}

	return d;
}