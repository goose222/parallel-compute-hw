#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define TILE_WIDTH 16


// __global__ static void MatrixMulKernel(const float* Md,const float* Nd,float* Pd,int Width)
__global__ void MatrixMulKernel(const float* Md, const float* Nd, float* Pd, int x, int y, int z, float gamma)
{
    float Pvalue = 0.0;
    //计算Pd和Md中元素的行索引
    int Row = blockIdx.x * TILE_WIDTH + threadIdx.x; //行
    int Col = blockIdx.y * TILE_WIDTH + threadIdx.y; //列

    __shared__ float a_share[TILE_WIDTH][TILE_WIDTH];
    __shared__ float b_share[TILE_WIDTH][TILE_WIDTH];

    for(int i=0; i<y/TILE_WIDTH;i++){
        a_share[threadIdx.x][threadIdx.y]=Md[Row*y+(i*TILE_WIDTH+threadIdx.y)];
        b_share[threadIdx.y][threadIdx.x]=Nd[Col*y+(i*TILE_WIDTH+threadIdx.x)];

        __syncthreads();

        for(int k=0; k<TILE_WIDTH; k++){
            float tmp = (a_share[threadIdx.x][k]-b_share[threadIdx.y][k]);
            Pvalue += pow(tmp,2.0);
        }
        __syncthreads();
    }

    Pd[Row * z + Col] = exp(-1.0 * Pvalue/(2.0 * pow(gamma,2.0)));
}

void MatrixMultiplication_CUDA(const float* M, const float* N, float* P, int x, int y, int z ,float gamma)
{
    cudaSetDevice(0);  //设置目标GPU

    float *Md, *Nd, *Pd;
    int size1 = x * y * sizeof(float);//字节长度
    int size2 = y * z * sizeof(float);//字节长度
    int size3 = x * z * sizeof(float);//字节长度

    cudaMalloc((void**)&Md, size1);
    cudaMalloc((void**)&Nd, size2);
    cudaMalloc((void**)&Pd, size3);

    //Copies a matrix from the memory* area pointed to by src to the memory area pointed to by dst
    cudaMemcpy(Md, M, size1, cudaMemcpyHostToDevice);
    cudaMemcpy(Nd, N, size2, cudaMemcpyHostToDevice);

    //
    dim3 dimGrid(x / TILE_WIDTH, z / TILE_WIDTH);	//网格的维度
    dim3 dimBlock(TILE_WIDTH, TILE_WIDTH);					//块的维度
    MatrixMulKernel <<< dimGrid, dimBlock >>>(Md, Nd, Pd, x, y, z, gamma);

    cudaMemcpy(P, Pd, size3, cudaMemcpyDeviceToHost);
    //释放设备上的矩阵
    cudaFree(Md);
    cudaFree(Nd);
    cudaFree(Pd);
}

void matgen(float* a, int x, int y)
{
    int i, j;
    for (i = 0; i < x; i++)
    {
        for (j = 0; j < y; j++)
        {
            a[i * x + j] = (float)rand() / RAND_MAX + (float)rand() / (RAND_MAX*RAND_MAX);
        }
    }
}

void MatrixMultiplication(const float* M, const float* N, float* P, int x, int y, int z ,float gamma)//x*y  & y*z
{
    int i, j, k;
    for (i = 0; i < x; i++)
    {
        for (j = 0; j < z; j++)
        {
            float sum = 0;
            for (k = 0; k < y; k++)
            {
                sum += pow((M[i * x + k] - N[k * z + j]),2);
            }
            P[i * x + j] = exp(-1.0 * sum/(2.0 * pow(gamma,2.0)));
        }
    }
}

double MatrixMul_GPU()
{
    float *M, *N, *Pg;
    int x = 1024;	//1024×1024矩阵乘法
    int y = 1024;
    int z = 1024;
    int gamma = 1;
    M = (float*)malloc(sizeof(float)* x * y);
    N = (float*)malloc(sizeof(float)* y * z);
    Pg = (float*)malloc(sizeof(float)* x * z); //保存GPU计算结果

    srand(0);

    matgen(M, x, y);			//产生矩阵M
    matgen(N, y, z);			//产生矩阵N

    double timeStart, timeEnd;	//定义时间，求时间差用
    timeStart = clock();
    MatrixMultiplication_CUDA(M, N, Pg, x, y, z, gamma);			//GPU上计算
    timeEnd = clock();

    free(M);
    free(N);
    free(Pg);
    return timeEnd - timeStart;
}

double MatrixMul_CPU()
{
    float *M, *N, *Pc;
    int Width = 1024;	//1024×1024矩阵乘法
    int x = 1024;	//1024×1024矩阵乘法
    int y = 1024;
    int z = 1024;
    float gamma = 1;
    M = (float*)malloc(sizeof(float)* x * y);
    N = (float*)malloc(sizeof(float)* y * z);
    Pc = (float*)malloc(sizeof(float)* x * z); //保存GPU计算结果

    srand(0);

    matgen(M, x, y);			//产生矩阵M
    matgen(N, y, z);			//产生矩阵N

    double timeStart, timeEnd; //定义时间，求时间差用
    timeStart = clock();
    MatrixMultiplication(M, N, Pc, x, y, z, gamma);				//CPU上计算
    timeEnd = clock();

    free(M);
    free(N);
    free(Pc);
    return timeEnd - timeStart;
}

int main()
{
    printf("CPU use time %g\n", MatrixMul_CPU());
    printf("GPU use time %g\n", MatrixMul_GPU());
    system("pause");
    return 0;
}