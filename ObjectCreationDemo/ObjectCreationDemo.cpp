#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

class DemoObject {
public:
    DemoObject(int id) : id(id) {
        std::cout << "Object " << id << " created\n";
        if (id % 5 == 0) {
            allocateLargeMemory();
        }
    }
    void performWork() {
        std::cout << "Object " << id << " doing some work...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Simulate some work
    }
    // Simulate large memory allocation
    void allocateLargeMemory() {
        std::cout << "Allocating 10MB of memory...\n";
        largeMemoryBlock = malloc(10 * 1024 * 1024);  // 10MB allocation

    }
private:
    int id;
    void* largeMemoryBlock = nullptr;  // Pointer to the large memory block

};

void createAndWorkOnObjects(int numObjects) {
    std::vector<DemoObject> objects;
    for (int i = 0; i < numObjects; ++i) {
        objects.push_back(DemoObject(i));
    }
    std::cout << "object created hit key to continue\n";

    getchar();

    // Simulate some work for each object
    for (auto& obj : objects) {
        obj.performWork();
    }
}

int main() {
    std::cout << "start etw and hit key to continue\n";

    getchar();
    const int numObjects = 50;  // Create 50 objects for demo

    // Create objects and simulate work
    createAndWorkOnObjects(numObjects);

    return 0;
}
