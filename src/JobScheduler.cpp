#include "../include/Job.h"
#include <list>
#include <pthread.h>
#include "../include/JobScheduler.h"

using namespace std;

//THREAD FUNCTION
void* JobScheduler::thread_main_loop(void){
    //store current job
    Job* cur_job;

    //thread's main loop
    while(1){
        //pick job from job list(when list is not empty)
        pthread_mutex_lock (&list_mutex);
        while(jobs.size() == 0)
            pthread_cond_wait(&cond_nonempty, &list_mutex);
        cout << "picked job" << endl;
        cur_job = jobs.front();
        jobs.pop_front();
        pthread_mutex_unlock (&list_mutex);

        //run the job
        cur_job->run();

        //on barrier created - wait for cond which is signaled by barrier
        cout << "thread waiting on pthread barrier" << endl;
        int code = pthread_barrier_wait(&barrier); //thread wait on barrier

        cout << "thread leaved from pthread barrier" << endl;

        pthread_mutex_lock (&barrier_mutex);
        if(code == PTHREAD_BARRIER_SERIAL_THREAD)
            pthread_cond_signal (&cond_barrier);
        cout << "thread signaled barrier cond" << endl;
        pthread_mutex_unlock (&barrier_mutex);

    }
}

JobScheduler::JobScheduler(int num_of_threads, Joiner* j){
    //keep pointer to joiner to access his data members
    joiner = j;
    this->num_of_threads = num_of_threads;

    //initialise synch structs
    pthread_mutex_init(&barrier_mutex, NULL);
    pthread_mutex_init(&list_mutex, NULL);
    pthread_cond_init(&cond_nonempty, NULL);
    pthread_cond_init(&cond_barrier, NULL);
    //make running threads
    pthread_t tid;
    for(int i = 0; i < num_of_threads; i++)
        pthread_create(&tid,NULL,thread_fun_wrapper,this);
}

JobScheduler::~JobScheduler(){

}

//joiner adds jobs through this function
void JobScheduler::add_job(Job* job){
    pthread_mutex_lock (&list_mutex);
    jobs.push_front(job);
    cout << "put job" << endl;

    pthread_cond_signal(&cond_nonempty);
    pthread_mutex_unlock (&list_mutex);
}

int JobScheduler::initBarrier(int num){
    //make a pthread barrier for num jobs
    return pthread_barrier_init(&barrier,NULL,num);
}

void JobScheduler::waitOnBarrier(){
    //make a pthread barrier
    cout << "waiting on barrier" << endl;
    // pthread_mutex_lock (&barrier_mutex);
    pthread_cond_wait(&cond_barrier,&barrier_mutex);
    pthread_barrier_destroy(&barrier);
    // pthread_mutex_unlock (&barrier_mutex);

}
