#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include <time.h>
#include <Windows.h>
#include <cuda.h>
#include <device_launch_parameters.h>
#include <cuda_runtime.h>

#include <assert.h>

// Tested on GTX1050

#define ELEM_PER_POINT				(1 << 5)	// E
#define ELEM_PER_POINT2				(1 << 6)	// E
#define N_ELEMS						(1 << 22)	// N
#define N_POINTS					(N_ELEMS / ELEM_PER_POINT)
#define N_POINTS2					(N_ELEMS / ELEM_PER_POINT2)

#define ARRAY_2D_WIDTH				1024
#define ARRAY_2D_HEIGHT				(N_POINTS/ARRAY_2D_WIDTH)
#define ARRAY_2D_HEIGHT2			(N_POINTS2/ARRAY_2D_WIDTH)
#define BLOCK_WIDTH					128
#define BLOCK_WIDTH2				32
#define BLOCK_HEIGHT				8
#define BLOCK_HEIGHT2				32

__int64 start, freq, end;
#define CHECK_TIME_START QueryPerformanceFrequency((LARGE_INTEGER*)&freq); QueryPerformanceCounter((LARGE_INTEGER*)&start)
#define CHECK_TIME_END(a) QueryPerformanceCounter((LARGE_INTEGER*)&end); a = (float)((float)(end - start) / (freq / 1000.0f))

#define IN
#define INOUT
#define OUT


typedef struct {
	float elem[ ELEM_PER_POINT ];
} POINT_ELEMENT;

typedef struct {
	float *elem[ ELEM_PER_POINT ];
} POINTS_SOA;

typedef struct {
	float elem[ ELEM_PER_POINT2 ];
} POINT_ELEMENT2;

typedef struct {
	float *elem[ ELEM_PER_POINT2 ];
} POINTS_SOA2;

int cal_N (int n){
	int res = 0;
	while(n!=1){
		n /= 2;
		res++;
	}
	return res;
}

__constant__ float constantBuffer[ 1000 ];

__global__ void TransformAOSKernel( INOUT POINT_ELEMENT *A, IN int m )
{
	int row = blockIdx.y * blockDim.y + threadIdx.y;
	int col = blockIdx.x * blockDim.x + threadIdx.x;
	int id = gridDim.x*blockDim.x*row + col;	// index

	for (int j = 2; j <= m; j++) {
		float tmp = 1.0f / (float) j;
		for (int i = 0; i < ELEM_PER_POINT; i++) {
			A[id].elem[i] += tmp*A[id].elem[i];
		}
	}
}

__global__ void TransformAOSKernel2( INOUT POINT_ELEMENT2 *A, IN int m )
{
	int row = blockIdx.y * blockDim.y + threadIdx.y;
	int col = blockIdx.x * blockDim.x + threadIdx.x;
	int id = gridDim.x*blockDim.x*row + col;

	for (int j = 2; j <= m; j++) {
		float tmp = 1.0f / (float) j;
		for (int i = 0; i < ELEM_PER_POINT2; i++) {
			A[id].elem[i] += tmp*A[id].elem[i];
		}
	}

}

__global__ void TransformSOAKernel( INOUT POINTS_SOA A, IN int m )
{
	int row = blockIdx.y * blockDim.y + threadIdx.y;
	int col = blockIdx.x * blockDim.x + threadIdx.x;
	int id = gridDim.x*blockDim.x*row + col;	// index

	for (int i = 0; i < ELEM_PER_POINT; i++) {
		for (int j = 2; j <= m; j++) {
			float tmp = 1.0f / (float) j;
			A.elem[i][id] += tmp*A.elem[i][id];
		}
	}
}

__global__ void TransformSOAKernel2( INOUT POINTS_SOA2 A, IN int m )
{
	int row = blockIdx.y * blockDim.y + threadIdx.y;
	int col = blockIdx.x * blockDim.x + threadIdx.x;
	int id = gridDim.x*blockDim.x*row + col;

	for (int i = 0; i < ELEM_PER_POINT2; i++) {
		for (int j = 2; j <= m; j++) {
			float tmp = 1.0f / (float) j;
			A.elem[i][id] += tmp*A.elem[i][id];
		}
	}
}

