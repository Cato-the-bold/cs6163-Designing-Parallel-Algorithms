#include "utilities.h"

// Standard Includes for MPI, C and OS calls
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int sleep_time = 1200 ; // 20 minutes



//**************************************************************************/
// UTILITY ROUTINES


// This routine handles program traps.  We put this in here so that the
// mpi program will be more robust to crashes and won't lock up the queue
// (Hopefully)
void program_trap(int sig) /*,code,scp,addr)*/ {
  const char *sigtype = "(undefined)" ;
  
  switch(sig) {
  case SIGBUS:
    sigtype = "a Bus Error" ;
    break ;
  case SIGSEGV:
    sigtype = "a Segmentation Violation" ;
    break ;
  case SIGILL:
    sigtype = "an Illegal Instruction Call" ;
    break ;
  case SIGSYS:
    sigtype = "an Illegal System Call" ;
    break ;
  case SIGFPE:
    sigtype = "a Floating Point Exception" ;
    break ;
  case SIGALRM:
    sigtype = "a Alarm Signal!" ;
    break ;
  }
  fprintf(stderr,"ERROR: Program terminated due to %s\n",sigtype) ;
  MPI_Abort(MPI_COMM_WORLD,-1) ;
}

// This routine redirects the OS signal traps to the above routine
// Also sets a 9 minute alarm to kill the program in case it runs
// away.  This really should prevent job queue problems for errant
// programs.
void chopsigs_() {
  signal(SIGBUS,program_trap) ;
  signal(SIGSEGV,program_trap) ;
  signal(SIGILL,program_trap) ;
  signal(SIGSYS,program_trap) ;
  signal(SIGFPE,program_trap) ;
  signal(SIGALRM,program_trap) ;
  // Send an alarm signal after 9 minutes elapses
  alarm(sleep_time) ;
}


// A utility routine for measuring time
double get_timer() {
  static double to = 0;
  double tn,t ;
  tn = MPI_Wtime() ;
  t = tn - to ;
  to = tn ;
  return t ;
}
