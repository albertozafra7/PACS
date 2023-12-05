#include <iostream>
#include <ctime>
#include <iomanip>

using namespace std;

long fibonacci (unsigned n) {
	if (n<2) return n;
	return fibonacci(n-1) + fibonacci(n-2);
}

int main() {
	clock_t time;
	time = clock();
	cout << "f(45) = " << fibonacci(45) << endl;
	time = clock() - time;
	cout << "Seconds: " << fixed << setprecision(4)
	<< ((float)time)/CLOCKS_PER_SEC << endl;
	return 0;
}

