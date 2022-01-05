# include <stdio.h>
# include <string.h>
# include <mpi.h>

const int MAX_STRING = 100; 

int main(void) {
    char beacon[MAX_STRING]; 
    char ack[3];  
    int comm_sz=0;
    int my_rank=0;
    double start_time=0, finish_time=0; 


    // MPI initialize
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    MPI_Barrier(MPI_COMM_WORLD);


    if(my_rank == 0) {//sender
        beacon[0]='a';
	beacon[1]='a';
	beacon[2]='b';

        sprintf(beacon, "Send beacon packet.");
        start_time = MPI_Wtime();
        printf("Process %d of %d > %s current time: %f\n", my_rank, comm_sz, beacon, start_time);

        MPI_Ssend(beacon, strlen(beacon) + 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        

        MPI_Recv(ack, MAX_STRING, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        finish_time = MPI_Wtime();

        printf("Process %d of %d > Received \"%s\" current time: %f\n", my_rank, comm_sz, ack, finish_time);
        printf("round-trip time: %f\n",finish_time-start_time);
    }

    if( my_rank == 1) {//reciever
	ack[0]='a';
	ack[1]='c';
	ack[2]='k';

        MPI_Recv(beacon, MAX_STRING, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Ssend(ack, strlen(ack) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;

}
