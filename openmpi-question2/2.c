# include <stdio.h>
#include <stdlib.h>
# include <string.h>
# include <mpi.h>

const int MAX_STRING = 1024;

int spilt(char * sentence, char ** word, char ** stc, const int my_rank, const int comm_sz);
void check_flag(int * flag, char * sentence, char ** word, char ** stc, const int my_rank, const int comm_sz);

int main(void) {

    int comm_sz;
    int my_rank;

    int flag=0;  
    char sentence[MAX_STRING];  
    char * stc, * word;
    char words[] = "hello! here is lde. play the game.";

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


    if(my_rank == 0) {
        stc = strdup(words);
        strcpy(sentence, stc);

        MPI_Ssend(&flag, 1, MPI_INT, (my_rank + 1) % comm_sz, 0, MPI_COMM_WORLD);
        MPI_Ssend(sentence, strlen(sentence) + 1, MPI_CHAR, (my_rank + 1) % comm_sz, 0, MPI_COMM_WORLD); 

    } 
    

    while(1) {
        
        MPI_Recv(&flag, 1, MPI_INT, (my_rank + comm_sz - 1) % comm_sz, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (flag == 0)  { 
            MPI_Recv(sentence, MAX_STRING, MPI_CHAR, (my_rank + comm_sz - 1) % comm_sz, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            stc = strdup(sentence);
        }
        
        check_flag(&flag, sentence, &word, &stc, my_rank, comm_sz);

	int dest=(my_rank + 1) % comm_sz;
        
        if(flag > comm_sz) {
            MPI_Finalize();
            break;
        }
        else if (flag > 1) { 
            MPI_Ssend(&flag, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Finalize();
            break;
        }
        else if (flag == 1) {
            MPI_Ssend(&flag, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
        else{ 
            MPI_Ssend(&flag, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Ssend(sentence, strlen(sentence) + 1, MPI_CHAR, dest, 0, MPI_COMM_WORLD);   
        }  
        
    }
    return 0;

}

int spilt(char * sentence, char ** word, char ** stc, const int my_rank, const int comm_sz) {
    *word = strsep(stc, " !\"#$&'()*+,-./:;<=>?@[\\]^_`{|}~%\0");

    while(word[0][0]==0){
	if(*stc == NULL) return 1;
        *word = strsep(stc, " !\"#$&'()*+,-./:;<=>?@[\\]^_`{|}~%\0");
        //printf("aaa");
    }
    printf("Process %d of %d : %s\n", my_rank, comm_sz, *word);
    if(*stc == NULL) return 1;
    strcpy(sentence, *stc);
    return 0;
}


void check_flag(int * flag, char * sentence, char ** word, char ** stc, const int my_rank, const int comm_sz) {
    if (*flag == 1) printf("my id is %d.\n", my_rank); 
    if(*flag == 0) *flag = spilt(sentence, word, stc, my_rank, comm_sz);
    else (*flag)++;
}