__global__ void TransformAOSwithConstantMemKernel( INOUT POINT_ELEMENT *A, IN int m )
{
	int row = blockIdx.y * blockDim.y + threadIdx.y;
	int col = blockIdx.x * blockDim.x + threadIdx.x;
	int id = gridDim.x*blockDim.x*row + col;	// index
	int j;

	for (int j = 2; j <= m; j++) {
		for (int i = 0; i < ELEM_PER_POINT; i++) {
			A[id].elem[i] += constantBuffer[j-1]*A[id].elem[i];
		}
	}
}

__global__ void TransformAOSwithConstantMemKernel2( INOUT POINT_ELEMENT2 *A, IN int m )
{
	int row = blockIdx.y * blockDim.y + threadIdx.y;
	int col = blockIdx.x * blockDim.x + threadIdx.x;
	int id = gridDim.x*blockDim.x*row + col;
	int j;

	for (int j = 2; j <= m; j++) {
		for (int i = 0; i < ELEM_PER_POINT2; i++) {
			A[id].elem[i] += constantBuffer[j-1]*A[id].elem[i];
		}
	}
}

__global__ void TransformSOAwithConstantMemKernel( INOUT POINTS_SOA A, IN int m )
{
	int row = blockIdx.y * blockDim.y + threadIdx.y;
	int col = blockIdx.x * blockDim.x + threadIdx.x;
	int id = gridDim.x*blockDim.x*row + col;	// index
	int i, j;

	for ( i = 0; i < ELEM_PER_POINT; ++i ) {
		for ( j = 2; j <= m; j++ ) {
			A.elem[ i ][ id ] += constantBuffer[j-1]*A.elem[ i ][ id ];
		}
	}
}

__global__ void TransformSOAwithConstantMemKernel2( INOUT POINTS_SOA2 A, IN int m )
{
	int row = blockIdx.y * blockDim.y + threadIdx.y;
	int col = blockIdx.x * blockDim.x + threadIdx.x;
	int id = gridDim.x*blockDim.x*row + col;
	int i, j;

	for ( i = 0; i < ELEM_PER_POINT2; ++i ) {
		for ( j = 2; j <= m; j++ ) {
			A.elem[ i ][ id ] += constantBuffer[j-1]*A.elem[ i ][ id ];
		}
	}
}

//E 16, N 22, M 128,8
void transform_points_AOS( INOUT POINT_ELEMENT *p_AOS, IN int n_points, IN int m )
{
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	
	POINT_ELEMENT *d_pAOS;
	size_t size = N_POINTS * sizeof(POINT_ELEMENT);
	cudaMalloc(&d_pAOS, size);
	cudaMemcpy(d_pAOS, p_AOS, size, cudaMemcpyHostToDevice);
	dim3 dimBlock (BLOCK_WIDTH, BLOCK_HEIGHT);
	dim3 dimGrid (ARRAY_2D_WIDTH / dimBlock.x, ARRAY_2D_HEIGHT / dimBlock.y);

	cudaEventRecord( start, 0 );	// time record start
	TransformAOSKernel <<<dimGrid, dimBlock>>> (d_pAOS, m);		// Kernel 호출
	cudaEventRecord( stop, 0 );		// time record stop
	cudaEventSynchronize( stop );

	cudaMemcpy(p_AOS, d_pAOS, size, cudaMemcpyDeviceToHost);
	cudaFree(d_pAOS);	// free allocation

	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime, start, stop);

	printf("*** GPU 01 - E:%d, N:%d, B:(%d,%d) M:global, AOS : GPU Time taken = %.3fms\n",
		ELEM_PER_POINT, cal_N(N_ELEMS), BLOCK_WIDTH, BLOCK_HEIGHT, elapsedTime);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
}

