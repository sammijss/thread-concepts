/*
 * Thread priority 
 sammi@sammi-VirtualBox:~/Desktop/thread-concepts$ chrt -m
 SCHED_OTHER min/max priority    : 0/0
 SCHED_FIFO min/max priority     : 1/99
 SCHED_RR min/max priority       : 1/99
 SCHED_BATCH min/max priority    : 0/0
 SCHED_IDLE min/max priority     : 0/0
 SCHED_DEADLINE min/max priority : 0/0
 */

/*
 * This program need sudo privileges to run to change the priority
 *
 * Run the program as below without affinity:
 * ./thread-priority
 *
 * Run the program as below with affinity:
 * sudo ./thread-priority THREADPRIORITY
 */

#include<mutex>
#include<thread>
#include<vector>
#include<cstring>
#include<iostream>
using namespace std;

static void
display_sched_attr(int policy, const struct sched_param *param)
{
    std::string policystr;

    switch(policy)
    {
        case SCHED_OTHER:
            policystr = "SCHED_OTHER";
            break;

        case SCHED_FIFO:
            policystr = "SCHED_FIFO";
            break;

        case SCHED_RR:
            policystr = "SCHED_RR";
            break;

        case SCHED_BATCH:
            policystr = "SCHED_BATCH";
            break;

        case SCHED_IDLE:
            policystr = "SCHED_IDLE";
            break;
        
        case SCHED_DEADLINE:
            policystr = "SCHED_DEADLINE";
            break;
    }
    std::cout <<"Policy (" <<policy <<"): " <<policystr <<", Priority: " <<param->sched_priority <<std::endl;
}

int main(int argc, const char* argv[])
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
                             //Wait for a second so that cpu affinity will be set by the main thread
                             std::this_thread::sleep_for(std::chrono::milliseconds(100));
                             while(true)
                             {
                                {
                                    std::lock_guard<std::mutex> iolock(iomutex);
                                    std::cout <<"Thread: " <<i <<" Running on CPU: " <<sched_getcpu() <<std::endl;
                                }
                                //std::this_thread::sleep_for(std::chrono::milliseconds(10));
                             }
                             });

        if ((argc > 1) && !strncmp(argv[1], "THREADPRIORITY", strlen("THREADPRIORITY")))
        {
            //Set the cpu affinity: Run all the threads on the same cpu
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(0, &cpuset);
            int retval = pthread_setaffinity_np(threads[i].native_handle(), sizeof(cpu_set_t), &cpuset);
            if (retval) {
                std::cout <<"Failed to set the affinity" <<std::endl;
            }

            //Set thread priority
            int policy;
            sched_param param;
            retval = pthread_getschedparam(threads[i].native_handle(), &policy, &param);
            if (retval) {
                std::cout <<"Failed to get the sched param" <<std::endl;
            }

            //display_sched_attr(policy, &param);

            policy = SCHED_RR;
            if (!(i%2)) {
                std::cout <<"Lower priority thread: " <<i <<std::endl;
                param.sched_priority = 1;
            } else {
                std::cout <<"Higher priority thread: " <<i <<std::endl;
                param.sched_priority = 2;
            }

            display_sched_attr(policy, &param);

            retval = pthread_setschedparam(threads[i].native_handle(), policy, &param);
            if (retval) {
                std::cout <<"Failed to set the sched param" <<std::endl;
            }
        }
    }

    //Joining the thread otherwise program will crash
    for (auto& thread : threads) {
        std::cout <<"Joining...\n";
        thread.join();  //The call will block here untill the first thread doesn't finish the job
        std::cout <<"Joining2...\n";
    }
    return(0);
}
