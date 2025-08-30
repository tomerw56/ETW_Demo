

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <string>
#include <windows.h>

/*! \file
* \brief We will hung one thread - you can see it not exiting.
*
*	You will see the thread id and use it on  WPA.
*  - Execution:
*    -# wpr -start generalprofile
*    -# ThreadingDemo.exe
*    -# wpr -stop threading_demo.etl
*/
 //! Regular worker.
void worker(int id) {
    DWORD thread_id = GetCurrentThreadId();
    std::string name = "WorkerThread_" + std::to_string(id);
    std::cout  << name << " " << thread_id << "\n";
    std::cout << "[" << name << "] Doing work...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500 + id * 100));
    std::cout << "[" << name << "] Finished.\n";
}


void unterminated_function()
{
    while (true) {}
}


 //! Hunging worker.
void hangingThread() {
    
    DWORD thread_id = GetCurrentThreadId();
    
    std::cout << "hung thread" << " " << thread_id << "\n";

    std::cout << "[HangingThread] Started and will hang.\n";

    // Simulate a hang — infinite sleep
    unterminated_function();
   
}

int main() {
    std::cout << "start etw and hit key to continue\n";

    getchar();

    std::vector<std::thread> threads;

    // Launch normal worker threads
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(worker, i);
    }

    // Launch a thread that hangs
    threads.emplace_back(hangingThread);

    // Join all threads except the hanging one
    for (int i = 0; i < 5; ++i) {
        threads[i].join();
    }

    std::cout << "[MainThread] Main finished; hanging thread still alive.\n";

    return 0;
}