//E 16, N 22, M 128,8
void transform_points_SOA( INOUT POINTS_SOA p_SOA, IN int n_points, IN int m )
{
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	POINTS_SOA d_pSOA;
	size_t size = N_POINTS * sizeof(float);

	for(int i=0;i<ELEM_PER_POINT;i++){
		cudaMalloc(&d_pSOA.elem[i], size);
		cudaMemcpy(d_pSOA.elem[i], p_SOA.elem[i], size, cudaMemcpyHostToDevice);
	}
	
	dim3 dimBlock(BLOCK_WIDTH, BLOCK_HEIGHT);
	dim3 dimGrid (ARRAY_2D_WIDTH/dimBlock.x, ARRAY_2D_HEIGHT/dimBlock.y);

	cudaEventRecord( start, 0 );	// time record start
	TransformSOAKernel <<<dimGrid, dimBlock>>> (d_pSOA, m);		// Kernel 호출
	cudaEventRecord( stop, 0 );		// time record stop
	cudaEventSynchronize( stop );

	for(int i=0;i<ELEM_PER_POINT;i++){
		cudaMemcpy(p_SOA.elem[i], d_pSOA.elem[i], size, cudaMemcpyDeviceToHost);
	}
	for(int i=0;i<ELEM_PER_POINT;i++){
		cudaFree(p_SOA.elem[i]);
	}

	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime, start, stop);

	printf("*** GPU 02 - E:%d, N:%d, B:(%d,%d) M:global, SOA : GPU Time taken = %.3fms\n",
		ELEM_PER_POINT, cal_N(N_ELEMS), BLOCK_WIDTH, BLOCK_HEIGHT, elapsedTime);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
}

//E 32, N 22, M 128,8
void transform_points_AOS3( INOUT POINT_ELEMENT2 *p_AOS, IN int n_points, IN int m )
{
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	POINT_ELEMENT2 *d_pAOS;
	size_t size = N_POINTS2 * sizeof(POINT_ELEMENT2);
	cudaMalloc(&d_pAOS, size);
	cudaMemcpy(d_pAOS, p_AOS, size, cudaMemcpyHostToDevice);
	dim3 dimBlock (BLOCK_WIDTH, BLOCK_HEIGHT);
	dim3 dimGrid (ARRAY_2D_WIDTH / dimBlock.x, ARRAY_2D_HEIGHT2 / dimBlock.y);

	cudaEventRecord( start, 0 );

	TransformAOSKernel2 <<<dimGrid, dimBlock>>> (d_pAOS, m);		// Kernel 호출

	cudaEventRecord( stop, 0 );
	cudaEventSynchronize( stop );

	cudaMemcpy(p_AOS, d_pAOS, size, cudaMemcpyDeviceToHost);
	cudaFree(d_pAOS);

	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime, start, stop);

	printf("*** GPU 03 - E:%d, N:%d, B:(%d,%d) M:global, AOS : GPU Time taken = %.3fms\n",
		ELEM_PER_POINT2, cal_N(N_ELEMS), BLOCK_WIDTH, BLOCK_HEIGHT, elapsedTime);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
}

//E 32, N 22, M 128,8
void transform_points_SOA3( INOUT POINTS_SOA2 p_SOA, IN int n_points, IN int m )
{
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	POINTS_SOA2 d_pSOA;
	size_t size = N_POINTS2 * sizeof(float);

	for(int i=0;i<ELEM_PER_POINT2;i++){
		cudaMalloc(&d_pSOA.elem[i], size);
		cudaMemcpy(d_pSOA.elem[i], p_SOA.elem[i], size, cudaMemcpyHostToDevice);
	}
	
	dim3 dimBlock(BLOCK_WIDTH, BLOCK_HEIGHT);
	dim3 dimGrid (ARRAY_2D_WIDTH/dimBlock.x, ARRAY_2D_HEIGHT2/dimBlock.y);

	cudaEventRecord( start, 0 );
	TransformSOAKernel2 <<<dimGrid, dimBlock>>> (d_pSOA, m);		// Kernel 호출
	cudaEventRecord( stop, 0 );
	cudaEventSynchronize( stop );

	for(int i=0;i<ELEM_PER_POINT2;i++){
		cudaMemcpy(p_SOA.elem[i], d_pSOA.elem[i], size, cudaMemcpyDeviceToHost);
	}
	for(int i=0;i<ELEM_PER_POINT2;i++){
		cudaFree(p_SOA.elem[i]);
	}

	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime, start, stop);

	printf("*** GPU 04 - E:%d, N:%d, B:(%d,%d) M:global, SOA : GPU Time taken = %.3fms\n",
		ELEM_PER_POINT2, cal_N(N_ELEMS), BLOCK_WIDTH, BLOCK_HEIGHT, elapsedTime);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
}

