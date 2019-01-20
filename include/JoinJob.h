#ifndef JOINJOB_H
#define JOINJOB_H

#include <stdint.h>
#include "Job.h"
#include "Joiner.h"

class Joiner;
class JoinJob: public Job{
    Joiner* joiner;
    int bucket_num;
    vector<uint64_t>* result_vector;
public:
    JoinJob(Joiner* j, int bucket_numm, vector<uint64_t>* rv){
        result_vector = rv;
        bucket_num = bucket_numm;
        joiner = j;
    }
    ~JoinJob(){}

    int run();
};

#endif
