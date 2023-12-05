#include <iostream>
#include <sys/time.h>

using namespace std;

int main() {
	struct timeval timestamp;
	gettimeofday(&timestamp, NULL);
	cout << "Seconds: " << timestamp.tv_sec << endl
	<< "Microseconds: " << timestamp.tv_usec << endl;
	return 0;
}