//E 32, N 22, M 32,32
void transform_points_AOS2( INOUT POINT_ELEMENT2 *p_AOS, IN int n_points, IN int m )
{
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	

	// mycode
	POINT_ELEMENT2 *d_pAOS;
	size_t size = N_POINTS2 * sizeof(POINT_ELEMENT2);
	cudaMalloc(&d_pAOS, size);
	cudaMemcpy(d_pAOS, p_AOS, size, cudaMemcpyHostToDevice);
	dim3 dimBlock (BLOCK_WIDTH2, BLOCK_HEIGHT2);
	dim3 dimGrid (ARRAY_2D_WIDTH / dimBlock.x, ARRAY_2D_HEIGHT2 / dimBlock.y);



	cudaEventRecord( start, 0 );
	//

//.............
	
	TransformAOSKernel2 <<<dimGrid, dimBlock>>> (d_pAOS, m);		// Kernel 호출

	//cudaDeviceSynchronize(); //It may stall the GPU pipeline.
	cudaEventRecord( stop, 0 );
	cudaEventSynchronize( stop );
	//
//.............
	//mycode
	cudaMemcpy(p_AOS, d_pAOS, size, cudaMemcpyDeviceToHost);
	cudaFree(d_pAOS);

//
	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime, start, stop);
//	printf("*** Array of structure: GPU Time taken = %.3fms\n", elapsedTime);
	printf("*** GPU 05 - E:%d, N:%d, B:(%d,%d) M:global, AOS : GPU Time taken = %.3fms\n",
		ELEM_PER_POINT2, cal_N(N_ELEMS), BLOCK_WIDTH2, BLOCK_HEIGHT2, elapsedTime);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
}

//E 32, N 22, M 32,32
void transform_points_SOA2( INOUT POINTS_SOA2 p_SOA, IN int n_points, IN int m )
{
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	POINTS_SOA2 d_pSOA;
	size_t size = N_POINTS2 * sizeof(float);

	for(int i=0;i<ELEM_PER_POINT2;i++){
		cudaMalloc(&d_pSOA.elem[i], size);
		cudaMemcpy(d_pSOA.elem[i], p_SOA.elem[i], size, cudaMemcpyHostToDevice);
	}
	
	dim3 dimBlock(BLOCK_WIDTH2, BLOCK_HEIGHT2);
	dim3 dimGrid (ARRAY_2D_WIDTH/dimBlock.x, ARRAY_2D_HEIGHT2/dimBlock.y);

	cudaEventRecord( start, 0 );
	TransformSOAKernel2 <<<dimGrid, dimBlock>>> (d_pSOA, m);		// Kernel 호출
	cudaEventRecord( stop, 0 );
	cudaEventSynchronize( stop );

	for(int i=0;i<ELEM_PER_POINT2;i++){
		cudaMemcpy(p_SOA.elem[i], d_pSOA.elem[i], size, cudaMemcpyDeviceToHost);
	}
	for(int i=0;i<ELEM_PER_POINT2;i++){
		cudaFree(p_SOA.elem[i]);
	}

	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime, start, stop);
//	printf("*** Structure of array: GPU Time taken = %.3fms\n", elapsedTime);
	printf("*** GPU 06 - E:%d, N:%d, B:(%d,%d) M:global, SOA : GPU Time taken = %.3fms\n",
		ELEM_PER_POINT2, cal_N(N_ELEMS), BLOCK_WIDTH2, BLOCK_HEIGHT2, elapsedTime);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
}

//E 16, N 22, M 128,8
void transform_points_AOS_with_constant( INOUT POINT_ELEMENT *p_AOS, IN int n_points, IN int m )
{
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	POINT_ELEMENT *d_pAOS;
	size_t size = N_POINTS * sizeof(POINT_ELEMENT);
	cudaMalloc(&d_pAOS, size);
	cudaMemcpy(d_pAOS, p_AOS, size, cudaMemcpyHostToDevice);
	dim3 dimBlock (BLOCK_WIDTH, BLOCK_HEIGHT);
	dim3 dimGrid (ARRAY_2D_WIDTH / dimBlock.x, ARRAY_2D_HEIGHT / dimBlock.y);

	cudaEventRecord( start, 0 );	// time record start
	TransformAOSwithConstantMemKernel <<<dimGrid, dimBlock>>> (d_pAOS, m);		// Kernel 호출
	cudaEventRecord( stop, 0 );		// time record stop
	cudaEventSynchronize( stop );

	cudaMemcpy(p_AOS, d_pAOS, size, cudaMemcpyDeviceToHost);
	cudaFree(d_pAOS);

	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime, start, stop);

	printf("*** GPU 07 - E:%d, N:%d, B:(%d,%d) M:constant, AOS : GPU Time taken = %.3fms\n",
		ELEM_PER_POINT, cal_N(N_ELEMS), BLOCK_WIDTH, BLOCK_HEIGHT, elapsedTime);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
}

