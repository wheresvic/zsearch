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



void call_from_thread(int tid)
{
	std::cout << "Launched by thread " << tid << std::endl;
}

void test_multiple_threads()
{
	const int num_threads = 10;

	std::thread t[num_threads];

	for (int i = 0; i < num_threads; ++i)
	{
		t[i] = std::thread(call_from_thread, i);
	}

	std::cout << "Launched from function" << std::endl;

	// Join the threads with the main thread
	for (int i = 0; i < num_threads; ++i)
	{
		t[i].join();
	}
}


// print function for "thread safe" printing using a stringstream
void print(const stringstream& s)
{
	cout << s.rdbuf(); cout.flush();
}


std::stack<int> products;
std::mutex xmutex;
std::condition_variable produced;
std::condition_variable consumed;
std::atomic<bool> done;

void producer(int id)
{
	std::stringstream producerCreatedLogMsg;
	producerCreatedLogMsg << "Producer " << id << " created" << std::endl;
	print(producerCreatedLogMsg);

	bool first = true;

	while (!done)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		int product = rand() % 10 + 1;

		std::unique_lock<std::mutex> lock(xmutex);

		// print(stringstream() << "Producer waiting" << std::endl);
		if (!first)
		{
			first = false;
			consumed.wait(lock);
		}

		products.push(product);
		std::stringstream productLogMsg;
		productLogMsg << "Producer " << id << " produced " << product << std::endl;
		print(productLogMsg);

		produced.notify_all();
	}
}

void consumer(int id)
{
	std::stringstream consumerCreatedLogMsg;
	consumerCreatedLogMsg << "Consumer " << id << " created" << std::endl;
	print(consumerCreatedLogMsg);

	while (!done)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));

		unique_lock<mutex> lock(xmutex);

		// print(stringstream() << "Consumer waiting" << std::endl);
		produced.wait(lock);

		std::stringstream consumerLogMsg;
		if (products.size() > 0)
		{
			int product = products.top();
			products.pop();

			consumerLogMsg << "Consumer " << id << " consumed " << product << std::endl;
			print(consumerLogMsg) ;
		}
		else
		{
			consumerLogMsg << "Consumer " << id << " has nothing to consume" << std::endl;
			print(consumerLogMsg);
		}

		consumed.notify_all();
	}
}

void kicker()
{
	std::stringstream kickerCreateLogMsg;
	kickerCreateLogMsg << "Kicker created" << std::endl;
	print(kickerCreateLogMsg);

	while (!done)
	{
		std::this_thread::sleep_for(std::chrono::seconds(2));

		unique_lock<mutex> lock(xmutex);

		int num = products.size();
		std::stringstream productionLogMsg;
		productionLogMsg << "--> " << num << " products produced" << std::endl;
		print(productionLogMsg);

		if (6 == (rand() % 6 + 1))
		{
			done = true;
		}
	}
}

int main()
{
	srand ( time(NULL) );

	done = false;

	vector<thread> producers_and_consumers;

	int num_producers = 5;
	int num_consumers = 10;

	// Create producers
	for(int i = 0; i < num_producers; ++i)
	{
		producers_and_consumers.push_back(thread(producer, i));
	}

	// Create consumers
	for(int i = 0; i < num_consumers; ++i)
	{
		producers_and_consumers.push_back(thread(consumer, i));
	}

	producers_and_consumers.push_back(thread(kicker));

	// Wait for consumers and producers to finish
	for(auto& t : producers_and_consumers)
	{
		t.join();
	}

	return 0;
}
