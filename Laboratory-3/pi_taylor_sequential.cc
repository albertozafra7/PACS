#include <iomanip>
#include <iostream>
#include <limits>
#include <chrono>

// Allow to change the floating point type
using my_float = long double;

my_float pi_taylor(size_t steps) {
	
	int sign = 1;
	my_float result = 0.0f;
	
	for(size_t i = 0; i < steps; i++){
	
		result += sign/static_cast<my_float>(2*i+1);
		sign=-sign;
		
	}

	return 4.0f*result;
}

int main(int argc, const char *argv[]) {

	// read the number of steps from the command line
	if (argc != 2) {
		std::cerr << "Invalid syntax: pi_taylor <steps>" << std::endl;
		exit(1);

	}

	size_t steps = std::stoll(argv[1]);
	auto start = std::chrono::steady_clock::now();
	auto pi = pi_taylor(steps);
	auto stop = std::chrono::steady_clock::now();

	std::cout << "For " << steps << ", pi value: "
				 << std::setprecision(std::numeric_limits<my_float>::digits10 + 1)
				 << pi << std::endl;
				
	std::cout << "It took " << std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count()
				 << " nanoseconds to execute the program." << std::endl;
}