//E 16, N 22, M 128,8
void transform_points_SOA_with_constant( INOUT POINTS_SOA p_SOA, IN int n_points, IN int m )
{
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	POINTS_SOA d_pSOA;
	size_t size = N_POINTS * sizeof(float);

	for(int i=0;i<ELEM_PER_POINT;i++){
		cudaMalloc(&d_pSOA.elem[i], size);
		cudaMemcpy(d_pSOA.elem[i], p_SOA.elem[i], size, cudaMemcpyHostToDevice);
	}
	
	dim3 dimBlock(BLOCK_WIDTH, BLOCK_HEIGHT);
	dim3 dimGrid (ARRAY_2D_WIDTH/dimBlock.x, ARRAY_2D_HEIGHT/dimBlock.y);

	cudaEventRecord( start, 0 );	// time record start
	TransformSOAwithConstantMemKernel <<<dimGrid, dimBlock>>> (d_pSOA, m);		// Kernel 호출
	cudaEventRecord( stop, 0 );		// time record stop
	cudaEventSynchronize( stop );

	for(int i=0;i<ELEM_PER_POINT;i++){
		cudaMemcpy(p_SOA.elem[i], d_pSOA.elem[i], size, cudaMemcpyDeviceToHost);
	}
	for(int i=0;i<ELEM_PER_POINT;i++){
		cudaFree(p_SOA.elem[i]);
	}

	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime, start, stop);

	printf("*** GPU 08 - E:%d, N:%d, B:(%d,%d) M:constant, SOA : GPU Time taken = %.3fms\n",
		ELEM_PER_POINT, cal_N(N_ELEMS), BLOCK_WIDTH, BLOCK_HEIGHT, elapsedTime);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
}

//E 32, N 22, M 128,8
void transform_points_AOS_with_constant3( INOUT POINT_ELEMENT2 *p_AOS, IN int n_points, IN int m )
{
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	

	// mycode
	POINT_ELEMENT2 *d_pAOS;
	size_t size = N_POINTS2 * sizeof(POINT_ELEMENT2);
	cudaMalloc(&d_pAOS, size);
	cudaMemcpy(d_pAOS, p_AOS, size, cudaMemcpyHostToDevice);
	dim3 dimBlock (BLOCK_WIDTH, BLOCK_HEIGHT);
	dim3 dimGrid (ARRAY_2D_WIDTH / dimBlock.x, ARRAY_2D_HEIGHT2 / dimBlock.y);



	cudaEventRecord( start, 0 );
	//

//.............
	
	TransformAOSwithConstantMemKernel2 <<<dimGrid, dimBlock>>> (d_pAOS, m);		// Kernel 호출

	//cudaDeviceSynchronize(); //It may stall the GPU pipeline.
	cudaEventRecord( stop, 0 );
	cudaEventSynchronize( stop );
	//
//.............
	//mycode
	cudaMemcpy(p_AOS, d_pAOS, size, cudaMemcpyDeviceToHost);
	cudaFree(d_pAOS);

//
	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime, start, stop);
//	printf("*** Array of structure: GPU Time taken = %.3fms\n", elapsedTime);
	printf("*** GPU 09 - E:%d, N:%d, B:(%d,%d) M:constant, AOS : GPU Time taken = %.3fms\n",
		ELEM_PER_POINT2, cal_N(N_ELEMS), BLOCK_WIDTH, BLOCK_HEIGHT, elapsedTime);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);


}

