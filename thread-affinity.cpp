/*
 * Processor affinity, or CPU pinning or "cache affinity", enables the binding and unbinding of a process or a thread to a central processing unit (CPU) or a range of CPUs.
 * So that the process or thread will execute only on the designated CPU or CPUs rather than any CPU.
 * Using this machenism we can improve the performance of the process because cache hit will be more as compare to cache miss.
 */

/*
 * Run the program as below without affinity:
 * ./thread-affinity
 *
 * Run the program as below with affinity:
 * ./thread-affinity THREADAFFINITY
 */

#include<mutex>
#include<thread>
#include<vector>
#include<cstring>
#include<iostream>
using namespace std;

int main(int argc, const char* argv[])
{
    //Find the maximum number of threads (core-hyperthreading) supported by this machine
    int numberofthreads = std::thread::hardware_concurrency();
    //const int numberofthreads = 100;

    //A vector of threads
    //To use this we need to use index like threads[index]
    //If we will use push_back then it will add element after the numberofthreads element
    //std::vector<std::thread> threads(numberofthreads);

    //A vector of threads
    std::vector<std::thread> threads;

    //A mutex to lock for synchronization
    std::mutex iomutex;

    for (int i=0; i<numberofthreads; ++i) {
        std::thread thread = std::thread([&iomutex, i](){
                             //Wait for a second so that cpu affinity will be set by the main thread
                             std::this_thread::sleep_for(std::chrono::milliseconds(100));
                             while(true)
                             {
                                {
                                    std::lock_guard<std::mutex> iolock(iomutex);
                                    std::cout <<"Thread: " <<i <<" Running on CPU: " <<sched_getcpu() <<std::endl;
                                }
                                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                             }
                             });

        //Set the cpu affinity
        //Without this (cpu affinity) threads will be switching between the CPUs.
        //Now, the thread with odd number will run on 1st cpu and thread with even number will run on 0th cpu.
        if ((argc > 1) && !strncmp(argv[1], "THREADAFFINITY", strlen("THREADAFFINITY")))
        {
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            if (!(i%2)) {
                CPU_SET(0, &cpuset);
            } else {
                CPU_SET(1, &cpuset);
            }
            int retval = pthread_setaffinity_np(thread.native_handle(), sizeof(cpu_set_t), &cpuset);
            if (retval) {
                std::cout <<"Failed to set the affinity" <<std::endl;
            }
        }

        threads.push_back(std::move(thread));
    }

    //Joining the thread otherwise program will crash
    for (auto& thread : threads) {
        std::cout <<"Joining...\n";
        thread.join();  //The call will block here untill the first thread doesn't finish the job
        std::cout <<"Joining2...\n";
    }
    return(0);
}
