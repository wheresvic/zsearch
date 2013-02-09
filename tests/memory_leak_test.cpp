#include <iostream>
#include <sstream>
#include <vector>
#include <stack>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <random>
#include <cstdlib>

using namespace std;

void work()
{
	char *c = new char[1000];
}

int main()
{
	work();

	return 0;
}
