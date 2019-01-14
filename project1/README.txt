First you will need to login to the HPCC computer systems.  All systems
from outside of the HPCC need to be accessed through the titan server
using ssh.  To begin use an ssh client to login to titan.hpc.msstate.edu
with your netid as your userid.  The initial password for your class account
will be given in class, change it after logging in using the passwd command.


To compile the files in this directory, you will first need to make some
changes to your .bashrc file in your home directory.  Add this swsetup line 
before the "if [ -z $PS1 ];then return; fi" line.

------------------------------------------------------------------------------
swsetup openmpi:pbs
------------------------------------------------------------------------------

After adding this to your .bashrc, you will need to login to the raptor 
cluster with either the command:

ssh raptor-login

or 

rsh raptor-login

Then you can compile using the command "make" in the project directory.

This directory contains several program files:

game.h:      This file defines a structs that is used in solving a puzzle
game.cc:     Implementation of methods defined in game.h
utilities.h: Define utility routines that will measure time and kill runaway
             jobs.
utilities.cc:Implementation of utility routines
main.cc:     Program main and implementation of server and client code.

easy_sample.dat:  A sample of puzzles that are computationally easy to solve
                  (to be used for debugging program)
hard_sample.dat:  A sample of puzzles that are computationally hard to solve
                  (to be used for measuring program performance)

debug0?.js:  A selection of job scripts for debugging runs on the
             parallel cluster

run??.js:    A selection of job scripts for performance runs on the
             parallel cluster
-----------------------------------------------------------------------------

How to submit jobs to the parallel cluster using the PBS batch queuing system:

To submit a job once the program has been compiled use one of the
provided PBS job scripts (these end in .js).  These job scripts have
been provided to run parallel jobs on the cluster.  To submit a job use 
the "qsub" command. (note:  "man qsub" to get detailed information on this 
command)

example:
qsub debug01.js

To see the status of jobs on the queue, use qstatall.  Example:

raptor-login[229]% qstatall
Job id           Name             User             Time Use S Queue
---------------- ---------------- ---------------- -------- - -----
68622.raptor     Work01P          lush                    0 R q64p48h


This lists information associated with the job.  The important things to note
are the Job id and the state (S).  The state tells the status of the job.  
Generally the status will be one of three values:  Q - queued waiting for 
available processors, R - running, E - exiting.

Additionally, if you decide that you don't want to run a job, or it seems to
not work as expected, you can run "qdel Job id" to delete it from the queue.
For example, to remove the above job from the queue enter the command

qdel 68622.raptor