//E 32, N 22, M 128,8
void transform_points_SOA_with_constant3( INOUT POINTS_SOA2 p_SOA, IN int n_points, IN int m )
{
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	POINTS_SOA2 d_pSOA;
	size_t size = N_POINTS2 * sizeof(float);

	for(int i=0;i<ELEM_PER_POINT2;i++){
		cudaMalloc(&d_pSOA.elem[i], size);
		cudaMemcpy(d_pSOA.elem[i], p_SOA.elem[i], size, cudaMemcpyHostToDevice);
	}
	
	dim3 dimBlock(BLOCK_WIDTH, BLOCK_HEIGHT);
	dim3 dimGrid (ARRAY_2D_WIDTH/dimBlock.x, ARRAY_2D_HEIGHT2/dimBlock.y);

	cudaEventRecord( start, 0 );
	TransformSOAwithConstantMemKernel2 <<<dimGrid, dimBlock>>> (d_pSOA, m);		// Kernel 호출
	cudaEventRecord( stop, 0 );
	cudaEventSynchronize( stop );

	for(int i=0;i<ELEM_PER_POINT2;i++){
		cudaMemcpy(p_SOA.elem[i], d_pSOA.elem[i], size, cudaMemcpyDeviceToHost);
	}
	for(int i=0;i<ELEM_PER_POINT2;i++){
		cudaFree(p_SOA.elem[i]);
	}

	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime, start, stop);
//	printf("*** Structure of array: GPU Time taken = %.3fms\n", elapsedTime);
	printf("*** GPU 10 - E:%d, N:%d, B:(%d,%d) M:constant, SOA : GPU Time taken = %.3fms\n",
		ELEM_PER_POINT2, cal_N(N_ELEMS), BLOCK_WIDTH, BLOCK_HEIGHT, elapsedTime);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
}

//E 32, N 22, M 32,32
void transform_points_AOS_with_constant2( INOUT POINT_ELEMENT2 *p_AOS, IN int n_points, IN int m )
{
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	

	// mycode
	POINT_ELEMENT2 *d_pAOS;
	size_t size = N_POINTS * sizeof(POINT_ELEMENT2);
	cudaMalloc(&d_pAOS, size);
	cudaMemcpy(d_pAOS, p_AOS, size, cudaMemcpyHostToDevice);
	dim3 dimBlock (BLOCK_WIDTH2, BLOCK_HEIGHT2);
	dim3 dimGrid (ARRAY_2D_WIDTH / dimBlock.x, ARRAY_2D_HEIGHT2 / dimBlock.y);



	cudaEventRecord( start, 0 );
	//

//.............
	
	TransformAOSwithConstantMemKernel2 <<<dimGrid, dimBlock>>> (d_pAOS, m);		// Kernel 호출

	//cudaDeviceSynchronize(); //It may stall the GPU pipeline.
	cudaEventRecord( stop, 0 );
	cudaEventSynchronize( stop );
	//
//.............
	//mycode
	cudaMemcpy(p_AOS, d_pAOS, size, cudaMemcpyDeviceToHost);
	cudaFree(d_pAOS);

//
	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime, start, stop);
//	printf("*** Array of structure: GPU Time taken = %.3fms\n", elapsedTime);
	printf("*** GPU 11 - E:%d, N:%d, B:(%d,%d) M:constant, AOS : GPU Time taken = %.3fms\n",
		ELEM_PER_POINT2, cal_N(N_ELEMS), BLOCK_WIDTH2, BLOCK_HEIGHT2, elapsedTime);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);


}

//E 32, N 22, M 32,32
void transform_points_SOA_with_constant2( INOUT POINTS_SOA2 p_SOA, IN int n_points, IN int m )
{
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	POINTS_SOA2 d_pSOA;
	size_t size = N_POINTS * sizeof(float);

	for(int i=0;i<ELEM_PER_POINT2;i++){
		cudaMalloc(&d_pSOA.elem[i], size);
		cudaMemcpy(d_pSOA.elem[i], p_SOA.elem[i], size, cudaMemcpyHostToDevice);
	}
	
	dim3 dimBlock(BLOCK_WIDTH2, BLOCK_HEIGHT2);
	dim3 dimGrid (ARRAY_2D_WIDTH/dimBlock.x, ARRAY_2D_HEIGHT2/dimBlock.y);

	cudaEventRecord( start, 0 );
	TransformSOAwithConstantMemKernel2 <<<dimGrid, dimBlock>>> (d_pSOA, m);		// Kernel 호출
	cudaEventRecord( stop, 0 );
	cudaEventSynchronize( stop );

	for(int i=0;i<ELEM_PER_POINT2;i++){
		cudaMemcpy(p_SOA.elem[i], d_pSOA.elem[i], size, cudaMemcpyDeviceToHost);
	}
	for(int i=0;i<ELEM_PER_POINT2;i++){
		cudaFree(p_SOA.elem[i]);
	}

	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime, start, stop);
