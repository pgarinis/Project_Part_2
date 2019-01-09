#include <list>
#include <pthread.h>
#include "../include/JobScheduler.h"

using namespace std;

JobScheduler::JobScheduler(int num_of_threads, Joiner* j){
    //keep pointer to joiner to access his data members
    joiner = j;
    this->num_of_threads = num_of_threads;
    histograms = (uint64_t**)malloc(num_of_threads * sizeof(uint64_t*));
    for(int i = 0; i < num_of_threads; i++)
        histograms[i] = (uint64_t*)calloc(joiner->h1_num_of_buckets , sizeof(uint64_t));

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


void JobScheduler::handle_segmentation(){
        //init a barrier to wait on hist jobs
        initBarrier(num_of_threads);

        //create and add Hist jobs to scheduler
        int half_threads = num_of_threads / 2;
        uint64_t num_of_rows_0 = joiner->query->get_relations()[joiner->predicate->relation1]->get_num_of_records();
        uint64_t num_of_rows_1 = joiner->query->get_relations()[joiner->predicate->relation2]->get_num_of_records();
        uint64_t cur;
        uint64_t part;

        //assign jobs to calculate sub_histograms for each part of the relation 1
        cur = 0;
        part = num_of_rows_0 / half_threads;
        for(int i = 0; i < half_threads-1; i++){
            //cout <<"rows0 : "<< num_of_rows_0 << endl;
            //cout << cur << " ... " << cur + num_of_rows_0 / half_threads - 1 << endl;
            add_job(new HistogramJob(histograms[i], joiner, 0, cur, cur + part - 1));
            cur +=  part;
        }
        //in case of odd number
        add_job(new HistogramJob(histograms[half_threads - 1] ,joiner, 0, cur, cur + part - 1 + num_of_rows_0 % 2));
        //cout << cur << " ... " << cur + num_of_rows_0 / half_threads - 1 + num_of_rows_0 % 2 << endl;

        //assign jobs to calculate sub_histograms for each part of the relation 2
        cur = 0;
        part = num_of_rows_1 / half_threads;
        for(int i = 0; i < half_threads-1; i++){
            //cout <<"rows1 : "<< num_of_rows_1 << endl;
            //cout << cur << " ... " << cur + num_of_rows_1 / half_threads - 1 << endl;
            add_job(new HistogramJob(histograms[half_threads + i], joiner, 1, cur, cur + part - 1));
            cur += part;
        }
        //in case of odd number
        add_job(new HistogramJob(histograms[num_of_threads-1], joiner, 1, cur, cur + part - 1 + num_of_rows_1 % 2));
        //cout << cur << " ... " << cur + num_of_rows_1 / half_threads - 1 + num_of_rows_1 % 2 << endl;

        waitOnBarrier();

        //allocate both hist arrays and initialise them
        joiner->hist_array[0] = (uint64_t*)calloc(joiner->h1_num_of_buckets, sizeof(uint64_t));
        joiner->hist_array[1] = (uint64_t*)calloc(joiner->h1_num_of_buckets, sizeof(uint64_t));

        //compute hist array for each relation
        for(int i = 0; i < half_threads; i ++)
            for(int j = 0; j < joiner->h1_num_of_buckets; j++){
                joiner->hist_array[0][j] += histograms[i][j];
                joiner->hist_array[1][j] += histograms[i + half_threads][j];
            }

        //allocate psum_arrays for both relations
        joiner->psum_array[0] = (uint64_t*)malloc(joiner->h1_num_of_buckets * sizeof(uint64_t));
        joiner->psum_array[1] = (uint64_t*)malloc(joiner->h1_num_of_buckets * sizeof(uint64_t));

        //compute psum_arrays
        joiner->psum_array[0][0] = 0;
        joiner->psum_array[1][0] = 0;
        for(int j = 1; j < joiner->h1_num_of_buckets; j++){
            joiner->psum_array[0][j] = joiner->psum_array[0][j-1] + joiner->hist_array[0][j-1];
            joiner->psum_array[1][j] = joiner->psum_array[1][j-1] + joiner->hist_array[1][j-1];
        }

        //create starting_indexes in order to not use locks  when creating new column
        uint64_t starting_indices[num_of_threads][joiner->h1_num_of_buckets];
        for(int j = 0; j < joiner->h1_num_of_buckets; j++){
            //special occasion if j == 0
            if(j == 0){
                starting_indices[0][j] = 0;
                starting_indices[half_threads][j] = 0;
            }
            else{
                starting_indices[0][j] = starting_indices[half_threads-1][j-1] + histograms[half_threads-1][j-1];
                starting_indices[half_threads][j] = starting_indices[num_of_threads-1][j-1] + histograms[num_of_threads-1][j-1];
            }

            for(int i = 1; i < half_threads; i ++){
                starting_indices[i][j] = starting_indices[i-1][j] + histograms[i-1][j];
                starting_indices[i+half_threads][j] = starting_indices[half_threads+i-1][j] + histograms[half_threads+i-1][j];
            }
        }


        //malloc new columns
        joiner->new_column[0] = (NewColumnEntry*)malloc(num_of_rows_0 * sizeof(NewColumnEntry));
        joiner->new_column[1] = (NewColumnEntry*)malloc(num_of_rows_1 * sizeof(NewColumnEntry));

        //calculate new column throught jobs
        cur = 0;
        part = num_of_rows_0 / half_threads;
        for(int i = 0; i < half_threads-1; i++){
            add_job(new PartitionJob(starting_indices[i], joiner, 0, cur, cur + part - 1));
            cur +=  part;
        }
        //in case of odd number
        add_job(new PartitionJob(starting_indices[half_threads - 1] ,joiner, 0, cur, cur + part - 1 + num_of_rows_0 % 2));
        //cout << cur << " ... " << cur + num_of_rows_0 / half_threads - 1 + num_of_rows_0 % 2 << endl;

        cur = 0;
        part = num_of_rows_1 / half_threads;
        for(int i = 0; i < half_threads-1; i++){
            //cout <<"rows1 : "<< num_of_rows_1 << endl;
            //cout << cur << " ... " << cur + num_of_rows_1 / half_threads - 1 << endl;
            add_job(new PartitionJob(starting_indices[half_threads + i], joiner, 1, cur, cur + part - 1));
            cur += part;
        }
        //in case of odd number
        add_job(new PartitionJob(starting_indices[num_of_threads-1], joiner, 1, cur, cur + part - 1 + num_of_rows_1 % 2));
        //cout << cur << " ... " << cur + num_of_rows_1 / half_threads - 1 + num_of_rows_1 % 2 << endl;

        waitOnBarrier();


}
void JobScheduler::handle_join(){}


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
        //cout << "picked job" << endl;
        cur_job = jobs.front();
        jobs.pop_front();
        pthread_mutex_unlock (&list_mutex);

        //run the job
        cur_job->run();

        //on barrier created - wait for cond which is signaled by barrier
        //cout << "thread waiting on pthread barrier" << endl;
        int code = pthread_barrier_wait(&barrier); //thread wait on barrier

        //cout << "thread left from pthread barrier" << endl;

        pthread_mutex_lock (&barrier_mutex);
        if(code == PTHREAD_BARRIER_SERIAL_THREAD)
            pthread_cond_signal (&cond_barrier);
        //cout << "thread signaled barrier cond" << endl;
        pthread_mutex_unlock (&barrier_mutex);

    }
}


//joiner adds jobs through this function
void JobScheduler::add_job(Job* job){
    pthread_mutex_lock (&list_mutex);
    jobs.push_front(job);
    //cout << "put job" << endl;

    pthread_cond_signal(&cond_nonempty);
    pthread_mutex_unlock (&list_mutex);
}

int JobScheduler::initBarrier(int num){
    //make a pthread barrier for num jobs
    return pthread_barrier_init(&barrier,NULL,num);
}

void JobScheduler::waitOnBarrier(){
    //make a pthread barrier
    //cout << "waiting on barrier" << endl;
    // pthread_mutex_lock (&barrier_mutex);
    pthread_cond_wait(&cond_barrier,&barrier_mutex);
    pthread_barrier_destroy(&barrier);
    // pthread_mutex_unlock (&barrier_mutex);

}
