# cs6163-Designing-Parallel-Algorithms

## Project 1:
**** Search for Intelligent Puzzles ****
This project is designed to explore the bag-of-tasks model of parallel programming. 

## Project2:
Replace these MPI collective operations with an algorithm that uses point-to-point MPI calls
1) AllToAll() that performs an all-to-all communication primitives using integers, and 
2) AllToAllPersonalized() that performs the all-to-all-personalized
communication primitive using integers.

Adapt the hypercube all-to-all broadcast algorithm to run on non-power-of-2
processors (e.g. 5,6,7,13).  You will do this by creating a network of
virtual processors on the smallest hypercube that can contain all of
the physical processors (at most you will have to add p-2 virtual
processors to accomplish this). Implement the
hypercube all-to-all broadcast algorithm on these virtual
processors.

For the all-to-all personalized algorithm, implement both the
hypercube algorithm that is an extension of the mesh based algorithm
and the optimal hypercube algorithm using e-cube routing and compare
the performance.  You do not need to support non-power of two cases
for these algorithms.

I wrote an in-place alogrithm that only rearrange the orginal chunks, but it is still 45% lower than the standard APIs.

## Project3:
cancelled by Dr. Luke.
