#include <iostream>
#include <chrono>
#include <string>

// ----- Matrix Multiplication Program Manually Created -----

using namespace std;
using namespace chrono;

// Function for creating a random double
double random_double(){
	const long max_rand = 1000000L;
	double lower_bound = 0;
	double upper_bound = 100;
 
	// Using random function to
	// get random double value
	double random_value = lower_bound + (upper_bound - lower_bound)*(rand() % max_rand)/max_rand;
	
	return random_value;
}

// Function for creating the matrices
double** create_matrix(size_t dimension){

	double** matrix = new double*[dimension];
	
	for(size_t i = 0; i < dimension; i++)
		matrix[i] = new double[dimension];
		
	return matrix;
}


// Function for filling the matrices
void fill_matrix(double **matrix, size_t dimension){

	double filler = 1;

	for(size_t i = 0; i < dimension; i++){
		for(size_t j = 0; j < dimension; j++){
			matrix[i][j] = filler;
			filler++;
		}
	}

}

// Function for filling the matrices
void fill_matrix(double **matrix, size_t dimension, double filler){

	for(size_t i = 0; i < dimension; i++){
		for(size_t j = 0; j < dimension; j++){
			matrix[i][j] = filler;
		}
	}

}

// Function for filling the matrices with random values
void fill_matrix_random(double **matrix, size_t dimension){

	double filler = 1;

	for(size_t i = 0; i < dimension; i++){
		for(size_t j = 0; j < dimension; j++){
			filler = random_double();
			matrix[i][j] = filler;
		}
	}

}

// Function for printing the matrices
void print_matrix(double **matrix, size_t dimension){

	for(size_t i = 0; i < dimension; i++){
		for(size_t j = 0; j < dimension; j++){
			cout << matrix[i][j] << "   ";
		}
		cout << endl;
	}
	
	cout << endl << endl;

}

// Function for freeing the memory reserved for the matrices
void free_matrix(double **matrix, size_t dimension){

	fill_matrix(matrix,dimension,0);
	for (size_t i = 0; i < dimension; i++) {
		delete[] matrix[i];
	}
	delete[] matrix;
}

//------------ Matrix multiplication ------------
void matrix_multiplication(double **multiplicand, double** multiplier, double** result, size_t dimension){

	double temp = 0;
	for(size_t i = 0; i < dimension; i++){
		for(size_t j = 0; j < dimension; j++){
			temp = 0;
			for(size_t k = 0; k < dimension; k++)
				temp += multiplicand[i][k]*multiplier[k][j];
			result[i][j] = temp;
		}
	}
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
	if(argc > 2 && (std::string(argv[2]).find("r")!=string::npos || std::string(argv[2]).find("t")!=string::npos))
		randomizer = true;
	if(argc > 3 && (std::string(argv[3]).find("f")!=string::npos || std::string(argv[3]).find("l")!=string::npos))
		print = false;
		
		
	
	// We create the matrices
	double** multiplicand = create_matrix(dimension);
	double** multiplier = create_matrix(dimension);
	double** result = create_matrix(dimension);
	
	// We fill the matrices
	if(randomizer){
		fill_matrix_random(multiplicand,dimension);
		fill_matrix_random(multiplier,dimension);
	} else {
		fill_matrix(multiplicand,dimension);
		fill_matrix(multiplier,dimension);
	}
	fill_matrix(result,dimension,0);
	
	if(print){
		// We print all the matrices
		cout << "Multiplicand" << endl;
		print_matrix(multiplicand,dimension);
		cout << "Multiplier" << endl;
		print_matrix(multiplier,dimension);
		cout << "Result" << endl;
		print_matrix(result,dimension);
	}
	
	// We initialize the chrono
	//auto start = clock();
	
	// We calculate the multiplication
	matrix_multiplication(multiplicand, multiplier, result, dimension);
	
	// We stop the chrono
	//auto end = clock();
	
	if(print){
		// We print the resultant matrix with the time elapsed in the multiplication
		cout << "Multiplication result" << endl;
		print_matrix(result,dimension);
		//cout << endl << "# Total elapsed time = " << (static_cast<double>((end - start)) / CLOCKS_PER_SEC*1000.0) << " miliseconds" << endl;
	} //else
		//cout << dimension << "," << (static_cast<double>((end - start)) / CLOCKS_PER_SEC*1000.0) << endl;
	
	
	// We free the memory reserved with the matrices
	free_matrix(multiplicand,dimension);
	free_matrix(multiplier,dimension);
	free_matrix(result,dimension);
	
	
	// ****
	time = clock() - time;
	
	cout << dimension << "," << ((float)time)/CLOCKS_PER_SEC << endl;
	
	

    return 0;
}
