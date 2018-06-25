#include <stdio.h>
#include <math.h>

void   Error_Exit(char *s);

void quicksort (unsigned int low, unsigned int high, double *A, unsigned int *P);
	// D&C ������� ������ �����ϴ� ����Ʈ �Լ�

void partition (unsigned int low, unsigned int high, unsigned int *pivotpoint, double *A, unsigned int *P);
	// ����Ʈ ���� �� pivotpoint�� �����ϱ� ���� ȣ��Ǵ� �Լ�

double Brute_Force_dist(unsigned int L, unsigned int R, unsigned int *pt1, unsigned int *pt2,
						double *X, double *Y, unsigned int *P1);
	// THR �̸��� ������ closest pair�� ���ϴ� �Լ� - T(n) = O(n^2)

double Middle_side(unsigned int M, unsigned int *pt1, unsigned int *pt2, double *X, double *Y,
				   unsigned int *P1, unsigned int *P2, unsigned int *P3, double d,
				   unsigned int L, unsigned int R);
	// CPL, CPR�� ���� �۾��� ��ģ �� �߰������� ���� �۾��� �����ϴ� �Լ�


void   Sort_by_X_or_Y ( double *A, unsigned int *P, unsigned int N )
{
		quicksort(0, N-1, A, P); // �迭 P�� index 0~N-1�� ���� 
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
	if(R-L+1 <= THR)										// �۾��� ���� ���� THR ������ ���
		return Brute_Force_dist(L, R, pt1, pt2, X, Y, P1);  // Brute Force Algorithm���� closest pair�� ã��
	else{
		CPL = Closest_Pair_DC(L, (L+R)/2, pt1, pt2, X, Y, P1, P2, P3, THR);
		temp_pt1 = *pt1; temp_pt2 = *pt2; d = CPL;  // ���� Closest_Pair_DC �Լ��� �ݺ� ȣ��ʿ� ���� pt1, pt2 ���� ���÷� ���ϱ� ������
													// �ռ� ���Ǵ� pt1�� pt2�� ������ ������ �δ� ������ �ʿ��ϴ�.
		
		if( (L+R)/2 +1 != R){
			CPR = Closest_Pair_DC((L+R)/2 +1, R, pt1, pt2, X, Y, P1, P2, P3, THR);  // L�� R ���̿� ���� 1�� ������ ���, ���� �ڵ带 ����
			if(CPL < CPR){														    // Closest_Pair_DC �Լ��� ȣ���� �� (L+R)/2 +1�� R���� ���� �Ǿ�
			*pt1 = temp_pt1; *pt2 = temp_pt2;                                       // Closest_Pair_DC �Լ��� ȣ���ϴ� ���� ���ǹ��ϰ� �ȴ�.
			}                                                                       // ���� �̷��� ��� CPR ���� ������� �ʵ��� �Ͽ���.
			else{                                                                   // �� ���� ���, CPR�� CPL�� ���Ͽ� �˸��� ����
				temp_pt1 = *pt1; temp_pt2 = *pt2; d = CPR;                          // d, pt1, pt2�� ���� ����ǵ��� �Ѵ�.
			}
		}

		CPM = Middle_side((L+R)/2, pt1, pt2, X, Y, P1, P2, P3, d, L, R);  // CPL, CPR�� ���� �۾��� ��ġ�� �߰������� ���� �۾��� ����
		if(d < CPM){
			*pt1 = temp_pt1; *pt2 = temp_pt2;
			return d;   // ������ d�� ����� ���� CPL, CPR �� ���� ���̰�, �� d�� CPM�� ���Ͽ� �� ���� ���� ���ϵǵ��� �Ѵ�.
		}
		else
			return CPM;
	}
}


void quicksort (unsigned int low, unsigned int high, double *A, unsigned int *P)
{
	unsigned int pivotpoint = low;
	if(high > low){
		partition(low, high, &pivotpoint, A, P);  // �迭���� pivotpoint�� ��ġ�� �����Ͽ�, �� ���ʿ� �ִ� ������ ��� pivotpoint���� �۰� �ǵ��� �Ѵ�.
		if(pivotpoint > low)  // ����ó���� ���� ���ǹ�
			quicksort(low, pivotpoint -1, A, P);
		if(pivotpoint < high)  // ����ó���� ���� ���ǹ�
			quicksort(pivotpoint +1, high, A, P);
	}
}


void partition (unsigned int low, unsigned int high, unsigned int *pivotpoint, double *A, unsigned int *P)
{
	unsigned int i, j, temp;
	double pivotitem;
	pivotitem = A[ P[low] ]; j = low;
	for(i = low+1 ; i <=high ; i++){ // ��� �迭�� ����� ��� ���� ���Ͽ�
		if(A[ P[i] ] < pivotitem){   // ���� Ž������ ���� pivotitem���� ���� ���
			j++;                     // ���Ŀ� pivotpoint�� �� ��ġ�� �� ĭ �ű� ��
			temp = P[i];             // �ش� ��ǥ�� i�� ��ġ�� ����
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
	min = sqrt( ( X[P1[L+1]]-X[P1[L]] )*( X[P1[L+1]]-X[P1[L]] )  +  ( Y[P1[L+1]]-Y[P1[L]] )*( Y[P1[L+1]]-Y[P1[L]] ) ); // �ʱⰪ ����
	*pt1 = P1[L]; *pt2 = P1[L+1];

	for (i=L ; i<R ; i++){
		for (j=i+1 ; j<=R ; j++){
			dis = sqrt( ( X[P1[j]]-X[P1[i]] )*( X[P1[j]]-X[P1[i]] )  +  ( Y[P1[j]]-Y[P1[i]] )*( Y[P1[j]]-Y[P1[i]] ) );
				// ������ǥ�迡�� ���� �� ������ �Ÿ� ���
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
	while(i>=L){ // ���� Ž��
			if(X[ P1[M] ] - X[ P1[i] ] < d){
				if(i==L){
					mid_L = i; break; } // i�� L�� �����ϰ� M�� i ������ �Ÿ��� d���� ���� ��� �ݺ��� ��Ż
			i--; continue;
		} // unsigned type�� Ư���� i���� ������ �Ǹ� �ȵǹǷ�, i���� 0�� �� i--���� ������ ���� �ݺ����� ��Ż�Ѵ�.
		else{
			mid_L = i+1;
			break;
		}
	}

	i = M;
	while(i<=R){ // ���� ���� ������� ���� Ž��
		if(X[ P1[i] ] - X[ P1[M] ] < d){
			i++; continue;
		}
		else
			break;
	}
	mid_R = i-1;

	k = mid_R - mid_L +1;
	for(i = 0 ; i<k ; i++)
		P2[i] = P1[mid_L +i];  // �߰��������� ���ֵǴ� ������ index�� p2�迭�� ����

	Sort_by_X_or_Y(Y, P2, k); // p2�迭�� ����� ������ y���� ���� ������������ ����

	for(i=0 ; i<k-1 ; i++){
		for(j=i+1 ; j<k ; j++){
			if( Y[ P2[j] ] - Y[ P2[i] ] >= d)  // p2�迭�� ���ĵ� �߰������� ������ Ž���ϸ� �Ÿ��� d���� ���� pair�� �ִ��� üũ
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