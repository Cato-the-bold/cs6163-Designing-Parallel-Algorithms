#include "utilities.h"
#include <mpi.h>

#include <iostream>
using std::cerr ;
using std::cout ;
using std::endl ;

/******************************************************************************
* numprocs:  number of processors                                             *
* myid:      my processor number (numbered 0 - numprocs-1)                    *
******************************************************************************/
int numprocs,myid ;

/******************************************************************************
* evaluate 2^i                                                                *
******************************************************************************/
inline unsigned int pow2(unsigned int i) { return 1 << i ; }

/******************************************************************************
* evaluate ceil(log2(i))                                                      *
******************************************************************************/
inline unsigned int log2(unsigned int i) {
  i-- ;
  unsigned int log = 1 ;
  for(i>>=1;i!=0;i>>=1)
    log++ ;
  return log ;
}

/******************************************************************************
* This function should implement the All to All Broadcast.  The value for each*
* processor is given by the argument send_value.  The recv_buffer argument is *
* an array of size p that will store the values that each processor transmits.*
* See Program 4.7 page 162 of the text.                                       *
******************************************************************************/

void AllToAll(int send_value[], int recv_buffer[], int size, MPI_Comm comm)
{
  MPI_Allgather(send_value,size,MPI_INT,recv_buffer,size,MPI_INT,comm) ;
}

#define MAPPING(vid, N) ((vid) > numprocs-1)? (vid)- N/2:(vid)

void AllToAll_2(int _buffer[], int recv_buffer[], int size, MPI_Comm comm)
{
	int vpartner, partner;
	MPI_Status status;
	int length = size;
	int N = 1 << log2((unsigned int)numprocs);

	for (int i = 0; i < size; i++) {
		recv_buffer[myid*size + i] = _buffer[i]; //send_buffer can be reused.
	}

	int me = myid;
	MPI_Request request, request2;

	for (int i = 1; i != N; i <<= 1) {
		vpartner = myid ^ i;
		partner = MAPPING(vpartner, N);
		int pair = me ^i;
		int p = (myid + N / 2) ^ i;
		bool v2 = false;

		if ((myid < N / 2) && ((myid + N / 2) >= numprocs)) {
			v2 = true;
		}

		MPI_Isend(recv_buffer + me*size, length, MPI_INT, partner, vpartner, comm, &request);
		if (v2) {
			MPI_Isend(recv_buffer + (me + N / 2)*size, length, MPI_INT, MAPPING(p, N), p, comm, &request2);
		}

		MPI_Recv(_buffer, length, MPI_INT, partner, MPI_ANY_TAG, comm, &status);
		if (status.MPI_SOURCE == myid);//ignore the two messages to itself.
		if (status.MPI_TAG == myid) {
			memcpy(recv_buffer + pair*size, _buffer, length * 4);
		}
		else if (status.MPI_TAG != myid) {
			memcpy(recv_buffer + (pair + N / 2)*size, _buffer, length * 4);
		}

		if (v2) {
			MPI_Recv(_buffer, length, MPI_INT, MAPPING(p, N), MPI_ANY_TAG, comm, &status);
			if (status.MPI_SOURCE == myid);//ignore the two messages to itself.
			memcpy(recv_buffer + (pair + N / 2)*size, _buffer, length * 4);
		}

		MPI_Wait(&request, &status);
		if (v2) {
			MPI_Wait(&request2, &status);
		}

		length *= 2;
		me = me>pair ? pair : me;

		MPI_Barrier(comm);
	}
}

/******************************************************************************
* This function should implement the All to All Personalized Broadcast.       *
* A value destined for each processor is given by the argument array          *
* send_buffer of size p.  The recv_buffer argument is an array of size p      *
* that will store the values that each processor transmits.                   *
* See pages 175-179 in the text.                                              *
******************************************************************************/

void AllToAllPersonalized(int send_buffer[], int recv_buffer[], int size, MPI_Comm comm) {
  MPI_Alltoall(send_buffer,size,MPI_INT,recv_buffer,size,MPI_INT,comm) ;
}

//hope I still understand it next week.
void AllToAllPersonalized_2(int send_buffer[], int recv_buffer[], int size, MPI_Comm comm) {
	int partner;
	MPI_Status status;
	int* _send = recv_buffer;
	int half_size = numprocs*size / 2;
	int* _recv = _send+ half_size;

	int dimension = log2(numprocs);
	int mask = 1<<(dimension-1);
	for (int i = 0; i < dimension; i++) {
		int chunk_size = mask*size;
		partner = myid ^ mask;
		//picks up and sends out. send_buffer  --->  _send
		int* t = _send;
		int* p_send_buf = send_buffer + ((mask & myid)? 0:chunk_size);
		//cout << "Ite:" << i << " id:" << myid << " partner:" << partner << " p_send_buf:" << p_send_buf - send_buffer << " mask:" << mask << endl;
		for (int j = 0; j < 1<<i; j++) {
			 memcpy(t,p_send_buf, chunk_size *4);
			 t += chunk_size;
			 p_send_buf += chunk_size <<1;
		}
		//printA("	_send", _send, half_size);
		int err = MPI_Sendrecv(_send, half_size, MPI_INT, partner, i, _recv, half_size, MPI_INT, partner, i, comm, &status);
		//printA("	_recv", _recv, half_size);

		//receives and fills the holes. _recv ---> send_buffer
		t = _recv;
		p_send_buf = send_buffer + ((mask & myid) ? 0 : chunk_size);
		for (int j = 0; j < 1 << i; j++) {
			memcpy(p_send_buf,t, chunk_size *4);
			t += chunk_size;
			p_send_buf += chunk_size <<1;
		}
		mask = mask >> 1;

		//MPI_Barrier(comm);
	}

	for (int i = 0; i < numprocs*size;i++) {
		recv_buffer[i] = send_buffer[i];
	}
}

