#ifndef JOBSCHEDULER_H
#define JOBSCHEDULER_H

#include "Job.h"
#include <list>
#include <pthread.h>
#include "Joiner.h"
using namespace std;

class JobScheduler{
    int num_of_threads;

    //list of jobs
    list<Job*> jobs;
    Joiner* joiner;

    //mutexes
    pthread_mutex_t list_mutex;
    pthread_cond_t cond_nonempty;

    //barrier
    pthread_mutex_t barrier_mutex;
    pthread_cond_t cond_barrier;
    pthread_barrier_t barrier;


public:
    //creates running threads
    JobScheduler(int num_of_threads, Joiner* j);
    ~JobScheduler();

    void* thread_main_loop(void);

    static void *thread_fun_wrapper(void *context)
    {
        return ((JobScheduler *)context)->thread_main_loop();
    }

    //joiner adds jobs through this function
    void add_job(Job* job);

    int initBarrier(int num);

    void waitOnBarrier();
};

#endif