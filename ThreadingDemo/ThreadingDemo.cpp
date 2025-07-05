#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <string>
#include <windows.h>

// Helper to set the thread name on Windows (visible in WPA)
void SetThreadName(const std::string& name) {
    const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
    struct THREADNAME_INFO {
        DWORD dwType = 0x1000;
        LPCSTR szName;
        DWORD dwThreadID;
        DWORD dwFlags = 0;
    };
#pragma pack(pop)

    THREADNAME_INFO info;
    info.szName = name.c_str();
    info.dwThreadID = GetCurrentThreadId();
    std::cout << " thread data [" << name << "] "<<" - - "<<info.dwThreadID <<"\n";

    __try {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}

void worker(int id) {
    std::string name = "WorkerThread_" + std::to_string(id);
    SetThreadName(name);

    std::cout << "[" << name << "] Doing work...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500 + id * 100));
    std::cout << "[" << name << "] Finished.\n";
}

void hangingThread() {
    SetThreadName("HangingThread");

    std::cout << "[HangingThread] Started and will hang.\n";

    // Simulate a hang — infinite sleep
   
    while(true) {}
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
