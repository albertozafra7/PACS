#include <iostream>
#include <Eigen/Dense>
#include <string>
#include <sys/time.h>

// ----- Matrix Multiplication Using the Library and without Vectorization -----

using namespace std;
using namespace Eigen;


// Function for filling the matrices
void fill_matrix(MatrixXd& matrix, size_t dimension){

	double filler = 1;

	for(size_t i = 0; i < dimension; i++){
		for(size_t j = 0; j < dimension; j++){
			matrix(i,j) = filler;
			filler++;
		}
	}

}

// Function for filling the matrices
void fill_matrix(MatrixXd& matrix, size_t dimension, double filler){

	for(size_t i = 0; i < dimension; i++){
		for(size_t j = 0; j < dimension; j++){
			matrix(i,j) = filler;
		}
	}

}


// Function for printing the matrices
void print_matrix(MatrixXd matrix, size_t dimension){

	for(size_t i = 0; i < dimension; i++){
		for(size_t j = 0; j < dimension; j++){
			cout << matrix(i,j) << "   ";
		}
		cout << endl;
	}
	
	cout << endl << endl;

}



// The program is executed: ./program (dimensions) (randomizer?)
int main(int argc, char *argv[]) {
	
	// We create the variables related to the time measurement and we initialize them
	struct timeval timestamp;
	gettimeofday(&timestamp, NULL);
	
	double initial_time = (double)timestamp.tv_sec*1.0e6 + (double)timestamp.tv_usec;
	double time_various = 0;
	double time_multiplication = 0;

	// Seed for the random number
	srand(5); // This seed is stablished for having always the same result of the random number, comment when testing the program


	// We get the arguments of the program execution
	size_t dimension = 2; // Dimension of the matrices
	bool randomizer = false; // For creating matrices with random parameters
	bool print = true; // For printing the matrices and the result
	if(argc > 1)
		dimension = stoi(argv[1]);
	if(argc > 2 && (std::string(argv[2]).find("r")!=string::npos || std::string(argv[2]).find("r")!=string::npos))
		randomizer = true;
	if(argc > 3 && (std::string(argv[3]).find("f")!=string::npos || std::string(argv[3]).find("l")!=string::npos))
		print = false;
	
	// We create the matrices
   MatrixXd multiplicand(dimension, dimension);
   MatrixXd multiplier(dimension, dimension);
   
	// We fill the matrices
	if(randomizer){
		multiplicand = MatrixXd::Random(dimension, dimension);
		multiplier = MatrixXd::Random(dimension, dimension);
	} else {
		fill_matrix(multiplicand,dimension);
		fill_matrix(multiplier,dimension);
	}
		
	if(print){
		// We print all the matrices
		cout << "Multiplicand" << endl;
		print_matrix(multiplicand,dimension);
		cout << "Multiplier" << endl;
		print_matrix(multiplier,dimension);
	}
	
	//------------ Matrix multiplication ------------
	
	// We get the initial time spend in matrix creation and allocation
	gettimeofday(&timestamp, NULL);
	//cout << (double)timestamp.tv_sec << "    " << (double)timestamp.tv_usec << endl;
	time_various = (double)timestamp.tv_sec*1.0e6 + (double)timestamp.tv_usec - initial_time;
	if(print)
		cout << "time_various = " << time_various << endl;
	
	
	// We calculate the multiplication
	MatrixXd result = multiplicand * multiplier;
	
	// We get the time spent in the matrix multiplication
	gettimeofday(&timestamp, NULL);
	//cout << (double)timestamp.tv_sec << "    " << (double)timestamp.tv_usec << endl;
	time_multiplication = (double)timestamp.tv_sec*1.0e6 + (double)timestamp.tv_usec - (time_various + initial_time);
	if(print)
		cout << "time_multiplication = " << time_multiplication << endl;
		
	
	if(print){
		// We print the resultant matrix with the time elapsed in the multiplication
		cout << "Multiplication result" << endl;
		print_matrix(result,dimension);
	}
		
	// We get the total various time
	gettimeofday(&timestamp, NULL);
	time_various += (double)timestamp.tv_sec*1.0e6 + (double)timestamp.tv_usec - (time_multiplication + time_various + initial_time);
	
	// We print the times (miliseconds) as dimension,time_multiplication,time_various
	cout << dimension << "," << time_multiplication/1000 << "," << time_various/1000 << endl;
	
	
    return 0;
}
