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
        // Read file to common string stream
        std::stringstream common_stream;
        common_stream << file_input.rdbuf();
        std::string common_string(common_stream.str());
        file_input.close();

        std::string formatted_string;
        // Add file check sum to string stream
        int pos = 0;
        do {
            std::stringstream local_stream;
            int new_pos = common_string.find("\n", pos + 1);
            std::string buf_string;
            if (new_pos != -1)
            {
                buf_string = common_string.substr(pos, new_pos - pos);
            }
            else
            {
                buf_string = common_string.substr(pos);
            }

            sum = 0;
            for (int iter : buf_string)
            {
                sum += iter;
            }
            local_stream << buf_string << "[" << sum << "] ";

            // Add process duration to string stream
            auto dur_time = std::chrono::high_resolution_clock::now() - start;
            local_stream << std::chrono::duration_cast<std::chrono::microseconds>(dur_time).count() << " microseconds";

            pos = new_pos;
            formatted_string += local_stream.str();
        } while (common_string.find("\n", pos) != -1);


        // Create and fill output file
        std::ofstream file_output(file_path);
        file_output << formatted_string;
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