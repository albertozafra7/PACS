#include <iostream>
#include <Eigen/Dense>
#include <chrono>
#include <string>
#include <ctime>

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
	
	// *** We define the timer and initalize it
	clock_t time;
	time = clock();
	
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
	
	// We initialize the chrono
	//auto start = chrono::clock();
	
	
	// We calculate the multiplication
	MatrixXd result = multiplicand * multiplier;
	
	// We stop the chrono
	//auto end = chrono::clock();
	
	// ****
	time = clock() - time;
	
	if(print){
		// We print the resultant matrix with the time elapsed in the multiplication
		cout << "Multiplication result" << endl;
		print_matrix(result,dimension);
		//cout << endl << "# Total elapsed time = " << (static_cast<double>((end - start)) / CLOCKS_PER_SEC*1000.0) << " miliseconds" << endl;
	} else
		//cout << dimension << "," << (static_cast<double>((end - start)) / CLOCKS_PER_SEC*1000.0) << endl;
		cout << dimension << "," << ((float)time)/CLOCKS_PER_SEC << endl;
		
	
	
    return 0;
}