void AllToAllPersonalized_ecube(int send_buffer[], int recv_buffer[], int size, MPI_Comm comm) {
	int partner;
	MPI_Status status;
	
	memcpy(recv_buffer+myid*size,send_buffer+myid*size,size*4);

	for (int i = 1; i < numprocs;i++) {
		partner = myid ^ i;
		//cout << "[ "<<i<<" ] "<< myid<<"'s partner:"<<partner<<" size:"<<size<<endl;
		int err = MPI_Sendrecv(send_buffer+partner*size,size,MPI_INT,partner,i,recv_buffer+partner*size,size,MPI_INT,partner,i,comm,&status);
		if (err) {
			cerr <<"error:"<<err<<endl;
		}
		//MPI_Barrier(comm);
	}
}


int main(int argc, char **argv) {

  chopsigs_() ;
  
  double time_passes, max_time ;
  /* Initialize MPI */
  MPI_Init(&argc,&argv) ;

  /* Get the number of processors and my processor identification */
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs) ;
  MPI_Comm_rank(MPI_COMM_WORLD,&myid) ;


  int test_runs = 8000/numprocs ;
  if(argc == 2)
    test_runs = atoi(argv[1]) ;
  const int max_size = pow2(16) ;
  int *recv_buffer = new int[numprocs*max_size] ;
  int *send_buffer = new int[numprocs*max_size] ;
  
  if(0 == myid) {
    cout << "Starting " << numprocs << " processors." << endl ;
  }



  /***************************************************************************/
  /* Check Timing for Single Node Broadcast emulating an alltoall broadcast  */
  /***************************************************************************/
  
  MPI_Barrier(MPI_COMM_WORLD) ;


  /* We can't accurately measure short times so we must execute this operation
     many times to get accurate timing measurements.*/
  for(int l=0;l<=16;l+=4) {
    int msize = pow2(l) ;
    /* Every call to get_timer resets the stopwatch.  The next call to 
       get_timer will return the amount of time since now */
    get_timer() ;
    for(int i=0;i<test_runs;++i) {
      /* Slow alltoall broadcast using p single node broadcasts */
      for(int p=0;p<numprocs;++p)
	recv_buffer[p] = 0 ;
      int send_info = myid + i*numprocs ;
      for(int k=0;k<msize;++k)
	send_buffer[k] = send_info ;
      AllToAll_2(send_buffer,recv_buffer,msize,MPI_COMM_WORLD) ;

      for(int p=0;p<numprocs;++p)
	if(recv_buffer[p*msize] != (p + i*numprocs))
        cerr << "recv failed on processor " << myid << " recv_buffer["
             << p << "] = "
             << recv_buffer[p*msize] << " should  be " << p + i*numprocs << endl ;
    }
    time_passes = get_timer() ;
  
    MPI_Reduce(&time_passes, &max_time, 1, MPI_DOUBLE,MPI_MAX, 0, MPI_COMM_WORLD) ;
    if(0 == myid)
      cout << "all to all broadcast for m="<< msize << " required " << max_time/double(test_runs)
	   << " seconds." << endl ;
  }

  /***************************************************************************/
  /* Check Timing for All to All personalized Broadcast Algorithm            */
  /***************************************************************************/

  MPI_Barrier(MPI_COMM_WORLD) ;

  for(int l=0;l<=16;l+=4) {
    int msize = pow2(l) ;
    /* Every call to get_timer resets the stopwatch.  The next call to 
       get_timer will return the amount of time since now */
    get_timer() ;

    for(int i=0;i<test_runs;++i) {
      for(int p=0;p<numprocs;++p)
	for(int k=0;k<msize;++k)
	  recv_buffer[p*msize+k] = 0 ;
      int factor = (myid&1==1)?-1:1 ;
      for(int p=0;p<numprocs;++p)
	for(int k=0;k<msize;++k)
	  send_buffer[p*msize+k]=myid*numprocs + p + i*myid*myid*factor;
      int send_info = myid + i*numprocs ;

      AllToAllPersonalized(send_buffer, recv_buffer, msize, MPI_COMM_WORLD); 
      //AllToAllPersonalized_2(send_buffer,recv_buffer,msize,MPI_COMM_WORLD) ;
      //AllToAllPersonalized_ecube(send_buffer, recv_buffer, msize, MPI_COMM_WORLD);   
 
      for(int p=0;p<numprocs;++p) {
	int factor = (p&1==1)?-1:1 ;
	if(recv_buffer[p*msize] != ( p*numprocs + myid + i*p*p*factor ))
	  cerr << "recv failed on processor " << myid << " recv_buffer["
	       << p << "] = "
	       << recv_buffer[p*msize] << " should  be " << p*numprocs + myid + i*p*p*factor << endl ;
      }
    }
  
    time_passes = get_timer() ;
  
    MPI_Reduce(&time_passes, &max_time, 1, MPI_DOUBLE,MPI_MAX, 0, MPI_COMM_WORLD) ;
    if(0 == myid)
      cout << "all-to-all-personalized broadcast, m=" << msize 
	   << " required " << max_time/double(test_runs)
	   << " seconds." << endl ;
  }
  
  /* We're finished, so call MPI_Finalize() to clean things up */
  MPI_Finalize() ;
  return 0 ;
}
