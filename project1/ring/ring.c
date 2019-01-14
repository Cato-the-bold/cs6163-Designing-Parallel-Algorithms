#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

main(int argc, char *argv[])
{
  int num_processors, my_id ;

  MPI_Init(&argc, &argv) ;
  MPI_Comm_size(MPI_COMM_WORLD, &num_processors) ;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id) ;

  printf("There are %d processors, my processor is number %d\n",
         num_processors, my_id) ;

  if(num_processors%2 != 0) {
    printf("Assumes an even number of processors\n") ;
    MPI_Finalize() ;
    exit(-1) ;
  }

  /* Send Messages in a Ring, first even processors send, then odd ones */
  if(my_id%2 == 0) {
    int destination_processor = (my_id+1)%num_processors ;
    int tag = 0 ;
    printf("sending from %d to %d\n",my_id,destination_processor) ;
    MPI_Send(&my_id,1,MPI_INT,destination_processor,tag,MPI_COMM_WORLD);
  } else {
    MPI_Status stat ;
    int val ;
    MPI_Recv(&val,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&stat) ;
    printf("processor %d receiving msg %d\n",my_id,val) ;
  }
  /* now odd processors send */
  if(my_id%2 != 0) {
    int destination_processor = (my_id+1)%num_processors ;
    int tag = 0 ;
    printf("sending from %d to %d\n",my_id,destination_processor) ;
    MPI_Send(&my_id,1,MPI_INT,destination_processor,tag,MPI_COMM_WORLD);
  } else {
    MPI_Status stat ;
    int val ;
    MPI_Recv(&val,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&stat) ;
    printf("processor %d receiving msg %d\n",my_id,val) ;
  }

  /* Were done so exit */
  MPI_Finalize() ;
  exit(0) ;
}
