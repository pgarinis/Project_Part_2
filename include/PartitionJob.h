#ifndef PARTITIONJOB_H
#define PARTITIONJOB_H

#include <stdint.h>
#include "Job.h"
#include "Joiner.h"

class Joiner;
class PartitionJob: public Job{
    Joiner* joiner;
    int column_num;
    uint64_t start_offset;
    uint64_t finish_offset;
    uint64_t* sub_indices;
public:
    PartitionJob(uint64_t* indices, Joiner* j, int col_num, uint64_t start, uint64_t finish){
        sub_indices = indices;
        joiner = j;
        column_num = col_num;
        start_offset = start;
        finish_offset = finish;
    }
    ~PartitionJob(){}

    int run();
};

#endif
