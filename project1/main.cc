#include "game.h"
#include "utilities.h"
// Standard Includes for MPI, C and OS calls
#include <mpi.h>

// C++ standard I/O and library includes
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctype.h>

using std::cout;
using std::cerr;
using std::endl;

using std::vector;
using std::string;

using std::ofstream;
using std::ifstream;
using std::ios;

enum MSG_TYPE {
	C_REQUEST,
	S_NEW_TASK,
	C_RESULT,
	S_END
};

const int BUFFER_SIZE = 1024*8;

void Server(int argc, char *argv[]) {
	// Check to make sure the server can run
	if (argc != 5) {
		cerr << "four arguments please!" << endl;
		MPI_Abort(MPI_COMM_WORLD, -1);
	}

	// Input case filename 
	ifstream input(argv[1], ios::in);

	// Output case filename
	ofstream output(argv[2], ios::out);

	int chunk_size = atoi(argv[3]);

	int use_server = atoi(argv[4]);

	int numProcessors;
	/* Get the number of processors and my processor identification */
	MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);

	int count = 0;
	int NUM_GAMES = 0;
	// get the number of games from the input file
	input >> NUM_GAMES;

	int current = 0;

	MPI_Status status;
	int flag;

	char buffer[BUFFER_SIZE];

	int cNum = 0;

	while (1) {
		//get_timer();
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
		//cout << "probe execution time = " << get_timer() << " seconds." << endl;
		if (flag) {
			//cout << "status:" << status.MPI_SOURCE << " tag: " << status.MPI_TAG << endl;
			if (status.MPI_TAG == C_RESULT) {
				MPI_Recv(buffer, BUFFER_SIZE, MPI_CHAR, status.MPI_SOURCE, C_RESULT, MPI_COMM_WORLD, &status);
				uint8_t cnt = buffer[0];
				char* b = buffer + 1;
				while (cnt-- != 0) {
					output << "found solution:" << endl;
					game_state s;
					s.Init((unsigned char*)b);
					s.Print(output);
					int size = *(int*)(&b[25]);
					move* sol = (move*)(b + 25 + sizeof(int));
					for (int i = 0; i < size; ++i) {
						s.makeMove(sol[i]);
						output << "-->" << endl;
						s.Print(output);
					}
					b += 25 + sizeof(int) + sizeof(move)*size;
					output << "solved" << endl << endl;
					count++;
				}
			}
			else if(status.MPI_TAG == C_REQUEST){
				MPI_Recv(buffer, 1, MPI_CHAR, status.MPI_SOURCE, C_REQUEST, MPI_COMM_WORLD, &status);
			}

			if (current >= NUM_GAMES) {
				MPI_Send(buffer, 1, MPI_CHAR, status.MPI_SOURCE, S_END, MPI_COMM_WORLD);
				if (++cNum == numProcessors - 1) {
					break;
				}
			}
			else {
				string input_string;
				int i = 0;
				int size = NUM_GAMES - current < chunk_size ? NUM_GAMES - current : chunk_size;
				for (i=0; i < size; i++) {
					input >> input_string;
					strcpy(buffer+i*26,input_string.c_str());
				}
				current += size;
				//get_timer();
				MPI_Send(buffer, size*26, MPI_CHAR, status.MPI_SOURCE, S_NEW_TASK, MPI_COMM_WORLD);
				//cout << "commucation execution time = " << get_timer() << " seconds." << endl;
			}
		}
		else if (use_server == 1) {
			string input_string;
			if (current >= NUM_GAMES) {
				continue;
			}
			input >> input_string;
			current++;
			unsigned char buf[IDIM*JDIM];
			for (int j = 0; j<IDIM*JDIM; ++j)
				buffer[j] = input_string[j];

			game_state game;
			game.Init((unsigned char*)buffer);
			// solution
			move solution[IDIM*JDIM];
			int size = 0;
			// Search for a solution to the puzzle
			bool found = depthFirstSearch(game, size, solution);
			if (found) {
				output << "found solution:" << endl;
				game.Print(output);
				for (int i = 0; i < size; ++i) {
					game.makeMove(solution[i]);
					output << "-->" << endl;
					game.Print(output);
				}
				output << "solved" << endl << endl;
				count++;
			}
		}
	}
	cout << "Found " << count << " solutions." << endl;
}
// Put the code for the client here
void Client() {
	unsigned char recv_buffer[BUFFER_SIZE];
	char send_buffer[BUFFER_SIZE];
	MPI_Status status;
	uint8_t findResult = 0;
	char* curr;
	while (1) {
		// send "request message" to the farmer
		if (findResult) {
			MPI_Send(send_buffer, curr - send_buffer, MPI_CHAR, 0, C_RESULT, MPI_COMM_WORLD);
			findResult = 0;
		}
		else {
			MPI_Send(send_buffer, 1, MPI_CHAR, 0, C_REQUEST, MPI_COMM_WORLD);
		}
		// receive message from the farmer
		MPI_Recv(recv_buffer, BUFFER_SIZE, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		// receive a "job finish" message
		if (status.MPI_TAG == S_END) {
			//cout << "c end" << endl;
			break;
		}
		else if (status.MPI_TAG == S_NEW_TASK) {
			int length;
			MPI_Get_count(&status, MPI_CHAR, &length);
			curr = send_buffer+1;
			for (int i = 0; i < length; i += 26) {
				game_state game;
				game.Init(&recv_buffer[i]);
				// solution
				move solution[IDIM*JDIM];
				int size = 0;
				// Search for a solution to the puzzle
				bool found = depthFirstSearch(game, size, solution);
				if (found) {
					memcpy(curr, &recv_buffer[i], 25);
					int* tmp = (int*)(curr + 25);
					*tmp = size;
					memcpy(curr + 25 + sizeof(int), solution, size * sizeof(move));
					curr += 25 + sizeof(int) + sizeof(move)*size;
					findResult += 1;
				}
			}
			send_buffer[0] = findResult;
			//cout << "sizeof result" << curr - send_buffer << endl;
		}
	}
}


int main(int argc, char *argv[]) {
	// This is a utility routine that installs an alarm to kill off this
	// process if it runs to long.  This will prevent jobs from hanging
	// on the queue keeping others from getting their work done.
	chopsigs_();

	// All MPI programs must call this function
	MPI_Init(&argc, &argv);

	int myId;
	int numProcessors;
	/* Get the number of processors and my processor identification */
	MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);
	MPI_Comm_rank(MPI_COMM_WORLD, &myId);

	if (myId == 0) {
		// Processor 0 runs the server code
		get_timer();// zero the timer
		Server(argc, argv);
		// Measure the running time of the server
		cout << "execution time = " << get_timer() << " seconds." << endl;
	}
	else {
		// all other processors run the client code.
		Client();
	}

	// All MPI programs must call this before exiting
	MPI_Finalize();
}

