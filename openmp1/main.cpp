#include <stdio.h>
#include <omp.h>
#include <array>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

#define MAX_INPUT_LENGTH 1024

template<typename Type>
Type compute(vector<Type> a){
    Type sum = 0;
    int length=a.size();
    int coreNum = omp_get_num_procs();
    printf("coreNum: %d\n", coreNum);

    Type *sumArray = new Type[coreNum];
    for (int i=0; i<coreNum; ++i)
    {
        sumArray[i] = 0;
    }

#pragma omp parallel
    {
        int id=omp_get_thread_num();
        //printf("thread ID: %d\n", id);
        for (int i=id; i<length; i=i+coreNum)
        {
            sumArray[id] = sumArray[id] + a[i];
        }
    }

    for (int i=0; i<coreNum; ++i)
    {
        sum = sum + sumArray[i];
    }
    delete sumArray;
    printf( "sum: %d\n" ,sum );
    return sum;
}

int main()
{
    vector<int> inputMy={1,2,3,4,5,6,7,8,9,10};

    /*OPENMP*/
    compute(inputMy);

    return 0;
}


