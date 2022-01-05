//
// Created by 63093 on 2021/10/12.
//

#include <omp.h>
#include <array>
#include <vector>
#include <iostream>

using namespace std;

template<typename Type>
Type calcuPartOfMatrixMulti(int row,int col,Type **pMatA, Type **pMatB,int N)
{
    Type resultValue = 0;
    for(int transNumber = 0 ; transNumber < N ; transNumber++) {
        resultValue += pMatA [row] [transNumber] * pMatB [transNumber] [col] ;
    }
    return resultValue;
}

template<typename Type>
Type ** MatrixMultiplication(Type **pMatA, Type **pMatB, Type **pMatC, int M, int N, int K){
    int coreNum = omp_get_num_procs();
#pragma omp parallel for num_threads(coreNum)
    for(int row = 0 ; row < M ; row++){
        for(int col = 0; col < K ; col++){
            pMatC [row] [col] = calcuPartOfMatrixMulti (row,col,pMatA,pMatB,N);
        }
    }
    return pMatC;
}

int main()
{
    //init the matrix
    const int M=2;
    const int N=2;
    const int K=3;

    int** pMatA = new int* [M];
    for(int i=0;i<M;i++){
        pMatA[i] = new int[N];
        for(int j=0;j<N;j++)
            pMatA[i][j]=1;
    }
    pMatA[0][1]=0;
    pMatA[1][0]=2;


    int** pMatB = new int* [N];
    for(int i=0;i<N;i++){
        pMatB[i] = new int[K];
        for(int j=0;j<K;j++)
            pMatB[i][j]=1;
    }
    pMatB[0][1]=3;
    pMatB[1][0]=2;

    int** pMatC = new int* [M];
    for(int i=0;i<M;i++){
        pMatC[i] = new int[K];
        for(int j=0;j<K;j++)
            pMatC[i][j]=0;
    }



    /*OPENMP*/
    MatrixMultiplication(pMatA,pMatB,pMatC,M,N,K);
    //cout<<pMatC[0][0]<<pMatC[0][1]<<pMatC[0][2]<<endl;
    //cout<<pMatC[1][0]<<pMatC[1][1]<<pMatC[1][2]<<endl;

    return 0;
}


