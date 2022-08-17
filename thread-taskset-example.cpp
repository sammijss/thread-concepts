/*
 * taskset command example
 * Without the command: 'taskset -c 1 ./thread-taskset-example' the threads will run on different cpus, but using the command all the threads will run on cpu 1 only.
 */

#include<mutex>
#include<thread>
#include<vector>
#include<iostream>
using namespace std;

int main()
{
    //Find the maximum number of threads (core-hyperthreading) supported by this machine
    int numberofthreads = std::thread::hardware_concurrency();

    //A vector of threads
    //To use this we need to use index like threads[index]
    //If we will use push_back then it will add element after the numberofthreads element
    std::vector<std::thread> threads(numberofthreads);

    //A mutex to lock for synchronization
    std::mutex iomutex;

    for (int i=0; i<numberofthreads; ++i) {
        threads[i] = std::thread([&iomutex, i](){
                             while(true)
                             {
                                {
                                    std::lock_guard<std::mutex> iolock(iomutex);
                                    std::cout <<"Thread: " <<i <<" Running on CPU: " <<sched_getcpu() <<std::endl;
                                }
                                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                             }
                             });
    }

    //Joining the thread otherwise program will crash
    for (auto& thread : threads) {
        std::cout <<"Joining...\n";
        thread.join();  //The call will block here untill the first thread doesn't finish the job
        std::cout <<"Joining2...\n";
    }
    return(0);
}
