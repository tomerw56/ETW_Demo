#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>


/*! \file
* \brief Io operations to demo the file monitor and the wprp use of SampleWPRControlProfiles
*
*	You will see the thread id and use it on  WPA.
*  - Execution:
*    -# wpr -start SampleWPRControlProfiles.wprp!MyFileIO.verbose
*    -# io_Operations.exe
*    -# wpr -stop io_operations.etl
*/

int main() {
    const int fileCount = 10;
    const int iterations = 5;
    const std::string baseName = "wpa_demo_file_";

    std::vector<std::string> files;
    for (int i = 0; i < fileCount; i++) {
        files.push_back(baseName + std::to_string(i) + ".txt");
    }

    for (int iter = 0; iter < iterations; ++iter) {
        std::cout << "Iteration " << iter + 1 << "/" << iterations << "\n";

        // 1. Write files
        for (const auto& fname : files) {
            std::ofstream out(fname, std::ios::binary);
            for (int i = 0; i < 10000; i++) {
                out << "Line " << i << " - some demo data for WPA\n";
            }
        }

        // 2. Read files back
        for (const auto& fname : files) {
            std::ifstream in(fname, std::ios::binary);
            std::string line;
            while (std::getline(in, line)) {
                // just touch data to force disk reads
                volatile auto len = line.size();
                (void)len;
            }
        }

        // 3. Delete files
        for (const auto& fname : files) {
            DeleteFileA(fname.c_str());
        }

        // Short pause so events spread out in WPA timeline
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "Done. Check WPA trace for File I/O events.\n";
    return 0;
}
