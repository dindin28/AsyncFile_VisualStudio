#include <windows.h> // CreateProcess

#include <fstream>  // std::ofstream, std::ifstream
#include <sstream>  // std::stringstream
#include <iostream> // std::cout
#include <thread>   // std::thread
#include <chrono>   // std::chrono...

#define INPUT_FILE "input_sample.txt"

void FileCheckSum(std::string file_path,
    std::chrono::time_point<std::chrono::high_resolution_clock> start)
{
    unsigned long long sum = 0;
    std::ifstream file_input(INPUT_FILE);
    if (file_input.is_open() == false) // Check if file opened
    {
        std::cout << "Could not open input_file" << std::endl;
    }
    else
    {
        // Read file to string stream
        std::stringstream sstream;
        sstream << file_input.rdbuf();
        file_input.close();

        // Add file check sum to string stream
        std::string string(sstream.str());
        for (int iter : string)
        {
            sum += iter;
        }
        sstream << "[" << sum << "]";

        // Add process duration to string stream
        auto dur_time = std::chrono::high_resolution_clock::now() - start;
        sstream << " " << std::chrono::duration_cast<std::chrono::microseconds>(dur_time).count() << " microseconds";

        // Create and fill output file
        std::ofstream file_output(file_path);
        file_output << sstream.rdbuf();
        file_output.close();
    }
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    // Function in thread (POSIX Subsystem)
    std::thread(FileCheckSum, "output_thread_sample.txt", std::chrono::high_resolution_clock::now()).join();

    // Function in process (Window Subsystem)
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(NULL,   // No module name (use command line)
        NULL,           // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)            // Pointer to PROCESS_INFORMATION structure
        )
    {
        FileCheckSum("output_child_sample.txt", std::chrono::high_resolution_clock::now());
    }
    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}