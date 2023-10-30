#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>
#include <thread>
#include <utility>
#include <vector>

using my_float = float;

void
pi_taylor_chunk(std::vector<my_float> &output,
        size_t thread_id, size_t start_step, size_t stop_step) {

	

	int sign = 1;
	if (start_step % 2 != 0)
		sign = -sign;
   	
	my_float result = 0.0f;
	my_float c = 0.0f;
	
	for(size_t i = start_step; i < stop_step; i++){
	
		my_float y = (sign/static_cast<my_float>(2*i+1)) - c;
		my_float t = result + y;
		
		c = (t - result) - y;
		result = t;
		sign = -sign;
		
	}
	
	output[thread_id] = result;


}

std::pair<size_t, size_t>
usage(int argc, const char *argv[]) {
    // read the number of steps from the command line
    if (argc != 3) {
        std::cerr << "Invalid syntax: pi_taylor <steps> <threads>" << std::endl;
        exit(1);
    }

    size_t steps = std::stoll(argv[1]);
    size_t threads = std::stoll(argv[2]);

    if (steps < threads ){
        std::cerr << "The number of steps should be larger than the number of threads" << std::endl;
        exit(1);

    }
    return std::make_pair(steps, threads);
}

int main(int argc, const char *argv[]) {


    auto ret_pair = usage(argc, argv);
    auto steps = ret_pair.first;
    auto threads = ret_pair.second;

    my_float pi;

    // We compute the chunk size
	size_t chunk_size = steps/threads;
	
	// This is the vector which has the thread management
	std::vector<std::thread> thread_vector;
	// This is the vector that gets the output of each thread
	std::vector<my_float> thread_output(threads,0.0f);

	// Those are the steps
	size_t start_step = 0;
	size_t stop_step = chunk_size;

	// Measurement of the starting time
	auto start = std::chrono::steady_clock::now();


	// ----- Thread management ----- 
	for(size_t i = 0; i < threads; i++){
	
		if(i == threads-1 && stop_step < steps)
			stop_step = steps;
	
		std::cout << "Thread with id = " << i << " start_step = " << start_step << " stop_step = " << stop_step << std::endl;

		// We create the thread
		thread_vector.push_back(std::thread(pi_taylor_chunk, std::ref(thread_output), i, start_step, stop_step));
		
		// We update the start and stop steps
		start_step = stop_step;
		stop_step = std::min(stop_step+chunk_size,steps);	// This is a safe measure in order to not do more computations than needed

	}
	
	
	// Wait for completion
	for(size_t j = 0; j < threads; j++){
		thread_vector[j].join();
	}
	
	//for(size_t j = 0; j < threads; j++){
	//	std::cout << thread_output[j] << std::endl;
	//}

	// Measurement of the end time
	auto stop = std::chrono::steady_clock::now();

	// Get the pi values
	pi = std::accumulate(thread_output.begin(), thread_output.end(), 0.0f);
	
	// Clean the threads
	thread_output.clear();
	thread_vector.clear();

	// Final pi calculation
	pi = 4.0f*pi;
	
	// Couts
	std::cout << "For " << steps << ", pi value: "
	<< std::setprecision(std::numeric_limits<long double>::digits10 + 1)
	<< pi << std::endl;
	
	std::cout << "It took " << std::chrono::duration_cast<std::chrono::microseconds>(stop-start).count()
			 << " microseconds to execute the program." << std::endl;

}