//	printf("*** Structure of array: GPU Time taken = %.3fms\n", elapsedTime);
	printf("*** GPU 12 - E:%d, N:%d, B:(%d,%d) M:constant, SOA : GPU Time taken = %.3fms\n",
		ELEM_PER_POINT2, cal_N(N_ELEMS), BLOCK_WIDTH2, BLOCK_HEIGHT2, elapsedTime);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
}

void generate_point_data( OUT POINT_ELEMENT *p_AOS, OUT POINTS_SOA *p_SOA, IN int n )
{
	int i;

	srand( ( unsigned int )time( NULL ) );

	for( i = 0; i < n; i++ )
	{
		for( int j = 0; j < ELEM_PER_POINT; ++j )
		{
			p_AOS[ i ].elem[ j ] = p_SOA->elem[ j ][ i ] = 100.0f*( ( float )rand() ) / RAND_MAX;
		}
	}
}

void generate_point_data2( OUT POINT_ELEMENT2 *p_AOS, OUT POINTS_SOA2 *p_SOA, IN int n )
{
	int i;

	srand( ( unsigned int )time( NULL ) );

	for( i = 0; i < n; i++ )
	{
		for( int j = 0; j < ELEM_PER_POINT2; ++j )
		{
			p_AOS[ i ].elem[ j ] = p_SOA->elem[ j ][ i ] = 100.0f*( ( float )rand() ) / RAND_MAX;
		}
	}
}

void generate_constant_data( IN int m )
{
	float *p_constant = new float[ m ];

	p_constant[ 0 ] = 0; // not used
	for( int i = 2; i <= m; ++i )
	{
		p_constant[ i-1 ] = 1.0f / ( float )i;
	}

	cudaMemcpyToSymbol( constantBuffer, p_constant, sizeof( float )* m );

	delete[] p_constant;
}

int main(void){

	float compute_time;
	int n_points, cutoff;
	POINT_ELEMENT *Points_AOS;
	POINTS_SOA Points_SOA;
	POINT_ELEMENT2 *Points_AOS2;
	POINTS_SOA2 Points_SOA2;

	n_points = N_POINTS;
	cutoff = 1000;
	Points_AOS = new POINT_ELEMENT[N_POINTS];
	Points_AOS2 = new POINT_ELEMENT2[N_POINTS];
	for( int i = 0; i < ELEM_PER_POINT; ++i )
	{
		Points_SOA.elem[ i ] = new float[ N_POINTS ];
	}
	for( int i = 0; i < ELEM_PER_POINT2; ++i )
	{
		Points_SOA2.elem[ i ] = new float[ N_POINTS ];
	}
	generate_point_data(Points_AOS, &Points_SOA, n_points);
	generate_point_data2(Points_AOS2, &Points_SOA2, n_points);
	generate_constant_data(cutoff);

	transform_points_AOS(Points_AOS, n_points, cutoff);	// E 16, N 22, M 128,8
	transform_points_SOA(Points_SOA, n_points, cutoff);	// E 16, N 22, M 128,8
	printf("\n");

	transform_points_AOS3(Points_AOS2, n_points, cutoff);	// E 32, N 22, M 128,8
	transform_points_SOA3(Points_SOA2, n_points, cutoff);	// E 32, N 22, M 128,8
	printf("\n");

	transform_points_AOS2(Points_AOS2, n_points, cutoff);	// E 32, N 22, M 32,32
	transform_points_SOA2(Points_SOA2, n_points, cutoff);	// E 32, N 22, M 32,32
	printf("\n");

	transform_points_AOS_with_constant(Points_AOS, n_points, cutoff);	// E 16, N 22, M 128,8
	transform_points_SOA_with_constant(Points_SOA, n_points, cutoff);	// E 16, N 22, M 128,8
	printf( "\n" );

	transform_points_AOS_with_constant3(Points_AOS2, n_points, cutoff);	// E 32, N 22, M 128,8
	transform_points_SOA_with_constant3(Points_SOA2, n_points, cutoff);	// E 32, N 22, M 128,8
	printf("\n");

	transform_points_AOS_with_constant2(Points_AOS2, n_points, cutoff);	// E 32, N 22, M 32,32
	transform_points_SOA_with_constant2(Points_SOA2, n_points, cutoff);	// E 32, N 22, M 32,32
	printf( "\n" );

	return 0;
}