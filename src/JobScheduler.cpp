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
        cout << " job ran xd" << endl;

        pthread_mutex_lock (&jobs_mutex);
        num_of_jobs--;
        if(num_of_jobs == 0){
            cout << "jobs finished "<<endl;
            sem_post(&sem);
            sem_wait(&sem);
        }
        pthread_mutex_unlock (&jobs_mutex);
    }
}

JobScheduler::JobScheduler(int num_of_threads, Joiner* j){
    //keep pointer to joiner to access his data members
    joiner = j;
    this->num_of_threads = num_of_threads;

    //initialise synch structs
    pthread_mutex_init(&list_mutex, NULL);
    pthread_mutex_init(&jobs_mutex, NULL);
    pthread_cond_init(&cond_nonempty, NULL);
    sem_init (&sem, 0, 0);

    //make running threads
    pthread_t tid;
    for(int i = 0; i < num_of_threads; i++)
        pthread_create(&tid,NULL,thread_fun_wrapper,this);
}

JobScheduler::~JobScheduler(){

}

//joiner adds jobs through this function
void JobScheduler::add_job(Job* job){
    pthread_mutex_lock(&list_mutex);
    jobs.push_front(job);
    cout << "put job" << endl;
    pthread_mutex_unlock(&list_mutex);
}
