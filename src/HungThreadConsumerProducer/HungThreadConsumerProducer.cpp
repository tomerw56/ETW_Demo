#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <chrono>
#include <string>
#include <windows.h>


/*! \file
* \brief Hung thread which will exit after a short while - this is done in order to simulate long running execution
*
*	You will see the thread id and use it on  WPA.
*  - Execution:
*    -# wpr -start generalprofile
*    -# HungThreadConsumerProducer.exe ->hit a key
*    -# wpr -stop hugconsumerproducer.etl
*/


void long_running_task(int id,int task)
{
    std::cout << "[Consumer " << id << "] HANGING on task " << task << "\n";
    auto start = std::chrono::steady_clock::now();
    bool time_passed = false;
    while (time_passed == false)
    {
        auto end = std::chrono::steady_clock::now();
        time_passed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() >= 2000;
    }
    std::cout << "HUNG Passed\n";
}

std::queue<int> taskQueue;
std::mutex queueMutex;
std::condition_variable cv;
std::mutex sharedMutex;
bool done = false;

void producer(int id) {
    

    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::unique_lock<std::mutex> lock(queueMutex);
        int task = id * 100 + i;
        taskQueue.push(task);
        std::cout << "[Producer " << id << "] Produced task " << task << "\n";
        lock.unlock();
        cv.notify_one();
    }
}

void consumer(int id, bool hang = false) {
    
    DWORD thread_id = GetCurrentThreadId();
    std::cout << "[Consumer " << id << "] " << thread_id  << "\n";

    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [] { return !taskQueue.empty() || done; });

        if (done && taskQueue.empty())
            break;

        int task = taskQueue.front();
        taskQueue.pop();
        std::cout << "[Consumer " << id << "]" << thread_id <<"Consumed task " << task << "\n";

        if (hang && task % 2 == 0) {
            long_running_task(id, task);
        }


        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Simulate work
    }
}

int main() {
    const int numProducers = 2;
    const int numConsumers = 3;
    std::cout << "start etw and hit key to continue"<<std::endl;

    getchar();

    std::vector<std::thread> threads;

    // Start producers
    for (int i = 0; i < numProducers; ++i) {
        threads.emplace_back(producer, i);
    }

    // Start consumers, one of them will hang
    for (int i = 0; i < numConsumers; ++i) {
        bool shouldHang = (i == 1);  // e.g., Consumer_1 will hang
        threads.emplace_back(consumer, i, shouldHang);
    }

    // Let producers finish
    for (int i = 0; i < numProducers; ++i) {
        threads[i].join();
    }

    // Allow consumers some time to work
    std::this_thread::sleep_for(std::chrono::seconds(3));
    done = true;
    cv.notify_all();

    // Join all consumers
    for (int i = numProducers; i < threads.size(); ++i) {
        threads[i].join();
    }

    std::cout << "[MainThread] All done.\n";
    return 0;
}
